/*
  pgoptionfiles.h - the file that's #included in pgoptionfiles.c and anything that calls it.
*/
/*
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
  Package: https://github.com/pgulutzan/pgoptionfiles, maybe https://github.com/ocelot-inc/ocelotgui later
*/

#ifndef PGOPTIONFILES_H
#define PGOPTIONFILES_H

#ifndef PGOPTIONFILES_DELIMITER
#define PGOPTIONFILES_DELIMITER '\n'
#endif

#ifndef PGOPTIONFILES_READ
#define PGOPTIONFILES_READ 0
#endif

/* PATH_MAX is probably 4096. In fact list size is usually < 100 bytes */
#ifndef PGOPTIONFILES_MAX_FILE_NAMES_LIST_SIZE
#define PGOPTIONFILES_MAX_FILE_NAMES_LIST_SIZE (PATH_MAX * 10)
#endif

/* say 1 to enable wait_pid() timeouts after a few seconds. */
#ifndef PGOPTIONFILES_USE_TIMEOUT
#define PGOPTIONFILES_USE_TIMEOUT 0
#endif

/* say 1 to require #include <mysql.h> instead of using the typedef + enum below */
#ifndef PGOPTIONFILES_INCLUDE_MYSQL
#define PGOPTIONFILES_INCLUDE_MYSQL 0
#endif

/* say 1 to eliminate the tracer, this is a debugging option */
#ifndef PGOPTIONFILES_TRACEE_ONLY
#define PGOPTIONFILES_TRACEE_ONLY 0
#endif

#if (PGOPTIONFILES_TRACEE_ONLY == 0)
#include <sys/ptrace.h>   /* This defines some PTRACE_ items as enum */
//#include <linux/ptrace.h> /* This defines same PTRACE_ items as int, that's why there are casts to enum */
#include <sys/user.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdint.h>
//#include <sys/types.h>
#include <sys/syscall.h> /* This should have SYS_lstat etc. */
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <limits.h>

void pgoptionfiles_tracee(const char *);
int pgoptionfiles_tracer(pid_t pid, char *file_names_list, char *error_list);
int pgoptionfiles_copy_from_tracee(pid_t tracee_pid, char *dest, const char *src);
void pgoptionfiles_tracee_error_or_message(const char *);
int pgoptionfiles_tracer_arg_number(size_t psi_entry_nr);

#if (PGOPTIONFILES_INCLUDE_MYSQL == 1)
#include <mysql.h>
#else
typedef struct MYSQL {
  char opaque_item[4096]; /* at time of writing sizeof("real mysql struct") == 1272 */
} MYSQL;
enum mysql_option {
  MYSQL_READ_DEFAULT_GROUP = 5
};
#endif

#if (PGOPTIONFILES_FREEBSD == 1)
/*
  Say 1 to indicate it's for FreeBSD rather than Linux.
  Not done. But perhaps it could be made to work ...
  Some equivalencies are already established and might be okay in Linux too:
  PT_TRACE_ME = PTRACE_TRACEME, PT_READ_D = PTRACE_PEEKDATA, PT_SYSCALL = PTRACE_SYSCALL,
  PT_GETREGS = PTRACE_GETREGS, PT_SETREGS = PTRACE_SETREGS.
  ... But you'll need to set flags for PTRACE_SCE + PTRACE_SCX (or the combination which
      is PTRACE_SYSCALL although that has a different meaning in Linux) which I guess is
      done with PT_SET_EVENT_MASK
  https://man.freebsd.org/cgi/man.cgi?query=ptrace
*/
#endif

#endif
