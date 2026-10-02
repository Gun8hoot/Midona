#include "includes/lz4.hpp"

#include <cstring>
#include <algorithm>

/* ===================================================================== */
/*  Internal helpers                                                     */
/* ===================================================================== */
namespace
{
	inline u32_t read32(const u8_t* p)
	{
		u32_t v;
		std::memcpy(&v, p, sizeof(v));
		return v;
	}

	/* 2654435761U == 2654435761 == 0x9E3779B1 (Knuth's multiplicative hash) */
	inline u32_t hash4(u32_t sequence)
	{
		return (sequence * 2654435761U) >> (32 - LZ4_HASHLOG);
	}
}

/* ===================================================================== */
/*  Block compression                                                    */
/* ===================================================================== */
int lz4::compressBlock(const u8_t* src, int srcSize,
					   u8_t* dst, int dstCapacity)
{
	const u8_t* const base     = src;
	const u8_t*       ip       = src;
	const u8_t* const iend     = src + srcSize;
	u8_t*             op       = dst;
	u8_t* const       oend     = dst + dstCapacity;

	/* ---------- tiny input: a single literal-only sequence ---------- */
	if (srcSize < LZ4_MFLIMIT + 1) {
		int lit    = srcSize;
		int needed = 1 + lit + lit / 255 + 1;
		if (needed > dstCapacity) return -1;

		u8_t* token = op++;
		if (lit >= LZ4_RUN_MASK) {
			int len = lit - LZ4_RUN_MASK;
			*token = (u8_t)(LZ4_RUN_MASK << LZ4_ML_BITS);
			while (len >= 255) { *op++ = 255; len -= 255; }
			*op++ = (u8_t)len;
		} else {
			*token = (u8_t)(lit << LZ4_ML_BITS);
		}
		std::memcpy(op, src, lit);
		op += lit;
		return (int)(op - dst);
	}

	const u8_t*       anchor     = ip;
	const u8_t* const mflimit    = iend - LZ4_MFLIMIT;
	const u8_t* const matchlimit = iend - LZ4_LASTLITERALS;

	/* Hash table holds an offset from `base`, or -1 when empty. */
	std::vector<int> hashTable(LZ4_HASHTABLESIZE, -1);

	/* The very first byte can never be a match start (nothing precedes it). */
	hashTable[hash4(read32(ip))] = 0;
	++ip;

	for (;;) {
		const u8_t* match = nullptr;

		/* ---------- find a match (with acceleration) ---------- */
		{
			const u8_t* forwardIp     = ip;
			int         step          = 1;
			int         searchMatchNb = 1 << 6;   /* "skip trigger" */
			u32_t       h             = hash4(read32(ip));

			for (;;) {
				u32_t const curH = h;
				ip        = forwardIp;
				forwardIp += step;
				step      = (searchMatchNb++ >> 6);

				if (forwardIp > mflimit) goto _last_literals;

				int ref = hashTable[curH];
				h                    = hash4(read32(forwardIp));
				hashTable[curH]      = (int)(ip - base);

				if (ref >= 0 &&
					(int)(ip - base) - ref <= LZ4_MAX_DISTANCE &&
					read32(base + ref) == read32(ip))
				{
					match = base + ref;
					break;
				}
			}
		}

		/* ---------- extend the match backwards ---------- */
		while (ip > anchor && match > base && ip[-1] == match[-1]) {
			--ip;
			--match;
		}

		/* ---------- emit one full sequence ---------- */
		for (;;) {
			int litLength = (int)(ip - anchor);

			/* length of the forward match (>= LZ4_MINMATCH) */
			const u8_t* s = ip    + LZ4_MINMATCH;
			const u8_t* m = match + LZ4_MINMATCH;
			while (s < matchlimit && *s == *m) { ++s; ++m; }
			int matchLength = (int)(s - ip) - LZ4_MINMATCH;

			/* worst-case output for this sequence */
			int litExt   = (litLength   >= LZ4_RUN_MASK)
						 ? (litLength   - LZ4_RUN_MASK) / 255 + 1 : 0;
			int matchExt = (matchLength >= LZ4_ML_MASK)
						 ? (matchLength - LZ4_ML_MASK ) / 255 + 1 : 0;
			if (op + 1 + litExt + litLength + 2 + matchExt > oend)
				return -1;

			u8_t* token = op++;

			/* literal length */
			if (litLength >= LZ4_RUN_MASK) {
				*token = (u8_t)(LZ4_RUN_MASK << LZ4_ML_BITS);
				int len = litLength - LZ4_RUN_MASK;
				while (len >= 255) { *op++ = 255; len -= 255; }
				*op++ = (u8_t)len;
			} else {
				*token = (u8_t)(litLength << LZ4_ML_BITS);
			}

			/* literals */
			std::memcpy(op, anchor, litLength);
			op += litLength;

			/* offset (little-endian, 2 bytes) */
			u32_t offset = (u32_t)(ip - match);
			*op++ = (u8_t)(offset);
			*op++ = (u8_t)(offset >> 8);

			/* match length */
			if (matchLength >= LZ4_ML_MASK) {
				*token |= (u8_t)LZ4_ML_MASK;
				int len = matchLength - LZ4_ML_MASK;
				while (len >= 255) { *op++ = 255; len -= 255; }
				*op++ = (u8_t)len;
			} else {
				*token |= (u8_t)matchLength;
			}

			ip     = s;
			anchor = ip;

			/* End of block? Last 12 bytes must remain literals. */
			if (ip >= mflimit) goto _last_literals;

			/* Insert ip-2 — helps the next search. */
			hashTable[hash4(read32(ip - 2))] = (int)(ip - 2 - base);

			/* Opportunistic immediate next match. */
			{
				u32_t hh  = hash4(read32(ip));
				int   ref = hashTable[hh];
				hashTable[hh] = (int)(ip - base);
				if (ref >= 0 &&
					(int)(ip - base) - ref <= LZ4_MAX_DISTANCE)
				{
					match = base + ref;
					if (read32(match) == read32(ip))
						continue;   /* emit another match, zero literals */
				}
			}
			break;                  /* back to searching */
		}

		++ip;   /* advance past the end of the previous match */
	}

_last_literals:
	{
		int litLength = (int)(iend - anchor);
		if (op + 1 + litLength + litLength / 255 + 1 > oend)
			return -1;

		u8_t* token = op++;
		if (litLength >= LZ4_RUN_MASK) {
			*token = (u8_t)(LZ4_RUN_MASK << LZ4_ML_BITS);
			int len = litLength - LZ4_RUN_MASK;
			while (len >= 255) { *op++ = 255; len -= 255; }
			*op++ = (u8_t)len;
		} else {
			*token = (u8_t)(litLength << LZ4_ML_BITS);
		}
		std::memcpy(op, anchor, litLength);
		op += litLength;
	}
	return (int)(op - dst);
}

