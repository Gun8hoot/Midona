
#pragma once

/*
 * FILE OPERATION ERROR
 */
// This error happen when the user put a outfile that look like a flag
# define EINVALID_OUTFILE "Invalid outfile "
// This error happen when the user forget to specify the file to compress/encrypt
# define EMISSING_FILEPATH "Missing input file"
// This error happen when the a file failed to be open. A space is at the end to specify the filename
# define EOPEN_FILE "Failed to open "
// This error happen when the a file failed to be close. A space is at the end to specify the filename
# define ECLOSE_FILE "Failed to close "
// This error happen when the input file is a directory symlink, socket, ...
#define EREG_FILE "The input file is not a regular file"
// This error happen if lstat failed to open the file
#define ELSTAT_FAILED "Failed to open file property "
// This error happen if the input file doesnt exist
#define EDONT_EXIST "The input file doesnt exist"
// This error happen if the user doesnt have read permission on the file
#define EREAD_ERROR "The input file doesnt have read permission"

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
