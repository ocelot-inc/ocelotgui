/*
  pgfindlib.c --   To get a list of paths and .so files along paths dynamic loader might choose, with some extra information

   Version: 0.9.8
   Last modified: September 25 2025

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>

#include "pgfindlib.h" /* definition of pgfindlib(), and some #define PGFINDLIB_... */

#if (PGFINDLIB_TOKEN_SOURCE_DT_RPATH_OR_DT_RUNPATH != 0)
#include <elf.h>
#include <link.h>
#endif

/* todo: don't ask for this if we do not stat */
#include <sys/stat.h>

#ifdef PGFINDLIB_FREEBSD
#include <sys/auxv.h>
#endif

#include <errno.h>

struct tokener
{
  const char *tokener_name;
  short int tokener_length;
  char tokener_comment_id;
};

#define PGFINDLIB_REASON_SO_CHECK 1
#define PGFINDLIB_REASON_SO_LIST 2

static int pgfindlib_strcat(char *buffer, unsigned int *buffer_length, const char *line, unsigned int buffer_max_length);
/* todo: make this obsolete */
static int pgfindlib_comment_in_row(char *comment, unsigned int comment_number, int additional_number);

static int pgfindlib_keycmp(const char *a, unsigned int a_len, const char *b);
static int pgfindlib_tokenize(const char *statement, struct tokener tokener_list[],
                              unsigned int *row_number, ino_t inode_list[], unsigned int *inode_count,
                              char *buffer, unsigned int *buffer_length, unsigned int buffer_max_length);


static int pgfindlib_file(char *buffer, unsigned int *buffer_length, const char *line, unsigned int buffer_max_length,
                          unsigned int *row_number,
                          ino_t inode_list[], unsigned int *inode_count, unsigned int *inode_warning_count,
                          struct tokener tokener_list_item, int program_e_machine);

static int pgfindlib_find_line_in_statement(const struct tokener tokener_list[], const char *line);
static int pgfindlib_row_version(char *buffer, unsigned int *buffer_length, unsigned buffer_max_length, unsigned int *row_number,
                                 ino_t inode_list[], unsigned int *inode_count);
static int pgfindlib_row_lib(char *buffer, unsigned int *buffer_length, unsigned buffer_max_length, unsigned int *row_number,
                             ino_t inode_list[], unsigned int *inode_count,
                             const char *lib, const char *platform, const char *origin);
#if (PGFINDLIB_INCLUDE_ROW_SOURCE_NAME == 1)
static int pgfindlib_row_source_name(char *buffer, unsigned int *buffer_length, unsigned buffer_max_length, unsigned int *row_number,
                             ino_t inode_list[], unsigned int *inode_count,
                             const char *source_name);
#endif
static int pgfindlib_replace_lib_or_platform_or_origin(char *one_library_or_file, unsigned int *replacements_count, const char *lib, const char *platform, const char *origin);
static int pgfindlib_get_origin_and_lib_and_platform(char *origin, char *lib, char *platform,
                                              char *buffer, unsigned int *buffer_length, unsigned buffer_max_length,
                                              int *program_e_machine, unsigned int *row_number,
                                              ino_t inode_list[], unsigned int *inode_count);
static int pgfindlib_so_cache(const struct tokener tokener_list[], int tokener_number,
                       char *malloc_buffer_1, unsigned int *malloc_buffer_1_length, unsigned malloc_buffer_1_max_length,
                       char **malloc_buffer_2, unsigned int *malloc_buffer_2_length, unsigned malloc_buffer_2_max_length);
static int pgfindlib_add_to_malloc_buffers(const char *new_item, int source_number,
                                    char *malloc_buffer_1, unsigned int *malloc_buffer_1_length, unsigned malloc_buffer_1_max_length,
                                    char **malloc_buffer_2, unsigned int *malloc_buffer_2_length, unsigned malloc_buffer_2_max_length);
static int pgfindlib_row_bottom_level(char *buffer, unsigned int *buffer_length, unsigned int buffer_max_length, unsigned int *row_number, 
                               const char *columns_list[]);

#if (PGFINDLIB_TOKEN_SOURCE_DT_RPATH_OR_DT_RUNPATH != 0)
/* Ordinarily link.h has extern ElfW(Dyn) _DYNAMIC but it's missing with FreeBSD */
extern __attribute__((weak)) ElfW(Dyn) _DYNAMIC[];
#endif

/* pgfindlib_standard_source_array and pgfindlib_standard_source_array_n must match. */
const char *pgfindlib_standard_source_array[] = {"LD_AUDIT", "LD_PRELOAD", "DT_RPATH", "LD_LIBRARY_PATH", "DT_RUNPATH",
                                                 "LD_RUN_PATH", "ld.so.cache", "default_paths", "LD_PGFINDLIB_PATH", ""};
const char pgfindlib_standard_source_array_n[]= {PGFINDLIB_TOKEN_SOURCE_LD_AUDIT,PGFINDLIB_TOKEN_SOURCE_LD_PRELOAD,
                                                 PGFINDLIB_TOKEN_SOURCE_DT_RPATH, PGFINDLIB_TOKEN_SOURCE_LD_LIBRARY_PATH,
                                                 PGFINDLIB_TOKEN_SOURCE_DT_RUNPATH, PGFINDLIB_TOKEN_SOURCE_LD_RUN_PATH,
                                                 PGFINDLIB_TOKEN_SOURCE_LD_SO_CACHE, PGFINDLIB_TOKEN_SOURCE_DEFAULT_PATHS,
                                                 PGFINDLIB_TOKEN_SOURCE_LD_PGFINDLIB_PATH, 0};
static int pgfindlib_qsort_compare(const void *p1, const void *p2);
static int pgfindlib_source_scan(const char *librarylist, char *buffer, unsigned int *buffer_length,
                                unsigned int tokener_number, unsigned int buffer_max_length,
                                const char *lib, const char *platform, const char *origin,
                                unsigned int *row_number,
                                ino_t inode_list[], unsigned int *inode_count, unsigned int *inode_warning_count,
                                struct tokener tokener_list[],
                                char *malloc_buffer_1, unsigned int *malloc_buffer_1_length, unsigned malloc_buffer_1_max_length,
                                char **malloc_buffer_2, unsigned int *malloc_buffer_2_length, unsigned malloc_buffer_2_max_length,
                                int program_e_machine);
static int pgfindlib_read_elf(const struct tokener tokener_list[], const char* possible_elf_file, int reason, int program_e_machine);
static int pgfindlib_comment_is_row(const char *comment, unsigned int comment_number,
                             char *buffer, unsigned int *buffer_length, unsigned int buffer_max_length, unsigned int *row_number,
                             ino_t inode_list[], unsigned int *inode_count);

#define PGFINDLIB_FREE_AND_RETURN \
{ \
  if (malloc_buffer_1 != NULL) { free(malloc_buffer_1); } \
  if (malloc_buffer_2 != NULL) { free(malloc_buffer_2); } \
  return rval; \
}