/* ===================================================================== */
/*  Block decompression                                                  */
/* ===================================================================== */
int lz4::decompressBlock(const u8_t* src, int srcSize,
						 u8_t* dst, int dstCapacity)
{
	const u8_t* ip   = src;
	const u8_t* iend = src + srcSize;
	u8_t*       op   = dst;
	u8_t*       oend = dst + dstCapacity;

	for (;;) {
		if (ip >= iend) return -1;
		u8_t token = *ip++;

		/* ---------------- literals ---------------- */
		int litLength = token >> LZ4_ML_BITS;
		if (litLength == LZ4_RUN_MASK) {
			int s;
			do {
				if (ip >= iend) return -1;
				s = *ip++;
				litLength += s;
			} while (s == 255);
		}
		if (ip + litLength > iend) return -1;
		if (op + litLength > oend) return -1;
		std::memcpy(op, ip, litLength);
		op += litLength;
		ip += litLength;

		/* Last sequence has no match. */
		if (ip == iend) break;

		/* ---------------- offset ---------------- */
		if (ip + 2 > iend) return -1;
		u32_t offset = (u32_t)ip[0] | ((u32_t)ip[1] << 8);
		ip += 2;
		if (offset == 0 || offset > (u32_t)(op - dst)) return -1;

		const u8_t* match = op - offset;

		/* ---------------- match length ---------------- */
		int matchLength = token & LZ4_ML_MASK;
		if (matchLength == LZ4_ML_MASK) {
			int s;
			do {
				if (ip >= iend) return -1;
				s = *ip++;
				matchLength += s;
			} while (s == 255);
		}
		matchLength += LZ4_MINMATCH;

		if (op + matchLength > oend) return -1;

		/* Byte-by-byte: source and destination can overlap (RLE). */
		for (int i = 0; i < matchLength; ++i)
			op[i] = match[i];
		op += matchLength;
	}
	return (int)(op - dst);
}

/* ===================================================================== */
/*  Convenience wrappers                                                 */
/* ===================================================================== */
std::vector<u8_t> lz4::compress(const std::vector<u8_t>& in)
{
	/* Worst-case expansion for LZ4 block is n + n/255 + 16 bytes. */
	std::vector<u8_t> out(in.size() + in.size() / 255 + 16);
	int n = compressBlock(in.data(), (int)in.size(),
						  out.data(), (int)out.size());
	if (n < 0) return std::vector<u8_t>();
	out.resize((std::size_t)n);
	return out;
}

std::vector<u8_t> lz4::decompress(const std::vector<u8_t>& in,
								  std::size_t uncompressedSize)
{
	std::vector<u8_t> out(uncompressedSize);
	int n = decompressBlock(in.data(), (int)in.size(),
							out.data(), (int)out.size());
	if (n < 0) return std::vector<u8_t>();
	out.resize((std::size_t)n);
	return out;
}
