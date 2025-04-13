/*
  pgfindlib.h - the file that's #included in pgfindlib.c and anything that calls it.

  Copyright (c) 2025 by Peter Gulutzan. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/
/*
  Package: https://github.com/pgulutzan/pgfindlib
*/

#ifndef PGFINDLIB_H
#define PGFINDLIB_H

extern int pgfindlib(const char *statement, char *buffer, unsigned int buffer_max_length);

#define PGFINDLIB_OK 0
#define PGFINDLIB_ERROR_BUFFER_MAX_LENGTH_TOO_SMALL -1
#define PGFINDLIB_ERROR_BUFFER_NULL -2
#define PGFINDLIB_ERROR_MAX_PATH_LENGTH_TOO_SMALL -3
#define PGFINDLIB_MALLOC_BUFFER_1_OVERFLOW -4
#define PGFINDLIB_MALLOC_BUFFER_2_OVERFLOW -5
#define PGFINDLIB_ERROR_STATEMENT_SYNTAX -6

/* PGFINDLIB_INCLUDE_ items must be 0 or 1. To not include, say e.g. -DPGFINDLIB_INCLUDE_ROW_VERSIONT=0 */

#ifndef PGFINDLIB_INCLUDE_LD_AUDIT
#define PGFINDLIB_INCLUDE_LD_AUDIT 1
#endif

#ifndef PGFINDLIB_INCLUDE_LD_PRELOAD
#define PGFINDLIB_INCLUDE_LD_PRELOAD 1
#endif

#ifndef PGFINDLIB_INCLUDE_DT_RPATH_OR_DT_RUNPATH
#define PGFINDLIB_INCLUDE_DT_RPATH_OR_DT_RUNPATH 1
#endif

#ifndef PGFINDLIB_INCLUDE_LD_LIBRARY_PATH
#define PGFINDLIB_INCLUDE_LD_LIBRARY_PATH 1
#endif

#ifndef PGFINDLIB_INCLUDE_LD_RUN_PATH
#define PGFINDLIB_INCLUDE_LD_RUN_PATH 1
#endif

#ifndef PGFINDLIB_INCLUDE_LD_SO_CACHE
#define PGFINDLIB_INCLUDE_LD_SO_CACHE 1
#endif

#ifndef PGFINDLIB_INCLUDE_DEFAULT_PATHS
#define PGFINDLIB_INCLUDE_DEFAULT_PATHS 1
#endif

#ifndef PGFINDLIB_INCLUDE_LD_PGFINDLIB_PATH
#define PGFINDLIB_INCLUDE_LD_PGFINDLIB_PATH 1
#endif

#ifndef PGFINDLIB_INCLUDE_GET_LIB_OR_PLATFORM
#define PGFINDLIB_INCLUDE_GET_LIB_OR_PLATFORM 1
#endif

#ifndef PGFINDLIB_INCLUDE_SYMLINKS
#define PGFINDLIB_INCLUDE_SYMLINKS 1
#endif

#ifndef PGFINDLIB_INCLUDE_HARDLINKS
#define PGFINDLIB_INCLUDE_HARDLINKS 1
#endif

#ifndef PGFINDLIB_INCLUDE_ROW_VERSION
#define PGFINDLIB_INCLUDE_ROW_VERSION 1
#endif

/* If this is changed to 1, every source gets a row comment */
#ifndef PGFINDLIB_INCLUDE_ROW_SOURCE_NAME
#define PGFINDLIB_INCLUDE_ROW_SOURCE_NAME 0
#endif

#ifndef PGFINDLIB_INCLUDE_ROW_LIB
#define PGFINDLIB_INCLUDE_ROW_LIB 1
#endif

#define PGFINDLIB_IF_GET_LIB_OR_PLATFORM             1
#define PGFINDLIB_IF_SYMLINKS                        1
#define PGFINDLIB_IF_HARDLINKS                       1

/* Every column in a buffer output row ends with ", " but you can redefine e.g. to ":" if worried that filenames contain commas */
#define PGFINDLIB_COLUMN_DELIMITER ", "
#define PGFINDLIB_ROW_DELIMITER "\n"
#define MAX_COLUMNS_PER_ROW 7
#define COLUMN_FOR_ROW_NUMBER 0
#define COLUMN_FOR_PATH 1
#define COLUMN_FOR_SOURCE 2
#define COLUMN_FOR_COMMENT_1 3
#define COLUMN_FOR_COMMENT_2 4
#define COLUMN_FOR_COMMENT_3 5