int pgfindlib(const char *statement, char *buffer, unsigned int buffer_max_length)
{
  if (buffer == NULL) return PGFINDLIB_ERROR_BUFFER_NULL;

  unsigned int buffer_length= 0;
  int rval;
  unsigned int row_number= 1;

  ino_t inode_list[PGFINDLIB_MAX_INODE_COUNT]; /* todo: disable if duplicate checking is off */
  unsigned int inode_count= 0;
  unsigned int inode_warning_count= 0;

  char *malloc_buffer_1= NULL;
  char **malloc_buffer_2= NULL;

#if (PGFINDLIB_INCLUDE_ROW_VERSION != 0)
  rval= pgfindlib_row_version(buffer, &buffer_length, buffer_max_length, &row_number, inode_list, &inode_count); /* first row including version number */
  if (rval != PGFINDLIB_OK) PGFINDLIB_FREE_AND_RETURN;
#endif

  int program_e_machine;
  char lib[PGFINDLIB_MAX_PATH_LENGTH]= ""; /* Usually this will be changed to whatever $LIB is. */
  char platform[PGFINDLIB_MAX_PATH_LENGTH]= ""; /* Usually this will be changed to whatever $LIB is. */
  char origin[PGFINDLIB_MAX_PATH_LENGTH]= ""; /* Usually this will be changed to whatever $LIB is. */

  rval= pgfindlib_get_origin_and_lib_and_platform(origin, lib, platform, buffer, &buffer_length, buffer_max_length,
                                                  &program_e_machine, &row_number, inode_list, &inode_count);
  if (rval != PGFINDLIB_OK) PGFINDLIB_FREE_AND_RETURN;

#if (PGFINDLIB_INCLUDE_ROW_LIB != 0)
  {
    rval= pgfindlib_row_lib(buffer, &buffer_length, buffer_max_length, &row_number, inode_list, &inode_count, lib, platform, origin);
    if (rval != PGFINDLIB_OK) PGFINDLIB_FREE_AND_RETURN
  }
#endif

  /* Put together the list of sources and sonames from the FROM and WHERE of the input. */
  /* MAX_TOKENS_COUNT is fixed but more than twice the number of official tokeners */
  struct tokener tokener_list[PGFINDLIB_MAX_TOKENS_COUNT];
  {
    rval= pgfindlib_tokenize(statement, tokener_list, &row_number, inode_list, &inode_count, buffer, &buffer_length, buffer_max_length);
    if (rval != PGFINDLIB_OK) PGFINDLIB_FREE_AND_RETURN
  }
  unsigned int rpath_or_runpath_count= 0;
  for (int i= 0; tokener_list[i].tokener_comment_id != PGFINDLIB_TOKEN_END; ++i)
  {
    if ((tokener_list[i].tokener_comment_id == PGFINDLIB_TOKEN_SOURCE_DT_RPATH)
     || (tokener_list[i].tokener_comment_id == PGFINDLIB_TOKEN_SOURCE_DT_RUNPATH))
      ++rpath_or_runpath_count;
  }

  pgfindlib_read_elf(tokener_list, "/tmp/pgfindlib_tests/test", 2, 1); 
  /* pgfindlib_read_elf(tokener_list, "/home/pgulutzan/pgfindlib/main", 1, 1); */
  /* pgfindlib_read_elf(tokener_list, "/lib32/libnsl.so.1", 1, 1); */
  
/* Preparation if DT_RPATH or DT_RUNPATH */
#if (PGFINDLIB_TOKEN_SOURCE_DT_RPATH_OR_DT_RUNPATH != 0)
  const ElfW(Dyn) *dynamic= _DYNAMIC;
  const ElfW(Dyn) *dt_rpath= NULL;
  const ElfW(Dyn) *dt_runpath= NULL;
  const char *dt_strtab= NULL;
  if (rpath_or_runpath_count > 0)
  {
    /* in theory "extern __attribute__((weak)) ... _DYNAMIC[];" could result in _DYNAMIC == NULL */
    if (_DYNAMIC == NULL)
    {
#if (PGFINDLIB_COMMENT_CANNOT_READ_RPATH != 0)
      rval= pgfindlib_comment_is_row("Cannot read DT_RPATH because _DYNAMIC is NULL",
                                     PGFINDLIB_COMMENT_CANNOT_READ_RPATH,
                                     buffer, &buffer_length, buffer_max_length, &row_number, inode_list, &inode_count);
      if (rval != PGFINDLIB_OK) PGFINDLIB_FREE_AND_RETURN
#endif
    }
    else /* _DYNAMIC != NULL */
    {
      while (dynamic->d_tag != DT_NULL)
      {
        if (dynamic->d_tag == DT_RPATH) dt_rpath= dynamic;
        if (dynamic->d_tag == DT_RUNPATH) dt_runpath= dynamic;
        if (dynamic->d_tag == DT_STRTAB) dt_strtab= (const char *)dynamic->d_un.d_val;
        ++dynamic;
      }
    }
  }
  /* So now we have dt_rpath and dt_runpath */
#endif

  /* Go through the list of sources and add to the lists: source# length pointer-to-path */

  unsigned int malloc_buffer_1_length; unsigned int malloc_buffer_1_max_length;
  unsigned int malloc_buffer_2_length; unsigned int malloc_buffer_2_max_length;

  malloc_buffer_1_max_length= 1000;
  malloc_buffer_2_max_length= 100;

repeat_malloc:
  malloc_buffer_1_length= 0;
  malloc_buffer_1= (char *)malloc(malloc_buffer_1_max_length);
  if (malloc_buffer_1 == NULL) { rval= PGFINDLIB_MALLOC_BUFFER_1_OVERFLOW; PGFINDLIB_FREE_AND_RETURN }
  malloc_buffer_2_length= 0;
  malloc_buffer_2= (char **)malloc(malloc_buffer_2_max_length * sizeof(char *));
  if (malloc_buffer_2 == NULL) { rval= PGFINDLIB_MALLOC_BUFFER_2_OVERFLOW; PGFINDLIB_FREE_AND_RETURN }

  for (unsigned int tokener_number= 0; ; ++tokener_number) /* for each source in source name list */
  {
    int comment_number;
    comment_number= tokener_list[tokener_number].tokener_comment_id;
    if (comment_number == PGFINDLIB_TOKEN_END) break;
    if ((comment_number < PGFINDLIB_TOKEN_SOURCE_LD_AUDIT) || (comment_number > PGFINDLIB_TOKEN_SOURCE_NONSTANDARD)) continue;
    char tmp_source_name[PGFINDLIB_MAX_TOKEN_LENGTH + 1]= ""; /* only needed momentarily for nonstandard sources */
    const char *ld= NULL;
    if (comment_number == PGFINDLIB_TOKEN_SOURCE_DT_RPATH)
    {
      if ((dt_strtab == NULL) || (dt_rpath == NULL)) continue;
      ld= dt_strtab + dt_rpath->d_un.d_val;
    }
    else if (comment_number == PGFINDLIB_TOKEN_SOURCE_DT_RUNPATH)
    {
      if ((dt_strtab == NULL) || (dt_runpath == NULL)) continue;
      ld= dt_strtab + dt_runpath->d_un.d_val;
    }
    else if (comment_number == PGFINDLIB_TOKEN_SOURCE_DEFAULT_PATHS)
    {
      ld= "/lib:/lib64:/usr/lib:/usr/lib64"; /* "lib64" is platform-dependent but dunno which platform */
    }
    else if (comment_number == PGFINDLIB_TOKEN_SOURCE_NONSTANDARD)
    {
      int tmp_source_name_length= tokener_list[tokener_number].tokener_length;
      memcpy(tmp_source_name, tokener_list[tokener_number].tokener_name, tmp_source_name_length);
      tmp_source_name[tmp_source_name_length]= '\0';
      ld= tmp_source_name;
    }
    else if (comment_number != PGFINDLIB_TOKEN_SOURCE_LD_SO_CACHE)
    {
      /* Not DT_RPATH | DT_RUNPATH | default_paths | nonstandard | ld_so_cache */
      /* So it must be LD_AUDIT | LD_PRELOAD | LD_LIBRARY_PATH | LD_RUN_PATH LD_PGFINDLIB_PATH */
      int tmp_source_name_length= tokener_list[tokener_number].tokener_length;
      memcpy(tmp_source_name, tokener_list[tokener_number].tokener_name, tmp_source_name_length);
      tmp_source_name[tmp_source_name_length]= '\0';
      ld= getenv(tmp_source_name);
    }
    if (comment_number == PGFINDLIB_TOKEN_SOURCE_LD_SO_CACHE)
    {
      rval= pgfindlib_so_cache(tokener_list, tokener_number,
                               malloc_buffer_1, &malloc_buffer_1_length, malloc_buffer_1_max_length,
                               malloc_buffer_2, &malloc_buffer_2_length, malloc_buffer_2_max_length);
    }
    else
    {
      rval= pgfindlib_source_scan(ld, buffer, &buffer_length, tokener_number,
                                 buffer_max_length, lib, platform, origin,
                                 &row_number,
                                 inode_list, &inode_count, &inode_warning_count,
                                 tokener_list,
                                 malloc_buffer_1, &malloc_buffer_1_length, malloc_buffer_1_max_length,
                                 malloc_buffer_2, &malloc_buffer_2_length, malloc_buffer_2_max_length,
                                 program_e_machine);
    }

    if (rval == PGFINDLIB_MALLOC_BUFFER_1_OVERFLOW)
    {
      free(malloc_buffer_1); malloc_buffer_1= NULL;
      free(malloc_buffer_2); malloc_buffer_2= NULL;
      malloc_buffer_1_max_length+= 1000;
      goto repeat_malloc;
    }
    if (rval == PGFINDLIB_MALLOC_BUFFER_2_OVERFLOW)
    {
      free(malloc_buffer_1); malloc_buffer_1= NULL;
      free(malloc_buffer_2); malloc_buffer_2= NULL;
      malloc_buffer_2_max_length+= 1000;
      goto repeat_malloc;
    }

    if (rval != PGFINDLIB_OK) PGFINDLIB_FREE_AND_RETURN

  }

  qsort(malloc_buffer_2, malloc_buffer_2_length, sizeof(char *), pgfindlib_qsort_compare);

  /*
    Phase 1 complete. At this point, we seem to have a sorted list of all the paths.
    In Phase 2, we must dump the paths into the output buffer along with the warnings.
    todo: malloc an inode_list about equal to malloc_buffer_2 i.e. # of rows (but more needed for comments I guess)
          the problem is that even comments look at node, and they could come before this
    todo: with a large number of sources, > 127 - 32, and signed char, the sort order might become wrong
  */
  rval= PGFINDLIB_OK;
#if (PGFINDLIB_INCLUDE_ROW_SOURCE_NAME == 1)
  int token_number_of_last_source= 0;
#endif
  for (unsigned int i= 0; i < malloc_buffer_2_length; ++i)
  {
    const char *item= malloc_buffer_2[i];
    /* First char is source number + 32 */
    int token_number_of_source= *item - 32;
#if (PGFINDLIB_INCLUDE_ROW_SOURCE_NAME == 1)
    for (unsigned int j= token_number_of_last_source;; ++j)
    {
      unsigned short int type= tokener_list[j].tokener_comment_id;
      if (type == PGFINDLIB_TOKEN_END) break;
      if ((type < PGFINDLIB_TOKEN_SOURCE_LD_AUDIT) || (type > PGFINDLIB_TOKEN_SOURCE_NONSTANDARD)) continue;
      if (j > token_number_of_source) break;
      if ((j > token_number_of_last_source) && (j <= token_number_of_source))
      {
        char source_name[64];
        unsigned short int len= tokener_list[j].tokener_length;
        if (len > 64 - 1) len= 64 - 1;
        memcpy(source_name, tokener_list[j].tokener_name, len);
        source_name[len]= '\0';
        rval= pgfindlib_row_source_name(buffer, &buffer_length, buffer_max_length, &row_number,
                             inode_list, &inode_count, source_name);
        if (rval != PGFINDLIB_OK) break;
      }
    }
    if (rval != PGFINDLIB_OK) break;
    token_number_of_last_source= token_number_of_source + 1;
#endif
    rval= pgfindlib_file(buffer, &buffer_length, item + 1, buffer_max_length, &row_number,
                       inode_list, &inode_count, &inode_warning_count, tokener_list[token_number_of_source], program_e_machine);
    if (rval != PGFINDLIB_OK) break;
  }
#if (PGFINDLIB_INCLUDE_ROW_SOURCE_NAME == 1)
  if (rval == PGFINDLIB_OK)
  {
    /* todo: repetitious */
    for (unsigned int j= token_number_of_last_source;; ++j)
    {
      unsigned short int type= tokener_list[j].tokener_comment_id;
      if (type == PGFINDLIB_TOKEN_END) break;
      if ((type < PGFINDLIB_TOKEN_SOURCE_LD_AUDIT) || (type > PGFINDLIB_TOKEN_SOURCE_NONSTANDARD)) continue;
      {
        char source_name[64];
        unsigned short int len= tokener_list[j].tokener_length;
        if (len > 64 - 1) len= 64 - 1;
        memcpy(source_name, tokener_list[j].tokener_name, len);
        source_name[len]= '\0';
        rval= pgfindlib_row_source_name(buffer, &buffer_length, buffer_max_length, &row_number,
                             inode_list, &inode_count, source_name);
        if (rval != PGFINDLIB_OK) break;
      }
    }
  }
#endif
/* free_and_return: */
  if (malloc_buffer_1 != NULL) { free(malloc_buffer_1); }
  if (malloc_buffer_2 != NULL) { free(malloc_buffer_2); }
  
  return rval;
}

int pgfindlib_strcat(char *buffer, unsigned int *buffer_length, const char *line, unsigned int buffer_max_length)
{
  unsigned int line_length= strlen(line);
  const char *pointer_to_line= line;
  while (*pointer_to_line == ' ') {++pointer_to_line; --line_length; } /* skip lead spaces */
  while ((line_length > 0) && (*(pointer_to_line + line_length - 1) == ' ')) --line_length; /* skip trail spaces */
  if (*buffer_length + line_length > buffer_max_length) return PGFINDLIB_ERROR_BUFFER_MAX_LENGTH_TOO_SMALL;
  memcpy(buffer + *buffer_length, pointer_to_line, line_length);
  *buffer_length+= line_length;
  *(buffer + *buffer_length)= '\0';
  return PGFINDLIB_OK;
}

