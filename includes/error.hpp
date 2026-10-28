
#pragma once

/*
 * FILE OPERATION ERROR
 */

# define EINVALID_OUTFILE "The file %s is invalid"
# define EMISSING_FILEPATH "Missing input file"
# define EOPEN_FILE "Failed to open %s"
# define ECLOSE_FILE "Failed to close %s"
#define EREG_FILE "The file %s is not a regular file"
#define ELSTAT_FAILED "Failed to get %s properties"
#define EDONT_EXIST "The file %s doesnt exist"
#define EREAD_ERROR "Failed to read on file %s"
#define EWRITE_ERROR "Failed to write on file %s"
#define EALLOC_ERROR "Failed to allocated memory for %s"

/*
 * ARGUMENTS ERROR
 */
// The error happen when the user doesnt specify -c, -e or -b on argv
# define EMISSING_ACTION "Missing --compress or/and --encrypt flag"
// The error happen when the user put a flag that doesnt exist
# define EUNKNOWN_FLAG "Unknown parameter "

/*
 * COMMON ERROR
 */
// The error happen the helper::expand() failed to recover the name of the user
# define EUSERNAME "Failed to get the current username"