/* Every COLUMN_FOR_COMMENT_x starts with a 3-digit number. Generlly informative ones are 1-49, severe or unexpected ones are 50-99, syntax errors are 100-150 */
/* Many non-syntax-related comments can be suppressed by changing the #define to 0. */
#define PGFINDLIB_COMMENT_VERSION                     1
#define PGFINDLIB_COMMENT_PGFINDLIB                   2
#define PGFINDLIB_COMMENT_URL                         3
#define PGFINDLIB_COMMENT_LIB                         4
#define PGFINDLIB_COMMENT_LIB_STRING                  5
#define PGFINDLIB_COMMENT_PLATFORM_STRING             6
#define PGFINDLIB_COMMENT_ORIGIN_STRING               7
#define PGFINDLIB_COMMENT_ASSUMING_LIB                8
#define PGFINDLIB_COMMENT_UNAME_FAILED                9
#define PGFINDLIB_COMMENT_ASSUMING_PLATFORM          10
#define PGFINDLIB_COMMENT_MAX_INODE_COUNT_TOO_SMALL  11
#define PGFINDLIB_COMMENT_REPLACE_STRING             12
#define PGFINDLIB_COMMENT_SYMLINK                    13
#define PGFINDLIB_COMMENT_DUPLICATE                  14
#define PGFINDLIB_COMMENT_ACCESS_LDCONFIG_FAILED     50
#define PGFINDLIB_COMMENT_CANNOT_READ_RPATH          51
#define PGFINDLIB_COMMENT_CANNOT_READ_RUNPATH        52
#define PGFINDLIB_COMMENT_LDCONFIG_FAILED            53
#define PGFINDLIB_COMMENT_ELF_AUX_INFO_FAILED        54
#define PGFINDLIB_COMMENT_READLINK_FAILED            55
#define PGFINDLIB_COMMENT_NO_TRUE_OR_CP              56
#define PGFINDLIB_COMMENT_EHDR_IDENT                 57
#define PGFINDLIB_COMMENT_CANT_FIND_DYNAMIC_LOADER   58
#define PGFINDLIB_COMMENT_CANT_ACCESS_DYNAMIC_LOADER 59
#define PGFINDLIB_COMMENT_ACCESS_FAILED              60
#define PGFINDLIB_COMMENT_LSTAT_FAILED               61
#define PGFINDLIB_COMMENT_ELF_OPEN_FAILED            70
#define PGFINDLIB_COMMENT_ELF_READ_FAILED            71
#define PGFINDLIB_COMMENT_ELF_HAS_INVALID_IDENT      72
#define PGFINDLIB_COMMENT_ELF_IS_NOT_EXEC_OR_DYN     73
#define PGFINDLIB_COMMENT_ELF_SHT_DYNAMIC_NOT_FOUND  74
#define PGFINDLIB_COMMENT_ELF_MACHINE_DOES_NOT_MATCH 75
#define PGFINDLIB_COMMENT_ELF_BUFFER_TOO_SMALL       76
#define PGFINDLIB_COMMENT_STATEMENT_SYNTAX_STATEMENT_IS_NULL                   100
#define PGFINDLIB_COMMENT_STATEMENT_SYNTAX_P_GREATER_STATEMENT_END             101
#define PGFINDLIB_COMMENT_STATEMENT_SYNTAX_QUOTE_WITHOUT_END_QUOTE             102
#define PGFINDLIB_COMMENT_STATEMENT_SYNTAX_TOKEN_TOO_LONG                      103
#define PGFINDLIB_COMMENT_STATEMENT_SYNTAX_TOO_MANY_TOKENS                     104
#define PGFINDLIB_COMMENT_STATEMENT_SYNTAX_COMMA_WITHOUT_ITEM                  105
#define PGFINDLIB_COMMENT_STATEMENT_SYNTAX_FROM_OUT_OF_ORDER                   106
#define PGFINDLIB_COMMENT_STATEMENT_SYNTAX_WHERE_OUT_OF_ORDER                  107
#define PGFINDLIB_COMMENT_STATEMENT_SYNTAX_TOO_MANY_TOKENS_WHEN_ADDING_DEFAULT 108

#define PGFINDLIB_TOKEN_SOURCE_DT_RPATH_OR_DT_RUNPATH       (0x12 * PGFINDLIB_INCLUDE_DT_RPATH_OR_DT_RUNPATH)

/* Token numbers. the ..._SOURCE_... ones correspond to what's in pgfindlib_standard_source_array in pgfindlib.c, plus 1 */
#define PGFINDLIB_TOKEN_SOURCE_LD_AUDIT              1
#define PGFINDLIB_TOKEN_SOURCE_LD_PRELOAD            2
#define PGFINDLIB_TOKEN_SOURCE_DT_RPATH              3
#define PGFINDLIB_TOKEN_SOURCE_LD_LIBRARY_PATH       4
#define PGFINDLIB_TOKEN_SOURCE_DT_RUNPATH            5
#define PGFINDLIB_TOKEN_SOURCE_LD_RUN_PATH           6
#define PGFINDLIB_TOKEN_SOURCE_LD_SO_CACHE           7
#define PGFINDLIB_TOKEN_SOURCE_DEFAULT_PATHS         8
#define PGFINDLIB_TOKEN_SOURCE_LD_PGFINDLIB_PATH     9
#define PGFINDLIB_TOKEN_SOURCE_NONSTANDARD          10
#define PGFINDLIB_TOKEN_FROM                        11
#define PGFINDLIB_TOKEN_WHERE                       12
#define PGFINDLIB_TOKEN_FILE                        13
#define PGFINDLIB_TOKEN_COMMA                       14
#define PGFINDLIB_TOKEN_END                         15
#define PGFINDLIB_TOKEN_UNKNOWN                     17

#ifndef PGFINDLIB_MAX_INODE_COUNT
#define PGFINDLIB_MAX_INODE_COUNT 1000
#endif

#ifndef PGFINDLIB_MAX_PATH_LENGTH
#define PGFINDLIB_MAX_PATH_LENGTH 4096
#endif

#ifndef PGFINDLIB_MAX_TOKENS_COUNT
#define PGFINDLIB_MAX_TOKENS_COUNT 100
#endif

#define PGFINDLIB_MAX_TOKEN_LENGTH PGFINDLIB_MAX_PATH_LENGTH

#ifndef PGFINDLIB_FREEBSD
#if defined(__FreeBSD__)
#define PGFINDLIB_FREEBSD
#endif
#endif

#define PGFINDLIB_VERSION_MAJOR 0
#define PGFINDLIB_VERSION_MINOR 9
#define PGFINDLIB_VERSION_PATCH 7

#endif