/*
  Compare line to each of the items in statement. If match for number-of-characters-in-soname: 1 true. Else: 0 false.
  Assume that line cannot contain ':' and ':' within statement is a delimiter. (Actually so is ',' the way tokenize works.)
  Ignore lead or trail spaces.
  Treat \n as end of line and ignore it.
  We have already checked that strlen(each soname) is <= PGFINDLIB_MAX_PATH_LENGTH.
  Beware: ldconfig -p lines start with a control character (tab?).
*/
int pgfindlib_find_line_in_statement(const struct tokener tokener_list[], const char *line)
{
  const char *pointer_to_line= line;
  while (*pointer_to_line <= ' ') ++pointer_to_line; /* skip lead spaces (or control characters!) in line */
  unsigned int line_length= strlen(pointer_to_line);
  if ((line_length > 1) && (pointer_to_line[line_length - 1] == '\n')) --line_length; /* skip trail \n */
  while ((line_length > 0) && (pointer_to_line[line_length - 1] == ' ')) --line_length; /* skip trail spaces */
  if (line_length == 0) return 0; /* false */
  for (unsigned int tokener_number= 0; ; ++tokener_number) /* for each source in source name list */
  {
    int comment_number;
    comment_number= tokener_list[tokener_number].tokener_comment_id;
    if (comment_number == PGFINDLIB_TOKEN_END) break;
    if (comment_number != PGFINDLIB_TOKEN_FILE) continue;
    const char *pointer_to_statement= tokener_list[tokener_number].tokener_name;
    unsigned soname_length= tokener_list[tokener_number].tokener_length;
    if (memcmp(pointer_to_line, pointer_to_statement, soname_length) == 0)
    {
      return 1; /* true */
    }
  }
  return 0; /* false (though actually we shouldn't get this far) */
}

/*
  Pass: library or file name
  Do: replace with same library or file name, except that $ORIGIN or $LIB or $PLATFORM is replaced
  Return: rval
  Todo: It seems loader replaces $LIB or $LIB/ but not $LIB in $LIBxxx, this replaces $LIB in $LIBxxx too.
  Todo: https://bugs.freebsd.org/bugzilla/show_bug.cgi?id=187114 suggests no need to check DF_ORIGIN flag nowadays
        but that hasn't been tested
*/
int pgfindlib_replace_lib_or_platform_or_origin(char *one_library_or_file, unsigned int *replacements_count, const char *lib, const char *platform, const char *origin)
{
  *replacements_count= 0;
  if (strchr(one_library_or_file, '$') == NULL) return PGFINDLIB_OK;
  char buffer_for_output[PGFINDLIB_MAX_PATH_LENGTH*2 + 1];
  char *p_line_out= &buffer_for_output[0];
  *p_line_out= '\0';
  for (const char *p_line_in= one_library_or_file; *p_line_in != '\0';)
  {
    if ((strncmp(p_line_in, "$ORIGIN", 7) == 0) || (strncmp(p_line_in, "${ORIGIN}", 9) == 0))
    {
      strcpy(p_line_out, origin);
      p_line_out+= strlen(origin);
      if (*(p_line_in + 1) == '{') p_line_in+= 9 - 7;
      p_line_in+= 7;
      ++*replacements_count;
    }
    else if ((strncmp(p_line_in, "$LIB", 4) == 0) || (strncmp(p_line_in, "${LIB}", 6) == 0))
    {
      strcpy(p_line_out, lib);
      p_line_out+= strlen(lib);
      if (*(p_line_in + 1) == '{') p_line_in+= 6 - 4;
      p_line_in+= 4;
      ++*replacements_count;
    }
    else if ((strncmp(p_line_in, "$PLATFORM", 9) == 0) || (strncmp(p_line_in, "${PLATFORM}", 11) == 0))
    {
      strcpy(p_line_out, platform);
      p_line_out+= strlen(platform);
      if (*(p_line_in + 1) == '{') p_line_in+= 11 - 9;
      p_line_in+= 9;
      ++*replacements_count;
    }
    else
    {
      *p_line_out= *p_line_in;
      ++p_line_out;
      ++p_line_in;
    }
    if ((p_line_out - &buffer_for_output[0]) > PGFINDLIB_MAX_PATH_LENGTH)
      return PGFINDLIB_ERROR_MAX_PATH_LENGTH_TOO_SMALL;
    *p_line_out= '\0';
  }
  strcpy(one_library_or_file, buffer_for_output);
  return PGFINDLIB_OK;
}

/*
  Return: what would loader change "$LIB" or "$PLATFORM" or "$ORIGIN" to
  Re method: Use a dummy utility, preferably bin/true, ls should also work but isn't as good
            (any program anywhere will work provided it requires any .so, and libc.so is such).
            First attempt: read ELF to get "ELF interpreter" i.e. loader name,
            which probably is /lib64/ld-linux-x86-64.so.2 or /lib/ld-linux.so.2,
            then use it with LD_DEBUG to see search_path when executing the dummy utility.
            If that fails: same idea but just running the dummy.
            If that fails: (lib) lib64 for 64-bit, lib for 32-bit. (platform) uname -m.
            FreeBSD is different.
  Todo: get PT_DYNAMIC the way we get PT_INTERN, instead of depending on extern _DYNAMIC (but this is hard, see read_elf()
  Todo: If there is a /tls (thread local storage) subdirectory or an x86_64 subdirectory,
        search all. But show library search path=main, main-tls, main/x86_64
  Todo: If dynamic loader is not the usual e.g. due to "-Wl,-I/tmp/my_ld.so" then add a comment.
*/
int pgfindlib_get_origin_and_lib_and_platform(char *origin, char *lib, char *platform,
                                              char *buffer, unsigned int *buffer_length, unsigned int buffer_max_length,
                                              int *program_e_machine, unsigned int *row_number,
                                              ino_t inode_list[], unsigned int *inode_count)
{
  int platform_change_count= 0;
  int rval= PGFINDLIB_OK;
  int lib_change_count= 0;

#ifdef PGFINDLIB_FREEBSD
  int aux_info_return= elf_aux_info(AT_EXECPATH, origin, PGFINDLIB_MAX_PATH_LENGTH);
  if (aux_info_return != 0)
  {
#if (PGFINDLIB_COMMENT_ELF_AUX_INFO_FAILED != 0)
    rval= pgfindlib_comment_is_row("elf_aux_info failed so $ORIGIN is unknown",
                                   PGFINDLIB_COMMENT_ELF_AUX_INFO_FAILED,
                                   buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
    if (rval != PGFINDLIB_OK) return rval;
#endif
    strcpy(origin, "");
  }
  strcpy(lib, "lib");
  /* platform should be set after the if/else/endif */
#else
  {
    int readlink_return;
    readlink_return= readlink("/proc/self/exe", origin, PGFINDLIB_MAX_PATH_LENGTH);
    if ((readlink_return < 0) || (readlink_return >= PGFINDLIB_MAX_PATH_LENGTH))
    {
#if (PGFINDLIB_COMMENT_READLINK_FAILED != 0)
      rval= pgfindlib_comment_is_row("readlink failed so $ORIGIN is unknown",
                         PGFINDLIB_COMMENT_READLINK_FAILED,
                         buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
      if (rval != PGFINDLIB_OK) return rval;
#endif
      strcpy(origin, "");
    }
    else *(origin + readlink_return)= '\0';
    for (char *p= origin + strlen(origin); p != origin; --p)
    {
      if (*p == '/')
      {
        *p= '\0';
        break;
      }
    }
  }
#endif /* ifdef PGFINDLIB_FREEBSD */

#if (PGFINDLIB_IF_GET_LIB_OR_PLATFORM != 0)
  /* utility name */
  /* FreeBSD probably won't have /bin/true, it seems to be a Linux thing, but maybe it will have id */
  char utility_name[256]= ""; /* todo: change to const char * */
  if (access("/bin/true", X_OK) == 0) strcpy(utility_name, "/bin/true");
  else if (access("/bin/cp", X_OK) == 0) strcpy(utility_name, "/bin/cp");
  else if (access("/usr/bin/true", X_OK) == 0) strcpy(utility_name, "/usr/bin/true");
  else if (access("/usr/bin/cp", X_OK) == 0) strcpy(utility_name, "/usr/bin/cp");
  else if (access("/bin/id", X_OK) == 0) strcpy(utility_name, "/bin/id");
  else if (access("/usr/bin/id", X_OK) == 0) strcpy(utility_name, "/usr/bin/id");
#if (PGFINDLIB_COMMENT_NO_TRUE_OR_CP != 0)
  if (strcmp(utility_name, "") == 0)
  {
    rval= pgfindlib_comment_is_row("no access to [/usr]/bin/true or [/usr]/bin/cp or /bin/id",
                         PGFINDLIB_COMMENT_NO_TRUE_OR_CP,
                         buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
    if (rval != PGFINDLIB_OK) return rval;
  }
#endif

/* todo: move this up, and if it's zilch then we cannot read elf here */
/* This was void* but that triggered -Warray-bounds during an rpm build. */
extern ElfW(Ehdr) __executable_start;
  ElfW(Ehdr)*ehdr= NULL;
  ehdr= (ElfW(Ehdr)*) &__executable_start; /* linker e.g. ld.bfd or ld.lld is supposed to add this */
  if (ehdr != NULL)
  {

    if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0)
    {
#if (PGFINDLIB_COMMENT_EHDR_IDENT != 0)
      rval= pgfindlib_comment_is_row("ehdr->ident not valid",
                         PGFINDLIB_COMMENT_EHDR_IDENT,
                         buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
      if (rval != PGFINDLIB_OK) return rval;
#endif
      ehdr= NULL;
    }
  }

  if (ehdr != NULL)
  {
    *program_e_machine= ehdr->e_machine;
  }
  else *program_e_machine= 0;
  
  /* first attempt */

  const char *dynamic_loader_name= NULL;
  if (ehdr != NULL)
  {
    const char *cc= (char *)ehdr; /* offsets are in bytes so I prefer to use a byte pointer */
    cc+= ehdr->e_phoff; /* -> start of program headers */
    ElfW(Phdr)*phdr;
    for (unsigned int i= 0; i < ehdr->e_phnum; ++i) /* loop through program headers */
    {
      phdr= (ElfW(Phdr)*)cc;
      if (phdr->p_type == PT_INTERP) /* i.e. ELF interpreter */
      {
        char *cc2= (char *)ehdr;
        cc2+= phdr->p_offset;
        dynamic_loader_name= cc2;
        break;
      }
      cc+= ehdr->e_phentsize;
    }
  }

  if (dynamic_loader_name == NULL)
  {
    if ((sizeof(void*)) == 8) dynamic_loader_name= "/lib64/ld-linux-x86-64.so.2"; /* make some gcc/glibc assumptions */
    else dynamic_loader_name= "/lib/ld-linux.so.2";
#if (PGFINDLIB_COMMENT_CANT_FIND_DYNAMIC_LOADER != 0)
    char comment[512];
    sprintf(comment, "can't get ehdr dynamic loader so assume %s", dynamic_loader_name);
    rval= pgfindlib_comment_is_row(comment,
                         PGFINDLIB_COMMENT_CANT_FIND_DYNAMIC_LOADER,
                         buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
    if (rval != PGFINDLIB_OK) return rval;
#endif
  }

  if (access(dynamic_loader_name, X_OK) != 0)
  {
#if (PGFINDLIB_COMMENT_CANT_ACCESS_DYNAMIC_LOADER != 0)
    char comment[512];
    sprintf(comment, "can't access %s", dynamic_loader_name);
    rval= pgfindlib_comment_is_row(comment,
                         PGFINDLIB_COMMENT_CANT_ACCESS_DYNAMIC_LOADER,
                         buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
    if (rval != PGFINDLIB_OK) return rval;
#endif
    dynamic_loader_name= NULL;
  }

  if (dynamic_loader_name != NULL)
  {
#define REPLACEE_IS_LIB 0
#define REPLACEE_IS_PLATFORM 1
    for (int i= REPLACEE_IS_LIB; i <= REPLACEE_IS_PLATFORM; ++i) /* 0 means "$LIB", 1 means "$PLATFORM" */
    {
      char replacee[16];
      if (i == REPLACEE_IS_LIB) strcpy(replacee, "$LIB");
      else strcpy(replacee, "$PLATFORM");
      int change_count= 0;
      FILE *fp;
      char popen_arg[PGFINDLIB_MAX_PATH_LENGTH + 1];
      sprintf(popen_arg,
      "env -u LD_DEBUG_OUTPUT LD_LIBRARY_PATH='/PRE_OOKPIK/%s/POST_OOKPIK' LD_DEBUG=libs %s --inhibit-cache %s 2>/dev/stdout",
      replacee, dynamic_loader_name, utility_name);
      fp= popen(popen_arg, "r");
      if (fp != NULL)
      {
        char buffer_for_ookpik[PGFINDLIB_MAX_PATH_LENGTH + 1];
        while (fgets(buffer_for_ookpik, sizeof(buffer_for_ookpik), fp) != NULL)
        {
          const char *pre_ookpik= strstr(buffer_for_ookpik, "PRE_OOKPIK/");
          if (pre_ookpik == NULL) continue;
          const char *post_ookpik= strstr(pre_ookpik, "POST_OOKPIK");
          if (post_ookpik == NULL) continue;
          pre_ookpik+= strlen("PRE_OOKPIK/");
          unsigned int len= post_ookpik - (pre_ookpik + 1);
          if (i == REPLACEE_IS_LIB) { memcpy(lib, pre_ookpik, len); *(lib + len)= '\0'; ++lib_change_count; }
          else { memcpy(platform, pre_ookpik, len); *(platform + len)= '\0'; ++platform_change_count; }
          ++change_count;
          break;
        }
        pclose(fp);
      }
      /* second attempt */
      if (change_count == 0)
      {
        sprintf(popen_arg,
        "env -u LD_DEBUG_OUTPUT LD_LIBRARY_PATH='/PRE_OOKPIK/%s/POST_OOKPIK' LD_DEBUG=libs %s 2>/dev/stdout",
        replacee, utility_name);
        fp= popen(popen_arg, "r");
        if (fp != NULL)
        {
          char buffer_for_ookpik[PGFINDLIB_MAX_PATH_LENGTH + 1];
          while (fgets(buffer_for_ookpik, sizeof(buffer_for_ookpik), fp) != NULL)
          {
            const char *pre_ookpik= strstr(buffer_for_ookpik, "PRE_OOKPIK/");
            if (pre_ookpik == NULL) continue;
            const char *post_ookpik= strstr(pre_ookpik, "POST_OOKPIK");
            if (post_ookpik == NULL) continue;
            pre_ookpik+= strlen("PRE_OOKPIK/");
            unsigned int len= post_ookpik - (pre_ookpik + 1);
            if (i == REPLACEE_IS_LIB) { memcpy(lib, pre_ookpik, len); *(lib + len)= '\0'; ++lib_change_count; }
            else { memcpy(platform, pre_ookpik, len); *(platform + len)= '\0'; ++platform_change_count; }
            break;
          }
          pclose(fp);
        }
      }
    }
  }
#endif /* if (PGFINDLIB_IF_GET_LIB_OR_PLATFORM != 0) */
  if (lib_change_count == 0)
  {
    if ((sizeof(void*)) == 8) strcpy(lib, "lib64"); /* default $LIB if pgfindlib__get_lib_or_platform doesn't work */
    else strcpy(lib, "lib");
#if (PGFINDLIB_COMMENT_ASSUMING_LIB != 0)
    char comment[5000];
    sprintf(comment, "assuming $LIB is %s", lib);
    rval= pgfindlib_comment_is_row(comment,
                         PGFINDLIB_COMMENT_ASSUMING_LIB,
                         buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
    if (rval != PGFINDLIB_OK) return rval;
#endif
  }

  if (platform_change_count == 0)
  {
    char buffer_for_replacement[PGFINDLIB_MAX_PATH_LENGTH + 1]= "?";
    FILE *fp= popen("LD_LIBRARY_PATH= LD_DEBUG= LD_PRELOAD= uname -m 2>/dev/null", "r");
    if (fp != NULL)
    {
      if (fgets(buffer_for_replacement, sizeof(buffer_for_replacement), fp) == NULL)
        {;}
      pclose(fp);
    }
#if (PGFINDLIB_COMMENT_UNAME_FAILED != 0)
    if (strcmp(buffer_for_replacement, "?") == 0)
    {
      rval= pgfindlib_comment_is_row("uname -m failed",
                         PGFINDLIB_COMMENT_UNAME_FAILED,
                         buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
      if (rval != PGFINDLIB_OK) return rval;
    }
#endif
    char *pointer_to_n= strchr(buffer_for_replacement, '\n');
    if (pointer_to_n != NULL) *pointer_to_n= '\0';
    strcpy(platform, buffer_for_replacement);
#if (PGFINDLIB_COMMENT_ASSUMING_PLATFORM != 0)
    char comment[5000];
    sprintf(comment, "assuming $PLATFORM is %s", platform);
    rval= pgfindlib_comment_is_row(comment,
                         PGFINDLIB_COMMENT_ASSUMING_PLATFORM,
                         buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
    if (rval != PGFINDLIB_OK) return rval;
#endif
  }
  return rval; /* which is doubtless PGFINDLIB_OK */
}

/*
 *For a comment column
  Put column of row in buffer.
  All warnings and comments must have form: 3-decimal-digit-number text delimiter
  The number should never change, the text should rarely change.
  Todo: flag to ignore column because column_number
        flag to exclude text and only show number
        limit text length
        enclose in ''s or ""s if contains delimiter
*/
int pgfindlib_comment_in_row(char *comment, unsigned int comment_number, int additional_number)
{
  const char *text;
  if (comment_number == PGFINDLIB_COMMENT_DUPLICATE) sprintf(comment, "%03d %s %d", comment_number, "duplicate of", additional_number);
  else
  {
    if (comment_number == PGFINDLIB_COMMENT_ACCESS_FAILED) text= "access(filename, R_OK) failed";
    if (comment_number == PGFINDLIB_COMMENT_LSTAT_FAILED) text= "lstat(filename) failed";
    if (comment_number == PGFINDLIB_COMMENT_SYMLINK) text= "symlink";
    if (comment_number == PGFINDLIB_COMMENT_MAX_INODE_COUNT_TOO_SMALL) text= "MAX_INODE_COUNT is too small";
    if (comment_number == PGFINDLIB_COMMENT_ELF_OPEN_FAILED) text= "elf open failed";
    if (comment_number == PGFINDLIB_COMMENT_ELF_READ_FAILED) text= "elf read failed";
    if (comment_number == PGFINDLIB_COMMENT_ELF_HAS_INVALID_IDENT) text= "elf has invalid ident";
    if (comment_number == PGFINDLIB_COMMENT_ELF_IS_NOT_EXEC_OR_DYN) text= "elf is not exec or dyn";
    if (comment_number == PGFINDLIB_COMMENT_ELF_SHT_DYNAMIC_NOT_FOUND) text= "elf sht_dynamic not found";
    if (comment_number == PGFINDLIB_COMMENT_ELF_MACHINE_DOES_NOT_MATCH) text= "elf machine does not match";
    sprintf(comment, "%03d %s", comment_number, text);
  }
  return PGFINDLIB_OK;
}

/* This can be called when a warning|error will be the only item in the row, e.g. when about to return syntax error
   But notice the fixed size of comment_with_number, don't call unless it's certain that the message is short
*/
int pgfindlib_comment_is_row(const char *comment, unsigned int comment_number,
                             char *buffer, unsigned int *buffer_length, unsigned int buffer_max_length, unsigned int *row_number,
                             ino_t inode_list[], unsigned int *inode_count)
{
  if (*inode_count != PGFINDLIB_MAX_INODE_COUNT) { inode_list[*inode_count]= -1; ++*inode_count; }
  const char *columns_list[MAX_COLUMNS_PER_ROW];  
  for (int i= 0; i < MAX_COLUMNS_PER_ROW; ++i) columns_list[i]= "";
  char comment_with_number[256];
  sprintf(comment_with_number, "%03d %s", comment_number, comment);
  columns_list[COLUMN_FOR_COMMENT_1]= comment_with_number;
  return pgfindlib_row_bottom_level(buffer, buffer_length, buffer_max_length, row_number, columns_list);
}

/*
  called "bottom level" because ultimately all pgfindlib_row_* functions should call here
  todo: move overflow check to here
*/
int pgfindlib_row_bottom_level(char *buffer, unsigned int *buffer_length, unsigned int buffer_max_length, unsigned int *row_number, 
                               const char *columns_list[])
{
  int rval;
  char row_number_string[8];
  unsigned int buffer_length_save= *buffer_length;
  for (int i= 0; i < MAX_COLUMNS_PER_ROW; ++i)
  {
    if (i == COLUMN_FOR_ROW_NUMBER)
    {
      sprintf(row_number_string, "%d", *row_number);
      rval= pgfindlib_strcat(buffer, buffer_length, row_number_string, buffer_max_length);
    }
    else rval= pgfindlib_strcat(buffer, buffer_length, columns_list[i], buffer_max_length);
    if (rval != PGFINDLIB_OK) goto overflow;
    rval= pgfindlib_strcat(buffer, buffer_length, PGFINDLIB_COLUMN_DELIMITER, buffer_max_length); /* ":" */
    if (rval != PGFINDLIB_OK) goto overflow;
  }
  rval= pgfindlib_strcat(buffer, buffer_length, PGFINDLIB_ROW_DELIMITER, buffer_max_length); /* "\n" */
  if (rval != PGFINDLIB_OK) goto overflow;
  ++*row_number;
  return rval;
overflow:
/* todo: this should be a row and there should be a guarantee that it will fit i.e. the regular strcat check is buffer_length - what's needed for overflow message */
  *buffer_length= buffer_length_save;
  pgfindlib_strcat(buffer, buffer_length, "OVFLW", buffer_max_length);
  return rval; /* i.e. return the rval that caused overflow */
}

static int pgfindlib_row_version(char *buffer, unsigned int *buffer_length, unsigned buffer_max_length, unsigned int *row_number, ino_t inode_list[], unsigned int *inode_count)
{
  if (*inode_count != PGFINDLIB_MAX_INODE_COUNT) { inode_list[*inode_count]= -1; ++*inode_count; }
  char row_program[64];
  sprintf(row_program, "%03d pgfindlib", PGFINDLIB_COMMENT_PGFINDLIB);
  char row_version[64];
  sprintf(row_version, "%03d version %d.%d.%d", PGFINDLIB_COMMENT_VERSION, PGFINDLIB_VERSION_MAJOR, PGFINDLIB_VERSION_MINOR, PGFINDLIB_VERSION_PATCH);
  char row_url[64];
  sprintf(row_url, "%03d https://github.com/pgulutzan/pgfindlib", PGFINDLIB_COMMENT_URL);
  const char *columns_list[MAX_COLUMNS_PER_ROW];
  for (int i= 0; i < MAX_COLUMNS_PER_ROW; ++i) columns_list[i]= "";
  columns_list[COLUMN_FOR_COMMENT_1]= row_program;
  columns_list[COLUMN_FOR_COMMENT_2]= row_version;
  columns_list[COLUMN_FOR_COMMENT_3]= row_url;
  return pgfindlib_row_bottom_level(buffer, buffer_length, buffer_max_length, row_number, columns_list);
}

int pgfindlib_row_lib(char *buffer, unsigned int *buffer_length, unsigned buffer_max_length, unsigned int *row_number,
                      ino_t inode_list[], unsigned int *inode_count,
                      const char *lib, const char *platform, const char *origin)
{
  if (*inode_count != PGFINDLIB_MAX_INODE_COUNT) { inode_list[*inode_count]= -1; ++*inode_count; }
  char column_lib[PGFINDLIB_MAX_PATH_LENGTH + 100];
  char column_platform[PGFINDLIB_MAX_PATH_LENGTH + 100];
  char column_origin[PGFINDLIB_MAX_PATH_LENGTH + 100];
  sprintf(column_lib, "%03d $LIB=%s", PGFINDLIB_COMMENT_LIB_STRING, lib);
  sprintf(column_platform, "%03d $PLATFORM=%s", PGFINDLIB_COMMENT_PLATFORM_STRING, platform);
  sprintf(column_origin, "%03d $ORIGIN=%s", PGFINDLIB_COMMENT_ORIGIN_STRING, origin);
  const char *columns_list[MAX_COLUMNS_PER_ROW];
  for (int i= 0; i < MAX_COLUMNS_PER_ROW; ++i) columns_list[i]= "";
  columns_list[COLUMN_FOR_COMMENT_1]= column_lib;
  columns_list[COLUMN_FOR_COMMENT_2]= column_platform;
  columns_list[COLUMN_FOR_COMMENT_3]= column_origin;
  return pgfindlib_row_bottom_level(buffer, buffer_length, buffer_max_length, row_number, columns_list);
}

/* Dump source name as a row-level comment. By default PGFINDLIB_INCLUDE_ROW_SOURCE_NAME == 0 so this is disabled */
int pgfindlib_row_source_name(char *buffer, unsigned int *buffer_length, unsigned buffer_max_length, unsigned int *row_number,
                             ino_t inode_list[], unsigned int *inode_count,
                             const char *source_name)
{
#if (PGFINDLIB_INCLUDE_ROW_SOURCE_NAME == 1)
  if (*inode_count != PGFINDLIB_MAX_INODE_COUNT) { inode_list[*inode_count]= -1; ++*inode_count; }
  const char *columns_list[MAX_COLUMNS_PER_ROW];
  for (int i= 0; i < MAX_COLUMNS_PER_ROW; ++i) columns_list[i]= "";
  columns_list[COLUMN_FOR_SOURCE]= source_name;
  return pgfindlib_row_bottom_level(buffer, buffer_length, buffer_max_length, row_number, columns_list);
#else
  (void) buffer; (void) buffer_length; (void) buffer_max_length; (void) row_number; (void) inode_list; (void) inode_count; (void) source_name;
  return PGFINDLIB_OK;
#endif
}

/*
  Put file name in buffer. Precede with, or include, comments if there are any.
  todo: pgfindlib_comment = source name if first in source and not done before
  todo: some parameters unused e.g. inode_warning_count
  todo: can use st_nlink to see how many hardlinks a file has (expect it to have at least 1)
  todo: maybe there's a way to follow a symlink -- we use lstat on the file, maybe stat() would do better
*/
int pgfindlib_file(char *buffer, unsigned int *buffer_length, const char *line, unsigned int buffer_max_length,
                          unsigned int *row_number,
                          ino_t inode_list[], unsigned int *inode_count, unsigned int *inode_warning_count,
                          struct tokener tokener_list_item,
                          int program_e_machine)
{
  (void) inode_warning_count;
  char line_copy[PGFINDLIB_MAX_PATH_LENGTH + 1]; /* todo: change following to "get rid of trailing \n" */
  {
    int i= 0; int j= 0;
    for (;;)
    {
      if (*(line + i) != '\n') { *(line_copy + j)= *(line + i); ++j; }
      if (*(line + i) == '\0') break;
      ++i;
    }
  }

  const char *columns_list[MAX_COLUMNS_PER_ROW];
  for (int i= 0; i < MAX_COLUMNS_PER_ROW; ++i) columns_list[i]= "";
  unsigned int columns_list_number= COLUMN_FOR_COMMENT_1;

  char warning_max_inode_count_is_too_small[64]= ""; /* Do not move warnings into "if ... {} ...", columns_list needs their addresses */
  char warning_lstat_failed[64]= "";
  char warning_access_failed[64]= "";
  char warning_symlink[64]= "";
  char warning_duplicate[64]= "";
  char warning_elf[64]= "";
  if (access(line_copy, R_OK) != 0) /* It's poorly documented but tests indicate X_OK doesn't matter and R_OK matters */
  {
#if (PGFINDLIB_COMMENT_ACCESS_FAILED != 0)
    pgfindlib_comment_in_row(warning_access_failed, PGFINDLIB_COMMENT_ACCESS_FAILED, 0);
    columns_list[columns_list_number++]= warning_access_failed;
#endif
  }
  ino_t inode;
  struct stat sb;
  if (lstat(line_copy, &sb) == -1)
  {
#if (PGFINDLIB_COMMENT_LSTAT_FAILED != 0)
    pgfindlib_comment_in_row(warning_lstat_failed, PGFINDLIB_COMMENT_LSTAT_FAILED, 0);
    columns_list[columns_list_number++]= warning_lstat_failed;
#endif
    inode= -1; /* a dummy so count of inodes is right but this won't be found later */
  }
  else
  {
    mode_t st_mode= sb.st_mode & S_IFMT;
    if ((st_mode !=S_IFREG) && (st_mode != S_IFLNK)) return PGFINDLIB_OK; /* not file or symlink so not candidate */
    /* Here, if (st_mode == S_IFLNK) and include_symlinks is off, return */
    /* Here, if (duplicate) and include duplicates is off, return */
    if (st_mode == S_IFLNK)
    {
#if (PGFINDLIB_COMMENT_SYMLINK != 0)
      /* todo: find out: symlink of what? */
      pgfindlib_comment_in_row(warning_symlink, PGFINDLIB_COMMENT_SYMLINK, 0);
      columns_list[columns_list_number++]= warning_symlink;
#endif
    }
    inode= sb.st_ino;
    for (unsigned int i= 0; i < *inode_count; ++i)
    {
      if (inode_list[i] == inode)
      {
#if (PGFINDLIB_COMMENT_DUPLICATE != 0)
       pgfindlib_comment_in_row(warning_duplicate, PGFINDLIB_COMMENT_DUPLICATE, i + 1); /* "+ 1" because row_number starts at 1 */
       columns_list[columns_list_number++]= warning_duplicate;
#endif
       break;
      }
    }
  }
  {
    struct tokener tokener_list[1]; /* won't be used */
    int elf_rval= pgfindlib_read_elf(tokener_list, line_copy, PGFINDLIB_REASON_SO_CHECK, program_e_machine);
    if (elf_rval != 0)
    {
      int unknown_failures= 0;
      /* These all have the same warning_elf so suppressing with "#if ... #endif" would be complicated. */
      if (elf_rval == PGFINDLIB_COMMENT_ELF_OPEN_FAILED)
        pgfindlib_comment_in_row(warning_elf, PGFINDLIB_COMMENT_ELF_OPEN_FAILED, 0);
      else if (elf_rval == PGFINDLIB_COMMENT_ELF_READ_FAILED)
        pgfindlib_comment_in_row(warning_elf, PGFINDLIB_COMMENT_ELF_READ_FAILED, 0);
      else if (elf_rval == PGFINDLIB_COMMENT_ELF_HAS_INVALID_IDENT)
        pgfindlib_comment_in_row(warning_elf, PGFINDLIB_COMMENT_ELF_HAS_INVALID_IDENT, 0);
      else if (elf_rval == PGFINDLIB_COMMENT_ELF_IS_NOT_EXEC_OR_DYN)
        pgfindlib_comment_in_row(warning_elf, PGFINDLIB_COMMENT_ELF_IS_NOT_EXEC_OR_DYN, 0);
      else if (elf_rval == PGFINDLIB_COMMENT_ELF_SHT_DYNAMIC_NOT_FOUND)
        pgfindlib_comment_in_row(warning_elf, PGFINDLIB_COMMENT_ELF_SHT_DYNAMIC_NOT_FOUND, 0);
      else if (elf_rval == PGFINDLIB_COMMENT_ELF_MACHINE_DOES_NOT_MATCH)
        pgfindlib_comment_in_row(warning_elf, PGFINDLIB_COMMENT_ELF_MACHINE_DOES_NOT_MATCH, 0);
      else ++unknown_failures; /* if this happens it's a bug */
      if (unknown_failures == 0) columns_list[columns_list_number++]= warning_elf;
    }
  }

  if (*inode_count == PGFINDLIB_MAX_INODE_COUNT)
  {
#if (PGFINDLIB_COMMENT_MAX_INODE_COUNT_TOO_SMALL != 0)
    pgfindlib_comment_in_row(warning_max_inode_count_is_too_small, PGFINDLIB_COMMENT_MAX_INODE_COUNT_TOO_SMALL, 0);
    columns_list[columns_list_number++]= warning_max_inode_count_is_too_small;
#endif
  }
  else
  {
    inode_list[*inode_count]= inode;
    ++*inode_count;  
  }
  char comment_string[256]; /* todo: check: too small */ /* "LD_AUDIT" "LD_PRELOAD" etc. */
  memcpy(comment_string, tokener_list_item.tokener_name, tokener_list_item.tokener_length);
  comment_string[tokener_list_item.tokener_length]= '\0';

  columns_list[COLUMN_FOR_PATH]= line_copy;
  columns_list[COLUMN_FOR_SOURCE]= comment_string;

  return pgfindlib_row_bottom_level(buffer, buffer_length, buffer_max_length, row_number, columns_list);
}

/*
  Tokenize comments
  [SELECT *]                   Possible future addition
  [FROM source [, source ...]  There's a default if it's missing
  [WHERE file [, file ...]     There's no default if it's missing but maybe there should be
  [ORDER BY id]                Possible future addition
  [LIMIT n]                    Possible future addition
  Keywords can be any case, source and file are case sensitive.
  Source and file can be enclosed in either ""s or ''s or ::s, which are stripped.
    (Although ' and " look more natural, running main 'a b' will fail because argv strips them already,
    so only : is reliable, say main ':a b:')
  Source can be: LD_LIBRARY_PATH etc. (the standard sources) but anything not in standard sources is considered a path
  Spaces are ignored, except that a keyword must be followed by at least one space
  Only possible end-of-token marks are space and comma and \0
  Possible errors:
    ' or " without ending ' or "
    , without following source name or file name
    source name or file name is too long (max = 255)
  End with _END
    const char *tokener_name;
    char tokener_length;
    char tokener_comment_id;
};
*/
/* Compare for equality with an ascii keyword already lower case,  */
int pgfindlib_keycmp(const char *a, unsigned int a_len, const char *b)
{
  if (a_len != strlen(b)) return -1;
  for (unsigned int i= 0; i < a_len; ++i)
  {
    if ((*a != *b) && (*a != (*b) - 32)) return -1;
    ++a; ++b;
  }
  return 0;
}
int pgfindlib_tokenize(const char *statement, struct tokener tokener_list[],
                       unsigned int *row_number, ino_t inode_list[], unsigned int *inode_count,
                       char *buffer, unsigned int *buffer_length, unsigned buffer_max_length)
{
  const char *p= statement;
  const char *p_next;
  unsigned int token_number= 0;

  if (p == NULL)
  {
    pgfindlib_comment_is_row("Syntax error. Statement is NULL", PGFINDLIB_COMMENT_STATEMENT_SYNTAX_STATEMENT_IS_NULL,
                            buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
    return PGFINDLIB_ERROR_STATEMENT_SYNTAX;
  }
  const char *statement_end= p + strlen(p);
  for (;;)
  {
    if (p > statement_end)
    {
      pgfindlib_comment_is_row("Syntax error. p > statement end", PGFINDLIB_COMMENT_STATEMENT_SYNTAX_P_GREATER_STATEMENT_END,
                            buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
      return PGFINDLIB_ERROR_STATEMENT_SYNTAX;
    }
    if (*p == ' ') { ++p; continue; }
    else if (*p == '\0') break;
    else if ((*p == '"') || (*p == 0x27) || (*p == ':')) /* if " or ' then skip to next " or ' */
    {
      p_next= strchr(p + 1, *p);
      if (p_next == NULL)
      {
        pgfindlib_comment_is_row("Syntax error. Quote without end quote", PGFINDLIB_COMMENT_STATEMENT_SYNTAX_QUOTE_WITHOUT_END_QUOTE,
                                 buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
        return PGFINDLIB_ERROR_STATEMENT_SYNTAX;
      }
    }
    else if (*p == ',')
      p_next= p + 1;
    else
    {
      p_next= p + 1;
      for (;;)
      {
        if ((*p_next == ' ') || (*p_next == ',') || (*p_next == '\0')) break;
        ++p_next;
      }
    }
    tokener_list[token_number].tokener_name= p;
    tokener_list[token_number].tokener_length= p_next - p;
    if (tokener_list[token_number].tokener_length >= PGFINDLIB_MAX_TOKEN_LENGTH)
    {
      pgfindlib_comment_is_row("Syntax error. Token too long", PGFINDLIB_COMMENT_STATEMENT_SYNTAX_TOKEN_TOO_LONG,
                               buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
      return PGFINDLIB_ERROR_STATEMENT_SYNTAX;
    }
    ++token_number;
    if (token_number >= PGFINDLIB_MAX_TOKENS_COUNT)
    {
      pgfindlib_comment_is_row("Syntax error. Too many tokens", PGFINDLIB_COMMENT_STATEMENT_SYNTAX_TOO_MANY_TOKENS,
                               buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
      return PGFINDLIB_ERROR_STATEMENT_SYNTAX;
    }
    if (*p_next == '\0') break;
    if (*p_next == ',')
    {
      tokener_list[token_number].tokener_name= p_next;
      tokener_list[token_number].tokener_length= 1;
      ++token_number;
      if (token_number >= PGFINDLIB_MAX_TOKENS_COUNT)
      {
        pgfindlib_comment_is_row("Syntax error. Comma without item", PGFINDLIB_COMMENT_STATEMENT_SYNTAX_COMMA_WITHOUT_ITEM,
                                 buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
        return PGFINDLIB_ERROR_STATEMENT_SYNTAX;
      }
    }
    p= p_next + 1;
  }
  /* tokener_list[token_number].tokener_name= "END!"; */
  /* tokener_list[token_number].tokener_length= 4; */
  tokener_list[token_number].tokener_comment_id= PGFINDLIB_TOKEN_END;
  unsigned int current_clause= 0;
  unsigned int from_count= 0;
  unsigned int where_count= 0; 
  for (int i= 0; tokener_list[i].tokener_comment_id != PGFINDLIB_TOKEN_END; ++i)
  {
    if (pgfindlib_keycmp(tokener_list[i].tokener_name, tokener_list[i].tokener_length, "from") == 0)
    {
      if (current_clause != 0)
      {
        pgfindlib_comment_is_row("Syntax error. FROM out of order", PGFINDLIB_COMMENT_STATEMENT_SYNTAX_FROM_OUT_OF_ORDER,
                                 buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
        return PGFINDLIB_ERROR_STATEMENT_SYNTAX;
      }
      tokener_list[i].tokener_comment_id= PGFINDLIB_TOKEN_FROM;
      current_clause= PGFINDLIB_TOKEN_FROM;
      ++from_count;
    }
    else if (pgfindlib_keycmp(tokener_list[i].tokener_name, tokener_list[i].tokener_length, "where") == 0)
    {
      if ((current_clause != 0) && (current_clause != PGFINDLIB_TOKEN_FROM))
      {
        pgfindlib_comment_is_row("Syntax error. WHERE out of order", PGFINDLIB_COMMENT_STATEMENT_SYNTAX_WHERE_OUT_OF_ORDER,
                                 buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
        return PGFINDLIB_ERROR_STATEMENT_SYNTAX;
      }
      tokener_list[i].tokener_comment_id= PGFINDLIB_COMMENT_STATEMENT_SYNTAX_WHERE_OUT_OF_ORDER;
      current_clause= PGFINDLIB_TOKEN_WHERE;
      ++where_count;
    }
    else if (tokener_list[i].tokener_name[0] == ',')
      tokener_list[i].tokener_comment_id= PGFINDLIB_TOKEN_COMMA;
    else
    {
      if (current_clause == PGFINDLIB_TOKEN_FROM)
      {
        for (int k= 0; ; ++k)
        {
          const char *c= pgfindlib_standard_source_array[k];
          if (strcmp(c, "") == 0) break;
          tokener_list[i].tokener_comment_id= PGFINDLIB_TOKEN_SOURCE_NONSTANDARD;
          if ((strlen(c) == (unsigned int) tokener_list[i].tokener_length) && (memcmp(c, tokener_list[i].tokener_name, tokener_list[i].tokener_length) == 0))
          {
            tokener_list[i].tokener_comment_id= pgfindlib_standard_source_array_n[k];
            break;
          }
        }
      }
      else if (current_clause == PGFINDLIB_TOKEN_WHERE)
      {
        tokener_list[i].tokener_comment_id= PGFINDLIB_TOKEN_FILE;
      }
      else /* presumably an error */
        tokener_list[i].tokener_comment_id= PGFINDLIB_TOKEN_UNKNOWN;
    }
  }
  /* If there was no FROM, make a list from standard sources and put it at end.
     (Later the eval doesn't care if FROM is after WHERE, though arbitrarily it's a syntax error if user inputs thus.) */
  if (from_count == 0)
  {
    /* token_number should still be at end, which will be overwritten */
    for (int i= 0; pgfindlib_standard_source_array_n[i] != 0; ++i)
    {
      if (token_number >= PGFINDLIB_MAX_TOKENS_COUNT - 1)
      {
        pgfindlib_comment_is_row("Syntax error. Too many tokens when adding default", PGFINDLIB_COMMENT_STATEMENT_SYNTAX_TOO_MANY_TOKENS_WHEN_ADDING_DEFAULT,
        buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
        return PGFINDLIB_ERROR_STATEMENT_SYNTAX;
      }
      tokener_list[token_number].tokener_name= pgfindlib_standard_source_array[i];
      tokener_list[token_number].tokener_length= strlen(pgfindlib_standard_source_array[i]);
      tokener_list[token_number].tokener_comment_id= pgfindlib_standard_source_array_n[i];
      ++token_number;
    }
    tokener_list[token_number].tokener_comment_id= PGFINDLIB_TOKEN_END;
  }
  for (int i= 0;; ++i)
  {
    /* char token_x[256]; */
    if (tokener_list[i].tokener_comment_id == PGFINDLIB_TOKEN_END) break;
  }
  return PGFINDLIB_OK;
}

#if (PGFINDLIB_TOKEN_SOURCE_LD_SO_CACHE != 0)
int pgfindlib_so_cache(const struct tokener tokener_list[], int tokener_number,
                       char *malloc_buffer_1, unsigned int *malloc_buffer_1_length, unsigned malloc_buffer_1_max_length,
                       char **malloc_buffer_2, unsigned int *malloc_buffer_2_length, unsigned malloc_buffer_2_max_length)
{
  int rval= PGFINDLIB_OK;
  const char *ldconfig; /* must be able to access ldconfig in some standard directory or user's path */
  for (int i= 0; i <= 5; ++i)
  {
    if (i == 0) ldconfig= "/sbin/ldconfig";
    else if (i == 1) ldconfig= "/usr/sbin/ldconfig";
    else if (i == 2) ldconfig= "/bin/ldconfig";
    else if (i == 3) ldconfig= "/usr/bin/ldconfig";
    else if (i == 4) ldconfig= "ldconfig";
    else if (i == 5) { ldconfig= NULL; break; }
    if (access(ldconfig, X_OK) == 0) break;
  }
  {
    /* Assume library is whatever's between first and last / on a line */
    /* Line 1 is probably "[n] libs found in cache `/etc/ld.so.cache'" */
    /* First ldconfig attempt will be Linux-style ldconfig -p, second will be FreeBSD-style ldconfig -r */
    int counter= 0;
    for (int ldconfig_attempts= 0; ldconfig_attempts < 2; ++ldconfig_attempts)
    {
      char ld_so_cache_line[PGFINDLIB_MAX_PATH_LENGTH * 5];
      char popen_arg[PGFINDLIB_MAX_PATH_LENGTH];
      if (ldconfig_attempts == 0) sprintf(popen_arg, "LD_LIBRARY_PATH= LD_DEBUG= LD_PRELOAD= %s -p 2>/dev/null ", ldconfig);
      else sprintf(popen_arg, "%s -r 2>/dev/null ", ldconfig);
      counter= 0;
      FILE *fp;
      fp= popen(popen_arg, "r");
      if (fp != NULL) /* popen failure unlikely even if ldconfig not found */
      {
        while (fgets(ld_so_cache_line, sizeof(ld_so_cache_line), fp) != NULL)
        {
          ++counter;
          char *pointer_to_ld_so_cache_line= ld_so_cache_line + strlen(ld_so_cache_line);
          for (;;)
          {
            if (*pointer_to_ld_so_cache_line == '/') break;
            if (pointer_to_ld_so_cache_line <= ld_so_cache_line) break;
            --pointer_to_ld_so_cache_line;
          }
          if (pointer_to_ld_so_cache_line == ld_so_cache_line) continue; /* blank line */
          ++pointer_to_ld_so_cache_line; /* So pointer is just after the final / which should be at the file name */
          if (pgfindlib_find_line_in_statement(tokener_list, pointer_to_ld_so_cache_line) == 0) continue;/* doesn't match requirement */
          char *address= strchr(ld_so_cache_line,'/');
          if (address != NULL)
          {
            rval= pgfindlib_add_to_malloc_buffers(address, tokener_number,
                                                  malloc_buffer_1, malloc_buffer_1_length, malloc_buffer_1_max_length,
                                                  malloc_buffer_2, malloc_buffer_2_length, malloc_buffer_2_max_length);
            if (rval != PGFINDLIB_OK)
            {
              pclose(fp);
              return rval;
            }
          }
        }
        pclose(fp);
      }
      if (counter > 0) break; /* So if ldconfig -p succeeds, don't try ldconfig -r */
    }
#ifdef OBSOLETE
#if (PGFINDLIB_COMMENT_LDCONFIG_FAILED != 0)
    if (counter == 0)
    {
      char comment[256];
      sprintf(comment, "%s -p or %s -r failed", ldconfig, ldconfig); 
      rval= pgfindlib_comment_is_row(comment,
                         PGFINDLIB_COMMENT_LDCONFIG_FAILED,
                         buffer, &buffer_length, buffer_max_length, &row_number, inode_list, &inode_count);
      if (rval != PGFINDLIB_OK) return rval;
    }
#endif
#endif
  }
  return PGFINDLIB_OK;
}
#endif /* #if (PGFINDLIB_TOKEN_SOURCE_LD_SO_CACHE != 0) */

/*
  Return rval = PGFINDLIB_MALLOC_BUFFER_1_OVERFLOW or PGFINDLIB_MALLOC_BUFFER_2_OVERFLOW
                if it won't fit, and this should percolate upward to force a new malloc.
  NB: malloc_buffer_2 is char **so length is #-of-items rather than #-of-chars
  At the front we add a char = source_number + 32 (because assumption is we won't have 127 - 32 sources)
*/
int pgfindlib_add_to_malloc_buffers(const char *new_item, int source_number,
                                    char *malloc_buffer_1, unsigned int *malloc_buffer_1_length, unsigned malloc_buffer_1_max_length,
                                    char **malloc_buffer_2, unsigned int *malloc_buffer_2_length, unsigned malloc_buffer_2_max_length)
{
  unsigned int strlen_new_item= strlen(new_item) + 2; /* because we'll allocate char-of-source at start and \0 at end */
  if (*malloc_buffer_1_length + strlen_new_item >= malloc_buffer_1_max_length) return PGFINDLIB_MALLOC_BUFFER_1_OVERFLOW;
  char c= source_number + 32;
  *(malloc_buffer_1 + *malloc_buffer_1_length)= c;
  strcpy(malloc_buffer_1 + *malloc_buffer_1_length + 1, new_item);
  if (*malloc_buffer_2_length + 1 >= malloc_buffer_2_max_length) return PGFINDLIB_MALLOC_BUFFER_2_OVERFLOW;
  malloc_buffer_2[*malloc_buffer_2_length]= malloc_buffer_1 + *malloc_buffer_1_length; 
  *malloc_buffer_1_length+= strlen_new_item;
  *malloc_buffer_2_length+= 1;
  return PGFINDLIB_OK;
}

/* Assume unsigned comparison per https://stackoverflow.com/questions/1356741/strcmp-and-signed-unsigned-chars */ 
int pgfindlib_qsort_compare(const void *p1, const void *p2)
{
  return strcmp(*(char *const *) p1, *(char *const *) p2);
}

/*
  Given something like /tmp/libcrypto.so, return only the last part i.e. libcrypto.so.
  Todo: Check whether there are places which could call this but instead are duplicating the instructions.
*/
static const char *pgfindlib_file_part(const char *one_library_or_file)
{
  const char *p= one_library_or_file + strlen(one_library_or_file);
  for (;;)
  {
    --p;
    if ((p < one_library_or_file) || (*p == '/')) { ++p; break; }
  }
  return p;
}

/* Pass: list of libraries separated by colons (or spaces or semicolons depending on source)
   I don't check wheher the colon is enclosed within ""s and don't expect a path name to contain a colon.
   If this is called for LD_AUDIT or LD_PRELOAD then librarylist is actually a filelist, which should still be okay.
   We won't get here for ld.so.cache.
*/
int pgfindlib_source_scan(const char *librarylist, char *buffer, unsigned int *buffer_length,
                                unsigned int tokener_number, unsigned int buffer_max_length,
                                const char *lib, const char *platform, const char *origin,
                                unsigned int *row_number,
                                ino_t inode_list[], unsigned int *inode_count, unsigned int *inode_warning_count,
                                struct tokener tokener_list[],
                                char *malloc_buffer_1, unsigned int *malloc_buffer_1_length, unsigned malloc_buffer_1_max_length,
                                char **malloc_buffer_2, unsigned int *malloc_buffer_2_length, unsigned malloc_buffer_2_max_length,
                                int program_e_machine)
{
  int rval;
  char delimiter1, delimiter2;
  int comment_number= tokener_list[tokener_number].tokener_comment_id;
  if (comment_number == PGFINDLIB_TOKEN_SOURCE_LD_AUDIT)
  { delimiter1= ':'; delimiter2= ':'; } /* colon or colon */
  else if (comment_number == PGFINDLIB_TOKEN_SOURCE_LD_PRELOAD)
  { delimiter1= ':'; delimiter2= ' '; } /* colon or space */
  else if (comment_number == PGFINDLIB_TOKEN_SOURCE_DT_RPATH)
  { delimiter1= ':'; delimiter2= ':';  } /* colon or colon */
  else if (comment_number == PGFINDLIB_TOKEN_SOURCE_LD_LIBRARY_PATH)
  { delimiter1= ':'; delimiter2= ';';  } /* colon or semicolon */
  else if (comment_number == PGFINDLIB_TOKEN_SOURCE_DT_RUNPATH)
  { delimiter1= ':'; delimiter2= ':';  } /* colon or colon */
  else if (comment_number == PGFINDLIB_TOKEN_SOURCE_LD_RUN_PATH)
  { delimiter1= ':'; delimiter2= ':';  } /* colon or colon */
  else if (comment_number == PGFINDLIB_TOKEN_SOURCE_DEFAULT_PATHS)
  { delimiter1= ':'; delimiter2= ';';  } /* colon or semicolon */
  else if (comment_number == PGFINDLIB_TOKEN_SOURCE_LD_PGFINDLIB_PATH)
  { delimiter1= ':'; delimiter2= ';';  } /* colon or semicolon */
  else /* presumably PGFINDLIB_TOKEN_SOURCE_NONSTANDARD a user-defined path */
  { delimiter1= ':'; delimiter2= ':';  } /* colon or colon, can be undocumented */
  {
    char source[PGFINDLIB_MAX_TOKEN_LENGTH + 1];
    int source_length= tokener_list[tokener_number].tokener_length;
    memcpy(source, tokener_list[tokener_number].tokener_name, source_length);
    source[source_length]= '\0';
  }
  char one_library_or_file[PGFINDLIB_MAX_PATH_LENGTH + 1];

  if (librarylist == NULL) return PGFINDLIB_OK;
  const char *p_in= librarylist;
  for (;;)
  {
    if (*p_in == '\0') break;

    char *pointer_to_one_library_or_file= &one_library_or_file[0];

    /* todo: maybe skip lead/trail spaces is unnecessary? pgrindlib_line_in_statement skips them too */
    for (;;)
    {
      if ((*p_in == delimiter1) || (*p_in == delimiter2) || (*p_in == '\0')) break;
      if ((*p_in != ' ') || (pointer_to_one_library_or_file != &one_library_or_file[0])) /* skip lead space */
      {
        *pointer_to_one_library_or_file= *p_in;
        ++pointer_to_one_library_or_file;
        *pointer_to_one_library_or_file= '\0';
      }
      ++p_in;
    }

    if ((*p_in == delimiter1) || (*p_in == delimiter2)) ++p_in; /* skip delimiter but don't skip \0 */
    for (;;) /* skip trail spaces */
    {
      --pointer_to_one_library_or_file;
      if (pointer_to_one_library_or_file < &one_library_or_file[0]) break;
      if (*pointer_to_one_library_or_file != ' ') break;
      *pointer_to_one_library_or_file= '\0';
    }
    if (strlen(one_library_or_file) > 0) /* If it's a blank we skip it. Is that right? */
    {
      unsigned int replacements_count;
      char orig_one_library_or_file[PGFINDLIB_MAX_PATH_LENGTH + 1];
      strcpy(orig_one_library_or_file, one_library_or_file);
      rval= pgfindlib_replace_lib_or_platform_or_origin(one_library_or_file, &replacements_count, lib, platform, origin);
      if (rval !=PGFINDLIB_OK) return rval;
#if (PGFINDLIB_COMMENT_REPLACE_STRING != 0)
      if (replacements_count > 0)
      {
        char source_name[32];
        unsigned int source_name_length= tokener_list[tokener_number].tokener_length;
        if (source_name_length > 32 - 1) source_name_length= 32 - 1;
        memcpy(source_name, tokener_list[tokener_number].tokener_name, source_name_length);
        source_name[source_name_length]= '\0';
        char comment[PGFINDLIB_MAX_PATH_LENGTH*2 + 128];
        sprintf(comment, "in source %s replaced %s with %s", source_name, orig_one_library_or_file, one_library_or_file);
        rval= pgfindlib_comment_is_row(comment,
                         PGFINDLIB_COMMENT_REPLACE_STRING,
                         buffer, buffer_length, buffer_max_length, row_number, inode_list, inode_count);
        if (rval != PGFINDLIB_OK) return rval;
      }
#endif
      /* if LD_AUDIT or LD_PRELOAD we want a file name */
      if ((comment_number == PGFINDLIB_TOKEN_SOURCE_LD_AUDIT) || (comment_number == PGFINDLIB_TOKEN_SOURCE_LD_PRELOAD)) 
      {
        const char *file_part= pgfindlib_file_part(one_library_or_file);
        if (pgfindlib_find_line_in_statement(tokener_list, file_part) == 0) continue; /* doesn't match requirement */
        rval= pgfindlib_file(buffer, buffer_length, one_library_or_file, buffer_max_length, row_number,
                             inode_list, inode_count, inode_warning_count, tokener_list[tokener_number], program_e_machine);
        if (rval != PGFINDLIB_OK) return rval;
      }
      else
      /* not LD_AUDIT or LD_PRELOAD so it should be a directory name */
      {
        /* char ls_line[PGFINDLIB_MAX_PATH_LENGTH * 5]; */
        DIR* dir= opendir(one_library_or_file);
        if (dir != NULL) /* perhaps would be null if directory not found */
        {
          struct dirent* dirent;
          while ((dirent= readdir(dir)) != NULL)
          {
            if ((dirent->d_type !=  DT_REG) &&  (dirent->d_type !=  DT_LNK)) continue; /* not regular file or symbolic link */
             if (pgfindlib_find_line_in_statement(tokener_list, dirent->d_name) == 0) continue; /* doesn't match requirement */
            {
              char combo[PGFINDLIB_MAX_PATH_LENGTH * 2 + 1];
              strcpy(combo, one_library_or_file);
              strcat(combo, "/");
              strcat(combo, dirent->d_name);
              rval= pgfindlib_add_to_malloc_buffers(combo, tokener_number,
                                                    malloc_buffer_1, malloc_buffer_1_length, malloc_buffer_1_max_length,
                                                    malloc_buffer_2, malloc_buffer_2_length, malloc_buffer_2_max_length);
              if (rval != PGFINDLIB_OK)
              {
                closedir(dir);
                return rval;
              }
            }
          }
          closedir(dir);
        }
      }
    }
  }
  return PGFINDLIB_OK;
}

/*
   Read an elf file. Possible reasons:
   There are reasons to read an elf file:
     (1) To confirm it really is an elf file, which means check elf_header.e_ident,
         and return a warning if it's not
     (2) to confirm it is ET_EXEC or ET_DYN (yes an executable program can say it's ET_DYN),
         and return a warning if it's not
     (3) to look for a list of .so files via the section header. (code exists but is ignored)
     (4) If statement includes FROM (program-name) instead of the default FROM (SELF) (not done)
     (5) to calculate $PLATFORM (not done).
     (6) to compensate for a failure to find _DYNAMIC which seems to be a FreeBSD problem (not done)
     (7) to get dynamic loader name (not done)
     (8) for a "listed" source, in addition to the standard sources
    Why section headers?
      The list of .so files does not show whether the files are DT_RPATH / DT_RUNPATH / part of link.
      And there might be nothing if there was stripping.
      Maybe gcc would add something re flag choices in a .note, this has not been checked.
      Surely PT_DYNAMIC would have better information, this was checked but abandoned.
      There should be a warning.
*/
int pgfindlib_read_elf(const struct tokener tokener_list[], const char* possible_elf_file, int reason, int program_e_machine)
{
#ifndef PGFINDLIB_HANDLE_SO_LIST
  (void) tokener_list;
#endif
  ElfW(Ehdr) elf_header;
  int rval= PGFINDLIB_OK;
  FILE *fd= NULL;
  fd= fopen(possible_elf_file, "rb");
  if (fd == NULL) {rval= PGFINDLIB_COMMENT_ELF_OPEN_FAILED; goto close_and_return; }
  size_t read_size;
  read_size= fread(&elf_header, sizeof(elf_header), 1, fd);
  if (read_size == 0) { rval= PGFINDLIB_COMMENT_ELF_READ_FAILED; goto close_and_return; }
  if (memcmp(elf_header.e_ident, ELFMAG, SELFMAG) != 0) { rval= PGFINDLIB_COMMENT_ELF_HAS_INVALID_IDENT; goto close_and_return; }
  if ((elf_header.e_type != ET_EXEC) && (elf_header.e_type != ET_DYN)) { rval= PGFINDLIB_COMMENT_ELF_IS_NOT_EXEC_OR_DYN; goto close_and_return; }
  if ((reason == PGFINDLIB_REASON_SO_CHECK) && (program_e_machine != elf_header.e_machine)) { rval= PGFINDLIB_COMMENT_ELF_MACHINE_DOES_NOT_MATCH; goto close_and_return; }
/* Effectively the rest of this function is commented out so only PGFINDLIB_REASON_SO_CHECK actually works. Doesn't act right anyway. */
#ifdef PGFINDLIB_HANDLE_SO_LIST
  if (reason == PGFINDLIB_REASON_SO_LIST)
  {
    /* SHT_DYNAMIC segment should have index of SHT_STRTAB which should have list of .so files etc. */
    char sht_buffer[PGFINDLIB_SHT_BUFFER_LENGTH]; /* multiple things fread into this buffer so beware of overwrite */ 
    char* sht_pointer= &sht_buffer[0];

    if (elf_header.e_shentsize * elf_header.e_shnum >= PGFINDLIB_SHT_BUFFER_LENGTH) { rval=PGFINDLIB_COMMENT_ELF_BUFFER_TOO_SMALL; goto close_and_return; }
    fseek(fd, elf_header.e_shoff, SEEK_SET);
    int read_size= fread(sht_buffer, 1, elf_header.e_shentsize * elf_header.e_shnum, fd);
    if (read_size == 0) { rval= PGFINDLIB_COMMENT_ELF_READ_FAILED; goto close_and_return; }
    ElfW(Shdr)*shdr= (ElfW(Shdr)*)sht_pointer;
    int is_sht_found= 0;
    for (int i= 0; i < elf_header.e_shnum; ++i)
    {
      if (shdr->sh_type == SHT_DYNAMIC)
      {
        /* sh_link should be index of the SHT_STRTAB which points to lib*.so etc. */
        /* sh_name should be offset within another SHT_STRTAB which has ".dynamic" but the check for that is skipped here */
        if (shdr->sh_link >= elf_header.e_shnum) { rval= PGFINDLIB_COMMENT_ELF_SHT_DYNAMIC_NOT_FOUND; goto close_and_return; }
        ElfW(Shdr)*shdr2= (ElfW(Shdr)*)sht_pointer;
        shdr2+= shdr->sh_link;
        if (shdr2->sh_type == SHT_STRTAB)
        {
          /* todo: if ((shdr2->flags & &SHF_ALLOC) == 0) it's probably not valid */
          is_sht_found= 1;
          if (shdr2->sh_size >= PGFINDLIB_SHT_BUFFER_LENGTH) return PGFINDLIB_COMMENT_ELF_BUFFER_TOO_SMALL;
          fseek(fd, shdr2->sh_offset, SEEK_SET);
          read_size= fread(sht_buffer, 1, shdr2->sh_size, fd);
          if (read_size == 0) { rval= PGFINDLIB_COMMENT_ELF_READ_FAILED; goto close_and_return; }
          if (*(sht_buffer + shdr2->sh_size - 1) != '\0') { rval= PGFINDLIB_COMMENT_ELF_SHT_DYNAMIC_NOT_FOUND; goto close_and_return; }
          for (int j= 0;;)
          {
            if (j >= shdr2->sh_size - 1) break;
            int string_length= strlen(sht_buffer + j);
            if (string_length > 0)
            {
              int x= pgfindlib_find_line_in_statement(tokener_list, sht_buffer + j);
            }
            j+= string_length + 1;
          }
        }
      }      
      ++shdr;
    }
    if (is_sht_found == 0) { rval= PGFINDLIB_COMMENT_ELF_SHT_DYNAMIC_NOT_FOUND; goto close_and_return; }
    if (elf_header.e_phentsize * elf_header.e_phnum >= PGFINDLIB_SHT_BUFFER_LENGTH) { rval=PGFINDLIB_COMMENT_ELF_BUFFER_TOO_SMALL; goto close_and_return; }
    fseek(fd, elf_header.e_phoff, SEEK_SET);
    read_size= fread(sht_buffer, 1, elf_header.e_phentsize * elf_header.e_phnum, fd);
    if (read_size == 0) { rval= PGFINDLIB_COMMENT_ELF_READ_FAILED; goto close_and_return; }
    char *pt_pointer= &sht_buffer[0];
    ElfW(Phdr)*phdr= (ElfW(Phdr)*)pt_pointer;
    int is_pt_found= 0;
    for (int i= 0; i < elf_header.e_phnum; ++i)
    {
      if (phdr->p_type == PT_DYNAMIC)
      {
        unsigned int dynamic_size= phdr->p_filesz;
        int dyn_size= 8; /* sizeof(struct ElfW(Dyn)); */
        fseek(fd, phdr->p_offset, SEEK_SET); /* or p_vaddr? */
        read_size= fread(sht_buffer, 1, phdr->p_filesz, fd);
        if (read_size == 0) { rval= PGFINDLIB_COMMENT_ELF_READ_FAILED; goto close_and_return; }
        ElfW(Dyn)*dyn= (ElfW(Dyn)*)pt_pointer;
        for (int kk= 0;; ++kk)
        {
          /* Expect sometimes to see DT_RUNPATH or DT_RPATH, but it's not happening. */
          ++dyn;
          ElfW(Dyn)*dyn2= (ElfW(Dyn)*)pt_pointer;
          if (dyn->d_tag == DT_NULL) break;        
          /* But also check if we've read past phdr->p_filesz */
        }
        break;
      }
      ++phdr;
    }
  }
#endif
close_and_return:
  if (fd != NULL) fclose(fd);
  return rval;
}
