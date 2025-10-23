/*
  Third-party code that may optionally be included with ocelotgui
  
  Last modified: 2020-09-23
  
  This code is not required. It does not have to be brought in for Ocelot's default
  configuration as a MySQL/MariaDB client.
  To see if it is brought in, see comments in ocelotgui.h before
  "#ifndef OCELOT_THIRD_PARTY".
  
  The contents of this file are copied from files in
  https://github.com/tarantool/tarantool-c
  The LICENSE of that directory is the BSD 2-clause license.
  There are also some public domain and MIT-license files.
  The copying of each file is done as follows:
  There is a "COPY:" note about the file name, the copying date,
  and all changes from the original (usually the only changes
  are removals of #includes since now there is only one file).
  Then the file contents.
  
  Peter Gulutzan is not the primary author of the
  code in this file, and only claims rights to the changes it wrote.
  It was copied to make builds of ocelotgui on non-Linux platforms
  easier. Anyone who wishes to use the code for any other purpose
  should go to the current upstream github repository.
  
  All notes, disclaimers, and licenses in this file apply
  only to the code in this file and not to the rest of the
  ocelotgui code, which is licensed as GPL version 2 unless
  otherwise specified.
*/

/* COPY:
   Changes: All THIRD_PARTY functions are affected by the
   following #pragmas because our compiler is not basic C and
   our usual assumption is that -fpermissive is on.
   There is a "diagnostic pop" at the end of this file.
   MinGW ignores #pragma
   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53431
   but for MinGW we changed CMakeLists.txt.
   Update August 2018: Alas GCC 7 won't let us turn off fpermissive, so explicit casts have been added
                       in about 100 places. But now we shouldn't need
                       #pragma GCC diagnostic warning "-fpermissive"
                       #pragma GCC diagnostic ignored "-Wpedantic"
   Update March 2023: A warning is happening with -Wall and newish compiler on Fedora 37,
                      I disable -Warray-bounds because I don't understand the problem. todo: revisit soon
*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Warray-bounds"
#if defined(__GNUC__) && (__GNUC__ >= 7)
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif

/* COPY:
   Changes: Windows doesn't have the same #includes as Linux.
   We commented out these #includes for all file copies in this file.
   We added these definitions in our attempts to work around.
   This is only for MinGW.
   See also https://msdn.microsoft.com/en-us/library/windows/desktop/ms737593(v=vs.85).aspx
   and https://stackoverflow.com/questions/4243027/winsock-compiling-error-it-cant-find-the-addrinfo-structures-and-some-relating
   We changed "#line ..." to "//#line ..." so we could see compiler errors more clearly.
   Todo: "#define _WIN32_WINNT 0x0501" (Windows XP) causes warnings. Should I #undef it later?
*/
#ifndef WINDOWS_KLUDGE_H
#define WINDOWS_KLUDGE_H

#ifdef WIN32
#define _WIN32_WINNT 0x501
#include <ws2tcpip.h>
#include <Winsock2.h>
struct iovec {
   void *iov_base;
   unsigned long iov_len;
};
#define _POSIX_PATH_MAX  256 /* as in ./bits/posix1_lim.h */
#define NI_MAXHOST      1025 /* as in netdb.h */
struct sockaddr_un {
  unsigned short sun_family;
  char	         sun_path[108];
};
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <netinet/tcp.h>
#endif
#endif

/*
   COPY: file name = third_party/base64.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/
#ifndef BASE64_H
#define BASE64_H
/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 * This is part of the libb64 project, and has been placed in the
 * public domain. For details, see
 * http://sourceforge.net/projects/libb64
 */
#ifdef __cplusplus
//extern "C" {
#endif

#define BASE64_CHARS_PER_LINE 72

static inline int
base64_bufsize(int binsize)
{
    int datasize = binsize * 4/3 + 4;
    int newlines = ((datasize + BASE64_CHARS_PER_LINE - 1)/
            BASE64_CHARS_PER_LINE);
    return datasize + newlines;
}

/**
 * Encode a binary stream into BASE64 text.
 *
 * @pre the buffer size is at least 4/3 of the stream
 * size + stream_size/72 (newlines) + 4
 *
 * @param[in]  in_bin           the binary input stream to decode
 * @param[in]  in_len		size of the input
 * @param[out] out_base64       output buffer for the encoded data
 * @param[in]  out_len          buffer size, must be at least
 *				4/3 of the input size
 *
 * @return the size of encoded output
 */

int
base64_encode(const char *in_bin, int in_len,
          char *out_base64, int out_len);

/**
 * Decode a BASE64 text into a binary
 *
 * @param[in]  in_base64	the BASE64 stream to decode
 * @param[in]  in_len		size of the input
 * @param[out] out_bin		output buffer size
 * @param[in]  out_len		buffer size
 *
 * @pre the output buffer size must be at least
 * 3/4 + 1 of the size of the input
 *
 * @return the size of decoded output
 */

int base64_decode(const char *in_base64, int in_len,
          char *out_bin, int out_len);

#ifdef __cplusplus
//} /* extern "C" */
#endif
#endif /* BASE64_H */

/*
   COPY: file name = third_party/PMurHash.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/
#ifndef THIRD_PARTY_PMURHASH_H_
#define THIRD_PARTY_PMURHASH_H_
/*-----------------------------------------------------------------------------
 * MurmurHash3 was written by Austin Appleby, and is placed in the public
 * domain.
 *
 * This implementation was written by Shane Day, and is also public domain.
 *
 * This is a portable ANSI C implementation of MurmurHash3_x86_32 (Murmur3A)
 * with support for progressive processing.
 */

/* ------------------------------------------------------------------------- */
/* Determine what native type to use for uint32_t */

/* We can't use the name 'uint32_t' here because it will conflict with
 * any version provided by the system headers or application. */

/* First look for special cases */
#if defined(_MSC_VER)
  #define MH_UINT32 unsigned long
#endif

/* If the compiler says it's C99 then take its word for it */
#if !defined(MH_UINT32) && ( \
     defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L )
  #include <stdint.h>
  #define MH_UINT32 uint32_t
#endif

/* Otherwise try testing against max value macros from limit.h */
#if !defined(MH_UINT32)
  #include  <limits.h>
  #if   (USHRT_MAX == 0xffffffffUL)
    #define MH_UINT32 unsigned short
  #elif (UINT_MAX == 0xffffffffUL)
    #define MH_UINT32 unsigned int
  #elif (ULONG_MAX == 0xffffffffUL)
    #define MH_UINT32 unsigned long
  #endif
#endif

#if !defined(MH_UINT32)
  #error Unable to determine type name for unsigned 32-bit int
#endif

/* I'm yet to work on a platform where 'unsigned char' is not 8 bits */
#define MH_UINT8  unsigned char


/* ------------------------------------------------------------------------- */
/* Prototypes */

#ifdef __cplusplus
//extern "C" {
#endif

void PMurHash32_Process(MH_UINT32 *ph1, MH_UINT32 *pcarry, const void *key, int len);
MH_UINT32 PMurHash32_Result(MH_UINT32 h1, MH_UINT32 carry, MH_UINT32 total_length);
MH_UINT32 PMurHash32(MH_UINT32 seed, const void *key, int len);

void PMurHash32_test(const void *key, int len, MH_UINT32 seed, void *out);

#ifdef __cplusplus
//}
#endif

#endif

/*
   COPY: file name = include/tarantool/tnt_call.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/
#ifndef TNT_CALL_H_INCLUDED
#define TNT_CALL_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file tnt_call.h
 * \brief Call request
 */

/**
 * \brief Construct call request and write it into stream
 *
 * \param s    stream object to write request to
 * \param proc procedure to call
 * \param plen procedure length
 * \param args tnt_object instance with messagepack array with args to call
 *             procedure with
 *
 * \retval number of bytes written to stream
 */
ssize_t
tnt_call(struct tnt_stream *s, const char *proc, size_t plen,
     struct tnt_stream *args);

/**
 * \brief Construct call request and write it into stream
 *        Version for Tarantool 1.6
 *
 * \param s    stream object to write request to
 * \param proc procedure to call
 * \param plen procedure length
 * \param args tnt_object instance with messagepack array with args to call
 *             procedure with
 *
 * \retval number of bytes written to stream
 */
ssize_t
tnt_call_16(struct tnt_stream *s, const char *proc, size_t proc_len,
        struct tnt_stream *args);
/**
 * \brief Construct eval request and write it into stream
 *
 * \param s    stream object to write request to
 * \param expr expression to evaluate
 * \param elen expression length
 * \param args tnt_object instance with messagepack array with args to eval
 *             expression with
 *
 * \retval number of bytes written to stream
 */
ssize_t
tnt_eval(struct tnt_stream *s, const char *expr, size_t elen,
     struct tnt_stream *args);

#endif /* TNT_CALL_H_INCLUDED */

/*
   COPY: file name = include/tarantool/tnt_delete.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/
#ifndef TNT_DELETE_H_INCLUDED
#define TNT_DELETE_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file tnt_delete.h
 * \brief Delete request
 */

/**
 * \brief Write delete request to stream
 *
 * \param s     stream instance
 * \param space space number to delete object from
 * \param index index to search key in
 * \param key   key to delete tuple with
 *
 * \retval number of bytes written to stream
 */
ssize_t
tnt_delete(struct tnt_stream *s, uint32_t space, uint32_t index,
       struct tnt_stream *key);

#endif /* TNT_DELETE_H_INCLUDED */

/*
   COPY: file name = include/tarantool/tnt_execute.h.
   Copying date = 2020-09-23.
   Changes: none.
*/
#ifndef TNT_EXECUTE_H_INCLUDED
#define TNT_EXECUTE_H_INCLUDED

/**
 * \file tnt_execute.h
 * \brief SQL execution request
 */

/**
 * \brief Construct SQL request and write it into stream
 *
 * \param s    stream object to write request to
 * \param expr SQL query string
 * \param elen query length
 * \param args tnt_object instance with messagepack array with params
 *             to bind to the request
 *
 * \retval number of bytes written to stream
 */
ssize_t
tnt_execute(struct tnt_stream *s, const char *expr, size_t elen,
        struct tnt_stream *params);

#endif /* TNT_EXECUTE_H_INCLUDED */

/*
   COPY: file name = include/tarantool/tnt_insert.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/
#ifndef TNT_INSERT_H_INCLUDED
#define TNT_INSERT_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file tnt_insert.h
 * \brief Insert/Replace request
 */

/**
 * \brief Construct insert request and write it into stream
 *
 * \param s     stream object to write request to
 * \param space space no to insert tuple into
 * \param tuple (tnt_object instance) msgpack array with tuple to insert to
 *
 * \retval number of bytes written to stream
 */
ssize_t
tnt_insert(struct tnt_stream *s, uint32_t space, struct tnt_stream *tuple);

/**
 * \brief Construct replace request and write it into stream
 *
 * \param s     stream object to write request to
 * \param space space no to replace tuple into
 * \param tuple (tnt_object instance) msgpack array with tuple to replace to
 *
 * \retval number of bytes written to stream
 */
ssize_t
tnt_replace(struct tnt_stream *s, uint32_t space, struct tnt_stream *tuple);

#endif /* TNT_INSERT_H_INCLUDED */

/*
   COPY: file name = include/tarantool/tnt_update.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/
#ifndef TNT_UPDATE_H_INCLUDED
#define TNT_UPDATE_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

//#include <tarantool/tnt_stream.h>

/**
 * \file tnt_update.h
 * \brief Update operation
 */

/**
 * \brief Generate and write update operation with predefined
 *
 * \param s     stream pointer
 * \param space space no
 * \param index index no
 * \param key   key to update
 * \param ops   ops to update (tnt_object)
 *
 * \returns count of bytes written
 * \retval  -1 oom
 * \sa tnt_update_cointainer
 * \sa tnt_update_cointainer_close
 */
ssize_t
tnt_update(struct tnt_stream *s, uint32_t space, uint32_t index,
       struct tnt_stream *key, struct tnt_stream *ops);

/**
 * \brief Add bit operation for update to tnt_object
 *
 * \param ops     operation container
 * \param fieldno field number
 * \param op      operation ('&', '|', '^')
 * \param value   value for update op
 *
 * \returns count of bytes written
 * \retval  -1 oom
 * \sa tnt_update_cointainer
 * \sa tnt_update_cointainer_close
 */
ssize_t
tnt_update_bit(struct tnt_stream *ops, uint32_t fieldno, char op,
           uint64_t value);

/**
 * \brief Add int arithmetic operation for update to tnt_object
 *
 * \param ops     operation container
 * \param fieldno field number
 * \param op      operation ('+', '-')
 * \param value   value for update op
 *
 * \returns count of bytes written
 * \retval  -1 oom
 * \sa tnt_update_cointainer
 * \sa tnt_update_cointainer_close
 */
ssize_t
tnt_update_arith_int(struct tnt_stream *ops, uint32_t fieldno, char op,
             int64_t value);

/**
 * \brief Add float arithmetic operation for update to tnt_object
 *
 * \param ops     operation container
 * \param fieldno field number
 * \param op      operation ('+', '-')
 * \param value   value for update op
 *
 * \returns count of bytes written
 * \retval  -1 oom
 * \sa tnt_update_cointainer
 * \sa tnt_update_cointainer_close
 */
ssize_t
tnt_update_arith_float(struct tnt_stream *ops, uint32_t fieldno, char op,
               float value);

/**
 * \brief Add double arithmetic operation for update to tnt_object
 *
 * \param ops     operation container
 * \param fieldno field number
 * \param op      operation ('+', '-')
 * \param value   value for update op
 *
 * \returns count of bytes written
 * \retval  -1 oom
 * \sa tnt_update_cointainer
 * \sa tnt_update_cointainer_close
 */
ssize_t
tnt_update_arith_double(struct tnt_stream *ops, uint32_t fieldno, char op,
                double value);

/**
 * \brief Add delete operation for update to tnt_object
 *
 * \param ops     operation container
 * \param fieldno field number
 * \param fieldco field count
 *
 * \returns count of bytes written
 * \retval  -1 oom
 * \sa tnt_update_cointainer
 * \sa tnt_update_cointainer_close
 */
ssize_t
tnt_update_delete(struct tnt_stream *ops, uint32_t fieldno,
          uint32_t fieldco);

/**
 * \brief Add insert before operation for update to tnt_object
 *
 * \param ops     operation container
 * \param fieldno field number
 * \param val     value to insert (tnt_object)
 *
 * \returns count of bytes written
 * \retval  -1 oom
 * \sa tnt_update_cointainer
 * \sa tnt_update_cointainer_close
 */
ssize_t
tnt_update_insert(struct tnt_stream *ops, uint32_t fieldno,
          struct tnt_stream *val);

/**
 * \brief Add assign operation for update to tnt_object
 *
 * \param ops     operation container
 * \param fieldno field number
 * \param val     value to assign (tnt_object)
 *
 * \returns count of bytes written
 * \retval  -1 oom
 * \sa tnt_update_cointainer
 * \sa tnt_update_cointainer_close
 */
ssize_t
tnt_update_assign(struct tnt_stream *ops, uint32_t fieldno,
          struct tnt_stream *val);

/**
 * \brief Add splice operation for update to tnt_object
 *
 * \param ops         operation container
 * \param fieldno     field number
 * \param position    cut from
 * \param offset      number of bytes to cut
 * \param buffer      buffer to insert instead
 * \param buffer_len  buffer length
 *
 * \returns count of bytes written
 * \retval  -1 oom
 * \sa tnt_update_cointainer
 * \sa tnt_update_cointainer_close
 */
ssize_t
tnt_update_splice(struct tnt_stream *ops, uint32_t fieldno,
          uint32_t position, uint32_t offset,
          const char *buffer, size_t buffer_len);

/**
 * \brief shortcut for tnt_object() with type == TNT_SBO_SPARSE
 */
struct tnt_stream *tnt_update_container(struct tnt_stream *ops);

/**
 * \brief shortcut for tnt_object_container_close()
 */
struct tnt_stream *
tnt_update_container(struct tnt_stream *ops);

int tnt_update_container_close(struct tnt_stream *ops);

int tnt_update_container_reset(struct tnt_stream *ops);

/**
 * \brief Generate and write upsert operation with predefined
 *
 * \param s     stream pointer
 * \param space space no
 * \param tuple (tnt_object instance) msgpack array with tuple to insert to
 * \param ops   ops to update (tnt_object)
 *
 * \returns count of bytes written
 * \retval  -1 oom
 * \sa tnt_update_cointainer
 * \sa tnt_update_cointainer_close
 */
ssize_t
tnt_upsert(struct tnt_stream *s, uint32_t space,
       struct tnt_stream *tuple, struct tnt_stream *ops);

#endif /* TNT_UPDATE_H_INCLUDED */


/*
   COPY: file name = include/tarantool/tnt_mem.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/


#ifndef TNT_MEM_H_INCLUDED
#define TNT_MEM_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \internal
 * \file tnt_mem.h
 * \brief Basic memory functions
 */

#define tntfunction_unused __attribute__((unused))

#if !defined __GNUC_MINOR__ || defined __INTEL_COMPILER || \
    defined __SUNPRO_C || defined __SUNPRO_CC
#define TNT_GCC_VERSION(major, minor) 0
#else
#define TNT_GCC_VERSION(major, minor) (__GNUC__ > (major) || \
    (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))
#endif

#if !defined(__has_builtin)
#define __has_builtin(x) 0 /* clang */
#endif

#if TNT_GCC_VERSION(2, 9) || __has_builtin(__builtin_expect)
#define tntlikely(x) __builtin_expect(!!(x), 1)
#define tntunlikely(x) __builtin_expect(!!(x), 0)
#else
#define tntlikely(x) (x)
#define tntunlikely(x) (x)
#endif

/**
 * \brief basic allocation function type
 *
 * \param ptr  pointer to allocation/deallocation block
 * \param size size of block to allocat/reallocate
 *
 * \retval pointer to newly alloced/realloced block
 * \retval NULL on error/free
 */
typedef void *(tnt_allocator_t)(void *ptr, size_t size);

/**
 * \brief initialize memory allocation function
 */
void *
tnt_mem_init(tnt_allocator_t alloc);

/**
 * \brief Internal function
 */
void *
tnt_mem_alloc(size_t size);

/**
 * \brief Internal function
 */
void *
tnt_mem_realloc(void *ptr, size_t size);

/**
 * \brief Internal function
 */
char *
tnt_mem_dup(char *sz);

/**
 * \brief Internal function
 */
void
tnt_mem_free(void *ptr);

#endif /* TNT_MEM_H_INCLUDED */

/*
   COPY: file name = /tnt/tnt_assoc.h.
   Copying Date = 2017-09-14.
   Changes: added ifndef ... endif
   Commented out three #undefs.
   #include <mhash.h> is included in the middle of tnt_assoc.h
   The MH_SOURCE section of tnt/tnt_assoc.h is copied to tnt/tnt_assoc.c.
*/

#ifndef TS_ASSOC_H_INCLUDED
#define TS_ASSOC_H_INCLUDED
#if defined(__cplusplus)
//extern "C" {
#endif /* defined(__cplusplus) */

struct assoc_key {
    const char *id;
    uint32_t id_len;
};

struct assoc_val {
    struct assoc_key key;
    void *data;
};

static inline int
mh_cmp_eq(
        const struct assoc_val **lval,
        const struct assoc_val **rval,
        void *arg) {
    (void )arg;
    if ((*lval)->key.id_len != (*rval)->key.id_len) return 0;
    return !memcmp((*lval)->key.id, (*rval)->key.id, (*rval)->key.id_len);
}

static inline int
mh_cmp_key_eq(
        const struct assoc_key *key,
        const struct assoc_val **val,
        void *arg) {
    (void )arg;
    if (key->id_len != (*val)->key.id_len) return 0;
    return !memcmp(key->id, (*val)->key.id, key->id_len);
}

static inline void *
tnt_mem_calloc(size_t count, size_t size) {
    size_t sz = count * size;
    void *alloc = tnt_mem_alloc(sz);
    if (!alloc) return 0;
    memset(alloc, 0, sz);
    return alloc;

}

#define mh_arg_t void *

#define mh_eq(a, b, arg)      mh_cmp_eq(a, b, arg)
#define mh_eq_key(a, b, arg)  mh_cmp_key_eq(a, b, arg)
#define mh_hash(x, arg)       PMurHash32(MUR_SEED, (*x)->key.id, (*x)->key.id_len)
#define mh_hash_key(x, arg)   PMurHash32(MUR_SEED, (x)->id, (x)->id_len);

/* type for hash value */
#define mh_node_t struct assoc_val *
/* type for hash key */
#define mh_key_t  const struct assoc_key *

#define MH_CALLOC(x, y) tnt_mem_calloc((x), (y))
#define MH_FREE(x)      tnt_mem_free((x))

#define mh_name               _assoc
#define MUR_SEED 13
//#include		      <PMurHash.h>
//#include                      <mhash.h>

/*
   COPY: file name = mhash.h.
   Copying Date = 2017-09-14.
   Changes: none.
      #include <mhash.h> is included in the middle of tnt_assoc.h
*/
#ifndef MHASH_H_INCLUDED
#define MHASH_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/* The MIT License

   Copyright (c) 2008, by Attractive Chaos <attractivechaos@aol.co.uk>

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#ifndef MH_INCREMENTAL_RESIZE
#define MH_INCREMENTAL_RESIZE 1
#endif

#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#define mh_cat(a, b) mh##a##_##b
#define mh_ecat(a, b) mh_cat(a, b)
#define _mh(x) mh_ecat(mh_name, x)

#define mh_unlikely(x)  __builtin_expect((x),0)

#ifndef   MH_CALLOC
#define   MH_CALLOC calloc
#endif /* MH_CALLOC */

#ifndef   MH_FREE
#define   MH_FREE free
#endif /* MH_FREE */

#ifndef   MH_TYPEDEFS
#define   MH_TYPEDEFS 1
typedef uint32_t mh_int_t;
#endif /* MH_TYPEDEFS */

#ifndef __ac_HASH_PRIME_SIZE
#define __ac_HASH_PRIME_SIZE 31
static const mh_int_t __ac_prime_list[__ac_HASH_PRIME_SIZE] = {
    3ul,		11ul,		23ul,		53ul,
    97ul,		193ul,		389ul,		769ul,
    1543ul,		3079ul,		6151ul,		12289ul,
    24593ul,	49157ul,	98317ul,	196613ul,
    393241ul,	786433ul,	1572869ul,	3145739ul,
    6291469ul,	12582917ul,	25165843ul,	50331653ul,
    100663319ul,	201326611ul,	402653189ul,	805306457ul,
    1610612741ul,	3221225473ul,	4294967291ul
};
#endif /* __ac_HASH_PRIME_SIZE */

#ifndef MH_HEADER
#define MH_HEADER

struct _mh(t) {
    mh_node_t *p;
    mh_int_t *b;
    mh_int_t n_buckets;
    mh_int_t n_dirty;
    mh_int_t size;
    mh_int_t upper_bound;
    mh_int_t prime;

    mh_int_t resize_cnt;
    mh_int_t resize_position;
    mh_int_t batch;
    struct _mh(t) *shadow;
};

#define mh_exist(h, i)		({ h->b[i >> 4] & (1 << (i % 16)); })
#define mh_dirty(h, i)		({ h->b[i >> 4] & (1 << (i % 16 + 16)); })

#define mh_setfree(h, i)	({ h->b[i >> 4] &= ~(1 << (i % 16)); })
#define mh_setexist(h, i)	({ h->b[i >> 4] |= (1 << (i % 16)); })
#define mh_setdirty(h, i)	({ h->b[i >> 4] |= (1 << (i % 16 + 16)); })

#define mh_node(h, i)		((const mh_node_t *) &((h)->p[(i)]))
#define mh_size(h)		({ (h)->size;		})
#define mh_capacity(h)		({ (h)->n_buckets;	})
#define mh_begin(h)		({ 0;			})
#define mh_end(h)		({ (h)->n_buckets;	})

#define mh_first(h) ({						\
    mh_int_t i;						\
    for (i = 0; i < mh_end(h); i++) {			\
        if (mh_exist(h, i))				\
            break;					\
    }							\
    i;							\
})

#define mh_next(h, i) ({					\
    mh_int_t n = i;						\
    if (n < mh_end(h)) {					\
        for (n = i + 1; n < mh_end(h); n++) {		\
            if (mh_exist(h, n))			\
                break;				\
        }						\
    }							\
    n;							\
})

#define mh_foreach(h, i) \
    for (i = mh_first(h); i < mh_end(h); i = mh_next(h, i))

#define MH_DENSITY 0.7

struct _mh(t) * _mh(new)();
void _mh(clear)(struct _mh(t) *h);
void _mh(delete)(struct _mh(t) *h);
void _mh(resize)(struct _mh(t) *h, mh_arg_t arg);
int _mh(start_resize)(struct _mh(t) *h, mh_int_t buckets, mh_int_t batch,
              mh_arg_t arg);
void _mh(reserve)(struct _mh(t) *h, mh_int_t size,
          mh_arg_t arg);
void __attribute__((noinline)) _mh(del_resize)(struct _mh(t) *h, mh_int_t x,
                           mh_arg_t arg);
size_t _mh(memsize)(struct _mh(t) *h);
void _mh(dump)(struct _mh(t) *h);

#define put_slot(h, node, arg) \
    _mh(put_slot)(h, node, arg)

static inline mh_node_t *
_mh(node)(struct _mh(t) *h, mh_int_t x)
{
    return (mh_node_t *) &(h->p[x]);
}


static inline mh_int_t
_mh(next_slot)(mh_int_t slot, mh_int_t inc, mh_int_t size)
{
    slot += inc;
    return slot >= size ? slot - size : slot;
}

#if defined(mh_hash_key) && defined(mh_eq_key)
/**
 * If it is necessary to search by something different
 * than a hash node, define mh_hash_key and mh_eq_key
 * and use mh_find().
 */
static inline mh_int_t
_mh(find)(struct _mh(t) *h, mh_key_t key, mh_arg_t arg)
{
    (void) arg;

    mh_int_t k = mh_hash_key(key, arg);
    mh_int_t i = k % h->n_buckets;
    mh_int_t inc = 1 + k % (h->n_buckets - 1);
    for (;;) {
        if ((mh_exist(h, i) && mh_eq_key(key, mh_node(h, i), arg)))
            return i;

        if (!mh_dirty(h, i))
            return h->n_buckets;

        i = _mh(next_slot)(i, inc, h->n_buckets);
    }
}
#endif

static inline mh_int_t
_mh(get)(struct _mh(t) *h, const mh_node_t *node,
     mh_arg_t arg)
{
    (void) arg;

    mh_int_t k = mh_hash(node, arg);
    mh_int_t i = k % h->n_buckets;
    mh_int_t inc = 1 + k % (h->n_buckets - 1);
    for (;;) {
        if ((mh_exist(h, i) && mh_eq(node, mh_node(h, i), arg)))
            return i;

        if (!mh_dirty(h, i))
            return h->n_buckets;

        i = _mh(next_slot)(i, inc, h->n_buckets);
    }
}

static inline mh_int_t
_mh(random)(struct _mh(t) *h, mh_int_t rnd)
{
    mh_int_t i = 0;
    for (i = 0; i < mh_size(h); i++, rnd++) {
        rnd %= h->n_buckets;
        if (mh_exist(h, rnd))
            return rnd;
    }

    return h->n_buckets;
}

static inline mh_int_t
_mh(put_slot)(struct _mh(t) *h, const mh_node_t *node,
          mh_arg_t arg)
{
    (void) arg;

    mh_int_t k = mh_hash(node, arg); /* hash key */
    mh_int_t i = k % h->n_buckets; /* offset in the hash table. */
    mh_int_t inc = 1 + k % (h->n_buckets - 1); /* overflow chain increment. */

    /* Skip through all collisions. */
    while (mh_exist(h, i)) {
        if (mh_eq(node, mh_node(h, i), arg))
            return i;               /* Found a duplicate. */
        /*
         * Mark this link as part of a collision chain. The
         * chain always ends with a non-marked link.
         * Note: the collision chain for this key may share
         * links with collision chains of other keys.
         */
        mh_setdirty(h, i);
        i = _mh(next_slot)(i, inc, h->n_buckets);
    }
    /*
     * Found an unused, but possibly dirty slot. Use it.
     * However, if this is a dirty slot, first check that
     * there are no duplicates down the collision chain. The
     * current link can also be from a collision chain of some
     * other key, but this is can't be established, so check
     * anyway.
     */
    mh_int_t save_i = i;
    while (mh_dirty(h, i)) {
        i = _mh(next_slot)(i, inc, h->n_buckets);

        if (mh_exist(h, i) && mh_eq(mh_node(h, i), node, arg))
            return i;               /* Found a duplicate. */
    }
    /* Reached the end of the collision chain: no duplicates. */
    return save_i;
}

/**
 * Find a node in the hash and replace it with a new value.
 * Save the old node in ret pointer, if it is provided.
 * If the old node didn't exist, just insert the new node.
 *
 * @retval != mh_end()   pos of the new node, ret is either NULL
 *                       or copy of the old node
 * @retval  mh_end()     out of memory, ret is unchanged.
 */
static inline mh_int_t
_mh(put)(struct _mh(t) *h, const mh_node_t *node, mh_node_t **ret,
     mh_arg_t arg)
{
    mh_int_t x = mh_end(h);
    int exist;
    if (h->size == h->n_buckets)
        /* no one free elements in the hash table */
        goto put_done;

#if MH_INCREMENTAL_RESIZE
    if (mh_unlikely(h->resize_position > 0))
        _mh(resize)(h, arg);
    else if (mh_unlikely(h->n_dirty >= h->upper_bound)) {
        if (_mh(start_resize)(h, h->n_buckets + 1, 0, arg))
            goto put_done;
    }
    if (h->resize_position)
        _mh(put)(h->shadow, node, NULL, arg);
#else
    if (mh_unlikely(h->n_dirty >= h->upper_bound)) {
        if (_mh(start_resize)(h, h->n_buckets + 1, h->size,
                      arg))
            goto put_done;
    }
#endif

    x = put_slot(h, node, arg);
    exist = mh_exist(h, x);

    if (!exist) {
        /* add new */
        mh_setexist(h, x);
        h->size++;
        if (!mh_dirty(h, x))
            h->n_dirty++;

        memcpy(&(h->p[x]), node, sizeof(mh_node_t));
        if (ret)
            *ret = NULL;
    } else {
        if (ret)
            memcpy(*ret, &(h->p[x]), sizeof(mh_node_t));
        /* replace old */
        memcpy(&(h->p[x]), node, sizeof(mh_node_t));
    }

put_done:
    return x;
}

static inline void
_mh(del)(struct _mh(t) *h, mh_int_t x,
     mh_arg_t arg)
{
    if (x != h->n_buckets && mh_exist(h, x)) {
        mh_setfree(h, x);
        h->size--;
        if (!mh_dirty(h, x))
            h->n_dirty--;
#if MH_INCREMENTAL_RESIZE
        if (mh_unlikely(h->resize_position))
            _mh(del_resize)(h, x, arg);
#endif
    }
}
#endif

static inline void
_mh(remove)(struct _mh(t) *h, const mh_node_t *node,
     mh_arg_t arg)
{
    mh_int_t k = _mh(get)(h, node, arg);
    if (k != mh_end(h))
        _mh(del)(h, k, arg);
}

#ifdef MH_SOURCE

void __attribute__((noinline))
_mh(del_resize)(struct _mh(t) *h, mh_int_t x,
        mh_arg_t arg)
{
    struct _mh(t) *s = h->shadow;
    uint32_t y = _mh(get)(s, (const mh_node_t *) &(h->p[x]),
                  arg);
    _mh(del)(s, y, arg);
    _mh(resize)(h, arg);
}

struct _mh(t) *
_mh(new)()
{
    struct _mh(t) *h = (struct _mh(t) *) MH_CALLOC(1, sizeof(*h));
    if (!h) return NULL;
    memset(h, 0, sizeof(struct _mh(t)));
    h->shadow = (struct _mh(t) *) MH_CALLOC(1, sizeof(*h));
    if (!h->shadow) goto error;
    h->prime = 0;
    h->n_buckets = __ac_prime_list[h->prime];
    h->p = (mh_node_t *) MH_CALLOC(h->n_buckets, sizeof(mh_node_t));
    if (!h->p) goto error;
    h->b = (mh_int_t *) MH_CALLOC(h->n_buckets / 16 + 1, sizeof(mh_int_t));
    if (!h->b) goto error;
    h->upper_bound = h->n_buckets * MH_DENSITY;
    return h;
error:
    _mh(delete)(h);
    return NULL;
}

void
_mh(clear)(struct _mh(t) *h)
{
    MH_FREE(h->p);
    h->prime = 0;
    h->n_buckets = __ac_prime_list[h->prime];
    h->p = (mh_node_t *) MH_CALLOC(h->n_buckets, sizeof(mh_node_t));
    h->upper_bound = h->n_buckets * MH_DENSITY;
}

void
_mh(delete)(struct _mh(t) *h)
{
    MH_FREE(h->shadow);
    MH_FREE(h->b);
    MH_FREE(h->p);
    MH_FREE(h);
}

/** Calculate hash size. */
size_t
_mh(memsize)(struct _mh(t) *h)
{
    size_t sz = 2 * sizeof(struct _mh(t));

    sz += h->n_buckets * sizeof(mh_node_t);
    sz += (h->n_buckets / 16 + 1) * sizeof(mh_int_t);
    if (h->resize_position) {
        h = h->shadow;
        sz += h->n_buckets * sizeof(mh_node_t);
        sz += (h->n_buckets / 16 + 1) * sizeof(mh_int_t);
    }
    return sz;
}

void
_mh(resize)(struct _mh(t) *h,
        mh_arg_t arg)
{
    struct _mh(t) *s = h->shadow;
#if MH_INCREMENTAL_RESIZE
    mh_int_t  batch = h->batch;
#endif
    mh_int_t i = 0;
    for (i = h->resize_position; i < h->n_buckets; i++) {
#if MH_INCREMENTAL_RESIZE
        if (batch-- == 0) {
            h->resize_position = i;
            return;
        }
#endif
        if (!mh_exist(h, i))
            continue;
        mh_int_t n = put_slot(s, mh_node(h, i), arg);
        s->p[n] = h->p[i];
        mh_setexist(s, n);
        s->n_dirty++;
    }
    MH_FREE(h->p);
    MH_FREE(h->b);
    s->size = h->size;
    memcpy(h, s, sizeof(*h));
    h->resize_cnt++;
}

int
_mh(start_resize)(struct _mh(t) *h, mh_int_t buckets, mh_int_t batch,
          mh_arg_t arg)
{
    if (h->resize_position) {
        /* resize has already been started */
        return 0;
    }
    if (buckets < h->n_buckets) {
        /* hash size is already greater than requested */
        return 0;
    }
    while (h->prime < __ac_HASH_PRIME_SIZE) {
        if (__ac_prime_list[h->prime] >= buckets)
            break;
        h->prime += 1;
    }

    h->batch = batch > 0 ? batch : h->n_buckets / (256 * 1024);
    if (h->batch < 256) {
        /*
         * Minimal batch must be greater or equal to
         * 1 / (1 - f), where f is upper bound percent
         * = MH_DENSITY
         */
        h->batch = 256;
    }

    struct _mh(t) *s = h->shadow;
    memcpy(s, h, sizeof(*h));
    s->resize_position = 0;
    s->n_buckets = __ac_prime_list[h->prime];
    s->upper_bound = s->n_buckets * MH_DENSITY;
    s->n_dirty = 0;
    s->p = (mh_node_t *) MH_CALLOC(s->n_buckets, sizeof(mh_node_t));
    if (s->p == NULL)
        return -1;
    s->b = (mh_int_t *) MH_CALLOC(s->n_buckets / 16 + 1, sizeof(mh_int_t));
    if (s->b == NULL) {
        MH_FREE(s->p);
        s->p = NULL;
        return -1;
    }
    _mh(resize)(h, arg);

    return 0;
}

void
_mh(reserve)(struct _mh(t) *h, mh_int_t size,
         mh_arg_t arg)
{
    _mh(start_resize)(h, size/MH_DENSITY, h->size, arg);
}

#ifndef mh_stat
#define mh_stat(buf, h) ({						\
        tbuf_printf(buf, "  n_buckets: %" PRIu32 CRLF		\
                "  n_dirty: %" PRIu32 CRLF			\
                "  size: %" PRIu32 CRLF			\
                "  resize_cnt: %" PRIu32 CRLF		\
                "  resize_position: %" PRIu32 CRLF,		\
                h->n_buckets,				\
                h->n_dirty,					\
                h->size,					\
                h->resize_cnt,				\
                h->resize_position);			\
            })
#endif

#ifdef MH_DEBUG
void
_mh(dump)(struct _mh(t) *h)
{
    printf("slots:\n");
    int k = 0, i = 0;
    for(i = 0; i < h->n_buckets; i++) {
        if (mh_dirty(h, i) || mh_exist(h, i)) {
            printf("   [%i] ", i);
            if (mh_exist(h, i)) {
                /* TODO(roman): fix this printf */
                printf("   -> %p", h->p[i]);
                k++;
            }
            if (mh_dirty(h, i))
                printf(" dirty");
            printf("\n");
        }
    }
    printf("end(%i)\n", k);
}
#endif

#endif

#if defined(MH_SOURCE) || defined(MH_UNDEF)
#undef MH_CALLOC
#undef MH_FREE
#undef MH_HEADER
#undef mh_node_t
#undef mh_arg_t
#undef mh_key_t
#undef mh_name
#undef mh_hash
#undef mh_hash_key
#undef mh_eq
#undef mh_eq_key
#undef mh_node
#undef mh_dirty
#undef mh_place
#undef mh_setdirty
#undef mh_setexist
#undef mh_setvalue
#undef mh_unlikely
#undef slot
#undef slot_and_dirty
#undef MH_DENSITY
#endif

//#undef mh_cat
//#undef mh_ecat
//#undef _mh
#endif /* MHASH_H_INCLUDED */


/*!
 * \file tnt_assoc.h
 */

/*! \struct mh_assoc_t */

/*! \fn void mh_assoc_clear(struct mh_assoc_t *h)
 * \brief Clear an associate array
 * \param h associate array
 */

/*! \fn void mh_assoc_delete(struct mh_assoc_t *h)
 * \brief Free an associate array
 * \param h associate array
 */

/*! \fn void mh_assoc_reserve(struct mh_assoc_t *h, uint32_t size, void *arg)
 * \brief Reserve place for elements
 * \param h associate array
 * \param size count of elements
 * \param arg  context for (eq/hash functions) (must be NULL)
 */

/*! \fn mh_node_t *mh_assoc_node(struct mh_assoc_t *h, uint32_t x)
 * \brief  Access value in specified slot
 * \param  h associate array
 * \param  x slot number
 * \retval value in slot
 */

/*! \fn uint32_t mh_assoc_find(struct mh_assoc_t *h, mh_key_t key, void *arg)
 * \brief  Search for element by key
 * \param  h    associate array
 * \param  key  key to search for
 * \param  arg  context for (eq/hash functions) (must be NULL)
 *
 * \retval != mh_end()   slot number, where element is contained
 * \retval    mh_end()   not found
 *
 * See also: \see mh_assoc_get or \see mh_assoc_find
 */

/*! \fn uint32_t mh_assoc_get(struct mh_assoc_t *h, mh_node_t *node, void *arg)
 * \brief Search for element by value
 * \param  h    associate array
 * \param  node node to search for
 * \param  arg  context for (eq/hash functions) (must be NULL)
 *
 * \retval != mh_end()   slot number, where element is contained
 * \retval    mh_end()   not found
 *
 * See also: \see mh_assoc_get or \see mh_assoc_find
 */

/*! \fn uint32_t mh_assoc_random(struct mh_assoc_t *h, uint32_t rnd)
 * \brief get random slot with existing element
 * \param h   associate array
 * \param rnd random number
 *
 * \retval != mh_end()   pos of the random node
 * \retval    mh_end()   last one
 */

/*! \fn uint32_t mh_assoc_put(struct mh_assoc_t *h, const mh_node_t *node,
 * 			      mh_node_t **ret, void *arg)
 * \brief put element into hash
 * \param h[in]    associate array
 * \param node[in] node to insert
 * \param ret[out] node, that's replaced
 * \param arg[in]  context for (eq/hash functions) (must be NULL)
 *
 * Find a node in the hash and replace it with a new value.
 * Save the old node in ret pointer, if it is provided.
 * If the old node didn't exist, just insert the new node.
 *
 * \retval != mh_end()   pos of the new node, ret is either NULL
 *                       or copy of the old node
 * \retval    mh_end()   out of memory, ret is unchanged.
 */

/*! \fn void mh_assoc_del(struct mh_assoc_t *h, uint32_t x, void *arg)
 * \brief delete element from specified slot
 * \param  h    associate array
 * \param  x    slot to delete element from
 * \param  arg  context for (eq/hash functions) (must be NULL)
 */

/*! \fn void mh_assoc_remove(struct mh_assoc_t *h, mh_node_t node, void *arg)
 * \brief delete specified element from assoc hash
 * \param  h    associate array
 * \param  node node to delete
 * \param  arg  context for (eq/hash functions) (must be NULL)
 */

/*! \fn struct mh_assoc_t *mh_assoc_new()
 * \brief allocate and initialize new associate array
 * \retval !NULL new associate array
 * \retval NULL  memory allocation error
 */

/*! \def mh_first(hash) */
/*! \def mh_next(hash) */
/*! \def mh_end(hash) */
/*! \def mh_foreach(hash, iter) */

#if defined(__cplusplus)
//}
#endif /* defined(__cplusplus) */

#endif /* TNT_ASSOC_H_INCLUDED */

/*
   COPY: file name = sha1.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/

#ifndef TS_SHA1_H_INCLUDED
#define TS_SHA1_H_INCLUDED

#include <stdint.h>

/* ================ sha1.h ================ */
/*
SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain
*/

typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    unsigned char buffer[64];
} SHA1_CTX;

void SHA1Transform(uint32_t state[5], const unsigned char buffer[64]);
void SHA1Init(SHA1_CTX* context);
void SHA1Update(SHA1_CTX* context, const unsigned char* data, uint32_t len);
void SHA1Final(unsigned char digest[20], SHA1_CTX* context);

#endif

/*
   COPY: file name = msgpuck.h.
   Copying Date = 2017-09-14.
   Changes: Commented out "extern" wherever possible.
*/

#ifndef MSGPUCK_H_INCLUDED
#define MSGPUCK_H_INCLUDED
/*
 * Copyright (c) 2013-2017 MsgPuck Authors
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file msgpuck.h
 * MsgPuck
 * \brief MsgPuck is a simple and efficient MsgPack encoder/decoder
 * library in a single self-contained file.
 *
 * Usage example:
 * \code
 * // Encode
 * char buf[1024];
 * char *w = buf;
 * w = mp_encode_array(w, 4)
 * w = mp_encode_uint(w, 10);
 * w = mp_encode_str(w, "hello world", strlen("hello world"));
 * w = mp_encode_bool(w, true);
 * w = mp_encode_double(w, 3.1415);
 *
 * // Validate
 * const char *b = buf;
 * int r = mp_check(&b, w);
 * assert(!r)
 * assert(b == w);
 *
 * // Decode
 * uint32_t size;
 * uint64_t ival;
 * const char *sval;
 * uint32_t sval_len;
 * bool bval;
 * double dval;
 *
 * const char *r = buf;
 *
 * size = mp_decode_array(&r);
 * // size is 4
 *
 * ival = mp_decode_uint(&r);
 * // ival is 10;
 *
 * sval = mp_decode_str(&r, &sval_len);
 * // sval is "hello world", sval_len is strlen("hello world")
 *
 * bval = mp_decode_bool(&r);
 * // bval is true
 *
 * dval = mp_decode_double(&r);
 * // dval is 3.1415
 *
 * assert(r == w);
 * \endcode
 *
 * \note Supported compilers.
 * The implementation requires a C99+ or C++03+ compatible compiler.
 *
 * \note Inline functions.
 * The implementation is compatible with both C99 and GNU inline functions.
 * Please link libmsgpuck.a static library for non-inlined versions of
 * functions and global tables.
 */

#if defined(__cplusplus) && !defined(__STDC_CONSTANT_MACROS)
#define __STDC_CONSTANT_MACROS 1 /* make С++ to be happy */
#endif
#if defined(__cplusplus) && !defined(__STDC_LIMIT_MACROS)
#define __STDC_LIMIT_MACROS 1    /* make С++ to be happy */
#endif
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#if defined(__cplusplus)
//extern "C" {
#endif /* defined(__cplusplus) */

/*
 * {{{ Platform-specific definitions
 */

/** \cond false **/

#if defined(__CC_ARM)         /* set the alignment to 1 for armcc compiler */
#define MP_PACKED    __packed
#else
#define MP_PACKED  __attribute__((packed))
#endif

#if defined(MP_SOURCE)
#error MP_SOURCE is not supported anymore, please link libmsgpuck.a
#endif

#if defined(__GNUC__) && !defined(__GNUC_STDC_INLINE__)
#if !defined(MP_LIBRARY)
#define MP_PROTO /* extern */ inline
#define MP_IMPL /* extern */ inline
#else /* defined(MP_LIBRARY) */
#define MP_PROTO
#define MP_IMPL
#endif
#define MP_ALWAYSINLINE
#else /* C99 inline */
#if !defined(MP_LIBRARY)
#define MP_PROTO inline
#define MP_IMPL inline
#else /* defined(MP_LIBRARY) */
#define MP_PROTO /* extern */ inline
#define MP_IMPL inline
#endif
#define MP_ALWAYSINLINE __attribute__((always_inline))
#endif /* GNU inline or C99 inline */

#if !defined __GNUC_MINOR__ || defined __INTEL_COMPILER || \
	defined __SUNPRO_C || defined __SUNPRO_CC
#define MP_GCC_VERSION(major, minor) 0
#else
#define MP_GCC_VERSION(major, minor) (__GNUC__ > (major) || \
	(__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))
#endif

#if !defined(__has_builtin)
#define __has_builtin(x) 0 /* clang */
#endif

#if MP_GCC_VERSION(2, 9) || __has_builtin(__builtin_expect)
#define mp_likely(x) __builtin_expect((x), 1)
#define mp_unlikely(x) __builtin_expect((x), 0)
#else
#define mp_likely(x) (x)
#define mp_unlikely(x) (x)
#endif

#if MP_GCC_VERSION(4, 5) || __has_builtin(__builtin_unreachable)
#define mp_unreachable() (assert(0), __builtin_unreachable())
#else
MP_PROTO void
mp_unreachable(void) __attribute__((noreturn));
MP_PROTO void
mp_unreachable(void) { assert(0); abort(); }
#define mp_unreachable() (assert(0))
#endif

#define mp_identity(x) (x) /* just to simplify mp_load/mp_store macroses */

#if MP_GCC_VERSION(4, 8) || __has_builtin(__builtin_bswap16)
#define mp_bswap_u16(x) __builtin_bswap16(x)
#else /* !MP_GCC_VERSION(4, 8) */
#define mp_bswap_u16(x) ( \
	(((x) <<  8) & 0xff00) | \
	(((x) >>  8) & 0x00ff) )
#endif

#if MP_GCC_VERSION(4, 3) || __has_builtin(__builtin_bswap32)
#define mp_bswap_u32(x) __builtin_bswap32(x)
#else /* !MP_GCC_VERSION(4, 3) */
#define mp_bswap_u32(x) ( \
	(((x) << 24) & UINT32_C(0xff000000)) | \
	(((x) <<  8) & UINT32_C(0x00ff0000)) | \
	(((x) >>  8) & UINT32_C(0x0000ff00)) | \
	(((x) >> 24) & UINT32_C(0x000000ff)) )
#endif

#if MP_GCC_VERSION(4, 3) || __has_builtin(__builtin_bswap64)
#define mp_bswap_u64(x) __builtin_bswap64(x)
#else /* !MP_GCC_VERSION(4, 3) */
#define mp_bswap_u64(x) (\
	(((x) << 56) & UINT64_C(0xff00000000000000)) | \
	(((x) << 40) & UINT64_C(0x00ff000000000000)) | \
	(((x) << 24) & UINT64_C(0x0000ff0000000000)) | \
	(((x) <<  8) & UINT64_C(0x000000ff00000000)) | \
	(((x) >>  8) & UINT64_C(0x00000000ff000000)) | \
	(((x) >> 24) & UINT64_C(0x0000000000ff0000)) | \
	(((x) >> 40) & UINT64_C(0x000000000000ff00)) | \
	(((x) >> 56) & UINT64_C(0x00000000000000ff)) )
#endif

#define MP_LOAD_STORE(name, type, bswap)					\
MP_PROTO type									\
mp_load_##name(const char **data);						\
MP_IMPL type									\
mp_load_##name(const char **data)						\
{										\
	struct MP_PACKED cast { type val; };					\
	type val = bswap(((struct cast *) *data)->val);				\
	*data += sizeof(type);							\
	return val;								\
}										\
MP_PROTO char *									\
mp_store_##name(char *data, type val);						\
MP_IMPL char *									\
mp_store_##name(char *data, type val)						\
{										\
	struct MP_PACKED cast { type val; };					\
	((struct cast *) (data))->val = bswap(val);				\
	return data + sizeof(type);						\
}

MP_LOAD_STORE(u8, uint8_t, mp_identity);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

MP_LOAD_STORE(u16, uint16_t, mp_bswap_u16);
MP_LOAD_STORE(u32, uint32_t, mp_bswap_u32);
MP_LOAD_STORE(u64, uint64_t, mp_bswap_u64);

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

MP_LOAD_STORE(u16, uint16_t, mp_identity);
MP_LOAD_STORE(u32, uint32_t, mp_identity);
MP_LOAD_STORE(u64, uint64_t, mp_identity);

#else
#error Unsupported __BYTE_ORDER__
#endif

#if !defined(__FLOAT_WORD_ORDER__)
#define __FLOAT_WORD_ORDER__ __BYTE_ORDER__
#endif /* defined(__FLOAT_WORD_ORDER__) */

#if __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__

/*
 * Idiots from msgpack.org byte-swaps even IEEE754 float/double types.
 * Some platforms (e.g. arm) cause SIGBUS on attempt to store
 * invalid float in registers, so code like flt = mp_bswap_float(flt)
 * can't be used here.
 */

union MP_PACKED mp_float_cast {
	uint32_t u32;
	float f;
};

union MP_PACKED mp_double_cast {
	uint64_t u64;
	double d;
};

MP_PROTO float
mp_load_float(const char **data);
MP_PROTO double
mp_load_double(const char **data);
MP_PROTO char *
mp_store_float(char *data, float val);
MP_PROTO char *
mp_store_double(char *data, double val);

MP_IMPL float
mp_load_float(const char **data)
{
	union mp_float_cast cast = *(union mp_float_cast *) *data;
	*data += sizeof(cast);
	cast.u32 = mp_bswap_u32(cast.u32);
	return cast.f;
}

MP_IMPL double
mp_load_double(const char **data)
{
	union mp_double_cast cast = *(union mp_double_cast *) *data;
	*data += sizeof(cast);
	cast.u64 = mp_bswap_u64(cast.u64);
	return cast.d;
}

MP_IMPL char *
mp_store_float(char *data, float val)
{
	union mp_float_cast cast;
	cast.f = val;
	cast.u32 = mp_bswap_u32(cast.u32);
	*(union mp_float_cast *) (data) = cast;
	return data + sizeof(cast);
}

MP_IMPL char *
mp_store_double(char *data, double val)
{
	union mp_double_cast cast;
	cast.d = val;
	cast.u64 = mp_bswap_u64(cast.u64);
	*(union mp_double_cast *) (data) = cast;
	return data + sizeof(cast);
}

#elif __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__

MP_LOAD_STORE(float, float, mp_identity);
MP_LOAD_STORE(double, double, mp_identity);

#else
#error Unsupported __FLOAT_WORD_ORDER__
#endif

#undef mp_identity
#undef MP_LOAD_STORE

/** \endcond */

/*
 * }}}
 */

/*
 * {{{ API definition
 */

/**
 * \brief MsgPack data types
 */
enum mp_type {
	MP_NIL = 0,
	MP_UINT,
	MP_INT,
	MP_STR,
	MP_BIN,
	MP_ARRAY,
	MP_MAP,
	MP_BOOL,
	MP_FLOAT,
	MP_DOUBLE,
	MP_EXT
};

/**
 * \brief Determine MsgPack type by a first byte \a c of encoded data.
 *
 * Example usage:
 * \code
 * assert(MP_ARRAY == mp_typeof(0x90));
 * \endcode
 *
 * \param c - a first byte of encoded data
 * \return MsgPack type
 */
MP_PROTO __attribute__((pure)) enum mp_type
mp_typeof(const char c);

/**
 * \brief Calculate exact buffer size needed to store an array header of
 * \a size elements. Maximum return value is 5. For performance reasons you
 * can preallocate buffer for maximum size without calling the function.
 * \param size - a number of elements
 * \return buffer size in bytes (max is 5)
 */
MP_PROTO __attribute__((const)) uint32_t
mp_sizeof_array(uint32_t size);

/**
 * \brief Encode an array header of \a size elements.
 *
 * All array members must be encoded after the header.
 *
 * Example usage:
 * \code
 * // Encode
 * char buf[1024];
 * char *w = buf;
 * w = mp_encode_array(w, 2)
 * w = mp_encode_uint(w, 10);
 * w = mp_encode_uint(w, 15);
 *
 * // Decode
 * const char *r = buf;
 * uint32_t size = mp_decode_array(&r);
 * for (uint32_t i = 0; i < size; i++) {
 *     uint64_t val = mp_decode_uint(&r);
 * }
 * assert (r == w);
 * \endcode
 * It is your responsibility to ensure that \a data has enough space.
 * \param data - a buffer
 * \param size - a number of elements
 * \return \a data + \link mp_sizeof_array() mp_sizeof_array(size) \endlink
 * \sa mp_sizeof_array
 */
MP_PROTO char *
mp_encode_array(char *data, uint32_t size);

/**
 * \brief Check that \a cur buffer has enough bytes to decode an array header
 * \param cur buffer
 * \param end end of the buffer
 * \retval 0 - buffer has enough bytes
 * \retval > 0 - the number of remaining bytes to read
 * \pre cur < end
 * \pre mp_typeof(*cur) == MP_ARRAY
 */
MP_PROTO __attribute__((pure)) ptrdiff_t
mp_check_array(const char *cur, const char *end);

/**
 * \brief Decode an array header from MsgPack \a data.
 *
 * All array members must be decoded after the header.
 * \param data - the pointer to a buffer
 * \return the number of elements in an array
 * \post *data = *data + mp_sizeof_array(retval)
 * \sa \link mp_encode_array() An usage example \endlink
 */
MP_PROTO uint32_t
mp_decode_array(const char **data);

/**
 * \brief Calculate exact buffer size needed to store a map header of
 * \a size elements. Maximum return value is 5. For performance reasons you
 * can preallocate buffer for maximum size without calling the function.
 * \param size - a number of elements
 * \return buffer size in bytes (max is 5)
 */
MP_PROTO __attribute__((const)) uint32_t
mp_sizeof_map(uint32_t size);

/**
 * \brief Encode a map header of \a size elements.
 *
 * All map key-value pairs must be encoded after the header.
 *
 * Example usage:
 * \code
 * char buf[1024];
 *
 * // Encode
 * char *w = buf;
 * w = mp_encode_map(b, 2);
 * w = mp_encode_str(b, "key1", 4);
 * w = mp_encode_str(b, "value1", 6);
 * w = mp_encode_str(b, "key2", 4);
 * w = mp_encode_str(b, "value2", 6);
 *
 * // Decode
 * const char *r = buf;
 * uint32_t size = mp_decode_map(&r);
 * for (uint32_t i = 0; i < size; i++) {
 *      // Use switch(mp_typeof(**r)) to support more types
 *     uint32_t key_len, val_len;
 *     const char *key = mp_decode_str(&r, key_len);
 *     const char *val = mp_decode_str(&r, val_len);
 * }
 * assert (r == w);
 * \endcode
 * It is your responsibility to ensure that \a data has enough space.
 * \param data - a buffer
 * \param size - a number of key/value pairs
 * \return \a data + \link mp_sizeof_map() mp_sizeof_map(size)\endlink
 * \sa mp_sizeof_map
 */
MP_PROTO char *
mp_encode_map(char *data, uint32_t size);

/**
 * \brief Check that \a cur buffer has enough bytes to decode a map header
 * \param cur buffer
 * \param end end of the buffer
 * \retval 0 - buffer has enough bytes
 * \retval > 0 - the number of remaining bytes to read
 * \pre cur < end
 * \pre mp_typeof(*cur) == MP_MAP
 */
MP_PROTO __attribute__((pure)) ptrdiff_t
mp_check_map(const char *cur, const char *end);

/**
 * \brief Decode a map header from MsgPack \a data.
 *
 * All map key-value pairs must be decoded after the header.
 * \param data - the pointer to a buffer
 * \return the number of key/value pairs in a map
 * \post *data = *data + mp_sizeof_array(retval)
 * \sa \link mp_encode_map() An usage example \endlink
 */
MP_PROTO uint32_t
mp_decode_map(const char **data);

/**
 * \brief Calculate exact buffer size needed to store an integer \a num.
 * Maximum return value is 9. For performance reasons you can preallocate
 * buffer for maximum size without calling the function.
 * Example usage:
 * \code
 * char **data = ...;
 * char *end = *data;
 * my_buffer_ensure(mp_sizeof_uint(x), &end);
 * // my_buffer_ensure(9, &end);
 * mp_encode_uint(buffer, x);
 * \endcode
 * \param num - a number
 * \return buffer size in bytes (max is 9)
 */
MP_PROTO __attribute__((const)) uint32_t
mp_sizeof_uint(uint64_t num);

/**
 * \brief Calculate exact buffer size needed to store an integer \a num.
 * Maximum return value is 9. For performance reasons you can preallocate
 * buffer for maximum size without calling the function.
 * \param num - a number
 * \return buffer size in bytes (max is 9)
 * \pre \a num < 0
 */
MP_PROTO __attribute__((const)) uint32_t
mp_sizeof_int(int64_t num);

/**
 * \brief Encode an unsigned integer \a num.
 * It is your responsibility to ensure that \a data has enough space.
 * \param data - a buffer
 * \param num - a number
 * \return \a data + mp_sizeof_uint(\a num)
 * \sa \link mp_encode_array() An usage example \endlink
 * \sa mp_sizeof_uint()
 */
MP_PROTO char *
mp_encode_uint(char *data, uint64_t num);

/**
 * \brief Encode a signed integer \a num.
 * It is your responsibility to ensure that \a data has enough space.
 * \param data - a buffer
 * \param num - a number
 * \return \a data + mp_sizeof_int(\a num)
 * \sa \link mp_encode_array() An usage example \endlink
 * \sa mp_sizeof_int()
 * \pre \a num < 0
 */
MP_PROTO char *
mp_encode_int(char *data, int64_t num);

/**
 * \brief Check that \a cur buffer has enough bytes to decode an uint
 * \param cur buffer
 * \param end end of the buffer
 * \retval 0 - buffer has enough bytes
 * \retval > 0 - the number of remaining bytes to read
 * \pre cur < end
 * \pre mp_typeof(*cur) == MP_UINT
 */
MP_PROTO __attribute__((pure)) ptrdiff_t
mp_check_uint(const char *cur, const char *end);

/**
 * \brief Check that \a cur buffer has enough bytes to decode an int
 * \param cur buffer
 * \param end end of the buffer
 * \retval 0 - buffer has enough bytes
 * \retval > 0 - the number of remaining bytes to read
 * \pre cur < end
 * \pre mp_typeof(*cur) == MP_INT
 */
MP_PROTO __attribute__((pure)) ptrdiff_t
mp_check_int(const char *cur, const char *end);

/**
 * \brief Decode an unsigned integer from MsgPack \a data
 * \param data - the pointer to a buffer
 * \return an unsigned number
 * \post *data = *data + mp_sizeof_uint(retval)
 */
MP_PROTO uint64_t
mp_decode_uint(const char **data);

/**
 * \brief Decode a signed integer from MsgPack \a data
 * \param data - the pointer to a buffer
 * \return an unsigned number
 * \post *data = *data + mp_sizeof_int(retval)
 */
MP_PROTO int64_t
mp_decode_int(const char **data);

/**
 * \brief Compare two packed unsigned integers.
 *
 * The function is faster than two mp_decode_uint() calls.
 * \param data_a unsigned int a
 * \param data_b unsigned int b
 * \retval < 0 when \a a < \a b
 * \retval   0 when \a a == \a b
 * \retval > 0 when \a a > \a b
 */
MP_PROTO __attribute__((pure)) int
mp_compare_uint(const char *data_a, const char *data_b);

/**
 * \brief Calculate exact buffer size needed to store a float \a num.
 * The return value is always 5. The function was added to provide integrity of
 * the library.
 * \param num - a float
 * \return buffer size in bytes (always 5)
 */
MP_PROTO __attribute__((const)) uint32_t
mp_sizeof_float(float num);

/**
 * \brief Calculate exact buffer size needed to store a double \a num.
 * The return value is either 5 or 9. The function was added to provide
 * integrity of the library. For performance reasons you can preallocate buffer
 * for maximum size without calling the function.
 * \param num - a double
 * \return buffer size in bytes (5 or 9)
 */
MP_PROTO __attribute__((const)) uint32_t
mp_sizeof_double(double num);

/**
 * \brief Encode a float \a num.
 * It is your responsibility to ensure that \a data has enough space.
 * \param data - a buffer
 * \param num - a float
 * \return \a data + mp_sizeof_float(\a num)
 * \sa mp_sizeof_float()
 * \sa \link mp_encode_array() An usage example \endlink
 */
MP_PROTO char *
mp_encode_float(char *data, float num);

/**
 * \brief Encode a double \a num.
 * It is your responsibility to ensure that \a data has enough space.
 * \param data - a buffer
 * \param num - a float
 * \return \a data + mp_sizeof_double(\a num)
 * \sa \link mp_encode_array() An usage example \endlink
 * \sa mp_sizeof_double()
 */
MP_PROTO char *
mp_encode_double(char *data, double num);

/**
 * \brief Check that \a cur buffer has enough bytes to decode a float
 * \param cur buffer
 * \param end end of the buffer
 * \retval 0 - buffer has enough bytes
 * \retval > 0 - the number of remaining bytes to read
 * \pre cur < end
 * \pre mp_typeof(*cur) == MP_FLOAT
 */
MP_PROTO __attribute__((pure)) ptrdiff_t
mp_check_float(const char *cur, const char *end);

/**
 * \brief Check that \a cur buffer has enough bytes to decode a double
 * \param cur buffer
 * \param end end of the buffer
 * \retval 0 - buffer has enough bytes
 * \retval > 0 - the number of remaining bytes to read
 * \pre cur < end
 * \pre mp_typeof(*cur) == MP_DOUBLE
 */
MP_PROTO __attribute__((pure)) ptrdiff_t
mp_check_double(const char *cur, const char *end);

/**
 * \brief Decode a float from MsgPack \a data
 * \param data - the pointer to a buffer
 * \return a float
 * \post *data = *data + mp_sizeof_float(retval)
 */
MP_PROTO float
mp_decode_float(const char **data);

/**
 * \brief Decode a double from MsgPack \a data
 * \param data - the pointer to a buffer
 * \return a double
 * \post *data = *data + mp_sizeof_double(retval)
 */
MP_PROTO double
mp_decode_double(const char **data);

/**
 * \brief Calculate exact buffer size needed to store a string header of
 * length \a num. Maximum return value is 5. For performance reasons you can
 * preallocate buffer for maximum size without calling the function.
 * \param len - a string length
 * \return size in chars (max is 5)
 */
MP_PROTO __attribute__((const)) uint32_t
mp_sizeof_strl(uint32_t len);

/**
 * \brief Equivalent to mp_sizeof_strl(\a len) + \a len.
 * \param len - a string length
 * \return size in chars (max is 5 + \a len)
 */
MP_PROTO __attribute__((const)) uint32_t
mp_sizeof_str(uint32_t len);

/**
 * \brief Calculate exact buffer size needed to store a binstring header of
 * length \a num. Maximum return value is 5. For performance reasons you can
 * preallocate buffer for maximum size without calling the function.
 * \param len - a string length
 * \return size in chars (max is 5)
 */
MP_PROTO __attribute__((const)) uint32_t
mp_sizeof_binl(uint32_t len);

/**
 * \brief Equivalent to mp_sizeof_binl(\a len) + \a len.
 * \param len - a string length
 * \return size in chars (max is 5 + \a len)
 */
MP_PROTO __attribute__((const)) uint32_t
mp_sizeof_bin(uint32_t len);

/**
 * \brief Encode a string header of length \a len.
 *
 * The function encodes MsgPack header (\em only header) for a string of
 * length \a len. You should append actual string data to the buffer manually
 * after encoding the header (exactly \a len bytes without trailing '\0').
 *
 * This approach is very useful for cases when the total length of the string
 * is known in advance, but the string data is not stored in a single
 * continuous buffer (e.g. network packets).
 *
 * It is your responsibility to ensure that \a data has enough space.
 * Usage example:
 * \code
 * char buffer[1024];
 * char *b = buffer;
 * b = mp_encode_strl(b, hdr.total_len);
 * char *s = b;
 * memcpy(b, pkt1.data, pkt1.len)
 * b += pkt1.len;
 * // get next packet
 * memcpy(b, pkt2.data, pkt2.len)
 * b += pkt2.len;
 * // get next packet
 * memcpy(b, pkt1.data, pkt3.len)
 * b += pkt3.len;
 *
 * // Check that all data was received
 * assert(hdr.total_len == (uint32_t) (b - s))
 * \endcode
 * Hint: you can dynamically reallocate the buffer during the process.
 * \param data - a buffer
 * \param len - a string length
 * \return \a data + mp_sizeof_strl(len)
 * \sa mp_sizeof_strl()
 */
MP_PROTO char *
mp_encode_strl(char *data, uint32_t len);

/**
 * \brief Encode a string of length \a len.
 * The function is equivalent to mp_encode_strl() + memcpy.
 * \param data - a buffer
 * \param str - a pointer to string data
 * \param len - a string length
 * \return \a data + mp_sizeof_str(len) ==
 * data + mp_sizeof_strl(len) + len
 * \sa mp_encode_strl
 */
MP_PROTO char *
mp_encode_str(char *data, const char *str, uint32_t len);

/**
 * \brief Encode a binstring header of length \a len.
 * See mp_encode_strl() for more details.
 * \param data - a bufer
 * \param len - a string length
 * \return data + mp_sizeof_binl(\a len)
 * \sa mp_encode_strl
 */
MP_PROTO char *
mp_encode_binl(char *data, uint32_t len);

/**
 * \brief Encode a binstring of length \a len.
 * The function is equivalent to mp_encode_binl() + memcpy.
 * \param data - a buffer
 * \param str - a pointer to binstring data
 * \param len - a binstring length
 * \return \a data + mp_sizeof_bin(\a len) ==
 * data + mp_sizeof_binl(\a len) + \a len
 * \sa mp_encode_strl
 */
MP_PROTO char *
mp_encode_bin(char *data, const char *str, uint32_t len);

/**
 * \brief Encode a sequence of values according to format string.
 * Example: mp_format(buf, sz, "[%d {%d%s%d%s}]", 42, 0, "false", 1, "true");
 * to get a msgpack array of two items: number 42 and map (0->"false, 2->"true")
 * Does not write items that don't fit to data_size argument.
 *
 * \param data - a buffer
 * \param data_size - a buffer size
 * \param format - zero-end string, containing structure of resulting
 * msgpack and types of next arguments.
 * Format can contain '[' and ']' pairs, defining arrays,
 * '{' and '}' pairs, defining maps, and format specifiers, described below:
 * %d, %i - int
 * %u - unsigned int
 * %ld, %li - long
 * %lu - unsigned long
 * %lld, %lli - long long
 * %llu - unsigned long long
 * %hd, %hi - short
 * %hu - unsigned short
 * %hhd, %hhi - char (as number)
 * %hhu - unsigned char (as number)
 * %f - float
 * %lf - double
 * %b - bool
 * %s - zero-end string
 * %.*s - string with specified length
 * %p - MsgPack data
 * %.*p - MsgPack data with specified length
 * %% is ignored
 * %smthelse assert and undefined behaviour
 * NIL - a nil value
 * all other symbols are ignored.
 *
 * \return the number of requred bytes.
 * \retval > data_size means that is not enough space
 * and whole msgpack was not encoded.
 */
size_t
mp_format(char *data, size_t data_size, const char *format, ...);

/**
 * \brief mp_format variation, taking variable argument list
 * Example:
 *  va_list args;
 *  va_start(args, fmt);
 *  mp_vformat(data, data_size, fmt, args);
 *  va_end(args);
 * \sa \link mp_format() mp_format() \endlink
 */
size_t
mp_vformat(char *data, size_t data_size, const char *format, va_list args);

/**
 * \brief print MsgPack data \a file using JSON-like format.
 * MP_EXT is printed as "undefined"
 * \param file - pointer to file (or NULL for stdout)
 * \param data - pointer to buffer containing msgpack object
 * \retval >=0 - the number of bytes printed
 * \retval -1 - error
 * \sa fprintf()
 */
int
mp_fprint(FILE *file, const char *data);

/**
 * \brief format MsgPack data to \a buf using JSON-like format.
 * \sa mp_fprint()
 * \param buf - buffer to use
 * \param size - buffer size. This function write at most size bytes
 * (including the terminating null byte ('\0').
 * \param data - pointer to buffer containing msgpack object
 * \retval <size - the number of characters printed (excluding the null byte)
 * \retval >=size - the number of characters (excluding the null byte),
 *                  which would have been written to the final string if
 *                  enough space had been available.
 * \retval -1 - error
 * \sa snprintf()
 */
int
mp_snprint(char *buf, int size, const char *data);

/**
 * \brief Check that \a cur buffer has enough bytes to decode a string header
 * \param cur buffer
 * \param end end of the buffer
 * \retval 0 - buffer has enough bytes
 * \retval > 0 - the number of remaining bytes to read
 * \pre cur < end
 * \pre mp_typeof(*cur) == MP_STR
 */
MP_PROTO __attribute__((pure)) ptrdiff_t
mp_check_strl(const char *cur, const char *end);

/**
 * \brief Check that \a cur buffer has enough bytes to decode a binstring header
 * \param cur buffer
 * \param end end of the buffer
 * \retval 0 - buffer has enough bytes
 * \retval > 0 - the number of remaining bytes to read
 * \pre cur < end
 * \pre mp_typeof(*cur) == MP_BIN
 */
MP_PROTO __attribute__((pure)) ptrdiff_t
mp_check_binl(const char *cur, const char *end);

/**
 * \brief Decode a length of a string from MsgPack \a data
 * \param data - the pointer to a buffer
 * \return a length of astring
 * \post *data = *data + mp_sizeof_strl(retval)
 * \sa mp_encode_strl
 */
MP_PROTO uint32_t
mp_decode_strl(const char **data);

/**
 * \brief Decode a string from MsgPack \a data
 * \param data - the pointer to a buffer
 * \param len - the pointer to save a string length
 * \return a pointer to a decoded string
 * \post *data = *data + mp_sizeof_str(*len)
 * \sa mp_encode_binl
 */
MP_PROTO const char *
mp_decode_str(const char **data, uint32_t *len);

/**
 * \brief Decode a length of a binstring from MsgPack \a data
 * \param data - the pointer to a buffer
 * \return a length of a binstring
 * \post *data = *data + mp_sizeof_binl(retval)
 * \sa mp_encode_binl
 */
MP_PROTO uint32_t
mp_decode_binl(const char **data);

/**
 * \brief Decode a binstring from MsgPack \a data
 * \param data - the pointer to a buffer
 * \param len - the pointer to save a binstring length
 * \return a pointer to a decoded binstring
 * \post *data = *data + mp_sizeof_str(*len)
 * \sa mp_encode_binl
 */
MP_PROTO const char *
mp_decode_bin(const char **data, uint32_t *len);

/**
 * \brief Decode a length of a string or binstring from MsgPack \a data
 * \param data - the pointer to a buffer
 * \return a length of a string
 * \post *data = *data + mp_sizeof_strbinl(retval)
 * \sa mp_encode_binl
 */
MP_PROTO uint32_t
mp_decode_strbinl(const char **data);

/**
 * \brief Decode a string or binstring from MsgPack \a data
 * \param data - the pointer to a buffer
 * \param len - the pointer to save a binstring length
 * \return a pointer to a decoded binstring
 * \post *data = *data + mp_sizeof_strbinl(*len)
 * \sa mp_encode_binl
 */
MP_PROTO const char *
mp_decode_strbin(const char **data, uint32_t *len);

/**
 * \brief Calculate exact buffer size needed to store the nil value.
 * The return value is always 1. The function was added to provide integrity of
 * the library.
 * \return buffer size in bytes (always 1)
 */
MP_PROTO __attribute__((const)) uint32_t
mp_sizeof_nil(void);

/**
 * \brief Encode the nil value.
 * It is your responsibility to ensure that \a data has enough space.
 * \param data - a buffer
 * \return \a data + mp_sizeof_nil()
 * \sa \link mp_encode_array() An usage example \endlink
 * \sa mp_sizeof_nil()
 */
MP_PROTO char *
mp_encode_nil(char *data);

/**
 * \brief Check that \a cur buffer has enough bytes to decode nil
 * \param cur buffer
 * \param end end of the buffer
 * \retval 0 - buffer has enough bytes
 * \retval > 0 - the number of remaining bytes to read
 * \pre cur < end
 * \pre mp_typeof(*cur) == MP_NIL
 */
MP_PROTO __attribute__((pure)) ptrdiff_t
mp_check_nil(const char *cur, const char *end);

/**
 * \brief Decode the nil value from MsgPack \a data
 * \param data - the pointer to a buffer
 * \post *data = *data + mp_sizeof_nil()
 */
MP_PROTO void
mp_decode_nil(const char **data);

/**
 * \brief Calculate exact buffer size needed to store a boolean value.
 * The return value is always 1. The function was added to provide integrity of
 * the library.
 * \return buffer size in bytes (always 1)
 */
MP_PROTO __attribute__((const)) uint32_t
mp_sizeof_bool(bool val);

/**
 * \brief Encode a bool value \a val.
 * It is your responsibility to ensure that \a data has enough space.
 * \param data - a buffer
 * \param val - a bool
 * \return \a data + mp_sizeof_bool(val)
 * \sa \link mp_encode_array() An usage example \endlink
 * \sa mp_sizeof_bool()
 */
MP_PROTO char *
mp_encode_bool(char *data, bool val);

/**
 * \brief Check that \a cur buffer has enough bytes to decode a bool value
 * \param cur buffer
 * \param end end of the buffer
 * \retval 0 - buffer has enough bytes
 * \retval > 0 - the number of remaining bytes to read
 * \pre cur < end
 * \pre mp_typeof(*cur) == MP_BOOL
 */
MP_PROTO __attribute__((pure)) ptrdiff_t
mp_check_bool(const char *cur, const char *end);

/**
 * \brief Decode a bool value from MsgPack \a data
 * \param data - the pointer to a buffer
 * \return a decoded bool value
 * \post *data = *data + mp_sizeof_bool(retval)
 */
MP_PROTO bool
mp_decode_bool(const char **data);

/**
 * \brief Decode an integer value as int32_t from MsgPack \a data.
 * \param data - the pointer to a buffer
 * \param[out] ret - the pointer to save a result
 * \retval  0 on success
 * \retval -1 if underlying mp type is not MP_INT or MP_UINT
 * \retval -1 if the result can't be stored in int32_t
 */
MP_PROTO int
mp_read_int32(const char **data, int32_t *ret);

/**
 * \brief Decode an integer value as int64_t from MsgPack \a data.
 * \param data - the pointer to a buffer
 * \param[out] ret - the pointer to save a result
 * \retval  0 on success
 * \retval -1 if underlying mp type is not MP_INT or MP_UINT
 * \retval -1 if the result can't be stored in int64_t
 */
MP_PROTO int
mp_read_int64(const char **data, int64_t *ret);

/**
 * \brief Decode a floating point value as double from MsgPack \a data.
 * \param data - the pointer to a buffer
 * \param[out] ret - the pointer to save a result
 * \retval  0 on success
 * \retval -1 if underlying mp type is not MP_INT, MP_UINT,
 *            MP_FLOAT, or MP_DOUBLE
 * \retval -1 if the result can't be stored in double
 */
MP_PROTO int
mp_read_double(const char **data, double *ret);

/**
 * \brief Skip one element in a packed \a data.
 *
 * The function is faster than mp_typeof + mp_decode_XXX() combination.
 * For arrays and maps the function also skips all members.
 * For strings and binstrings the function also skips the string data.
 *
 * Usage example:
 * \code
 * char buf[1024];
 *
 * char *w = buf;
 * // First MsgPack object
 * w = mp_encode_uint(w, 10);
 *
 * // Second MsgPack object
 * w = mp_encode_array(w, 4);
 *    w = mp_encode_array(w, 2);
 *         // Begin of an inner array
 *         w = mp_encode_str(w, "second inner 1", 14);
 *         w = mp_encode_str(w, "second inner 2", 14);
 *         // End of an inner array
 *    w = mp_encode_str(w, "second", 6);
 *    w = mp_encode_uint(w, 20);
 *    w = mp_encode_bool(w, true);
 *
 * // Third MsgPack object
 * w = mp_encode_str(w, "third", 5);
 * // EOF
 *
 * const char *r = buf;
 *
 * // First MsgPack object
 * assert(mp_typeof(**r) == MP_UINT);
 * mp_next(&r); // skip the first object
 *
 * // Second MsgPack object
 * assert(mp_typeof(**r) == MP_ARRAY);
 * mp_decode_array(&r);
 *     assert(mp_typeof(**r) == MP_ARRAY); // inner array
 *     mp_next(&r); // -->> skip the entire inner array (with all members)
 *     assert(mp_typeof(**r) == MP_STR); // second
 *     mp_next(&r);
 *     assert(mp_typeof(**r) == MP_UINT); // 20
 *     mp_next(&r);
 *     assert(mp_typeof(**r) == MP_BOOL); // true
 *     mp_next(&r);
 *
 * // Third MsgPack object
 * assert(mp_typeof(**r) == MP_STR); // third
 * mp_next(&r);
 *
 * assert(r == w); // EOF
 *
 * \endcode
 * \param data - the pointer to a buffer
 * \post *data = *data + mp_sizeof_TYPE() where TYPE is mp_typeof(**data)
 */
MP_PROTO void
mp_next(const char **data);

/**
 * \brief Equivalent to mp_next() but also validates MsgPack in \a data.
 * \param data - the pointer to a buffer
 * \param end - the end of a buffer
 * \retval 0 when MsgPack in \a data is valid.
 * \retval != 0 when MsgPack in \a data is not valid.
 * \post *data = *data + mp_sizeof_TYPE() where TYPE is mp_typeof(**data)
 * \post *data is not defined if MsgPack is not valid
 * \sa mp_next()
 */
MP_PROTO int
mp_check(const char **data, const char *end);

/*
 * }}}
 */

/*
 * {{{ Implementation
 */

/** \cond false */
extern const enum mp_type mp_type_hint[];
extern const int8_t mp_parser_hint[];
extern const char *mp_char2escape[];

MP_IMPL MP_ALWAYSINLINE enum mp_type
mp_typeof(const char c)
{
	return mp_type_hint[(uint8_t) c];
}

MP_IMPL uint32_t
mp_sizeof_array(uint32_t size)
{
	if (size <= 15) {
		return 1;
	} else if (size <= UINT16_MAX) {
		return 1 + sizeof(uint16_t);
	} else {
		return 1 + sizeof(uint32_t);
	}
}

MP_IMPL char *
mp_encode_array(char *data, uint32_t size)
{
	if (size <= 15) {
		return mp_store_u8(data, 0x90 | size);
	} else if (size <= UINT16_MAX) {
		data = mp_store_u8(data, 0xdc);
		data = mp_store_u16(data, size);
		return data;
	} else {
		data = mp_store_u8(data, 0xdd);
		return mp_store_u32(data, size);
	}
}

MP_IMPL ptrdiff_t
mp_check_array(const char *cur, const char *end)
{
	assert(cur < end);
	assert(mp_typeof(*cur) == MP_ARRAY);
	uint8_t c = mp_load_u8(&cur);
	if (mp_likely(!(c & 0x40)))
		return cur - end;

	assert(c >= 0xdc && c <= 0xdd); /* must be checked above by mp_typeof */
	uint32_t hsize = 2U << (c & 0x1); /* 0xdc->2, 0xdd->4 */
	return hsize - (end - cur);
}

MP_PROTO uint32_t
mp_decode_array_slowpath(uint8_t c, const char **data);

MP_IMPL uint32_t
mp_decode_array_slowpath(uint8_t c, const char **data)
{
	uint32_t size;
	switch (c & 0x1) {
	case 0xdc & 0x1:
		size = mp_load_u16(data);
		return size;
	case 0xdd & 0x1:
		size = mp_load_u32(data);
		return size;
	default:
		mp_unreachable();
	}
}

MP_IMPL MP_ALWAYSINLINE uint32_t
mp_decode_array(const char **data)
{
	uint8_t c = mp_load_u8(data);

	if (mp_likely(!(c & 0x40)))
		return (c & 0xf);

	return mp_decode_array_slowpath(c, data);
}

MP_IMPL uint32_t
mp_sizeof_map(uint32_t size)
{
	if (size <= 15) {
		return 1;
	} else if (size <= UINT16_MAX) {
		return 1 + sizeof(uint16_t);
	} else {
		return 1 + sizeof(uint32_t);
	}
}

MP_IMPL char *
mp_encode_map(char *data, uint32_t size)
{
	if (size <= 15) {
		return mp_store_u8(data, 0x80 | size);
	} else if (size <= UINT16_MAX) {
		data = mp_store_u8(data, 0xde);
		data = mp_store_u16(data, size);
		return data;
	} else {
		data = mp_store_u8(data, 0xdf);
		data = mp_store_u32(data, size);
		return data;
	}
}

MP_IMPL ptrdiff_t
mp_check_map(const char *cur, const char *end)
{
	assert(cur < end);
	assert(mp_typeof(*cur) == MP_MAP);
	uint8_t c = mp_load_u8(&cur);
	if (mp_likely((c & ~0xfU) == 0x80))
		return cur - end;

	assert(c >= 0xde && c <= 0xdf); /* must be checked above by mp_typeof */
	uint32_t hsize = 2U << (c & 0x1); /* 0xde->2, 0xdf->4 */
	return hsize - (end - cur);
}

MP_IMPL uint32_t
mp_decode_map(const char **data)
{
	uint8_t c = mp_load_u8(data);
	switch (c) {
	case 0xde:
		return mp_load_u16(data);
	case 0xdf:
		return mp_load_u32(data);
	default:
		if (mp_unlikely(c < 0x80 || c > 0x8f))
			mp_unreachable();
		return c & 0xf;
	}
}

MP_IMPL uint32_t
mp_sizeof_uint(uint64_t num)
{
	if (num <= 0x7f) {
		return 1;
	} else if (num <= UINT8_MAX) {
		return 1 + sizeof(uint8_t);
	} else if (num <= UINT16_MAX) {
		return 1 + sizeof(uint16_t);
	} else if (num <= UINT32_MAX) {
		return 1 + sizeof(uint32_t);
	} else {
		return 1 + sizeof(uint64_t);
	}
}

MP_IMPL uint32_t
mp_sizeof_int(int64_t num)
{
	assert(num < 0);
	if (num >= -0x20) {
		return 1;
	} else if (num >= INT8_MIN && num <= INT8_MAX) {
		return 1 + sizeof(int8_t);
	} else if (num >= INT16_MIN && num <= UINT16_MAX) {
		return 1 + sizeof(int16_t);
	} else if (num >= INT32_MIN && num <= UINT32_MAX) {
		return 1 + sizeof(int32_t);
	} else {
		return 1 + sizeof(int64_t);
	}
}

MP_IMPL ptrdiff_t
mp_check_uint(const char *cur, const char *end)
{
	assert(cur < end);
	assert(mp_typeof(*cur) == MP_UINT);
	uint8_t c = mp_load_u8(&cur);
	return mp_parser_hint[c] - (end - cur);
}

MP_IMPL ptrdiff_t
mp_check_int(const char *cur, const char *end)
{
	assert(cur < end);
	assert(mp_typeof(*cur) == MP_INT);
	uint8_t c = mp_load_u8(&cur);
	return mp_parser_hint[c] - (end - cur);
}

MP_IMPL char *
mp_encode_uint(char *data, uint64_t num)
{
	if (num <= 0x7f) {
		return mp_store_u8(data, num);
	} else if (num <= UINT8_MAX) {
		data = mp_store_u8(data, 0xcc);
		return mp_store_u8(data, num);
	} else if (num <= UINT16_MAX) {
		data = mp_store_u8(data, 0xcd);
		return mp_store_u16(data, num);
	} else if (num <= UINT32_MAX) {
		data = mp_store_u8(data, 0xce);
		return mp_store_u32(data, num);
	} else {
		data = mp_store_u8(data, 0xcf);
		return mp_store_u64(data, num);
	}
}

MP_IMPL char *
mp_encode_int(char *data, int64_t num)
{
	assert(num < 0);
	if (num >= -0x20) {
		return mp_store_u8(data, 0xe0 | num);
	} else if (num >= INT8_MIN) {
		data = mp_store_u8(data, 0xd0);
		return mp_store_u8(data, num);
	} else if (num >= INT16_MIN) {
		data = mp_store_u8(data, 0xd1);
		return mp_store_u16(data, num);
	} else if (num >= INT32_MIN) {
		data = mp_store_u8(data, 0xd2);
		return mp_store_u32(data, num);
	} else {
		data = mp_store_u8(data, 0xd3);
		return mp_store_u64(data, num);
	}
}

MP_IMPL uint64_t
mp_decode_uint(const char **data)
{
	uint8_t c = mp_load_u8(data);
	switch (c) {
	case 0xcc:
		return mp_load_u8(data);
	case 0xcd:
		return mp_load_u16(data);
	case 0xce:
		return mp_load_u32(data);
	case 0xcf:
		return mp_load_u64(data);
	default:
		if (mp_unlikely(c > 0x7f))
			mp_unreachable();
		return c;
	}
}

MP_IMPL int
mp_compare_uint(const char *data_a, const char *data_b)
{
	uint8_t ca = mp_load_u8(&data_a);
	uint8_t cb = mp_load_u8(&data_b);

	int r = ca - cb;
	if (r != 0)
		return r;

	if (ca <= 0x7f)
		return 0;

	uint64_t a, b;
	switch (ca & 0x3) {
	case 0xcc & 0x3:
		a = mp_load_u8(&data_a);
		b = mp_load_u8(&data_b);
		break;
	case 0xcd & 0x3:
		a = mp_load_u16(&data_a);
		b = mp_load_u16(&data_b);
		break;
	case 0xce & 0x3:
		a = mp_load_u32(&data_a);
		b = mp_load_u32(&data_b);
		break;
	case 0xcf & 0x3:
		a = mp_load_u64(&data_a);
		b = mp_load_u64(&data_b);
		return a < b ? -1 : a > b;
		break;
	default:
		mp_unreachable();
	}

	int64_t v = (a - b);
	return (v > 0) - (v < 0);
}

MP_IMPL int64_t
mp_decode_int(const char **data)
{
	uint8_t c = mp_load_u8(data);
	switch (c) {
	case 0xd0:
		return (int8_t) mp_load_u8(data);
	case 0xd1:
		return (int16_t) mp_load_u16(data);
	case 0xd2:
		return (int32_t) mp_load_u32(data);
	case 0xd3:
		return (int64_t) mp_load_u64(data);
	default:
		if (mp_unlikely(c < 0xe0))
			mp_unreachable();
		return (int8_t) (c);
	}
}

MP_IMPL uint32_t
mp_sizeof_float(float num)
{
	(void) num;
	return 1 + sizeof(float);
}

MP_IMPL uint32_t
mp_sizeof_double(double num)
{
	(void) num;
	return 1 + sizeof(double);
}

MP_IMPL ptrdiff_t
mp_check_float(const char *cur, const char *end)
{
	assert(cur < end);
	assert(mp_typeof(*cur) == MP_FLOAT);
	return 1 + sizeof(float) - (end - cur);
}

MP_IMPL ptrdiff_t
mp_check_double(const char *cur, const char *end)
{
	assert(cur < end);
	assert(mp_typeof(*cur) == MP_DOUBLE);
	return 1 + sizeof(double) - (end - cur);
}

MP_IMPL char *
mp_encode_float(char *data, float num)
{
	data = mp_store_u8(data, 0xca);
	return mp_store_float(data, num);
}

MP_IMPL char *
mp_encode_double(char *data, double num)
{
	data = mp_store_u8(data, 0xcb);
	return mp_store_double(data, num);
}

MP_IMPL float
mp_decode_float(const char **data)
{
	uint8_t c = mp_load_u8(data);
	assert(c == 0xca);
	(void) c;
	return mp_load_float(data);
}

MP_IMPL double
mp_decode_double(const char **data)
{
	uint8_t c = mp_load_u8(data);
	assert(c == 0xcb);
	(void) c;
	return mp_load_double(data);
}

MP_IMPL uint32_t
mp_sizeof_strl(uint32_t len)
{
	if (len <= 31) {
		return 1;
	} else if (len <= UINT8_MAX) {
		return 1 + sizeof(uint8_t);
	} else if (len <= UINT16_MAX) {
		return 1 + sizeof(uint16_t);
	} else {
		return 1 + sizeof(uint32_t);
	}
}

MP_IMPL uint32_t
mp_sizeof_str(uint32_t len)
{
	return mp_sizeof_strl(len) + len;
}

MP_IMPL uint32_t
mp_sizeof_binl(uint32_t len)
{
	if (len <= UINT8_MAX) {
		return 1 + sizeof(uint8_t);
	} else if (len <= UINT16_MAX) {
		return 1 + sizeof(uint16_t);
	} else {
		return 1 + sizeof(uint32_t);
	}
}

MP_IMPL uint32_t
mp_sizeof_bin(uint32_t len)
{
	return mp_sizeof_binl(len) + len;
}

MP_IMPL char *
mp_encode_strl(char *data, uint32_t len)
{
	if (len <= 31) {
		return mp_store_u8(data, 0xa0 | (uint8_t) len);
	} else if (len <= UINT8_MAX) {
		data = mp_store_u8(data, 0xd9);
		return mp_store_u8(data, len);
	} else if (len <= UINT16_MAX) {
		data = mp_store_u8(data, 0xda);
		return mp_store_u16(data, len);
	} else {
		data = mp_store_u8(data, 0xdb);
		return mp_store_u32(data, len);
	}
}

MP_IMPL char *
mp_encode_str(char *data, const char *str, uint32_t len)
{
	data = mp_encode_strl(data, len);
	memcpy(data, str, len);
	return data + len;
}

MP_IMPL char *
mp_encode_binl(char *data, uint32_t len)
{
	if (len <= UINT8_MAX) {
		data = mp_store_u8(data, 0xc4);
		return mp_store_u8(data, len);
	} else if (len <= UINT16_MAX) {
		data = mp_store_u8(data, 0xc5);
		return mp_store_u16(data, len);
	} else {
		data = mp_store_u8(data, 0xc6);
		return mp_store_u32(data, len);
	}
}

MP_IMPL char *
mp_encode_bin(char *data, const char *str, uint32_t len)
{
	data = mp_encode_binl(data, len);
	memcpy(data, str, len);
	return data + len;
}

MP_IMPL ptrdiff_t
mp_check_strl(const char *cur, const char *end)
{
	assert(cur < end);
	assert(mp_typeof(*cur) == MP_STR);

	uint8_t c = mp_load_u8(&cur);
	if (mp_likely(c & ~0x1f) == 0xa0)
		return cur - end;

	assert(c >= 0xd9 && c <= 0xdb); /* must be checked above by mp_typeof */
	uint32_t hsize = 1U << (c & 0x3) >> 1; /* 0xd9->1, 0xda->2, 0xdb->4 */
	return hsize - (end - cur);
}

MP_IMPL ptrdiff_t
mp_check_binl(const char *cur, const char *end)
{
	uint8_t c = mp_load_u8(&cur);
	assert(cur < end);
	assert(mp_typeof(c) == MP_BIN);

	assert(c >= 0xc4 && c <= 0xc6); /* must be checked above by mp_typeof */
	uint32_t hsize = 1U << (c & 0x3); /* 0xc4->1, 0xc5->2, 0xc6->4 */
	return hsize - (end - cur);
}

MP_IMPL uint32_t
mp_decode_strl(const char **data)
{
	uint8_t c = mp_load_u8(data);
	switch (c) {
	case 0xd9:
		return mp_load_u8(data);
	case 0xda:
		return mp_load_u16(data);
	case 0xdb:
		return mp_load_u32(data);
	default:
		if (mp_unlikely(c < 0xa0 || c > 0xbf))
			mp_unreachable();
		return c & 0x1f;
	}
}

MP_IMPL const char *
mp_decode_str(const char **data, uint32_t *len)
{
	assert(len != NULL);

	*len = mp_decode_strl(data);
	const char *str = *data;
	*data += *len;
	return str;
}

MP_IMPL uint32_t
mp_decode_binl(const char **data)
{
	uint8_t c = mp_load_u8(data);

	switch (c) {
	case 0xc4:
		return mp_load_u8(data);
	case 0xc5:
		return mp_load_u16(data);
	case 0xc6:
		return mp_load_u32(data);
	default:
		mp_unreachable();
	}
}

MP_IMPL const char *
mp_decode_bin(const char **data, uint32_t *len)
{
	assert(len != NULL);

	*len = mp_decode_binl(data);
	const char *str = *data;
	*data += *len;
	return str;
}

MP_IMPL uint32_t
mp_decode_strbinl(const char **data)
{
	uint8_t c = mp_load_u8(data);

	switch (c) {
	case 0xd9:
		return mp_load_u8(data);
	case 0xda:
		return mp_load_u16(data);
	case 0xdb:
		return mp_load_u32(data);
	case 0xc4:
		return mp_load_u8(data);
	case 0xc5:
		return mp_load_u16(data);
	case 0xc6:
		return mp_load_u32(data);
	default:
		if (mp_unlikely(c < 0xa0 || c > 0xbf))
			mp_unreachable();
		return c & 0x1f;
	}
}

MP_IMPL const char *
mp_decode_strbin(const char **data, uint32_t *len)
{
	assert(len != NULL);

	*len = mp_decode_strbinl(data);
	const char *str = *data;
	*data += *len;
	return str;
}

MP_IMPL uint32_t
mp_sizeof_nil()
{
	return 1;
}

MP_IMPL char *
mp_encode_nil(char *data)
{
	return mp_store_u8(data, 0xc0);
}

MP_IMPL ptrdiff_t
mp_check_nil(const char *cur, const char *end)
{
	assert(cur < end);
	assert(mp_typeof(*cur) == MP_NIL);
	return 1 - (end - cur);
}

MP_IMPL void
mp_decode_nil(const char **data)
{
	uint8_t c = mp_load_u8(data);
	assert(c == 0xc0);
	(void) c;
}

MP_IMPL uint32_t
mp_sizeof_bool(bool val)
{
	(void) val;
	return 1;
}

MP_IMPL char *
mp_encode_bool(char *data, bool val)
{
	return mp_store_u8(data, 0xc2 | (val & 1));
}

MP_IMPL ptrdiff_t
mp_check_bool(const char *cur, const char *end)
{
	assert(cur < end);
	assert(mp_typeof(*cur) == MP_BOOL);
	return 1 - (end - cur);
}

MP_IMPL bool
mp_decode_bool(const char **data)
{
	uint8_t c = mp_load_u8(data);
	switch (c) {
	case 0xc3:
		return true;
	case 0xc2:
		return false;
	default:
		mp_unreachable();
	}
}

MP_IMPL int
mp_read_int32(const char **data, int32_t *ret)
{
	uint32_t uval;
	const char *p = *data;
	uint8_t c = mp_load_u8(&p);
	switch (c) {
	case 0xd0:
		*ret = (int8_t) mp_load_u8(&p);
		break;
	case 0xd1:
		*ret = (int16_t) mp_load_u16(&p);
		break;
	case 0xd2:
		*ret = (int32_t) mp_load_u32(&p);
		break;
	case 0xcc:
		*ret = mp_load_u8(&p);
		break;
	case 0xcd:
		*ret = mp_load_u16(&p);
		break;
	case 0xce:
		uval = mp_load_u32(&p);
		if (mp_unlikely(uval > INT32_MAX))
			return -1;
		*ret = uval;
		break;
	default:
		if (mp_unlikely(c < 0xe0 && c > 0x7f))
			return -1;
		*ret = (int8_t) c;
		break;
	}
	*data = p;
	return 0;
}

MP_IMPL int
mp_read_int64(const char **data, int64_t *ret)
{
	uint64_t uval;
	const char *p = *data;
	uint8_t c = mp_load_u8(&p);
	switch (c) {
	case 0xd0:
		*ret = (int8_t) mp_load_u8(&p);
		break;
	case 0xd1:
		*ret = (int16_t) mp_load_u16(&p);
		break;
	case 0xd2:
		*ret = (int32_t) mp_load_u32(&p);
		break;
	case 0xd3:
		*ret = (int64_t) mp_load_u64(&p);
		break;
	case 0xcc:
		*ret = mp_load_u8(&p);
		break;
	case 0xcd:
		*ret = mp_load_u16(&p);
		break;
	case 0xce:
		*ret = mp_load_u32(&p);
		break;
	case 0xcf:
		uval = mp_load_u64(&p);
		if (uval > INT64_MAX)
			return -1;
		*ret = uval;
		break;
	default:
		if (mp_unlikely(c < 0xe0 && c > 0x7f))
			return -1;
		*ret = (int8_t) c;
		break;
	}
	*data = p;
	return 0;
}

MP_IMPL int
mp_read_double(const char **data, double *ret)
{
	int64_t ival;
	uint64_t uval;
	double val;
	const char *p = *data;
	uint8_t c = mp_load_u8(&p);
	switch (c) {
	case 0xd0:
		*ret = (int8_t) mp_load_u8(&p);
		break;
	case 0xd1:
		*ret = (int16_t) mp_load_u16(&p);
		break;
	case 0xd2:
		*ret = (int32_t) mp_load_u32(&p);
		break;
	case 0xd3:
		val = ival = (int64_t) mp_load_u64(&p);
		if ((int64_t)val != ival)
			return -1;
		*ret = val;
		break;
	case 0xcc:
		*ret = mp_load_u8(&p);
		break;
	case 0xcd:
		*ret = mp_load_u16(&p);
		break;
	case 0xce:
		*ret = mp_load_u32(&p);
		break;
	case 0xcf:
		val = uval = mp_load_u64(&p);
		if ((uint64_t)val != uval)
			return -1;
		*ret = val;
		break;
	case 0xca:
		*ret = mp_load_float(&p);
		break;
	case 0xcb:
		*ret = mp_load_double(&p);
		break;
	default:
		if (mp_unlikely(c < 0xe0 && c > 0x7f))
			return -1;
		*ret = (int8_t) c;
		break;
	}
	*data = p;
	return 0;
}

/** See mp_parser_hint */
enum {
	MP_HINT = -32,
	MP_HINT_STR_8 = MP_HINT,
	MP_HINT_STR_16 = MP_HINT - 1,
	MP_HINT_STR_32 = MP_HINT - 2,
	MP_HINT_ARRAY_16 = MP_HINT - 3,
	MP_HINT_ARRAY_32 = MP_HINT - 4,
	MP_HINT_MAP_16 = MP_HINT - 5,
	MP_HINT_MAP_32 = MP_HINT - 6,
	MP_HINT_EXT_8 = MP_HINT - 7,
	MP_HINT_EXT_16 = MP_HINT - 8,
	MP_HINT_EXT_32 = MP_HINT - 9
};

MP_PROTO void
mp_next_slowpath(const char **data, int64_t k);

MP_IMPL void
mp_next_slowpath(const char **data, int64_t k)
{
	for (; k > 0; k--) {
		uint8_t c = mp_load_u8(data);
		int l = mp_parser_hint[c];
		if (mp_likely(l >= 0)) {
			*data += l;
			continue;
		} else if (mp_likely(l > MP_HINT)) {
			k -= l;
			continue;
		}

		uint32_t len;
		switch (l) {
		case MP_HINT_STR_8:
			/* MP_STR (8) */
			len = mp_load_u8(data);
			*data += len;
			break;
		case MP_HINT_STR_16:
			/* MP_STR (16) */
			len = mp_load_u16(data);
			*data += len;
			break;
		case MP_HINT_STR_32:
			/* MP_STR (32) */
			len = mp_load_u32(data);
			*data += len;
			break;
		case MP_HINT_ARRAY_16:
			/* MP_ARRAY (16) */
			k += mp_load_u16(data);
			break;
		case MP_HINT_ARRAY_32:
			/* MP_ARRAY (32) */
			k += mp_load_u32(data);
			break;
		case MP_HINT_MAP_16:
			/* MP_MAP (16) */
			k += 2 * mp_load_u16(data);
			break;
		case MP_HINT_MAP_32:
			/* MP_MAP (32) */
			k += 2 * mp_load_u32(data);
			break;
		case MP_HINT_EXT_8:
			/* MP_EXT (8) */
			len = mp_load_u8(data);
			mp_load_u8(data);
			*data += len;
			break;
		case MP_HINT_EXT_16:
			/* MP_EXT (16) */
			len = mp_load_u16(data);
			mp_load_u8(data);
			*data += len;
			break;
		case MP_HINT_EXT_32:
			/* MP_EXT (32) */
			len = mp_load_u32(data);
			mp_load_u8(data);
			*data += len;
			break;
		default:
			mp_unreachable();
		}
	}
}

MP_IMPL void
mp_next(const char **data)
{
	int64_t k = 1;
	for (; k > 0; k--) {
		uint8_t c = mp_load_u8(data);
		int l = mp_parser_hint[c];
		if (mp_likely(l >= 0)) {
			*data += l;
			continue;
		} else if (mp_likely(c == 0xd9)){
			/* MP_STR (8) */
			uint8_t len = mp_load_u8(data);
			*data += len;
			continue;
		} else if (l > MP_HINT) {
			k -= l;
			continue;
		} else {
			*data -= sizeof(uint8_t);
			return mp_next_slowpath(data, k);
		}
	}
}

MP_IMPL int
mp_check(const char **data, const char *end)
{
#define MP_CHECK_LEN(_l) \
	if (mp_unlikely((size_t)(end - *data) < (size_t)(_l))) \
		return 1;

	int64_t k;
	for (k = 1; k > 0; k--) {
		MP_CHECK_LEN(1);
		uint8_t c = mp_load_u8(data);
		int l = mp_parser_hint[c];
		if (mp_likely(l >= 0)) {
			MP_CHECK_LEN(l);
			*data += l;
			continue;
		} else if (mp_likely(l > MP_HINT)) {
			k -= l;
			continue;
		}

		uint32_t len;
		switch (l) {
		case MP_HINT_STR_8:
			/* MP_STR (8) */
			MP_CHECK_LEN(sizeof(uint8_t));
			len = mp_load_u8(data);
			MP_CHECK_LEN(len);
			*data += len;
			break;
		case MP_HINT_STR_16:
			/* MP_STR (16) */
			MP_CHECK_LEN(sizeof(uint16_t));
			len = mp_load_u16(data);
			MP_CHECK_LEN(len);
			*data += len;
			break;
		case MP_HINT_STR_32:
			/* MP_STR (32) */
			MP_CHECK_LEN(sizeof(uint32_t))
			len = mp_load_u32(data);
			MP_CHECK_LEN(len);
			*data += len;
			break;
		case MP_HINT_ARRAY_16:
			/* MP_ARRAY (16) */
			MP_CHECK_LEN(sizeof(uint16_t));
			k += mp_load_u16(data);
			break;
		case MP_HINT_ARRAY_32:
			/* MP_ARRAY (32) */
			MP_CHECK_LEN(sizeof(uint32_t));
			k += mp_load_u32(data);
			break;
		case MP_HINT_MAP_16:
			/* MP_MAP (16) */
			MP_CHECK_LEN(sizeof(uint16_t));
			k += 2 * mp_load_u16(data);
			break;
		case MP_HINT_MAP_32:
			/* MP_MAP (32) */
			MP_CHECK_LEN(sizeof(uint32_t));
			k += 2 * mp_load_u32(data);
			break;
		case MP_HINT_EXT_8:
			/* MP_EXT (8) */
			MP_CHECK_LEN(sizeof(uint8_t) + sizeof(uint8_t));
			len = mp_load_u8(data);
			mp_load_u8(data);
			MP_CHECK_LEN(len);
			*data += len;
			break;
		case MP_HINT_EXT_16:
			/* MP_EXT (16) */
			MP_CHECK_LEN(sizeof(uint16_t) + sizeof(uint8_t));
			len = mp_load_u16(data);
			mp_load_u8(data);
			MP_CHECK_LEN(len);
			*data += len;
			break;
		case MP_HINT_EXT_32:
			/* MP_EXT (32) */
			MP_CHECK_LEN(sizeof(uint32_t) + sizeof(uint8_t));
			len = mp_load_u32(data);
			mp_load_u8(data);
			MP_CHECK_LEN(len);
			*data += len;
			break;
		default:
			mp_unreachable();
		}
	}

	assert(*data <= end);
#undef MP_CHECK_LEN
	return 0;
}

/** \endcond */

/*
 * }}}
 */

#if defined(__cplusplus)
//} /* extern "C" */
#endif /* defined(__cplusplus) */

#undef MP_LIBRARY
#undef MP_PROTO
#undef MP_IMPL
#undef MP_ALWAYSINLINE
#undef MP_GCC_VERSION

#endif /* MSGPUCK_H_INCLUDED */

/*
   COPY: file name = tnt/uri.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/
#ifndef TARANTOOL_URI_H_INCLUDED
#define TARANTOOL_URI_H_INCLUDED
/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stddef.h>
//#include <netdb.h> /* NI_MAXHOST, NI_MAXSERV */
#include <limits.h> /* _POSIX_PATH_MAX */

#if defined(__cplusplus)
//extern "C" {
#endif /* defined(__cplusplus) */

struct uri {
    const char *scheme;
    size_t scheme_len;
    const char *login;
    size_t login_len;
    const char *password;
    size_t password_len;
    const char *host;
    size_t host_len;
    const char *service;
    size_t service_len;
    const char *path;
    size_t path_len;
    const char *query;
    size_t query_len;
    const char *fragment;
    size_t fragment_len;
    int host_hint;
};

#define URI_HOST_UNIX "unix/"
#define URI_MAXHOST NI_MAXHOST
#define URI_MAXSERVICE _POSIX_PATH_MAX /* _POSIX_PATH_MAX always > NI_MAXSERV */

int
uri_parse(struct uri *uri, const char *str);

char *
uri_format(const struct uri *uri);

enum uri_host_hint {
    URI_NAME = 0,
    URI_IPV4 = 1,
    URI_IPV6 = 2,
    URI_UNIX = 3,
};

#if defined(__cplusplus)
//} /* extern "C" */
#endif /* defined(__cplusplus) */

#endif /* TARANTOOL_URI_H_INCLUDED */


/*
   COPY: file name = include/tarantool/tnt_proto.h.
   Copying Date = 2017-09-14.
   Changes: none. But now SQL_INFO = 0x42, I don't know when that happened on the server.
*/

#ifndef TNT_PROTO_H_INCLUDED
#define TNT_PROTO_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file tnt_proto.h
 * \brief IProto protocol constants
 */

/**
 * \brief Request/response header field types (keys)
 */
enum tnt_header_key_t {
    TNT_CODE      = 0x00,
    TNT_SYNC      = 0x01,
    TNT_SERVER_ID = 0x02,
    TNT_LSN       = 0x03,
    TNT_TIMESTAMP = 0x04,
    TNT_SCHEMA_ID = 0x05
};

/**
 * \brief Request body field types (keys)
 */
enum tnt_body_key_t {
    TNT_SPACE = 0x10,
    TNT_INDEX = 0x11,
    TNT_LIMIT = 0x12,
    TNT_OFFSET = 0x13,
    TNT_ITERATOR = 0x14,
    TNT_INDEX_BASE = 0x15,
    TNT_KEY = 0x20,
    TNT_TUPLE = 0x21,
    TNT_FUNCTION = 0x22,
    TNT_USERNAME = 0x23,
    TNT_SERVER_UUID = 0x24,
    TNT_CLUSTER_UUID = 0x25,
    TNT_VCLOCK = 0x26,
    TNT_EXPRESSION = 0x27,
    TNT_OPS = 0x28,
    TNT_SQL_TEXT = 0x40,
    TNT_SQL_BIND = 0x41,
};

/**
 * \brief Response body field types (keys)
 */
enum tnt_response_key_t {
    TNT_DATA = 0x30,
    TNT_ERROR = 0x31,
    TNT_METADATA = 0x32,
    TNT_SQL_INFO = 0x42,
};

/**
 * \brief Request types
 */
enum tnt_request_t {
    TNT_OP_SELECT    = 1,
    TNT_OP_INSERT    = 2,
    TNT_OP_REPLACE   = 3,
    TNT_OP_UPDATE    = 4,
    TNT_OP_DELETE    = 5,
    TNT_OP_CALL_16   = 6,
    TNT_OP_AUTH      = 7,
    TNT_OP_EVAL      = 8,
    TNT_OP_UPSERT    = 9,
    TNT_OP_CALL      = 10,
    TNT_OP_EXECUTE   = 11,
    TNT_OP_PING      = 64,
    TNT_OP_JOIN      = 65,
    TNT_OP_SUBSCRIBE = 66
};

/**
 * \brief Update operations
 */
enum tnt_update_op_t {
    TNT_UOP_ADDITION = '+',
    TNT_UOP_SUBSTRACT = '-',
    TNT_UOP_AND = '&',
    TNT_UOP_XOR = '^',
    TNT_UOP_OR = '|',
    TNT_UOP_DELETE = '#',
    TNT_UOP_INSERT = '!',
    TNT_UOP_ASSIGN = '=',
    TNT_UOP_SPLICE = ':',
};

/**
 * \brief Iterator types
 */
enum tnt_iterator_t {
    TNT_ITER_EQ = 0,
    TNT_ITER_REQ,
    TNT_ITER_ALL,
    TNT_ITER_LT,
    TNT_ITER_LE,
    TNT_ITER_GE,
    TNT_ITER_GT,
    TNT_ITER_BITS_ALL_SET,
    TNT_ITER_BITS_ANY_SET,
    TNT_ITER_BITS_ALL_NOT_SET,
    TNT_ITER_OVERLAP,
    TNT_ITER_NEIGHBOR,
};

/**
 * \internal
 */
#define TNT_SCRAMBLE_SIZE 20
/**
 * \internal
 */
#define TNT_GREETING_SIZE 128
/**
 * \internal
 */
#define TNT_VERSION_SIZE  64
/**
 * \internal
 */
#define TNT_SALT_SIZE     44

/**
 * \brief System spaces
 */
enum tnt_spaces_t {
    tnt_sp_space = 280,
    tnt_sp_index = 288,
    tnt_sp_func  = 296,
    tnt_sp_user  = 304,
    tnt_sp_priv  = 312,
    tnt_vsp_space = 281,
    tnt_vsp_index = 289,
    tnt_vsp_func  = 297,
    tnt_vsp_user  = 305,
    tnt_vsp_priv  = 313,
};

/**
 * \brief System indexes
 */
enum tnt_indexes_t {
    tnt_vin_primary = 0,
    tnt_vin_owner   = 1,
    tnt_vin_name    = 2,
};

/**
 * \brief Error code types
 */
enum tnt_errcode_t {
    TNT_ER_UNKNOWN = 0,
    TNT_ER_ILLEGAL_PARAMS = 1,
    TNT_ER_MEMORY_ISSUE = 2,
    TNT_ER_TUPLE_FOUND = 3,
    TNT_ER_TUPLE_NOT_FOUND = 4,
    TNT_ER_UNSUPPORTED = 5,
    TNT_ER_NONMASTER = 6,
    TNT_ER_READONLY = 7,
    TNT_ER_INJECTION = 8,
    TNT_ER_CREATE_SPACE = 9,
    TNT_ER_SPACE_EXISTS = 10,
    TNT_ER_DROP_SPACE = 11,
    TNT_ER_ALTER_SPACE = 12,
    TNT_ER_INDEX_TYPE = 13,
    TNT_ER_MODIFY_INDEX = 14,
    TNT_ER_LAST_DROP = 15,
    TNT_ER_TUPLE_FORMAT_LIMIT = 16,
    TNT_ER_DROP_PRIMARY_KEY = 17,
    TNT_ER_KEY_PART_TYPE = 18,
    TNT_ER_EXACT_MATCH = 19,
    TNT_ER_INVALID_MSGPACK = 20,
    TNT_ER_PROC_RET = 21,
    TNT_ER_TUPLE_NOT_ARRAY = 22,
    TNT_ER_FIELD_TYPE = 23,
    TNT_ER_FIELD_TYPE_MISMATCH = 24,
    TNT_ER_SPLICE = 25,
    TNT_ER_ARG_TYPE = 26,
    TNT_ER_TUPLE_IS_TOO_LONG = 27,
    TNT_ER_UNKNOWN_UPDATE_OP = 28,
    TNT_ER_UPDATE_FIELD = 29,
    TNT_ER_FIBER_STACK = 30,
    TNT_ER_KEY_PART_COUNT = 31,
    TNT_ER_PROC_LUA = 32,
    TNT_ER_NO_SUCH_PROC = 33,
    TNT_ER_NO_SUCH_TRIGGER = 34,
    TNT_ER_NO_SUCH_INDEX = 35,
    TNT_ER_NO_SUCH_SPACE = 36,
    TNT_ER_NO_SUCH_FIELD = 37,
    TNT_ER_SPACE_FIELD_COUNT = 38,
    TNT_ER_INDEX_FIELD_COUNT = 39,
    TNT_ER_WAL_IO = 40,
    TNT_ER_MORE_THAN_ONE_TUPLE = 41,
    TNT_ER_ACCESS_DENIED = 42,
    TNT_ER_CREATE_USER = 43,
    TNT_ER_DROP_USER = 44,
    TNT_ER_NO_SUCH_USER = 45,
    TNT_ER_USER_EXISTS = 46,
    TNT_ER_PASSWORD_MISMATCH = 47,
    TNT_ER_UNKNOWN_REQUEST_TYPE = 48,
    TNT_ER_UNKNOWN_SCHEMA_OBJECT = 49,
    TNT_ER_CREATE_FUNCTION = 50,
    TNT_ER_NO_SUCH_FUNCTION = 51,
    TNT_ER_FUNCTION_EXISTS = 52,
    TNT_ER_FUNCTION_ACCESS_DENIED = 53,
    TNT_ER_FUNCTION_MAX = 54,
    TNT_ER_SPACE_ACCESS_DENIED = 55,
    TNT_ER_USER_MAX = 56,
    TNT_ER_NO_SUCH_ENGINE = 57,
    TNT_ER_RELOAD_CFG = 58,
    TNT_ER_CFG = 59,
    TNT_ER_SOPHIA = 60,
    TNT_ER_LOCAL_SERVER_IS_NOT_ACTIVE = 61,
    TNT_ER_UNKNOWN_SERVER = 62,
    TNT_ER_CLUSTER_ID_MISMATCH = 63,
    TNT_ER_INVALID_UUID = 64,
    TNT_ER_CLUSTER_ID_IS_RO = 65,
    TNT_ER_RESERVED66 = 66,
    TNT_ER_SERVER_ID_IS_RESERVED = 67,
    TNT_ER_INVALID_ORDER = 68,
    TNT_ER_MISSING_REQUEST_FIELD = 69,
    TNT_ER_IDENTIFIER = 70,
    TNT_ER_DROP_FUNCTION = 71,
    TNT_ER_ITERATOR_TYPE = 72,
    TNT_ER_REPLICA_MAX = 73,
    TNT_ER_INVALID_XLOG = 74,
    TNT_ER_INVALID_XLOG_NAME = 75,
    TNT_ER_INVALID_XLOG_ORDER = 76,
    TNT_ER_NO_CONNECTION = 77,
    TNT_ER_TIMEOUT = 78,
    TNT_ER_ACTIVE_TRANSACTION = 79,
    TNT_ER_NO_ACTIVE_TRANSACTION = 80,
    TNT_ER_CROSS_ENGINE_TRANSACTION = 81,
    TNT_ER_NO_SUCH_ROLE = 82,
    TNT_ER_ROLE_EXISTS = 83,
    TNT_ER_CREATE_ROLE = 84,
    TNT_ER_INDEX_EXISTS = 85,
    TNT_ER_TUPLE_REF_OVERFLOW = 86,
    TNT_ER_ROLE_LOOP = 87,
    TNT_ER_GRANT = 88,
    TNT_ER_PRIV_GRANTED = 89,
    TNT_ER_ROLE_GRANTED = 90,
    TNT_ER_PRIV_NOT_GRANTED = 91,
    TNT_ER_ROLE_NOT_GRANTED = 92,
    TNT_ER_MISSING_SNAPSHOT = 93,
    TNT_ER_CANT_UPDATE_PRIMARY_KEY = 94,
    TNT_ER_UPDATE_INTEGER_OVERFLOW = 95,
    TNT_ER_GUEST_USER_PASSWORD = 96,
    TNT_ER_TRANSACTION_CONFLICT = 97,
    TNT_ER_UNSUPPORTED_ROLE_PRIV = 98,
    TNT_ER_LOAD_FUNCTION = 99,
    TNT_ER_FUNCTION_LANGUAGE = 100,
    TNT_ER_RTREE_RECT = 101,
    TNT_ER_PROC_C = 102,
    TNT_ER_UNKNOWN_RTREE_INDEX_DISTANCE_TYPE = 103,
    TNT_ER_PROTOCOL = 104,
    TNT_ER_UPSERT_UNIQUE_SECONDARY_KEY = 105,
    TNT_ER_WRONG_INDEX_RECORD = 106,
    TNT_ER_WRONG_INDEX_PARTS = 107,
    TNT_ER_WRONG_INDEX_OPTIONS = 108,
    TNT_ER_WRONG_SCHEMA_VERSION = 109,
    TNT_ER_SLAB_ALLOC_MAX = 110,
};

#endif /* TNT_PROTO_H_INCLUDED */

/*
   COPY: file name = tnt/tnt_proto_internal.h.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #include lines.
*/
#ifndef TNT_PROTO_INTERNAL_H_INCLUDED
#define TNT_PROTO_INTERNAL_H_INCLUDED

//#include <msgpuck.h>

//#include <tarantool/tnt_proto.h>

struct tnt_iheader {
    char header[25];
    char *end;
};

static inline bool
is_call(enum tnt_request_t op) {
    return (op == TNT_OP_CALL || op == TNT_OP_CALL_16);
}

static inline int
encode_header(struct tnt_iheader *hdr, uint32_t code, uint64_t sync)
{
    memset(hdr, 0, sizeof(struct tnt_iheader));
    char *h = mp_encode_map(hdr->header, 2);
    h = mp_encode_uint(h, TNT_CODE);
    h = mp_encode_uint(h, code);
    h = mp_encode_uint(h, TNT_SYNC);
    h = mp_encode_uint(h, sync);
    hdr->end = h;
    return 0;
}

static inline size_t
mp_sizeof_luint32(uint64_t num) {
    if (num <= UINT32_MAX)
        return 1 + sizeof(uint32_t);
    return 1 + sizeof(uint64_t);
}

static inline char *
mp_encode_luint32(char *data, uint64_t num) {
    if (num <= UINT32_MAX) {
        data = mp_store_u8(data, 0xce);
        return mp_store_u32(data, num);
    }
    data = mp_store_u8(data, 0xcf);
    return mp_store_u64(data, num);
}
#endif /* TNT_PROTO_INTERNAL_H */

/*
   COPY: file name = include/tarantool/tnt_opt.h.
   Copying Date = 2017-09-14.
   Changes: none.*/

#ifndef TNT_OPT_H_INCLUDED
#define TNT_OPT_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/time.h>

/**
 * \file tnt_opt.h
 * \brief Networking layer options
 */

struct tnt_iob;

/**
 * \brief Callback type for read (instead of reading from socket)
 *
 * \param b   context for read operation
 * \param buf buf to read to
 * \param len size to read
 *
 * \returns size that was read
 * \retval  -1 error, errno must be set
 */
typedef ssize_t (*recv_cb_t)(struct tnt_iob *b, void *buf, size_t len);

/**
 * \brief Callback type for write (instead of writing into socket)
 *
 * \param b   context for write operation
 * \param buf buf to write
 * \param len size to write
 *
 * \returns size that was written
 * \retval  -1 error, errno must be set
 */
typedef ssize_t (*send_cb_t)(struct tnt_iob *b, void *buf, size_t len);

/**
 * \brief Callback type for write with iovec (instead of writing into socket)
 *
 * \param b   context for write operation
 * \param buf iovec to write
 * \param len iovec len
 *
 * \returns size that was written
 * \retval  -1 error, errno must be set
 */
typedef ssize_t (*sendv_cb_t)(struct tnt_iob *b, const struct iovec *iov, int iov_count);

/**
 * \brief Options for connection
 */
enum tnt_opt_type {
    TNT_OPT_URI, /*!< Options for setting URI */
    TNT_OPT_TMOUT_CONNECT, /*!< Option for setting timeout on connect */
    TNT_OPT_TMOUT_RECV, /*!< Option for setting timeout on recv */
    TNT_OPT_TMOUT_SEND, /*!< Option for setting timeout in send */
    TNT_OPT_SEND_CB, /*!< callback, that's executed on send
              * \sa send_cb_t
              */
    TNT_OPT_SEND_CBV, /*!< callback, that's executed on send with iovector
               * \sa sendv_cb_t
               */
    TNT_OPT_SEND_CB_ARG, /*!< callback context for send */
    TNT_OPT_SEND_BUF, /*!< Option for setting send buffer size */
    TNT_OPT_RECV_CB,  /*!< callback, that's executed on recv */
    TNT_OPT_RECV_CB_ARG, /*!< callback context for recv
                  * \sa recv_cb_t
                  */
    TNT_OPT_RECV_BUF /*!< Option for setting recv buffer size */
};

/**
 * \internal
 * \brief structure, that is used for options
 */
struct tnt_opt {
    const char *uristr;
    struct uri *uri;
    struct timeval tmout_connect;
    struct timeval tmout_recv;
    struct timeval tmout_send;
    void *send_cb;
    void *send_cbv;
    void *send_cb_arg;
    int send_buf;
    void *recv_cb;
    void *recv_cb_arg;
    int recv_buf;
};

/**
 * \internal
 */
int
tnt_opt_init(struct tnt_opt *opt);

/**
 * \internal
 */
void
tnt_opt_free(struct tnt_opt *opt);

/**
 * \internal
 */
int
tnt_opt_set(struct tnt_opt *opt, enum tnt_opt_type name, va_list args);

#endif /* TNT_OPT_H_INCLUDED */

/*
   COPY: file name = include/tarantool/tnt_reply.h.
   Copying Date = 2017-09-14.
   Changes: rename int tnt_reply() xtnt_reply() because it shadows struct tnt_reply.
*/


#ifndef TNT_REPLY_H_INCLUDED
#define TNT_REPLY_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file tnt_reply.h
 * \brief Basic reply structure (parsing responses, e.t.c)
 */

/**
 * \brief Size of iproto header
 */
#define TNT_REPLY_IPROTO_HDR_SIZE	5

/**
 * \brief Callback for recv reply from buffer
 *
 * \param ptr  pointer to buffer and offset
 * \param dst  copy reply to
 * \param size size to recv to
 *
 * \returns size of bytes written
 * \retval  -1 error
 */
typedef ssize_t (*tnt_reply_t)(void *ptr, char *dst, ssize_t size);

/*!
 * \brief basic reply structure
 */
struct tnt_reply {
    int alloc;		/*!< allocation mark */
    uint64_t bitmap;	/*!< bitmap of field IDs that was read */
    const char *buf;	/*!< points to beginning of buffer */
    size_t buf_size;	/*!< size of query buffer */
    uint64_t code;		/*!< response code (0 is success, error number if not) */
    uint64_t sync;		/*!< synchronization id */
    uint64_t schema_id;	/*!< unique schema id */
    const char *error;	/*!< error message (NULL if not present) */
    const char *error_end;	/*!< end of error message (NULL if not present) */
    const char *data;	/*!< tuple data (NULL if not present) */
    const char *data_end;	/*!< end if tuple data (NULL if not present) */
    const char *metadata;	/*!< tuple metadata (NULL if not present) */
    const char *metadata_end; /*!< end if tuple metadata (NULL if not present) */
    const char *sqlinfo;	/*!< map sqlinfo (NULL if not present) */
    const char *sqlinfo_end;/*!< end if map sqlinfo (NULL if not present) */
};

/*!
 * \brief Get error number
 */
#define TNT_REPLY_ERR(R) ((R)->code)

/*!
 * \brief Allocate and init reply object
 *
 * if reply pointer is NULL, then new stream will be created
 *
 * \param r reply object pointer
 *
 * \returns reply object pointer
 * \retval  NULL memory allocation failure
 */
struct tnt_reply *
tnt_reply_init(struct tnt_reply *r);

/*!
 * \brief Free previously inited reply object
 *
 * \param r reply object pointer
 */
void
tnt_reply_free(struct tnt_reply *r);

/*!
 * \brief process buffer as iproto reply
 *
 * \param[in]  r    reply object pointer
 * \param[in]  buf  buffer data pointer
 * \param[in]  size buffer data size
 * \param[out] off  returned offset, may be NULL
 *
 * if reply is fully read, then zero is returned and offset set to the
 * end of reply data in buffer.
 *
 * if reply is not complete, then 1 is returned and offset set to the
 * size needed to read.
 *
 * if there were error while parsing reply, -1 is returned.
 *
 * \returns status of processing reply
 * \retval 0  read reply
 * \retval 1  need 'offset' bytes more
 * \retval -1 error while parsing request
 */
int
xtnt_reply(struct tnt_reply *r, char *buf, size_t size, size_t *off);

/*!
 * \brief Process iproto reply with supplied recv function
 *
 * \param r   reply object pointer
 * \param rcv supplied recv function
 * \param ptr recv function argument
 *
 * \returns status of parsing
 * \retval  0 ok
 * \retval -1 error, while parsing response
 */
int
tnt_reply_from(struct tnt_reply *r, tnt_reply_t rcv, void *ptr);

/*!
 * \brief Process buffer as reply header without copying processed bytes
 *
 * \param[in]  r    reply object pointer
 * \param[in]  buf  buffer data pointer
 * \param[in]  size buffer data size
 * \param[out] off  returned offset, may be NULL
 *
 * if buffer contains valid reply header, then zero is returned and offset set to the
 * end of reply header in buffer.
 *
 * if there were error while parsing buffer, -1 is returned.
 *
 * \returns status of processing reply
 * \retval 0  process reply header
 * \retval -1 error while parsing buffer
 */
int
tnt_reply_hdr0(struct tnt_reply *r, const char *buf, size_t size, size_t *off);

/*!
 * \brief Process buffer as reply body without copying processed bytes
 *
 * \param[in]  r    reply object pointer
 * \param[in]  buf  buffer data pointer
 * \param[in]  size buffer data size
 * \param[out] off  returned offset, may be NULL
 *
 * if buffer contains valid reply body, then zero is returned and offset set to the
 * end of reply body in buffer.
 *
 * if there were error while parsing buffer, -1 is returned.
 *
 * \returns status of processing reply
 * \retval 0  process reply body
 * \retval -1 error while parsing buffer
 */
int
tnt_reply_body0(struct tnt_reply *r, const char *buf, size_t size, size_t *off);

/*!
 * \brief Process buffer as reply without copying processed bytes
 *
 * \param[in]  r    reply object pointer
 * \param[in]  buf  buffer data pointer
 * \param[in]  size buffer data size
 * \param[out] off  returned offset, may be NULL
 *
 * if buffer contains full and valid reply, then zero is returned and offset set to the
 * end of reply in buffer.
 *
 * if buffer doesn't contain full reply, then 1 is returned and offset set to the
 * size needed to read.
 *
 * if there were error while parsing buffer, -1 is returned.
 *
 * \returns status of processing reply
 * \retval 0  process reply
 * \retval 1  need 'offset' bytes more
 * \retval -1 error while parsing buffer
 */
int
tnt_reply0(struct tnt_reply *r, const char *buf, size_t size, size_t *off);

#endif /* TNT_REPLY_H_INCLUDED */

/*
   COPY: file name = include/tarantool/tnt_request.h.
   Copying Date = 2017-09-14.
   Changes: commented out #includes.
*/

#ifndef TNT_REQUEST_H_INCLUDED
#define TNT_REQUEST_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file tnt_request.h
 * \brief Request creation using connection schema
 */

//#include <tarantool/tnt_proto.h>

struct tnt_request {
    struct {
        uint64_t sync; /*!< Request sync id. Generated when encoded */
        enum tnt_request_t type; /*!< Request type */
    } hdr; /*!< fields for header */
    uint32_t space_id; /*!< Space number */
    uint32_t index_id; /*!< Index number */
    uint32_t offset; /*!< Offset for select */
    uint32_t limit; /*!< Limit for select */
    enum tnt_iterator_t iterator; /*!< Iterator for select */
    /* Search key, proc name or eval expression */
    const char *key; /*!< Pointer for
              * key for select/update/delete,
              * procedure  for call,
              * expression for eval,
              * operations for upsert
              */
    const char *key_end;
    struct tnt_stream *key_object; /*!< Pointer for key object
                    * if allocated inside requests
                    * functions
                    */
    const char *tuple; /*!< Pointer for
                * tuple for insert/replace,
                * ops for update
                * default tuple for upsert,
                * args for eval/call
                */
    const char *tuple_end;
    struct tnt_stream *tuple_object; /*!< Pointer for tuple object
                      * if allocated inside requests
                      * functions
                      */
    int index_base; /*!< field offset for UPDATE */
    int alloc; /*!< allocation mark */
};

/**
 * \brief Allocate and initialize request object
 *
 * if request pointer is NULL, then new request will be created
 *
 * \param req    pointer to request
 * \param stream pointer to stream for schema (may be NULL)
 *
 * \returns pointer to request object
 * \retval  NULL memory allocation failure
 */
struct tnt_request *
tnt_request_init(struct tnt_request *req);
/**
 * \brief Free request object
 *
 * \param req request object
 */
void
tnt_request_free(struct tnt_request *req);

/**
 * \brief Set request space from number
 *
 * \param req   request object
 * \param space space number
 *
 * \retval 0 ok
 * \sa tnt_request_set_space
 */
int
tnt_request_set_space(struct tnt_request *req, uint32_t space);

/**
 * \brief Set request index from number
 *
 * \param req   request object
 * \param index index number
 *
 * \retval 0  ok
 * \sa tnt_request_set_index
 */
int
tnt_request_set_index(struct tnt_request *req, uint32_t index);

/**
 * \brief Set offset for select
 *
 * \param req    request pointer
 * \param offset offset to set
 *
 * \retval 0 ok
 */
int
tnt_request_set_offset(struct tnt_request *req, uint32_t offset);

/**
 * \brief Set limit for select
 *
 * \param req   request pointer
 * \param limit limit to set
 *
 * \retval 0 ok
 */
int
tnt_request_set_limit(struct tnt_request *req, uint32_t limit);

/**
 * \brief Set iterator for select
 *
 * \param req  request pointer
 * \param iter iter to set
 *
 * \retval 0 ok
 */
int
tnt_request_set_iterator(struct tnt_request *req, enum tnt_iterator_t iter);

/**
 * \brief Set index base for update/upsert operation
 *
 * \param req        request pointer
 * \param index_base field offset to set
 *
 * \retval 0 ok
 */
int
tnt_request_set_index_base(struct tnt_request *req, uint32_t index_base);

/**
 * \brief Set key from predefined object
 *
 * \param req request pointer
 * \param s   tnt_object pointer
 *
 * \retval 0 ok
 */
int
tnt_request_set_key(struct tnt_request *req, struct tnt_stream *s);

/**
 * \brief Set key from print-like function
 *
 * \param req request pointer
 * \param fmt format string
 * \param ... arguments for format string
 *
 * \retval 0  ok
 * \retval -1 oom/format error
 * \sa tnt_object_format
 */
int
tnt_request_set_key_format(struct tnt_request *req, const char *fmt, ...);

/**
 * \brief Set function from string
 *
 * \param req  request pointer
 * \param func function string
 * \param flen function string length
 *
 * \retval 0 ok
 */
int
tnt_request_set_func(struct tnt_request *req, const char *func, uint32_t flen);

/**
 * \brief Set function from NULL-terminated string
 *
 * \param req  request pointer
 * \param func function string
 *
 * \retval 0 ok
 */
int
tnt_request_set_funcz(struct tnt_request *req, const char *func);

/**
 * \brief Set expression from string
 *
 * \param req  request pointer
 * \param expr expression string
 * \param elen expression string length
 *
 * \retval 0  ok
 * \retval -1 error
 */
int
tnt_request_set_expr(struct tnt_request *req, const char *expr, uint32_t elen);

/**
 * \brief Set expression from NULL-terminated string
 *
 * \param req  request pointer
 * \param expr expression string
 *
 * \retval 0  ok
 * \retval -1 error
 */
int
tnt_request_set_exprz(struct tnt_request *req, const char *expr);

/**
 * \brief Set tuple from predefined object
 *
 * \param req request pointer
 * \param s   tnt_object pointer
 *
 * \retval 0 ok
 */
int
tnt_request_set_tuple(struct tnt_request *req, struct tnt_stream *s);

/**
 * \brief Set tuple from print-like function
 *
 * \param req request pointer
 * \param fmt format string
 * \param ... arguments for format string
 *
 * \retval 0  ok
 * \retval -1 oom/format error
 * \sa tnt_object_format
 */
int
tnt_request_set_tuple_format(struct tnt_request *req, const char *fmt, ...);

/**
 * \brief Set operations from predefined object
 *
 * \param req request pointer
 * \param s   tnt_object pointer
 *
 * \retval 0 ok
 */
int
tnt_request_set_ops(struct tnt_request *req, struct tnt_stream *s);

/**
 * \brief Encode request to stream object
 *
 * \param s   stream pointer
 * \param req request pointer
 *
 * \retval >0 ok, sync is returned
 * \retval -1 out of memory
 */
int64_t
tnt_request_compile(struct tnt_stream *s, struct tnt_request *req);

/**
 * \brief Encode request to stream object.
 *
 * \param[in]  s    stream pointer
 * \param[in]  req  request pointer
 * \param[out] sync pointer to compiled request
 *
 * \retval 0  ok
 * \retval -1 out of memory
 */
int
tnt_request_writeout(struct tnt_stream *s, struct tnt_request *req,
             uint64_t *sync);
/**
 * \brief create select request object
 * \sa tnt_request_init
 */
struct tnt_request *
tnt_request_select(struct tnt_request *req);

/**
 * \brief create insert request object
 * \sa tnt_request_init
 */
struct tnt_request *
tnt_request_insert(struct tnt_request *req);

/**
 * \brief create replace request object
 * \sa tnt_request_init
 */
struct tnt_request *
tnt_request_replace(struct tnt_request *req);

/**
 * \brief create update request object
 * \sa tnt_request_init
 */
struct tnt_request *
tnt_request_update(struct tnt_request *req);

/**
 * \brief create delete request object
 * \sa tnt_request_init
 */
struct tnt_request *
tnt_request_delete(struct tnt_request *req);

/**
 * \brief create call request object
 * \sa tnt_request_init
 */
struct tnt_request *
tnt_request_call(struct tnt_request *req);

/**
 * \brief create call request object
 * \sa tnt_request_init
 */
struct tnt_request *
tnt_request_call_16(struct tnt_request *req);

/**
 * \brief create auth request object
 * \sa tnt_request_init
 */
struct tnt_request *
tnt_request_auth(struct tnt_request *req);

/**
 * \brief create eval request object
 * \sa tnt_request_init
 */
struct tnt_request *
tnt_request_eval(struct tnt_request *req);

/**
 * \brief create upsert request object
 * \sa tnt_request_init
 */
struct tnt_request *
tnt_request_upsert(struct tnt_request *req);

/**
 * \brief create ping request object
 * \sa tnt_request_init
 */
struct tnt_request *
tnt_request_ping(struct tnt_request *req);

#endif /* TNT_REQUEST_H_INCLUDED */

/*
   COPY: file name = include/tarantool/tnt_select.h.
   Copying Date = 2017-09-14.
   Changes: removed #includes.
*/
#ifndef TNT_SELECT_H_INCLUDED
#define TNT_SELECT_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

//#include <tarantool/tnt_stream.h>

/**
 * \file tnt_select.h
 * \brief Select request
 */

/**
 * \brief Construct select request and write it into stream
 *
 * \param s        stream object
 * \param space    space no
 * \param index    index no
 * \param limit    limit of tuples to select
 * \param offset   offset of tuples to select
 * \param iterator iterator to use for select
 * \param key      key for select
 *
 * \returns        number of bytes written to stream
 * \retval      -1 oom
 */
ssize_t
tnt_select(struct tnt_stream *s, uint32_t space, uint32_t index,
       uint32_t limit, uint32_t offset, uint8_t iterator,
       struct tnt_stream *key);

#endif /* TNT_SELECT_H_INCLUDED */


/*
   COPY: file name = include/tarantool/tnt_iter.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/
#ifndef TNT_ITER_H_INCLUDED
#define TNT_ITER_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file tnt_iter.h
 * \brief Custom iterator types (msgpack/reply)
 */

/*!
 * iterator types
 */
enum tnt_iter_type {
    TNT_ITER_ARRAY,
    TNT_ITER_MAP,
    TNT_ITER_REPLY,
//	TNT_ITER_REQUEST,
//	TNT_ITER_STORAGE
};

/*!
 * \brief msgpack array iterator
 */
struct tnt_iter_array {
    const char *data; /*!< pointer to the beginning of array */
    const char *first_elem; /*!< pointer to the first element of array */
    const char *elem; /*!< pointer to current element of array */
    const char *elem_end; /*!< pointer to current element end of array */
    uint32_t elem_count; /*!< number of elements in array */
    int cur_index; /*!< index of current element */
};

/* msgpack array iterator accessors */

/**
 * \brief access msgpack array iterator
 */
#define TNT_IARRAY(I) (&(I)->data.array)

/**
 * \brief access current element form iterator
 */
#define TNT_IARRAY_ELEM(I) TNT_IARRAY(I)->elem

/**
 * \brief access end of current element from iterator
 */
#define TNT_IARRAY_ELEM_END(I) TNT_IARRAY(I)->elem_end

/*!
 * \brief msgpack map iterator
 */
struct tnt_iter_map {
    const char *data; /*!< pointer to the beginning of map */
    const char *first_key; /*!< pointer to the first key of map */
    const char *key; /*!< pointer to current key of map */
    const char *key_end; /*!< pointer to current key end */
    const char *value; /*!< pointer to current value of map */
    const char *value_end; /*!< pointer to current value end */
    uint32_t pair_count; /*!< number of key-values pairs in array */
    int cur_index; /*!< index of current pair */
};

/* msgpack array iterator accessors */

/**
 * \brief access msgpack map iterator
 */
#define TNT_IMAP(I) (&(I)->data.map)

/**
 * \brief access current key from iterator
 */
#define TNT_IMAP_KEY(I) TNT_IMAP(I)->key

/**
 * \brief access current key end from iterator
 */
#define TNT_IMAP_KEY_END(I) TNT_IMAP(I)->key_end

/**
 * \brief access current value from iterator
 */
#define TNT_IMAP_VAL(I) TNT_IMAP(I)->value

/**
 * \brief access current value end from iterator
 */
#define TNT_IMAP_VAL_END(I) TNT_IMAP(I)->value_end

/*!
 * \brief reply iterator
 */
struct tnt_iter_reply {
    struct tnt_stream *s; /*!< stream pointer */
    struct tnt_reply r;   /*!< current reply */
};

/* reply iterator accessors */

/**
 * \brief access reply iterator
 */
#define TNT_IREPLY(I) (&(I)->data.reply)

/**
 * \brief access current reply form iterator
 */
#define TNT_IREPLY_PTR(I) &TNT_IREPLY(I)->r

/* request iterator */
// struct tnt_iter_request {
// 	struct tnt_stream *s; /* stream pointer */
// 	struct tnt_request r; /* current request */
// };

/* request iterator accessors */
// #define TNT_IREQUEST(I) (&(I)->data.request)
// #define TNT_IREQUEST_PTR(I) &TNT_IREQUEST(I)->r
// #define TNT_IREQUEST_STREAM(I) TNT_IREQUEST(I)->s

/* storage iterator */
// struct tnt_iter_storage {
// 	struct tnt_stream *s; /* stream pointer */
// 	struct tnt_tuple t;   /* current fetched tuple */
// };

/* storage iterator accessors */
// #define TNT_ISTORAGE(I) (&(I)->data.storage)
// #define TNT_ISTORAGE_TUPLE(I) &TNT_ISTORAGE(I)->t
// #define TNT_ISTORAGE_STREAM(I) TNT_ISTORAGE(I)->s

/**
 * \brief iterator status
 */
enum tnt_iter_status {
    TNT_ITER_OK, /*!< iterator is ok */
    TNT_ITER_FAIL /*!< error or end of iteration */
};

/**
 * \brief Common iterator object
 */
struct tnt_iter {
    enum tnt_iter_type type; /*!< iterator type
                  * \sa enum tnt_iter_type
                  */
    enum tnt_iter_status status; /*!< iterator status
                      * \sa enum tnt_iter_status
                      */
    int alloc; /*!< allocation mark */
    /* interface callbacks */
    int  (*next)(struct tnt_iter *iter); /*!< callback for next element */
    void (*rewind)(struct tnt_iter *iter); /*!< callback for rewind */
    void (*free)(struct tnt_iter *iter); /*!< callback for free of custom iter type */
    /* iterator data */
    union {
        struct tnt_iter_array array; /*!< msgpack array iterator */
        struct tnt_iter_map map; /*!< msgpack map iterator */
        struct tnt_iter_reply reply; /*!< reply iterator */
//		struct tnt_iter_request request;
//		struct tnt_iter_storage storage;
    } data;
};

/**
 * \brief create msgpack array iterator from object
 *
 * if iterator pointer is NULL, then new iterator will be created.
 *
 * \param i pointer to allocated structure
 * \param s tnt_object/tnt_buf instance with array to traverse
 *
 * \returns iterator pointer
 * \retval  NULL on error.
 */
struct tnt_iter *
tnt_iter_array_object(struct tnt_iter *i, struct tnt_stream *s);

/**
 * \brief create msgpack array iterator from pointer
 *
 * if iterator pointer is NULL, then new iterator will be created.
 *
 * \param i pointer to allocated structure
 * \param data pointer to data with array
 * \param size size of data (may be more, it won't go outside)
 *
 * \returns iterator pointer
 * \retval  NULL on error.
 */
struct tnt_iter *
tnt_iter_array(struct tnt_iter *i, const char *data, size_t size);

/**
 * \brief create msgpack map iterator from object
 *
 * if iterator pointer is NULL, then new iterator will be created.
 *
 * \param i pointer to allocated structure
 * \param s tnt_object/tnt_buf instance with map to traverse
 *
 * \returns iterator pointer
 * \retval  NULL error.
 */
struct tnt_iter *
tnt_iter_map_object(struct tnt_iter *i, struct tnt_stream *s);

/**
 * \brief create msgpack map iterator from pointer
 *
 * if iterator pointer is NULL, then new iterator will be created.
 *
 * \param i pointer to allocated structure
 * \param data pointer to data with map
 * \param size size of data (may be more, it won't go outside)
 *
 * \returns iterator pointer
 * \retval  NULL error.
 */
struct tnt_iter *
tnt_iter_map(struct tnt_iter *i, const char *data, size_t size);

/**
 * \brief create and initialize tuple reply iterator;
 *
 * \param i pointer to allocated structure
 * \param s tnt_net stream pointer
 *
 * if stream iterator pointer is NULL, then new stream
 * iterator will be created.
 *
 * \returns stream iterator pointer
 * \retval NULL error.
*/
struct tnt_iter *
tnt_iter_reply(struct tnt_iter *i, struct tnt_stream *s);

// struct tnt_iter *tnt_iter_request(struct tnt_iter *i, struct tnt_stream *s);
// struct tnt_iter *tnt_iter_storag(struct tnt_iter *i, struct tnt_stream *s);

/**
 * \brief free iterator.
 *
 * \param i iterator pointer
 */
void
tnt_iter_free(struct tnt_iter *i);

/**
 * \brief iterate to next element in tuple
 *
 * \param i iterator pointer
 *
 * depend on iterator tuple, sets to the
 * next msgpack field or next response in the stream.
 *
 * \retval 0 end of iteration
 * \retval 1 next step of iteration
 */
int
tnt_next(struct tnt_iter *i);

/**
 * \brief reset iterator pos to beginning
 *
 * \param i iterator pointer
 */
void
tnt_rewind(struct tnt_iter *i);


#endif /* TNT_ITER_H_INCLUDED */

/*
   COPY: file name = include/tarantool/tnt_auth.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/
#ifndef TNT_AUTH_H_INCLUDED
#define TNT_AUTH_H_INCLUDED
/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file tnt_auth.h
 * \brief aithentication request
 */

/**
 * \brief Write authentication request to stream
 *
 * if user == NULL or user == "guest" then it'll be deauth
 *
 * \param s    stream instance
 * \param user user name
 * \param ulen user name length
 * \param pass password
 * \param plen password length
 *
 * \retval number of bytes written to stream
 */
ssize_t
tnt_auth(struct tnt_stream *s, const char *user, int ulen, const char *pass,
     int plen);

/**
 * \brief tnt_auth version with providing base64 encoded salt from tarantool
 */
ssize_t
tnt_auth_raw(struct tnt_stream *s, const char *user, int ulen,
         const char *pass, int plen, const char *base64_salt);
/**
 * \brief Write deauth request to stream
 *
 * shortcut for tnt_auth(s, NULL, 0, NULL, 0)
 *
 * \param s stream instance
 *
 * \retval number of bytes written to stream
 */
ssize_t
tnt_deauth(struct tnt_stream *s);

#endif /* TNT_AUTH_H_INCLUDED */


/*
   COPY: file name = include/tarantool/tnt_stream.h.
   Copying Date = 2017-09-14.
   Changes: commented out #includes.
*/

#ifndef TNT_STREAM_H_INCLUDED
#define TNT_STREAM_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file tnt_stream.h
 * \brief Basic stream object
 */

#include <sys/types.h>
//#include <sys/uio.h>

//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_request.h>

/**
 * \brief Basic stream object
 * all function pointers are NULL, if operation is not supported
 */
struct tnt_stream {
    int alloc; /*!< Allocation mark */
    ssize_t (*write)(struct tnt_stream *s, const char *buf, size_t size); /*!< write to buffer function */
    ssize_t (*writev)(struct tnt_stream *s, struct iovec *iov, int count); /*!< writev function */
    ssize_t (*write_request)(struct tnt_stream *s, struct tnt_request *r, uint64_t *sync); /*!< write request function */

    ssize_t (*read)(struct tnt_stream *s, char *buf, size_t size); /*!< read from buffer function */
    int (*read_reply)(struct tnt_stream *s, struct tnt_reply *r); /*!< read reply from buffer */

    void (*free)(struct tnt_stream *s); /*!< free custom buffer types (destructor) */

    void *data; /*!< subclass data */
    uint32_t wrcnt; /*!< count of write operations */
    uint64_t reqid; /*!< request id of current operation */
};

/**
 * \brief Base function for allocating stream. For internal use only.
 */
struct tnt_stream *tnt_stream_init(struct tnt_stream *s);
/**
 * \brief Base function for freeing stream. For internal use only.
 */
void tnt_stream_free(struct tnt_stream *s);

/**
 * \brief set reqid number. It's incremented at every request compilation.
 * default is 0
 */
uint32_t tnt_stream_reqid(struct tnt_stream *s, uint32_t reqid);

#endif /* TNT_STREAM_H_INCLUDED */

/*
   COPY: file name = include/tarantool/tnt_iob.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/

#ifndef TNT_IOB_H_INCLUDED
#define TNT_IOB_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \internal
 * \file tnt_iob.h
 * \brief Basic network layer static sized buffer
 */

typedef ssize_t (*tnt_iob_tx_t)(void *ptr, const char *buf, size_t size);
typedef ssize_t (*tnt_iob_txv_t)(void *ptr, struct iovec *iov, int count);

struct tnt_iob {
    char *buf;
    size_t off;
    size_t top;
    size_t size;
    tnt_iob_tx_t tx;
    tnt_iob_txv_t txv;
    void *ptr;
};

int
tnt_iob_init(struct tnt_iob *iob, size_t size, tnt_iob_tx_t tx,
         tnt_iob_txv_t txv, void *ptr);

void
tnt_iob_clear(struct tnt_iob *iob);

void
tnt_iob_free(struct tnt_iob *iob);

#endif /* TNT_IOB_H_INCLUDED */

/*
   COPY: file name = include/tarantool/tnt_net.h.
   Copying Date = 2017-09-14.
   Changes: commented out #includes.
            rename enum tnt_error tnt_error() to xtnt_error() because it shadows enum tnt_error, and pre-declare.
*/

#ifndef TNT_NET_H_INCLUDED
#define TNT_NET_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file tnt_net.h
 * \brief Basic tarantool client library header for network stream layer
 */

#ifdef __cplusplus
//extern "C" {
#endif

#include <stdint.h>
#include <stdarg.h>

#include <sys/types.h>
#include <sys/time.h>

//#include <tarantool/tnt_opt.h>
//#include <tarantool/tnt_iob.h>

/**
 * \brief Internal error codes
 */
enum tnt_error {
    TNT_EOK, /*!< Everything is OK */
    TNT_EFAIL, /*!< Fail */
    TNT_EMEMORY, /*!< Memory allocation failed */
    TNT_ESYSTEM, /*!< System error */
    TNT_EBIG, /*!< Buffer is too big */
    TNT_ESIZE, /*!< Bad buffer size */
    TNT_ERESOLVE, /*!< gethostbyname(2) failed */
    TNT_ETMOUT, /*!< Operation timeout */
    TNT_EBADVAL, /*!< Bad argument (value) */
    TNT_ELOGIN, /*!< Failed to login */
    TNT_LAST /*!< Not an error */
};

/**
 * \brief Network stream structure
 */
struct tnt_stream_net {
    struct tnt_opt opt; /*!< Options for connection */
    int connected; /*!< Connection status. 1 - true, 0 - false */
    int fd; /*!< fd of connection */
    struct tnt_iob sbuf; /*!< Send buffer */
    struct tnt_iob rbuf; /*!< Recv buffer */
    enum tnt_error error; /*!< If retval == -1, then error is set. */
    int errno_; /*!< If TNT_ESYSTEM then errno_ is set */
    char *greeting; /*!< Pointer to greeting, if connected */
    struct tnt_schema *schema; /*!< Collation for space/index string<->number */
    int inited; /*!< 1 if iob/schema were allocated */
};

enum tnt_error
xtnt_error(struct tnt_stream *);

/*!
 * \internal
 * \brief Cast tnt_stream to tnt_net
 */
#define TNT_SNET_CAST(S) ((struct tnt_stream_net*)(S)->data)

/**
 * \brief Create tnt_net stream instance
 *
 * \param s stream pointer, maybe NULL
 *
 * If stream pointer is NULL, then new stream will be created.
 *
 * \returns stream pointer
 * \retval NULL oom
 *
 * \code{.c}
 * struct tnt_stream *tnt = tnt_net(NULL);
 * assert(tnt);
 * assert(tnt_set(s, TNT_OPT_URI, "login:passw@localhost:3302") != -1);
 * assert(tnt_connect(s) != -1);
 * ...
 * tnt_close(s);
 * \endcode
 */
struct tnt_stream *
tnt_net(struct tnt_stream *s);

/**
 * \brief Set options for connection
 *
 * \param s   stream pointer
 * \param opt option to set
 * \param ... option value
 *
 * \returns status
 * \retval -1 error
 * \retval  0 ok
 * \sa enum tnt_opt_type
 *
 * \code{.c}
 * assert(tnt_set(s, TNT_OPT_SEND_BUF, 16*1024) != -1);
 * assert(tnt_set(s, TNT_OPT_RECV_BUF, 16*1024) != -1);
 * assert(tnt_set(s, TNT_OPT_URI, "login:passw@localhost:3302") != -1);
 * \endcode
 *
 * \note
 * URI format:
 * * "[login:password@]host:port" for tcp sockets
 * * "[login:password@]/tmp/socket_path.sock" for unix sockets
 * \sa enum tnt_opt_type
 */
int
tnt_set(struct tnt_stream *s, int opt, ...);

/*!
 * \internal
 * \brief Initialize network stream
 *
 * It must happened before connection, but after options are set.
 * 1) creation of tnt_iob's (sbuf,rbuf)
 * 2) schema creation
 *
 * \param s stream for initialization
 *
 * \returns status
 * \retval 0  ok
 * \retval -1 error (oom/einval)
 */
int
tnt_init(struct tnt_stream *s);

/**
 * \brief Connect to tarantool with preconfigured and allocated settings
 *
 * \param s stream pointer
 *
 * \retval 0  ok
 * \retval -1 error (network/oom)
 */
int
tnt_connect(struct tnt_stream *s);

/**
 * \brief Close connection
 * \param s stream pointer
 */
void
tnt_close(struct tnt_stream *s);

/**
 * \brief Send written to buffer queries
 *
 * \param s tnt_stream
 *
 * \returns number of bytes written to socket
 * \retval -1 on network error
 */
ssize_t
tnt_flush(struct tnt_stream *s);

/**
 * \brief Get tnt_net stream fd
 */
int
tnt_fd(struct tnt_stream *s);

/**
 * \brief Error accessor for tnt_net stream
 */
enum tnt_error
tnt_error_function(struct tnt_stream *s);

/**
 * \brief Format error as string
 */
char *
tnt_strerror(struct tnt_stream *s);

/**
 * \brief Get last errno on socket
 */
int
tnt_errno(struct tnt_stream *s);

/**
 * \brief Flush space/index schema and get it from server
 *
 * \param s stream pointer
 *
 * \returns result
 * \retval  -1 error
 * \retval  0  ok
 */
int
tnt_reload_schema(struct tnt_stream *s);

/**
 * \brief Get space number from space name
 *
 * \returns space number
 * \retval  -1 error
 */
int tnt_get_spaceno(struct tnt_stream *s, const char *space, size_t space_len);

/**
 * \brief Get index number from index name and spaceid
 *
 * \returns index number
 * \retval  -1 error
 */
int tnt_get_indexno(struct tnt_stream *s, int spaceno, const char *index,
            size_t index_len);

#ifdef __cplusplus
//}
#endif

#endif /* TNT_NET_H_INCLUDED */

/*
   COPY: file name = tnt/pmatomic.h.
   Copying Date = 2017-09-14.
   Changes: Removed none.
*/

/*-
 * pmatomic.h - Poor Man's atomics
 *
 * Borrowed from FreeBSD (original copyright follows).
 *
 * Standard atomic facilities in stdatomic.h are great, unless you are
 * stuck with an old compiler, or you attempt to compile code using
 * stdatomic.h in C++ mode [gcc 4.9], or if you were desperate enough to
 * enable OpenMP in C mode [gcc 4.9].
 *
 * There are several discrepancies between gcc and clang, namely clang
 * refuses to apply atomic operations to non-atomic types while gcc is
 * more tolerant.
 *
 * For these reasons we provide a custom implementation of operations on
 * atomic types:
 *
 *   A. same names/semantics as in stdatomic.h;
 *   B. all names prefixed with 'pm_' to avoid name collisions;
 *   C. applicable to non-atomic types.
 *
 * Ex:
 *     int i;
 *     pm_atomic_fetch_add_explicit(&i, 1, pm_memory_order_relaxed);
 *
 * Note: do NOT use _Atomic keyword (see gcc issues above).
 */

/*-
 * Migration strategy
 *
 * Switching to <stdatomic.h> will be relatively easy. A
 * straightforward text replace on the codebase removes 'pm_' prefix
 * in names. Compiling with clang reveals missing _Atomic qualifiers.
 */

/*-
 * Logistics
 *
 * In order to make it possible to merge with the updated upstream we
 * restrict modifications in this file to the bare minimum. For this
 * reason we comment unused code regions with #if 0 instead of removing
 * them.
 *
 * Renames are carried out by a script generating the final header.
 */

/*-
 * Copyright (c) 2011 Ed Schouten <ed@FreeBSD.org>
 *                    David Chisnall <theraven@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: releng/10.1/sys/sys/stdatomic.h 264496 2014-04-15 09:41:52Z tijl $
 */

#ifndef PMATOMIC_H__
#define	PMATOMIC_H__

/* Compiler-fu */
#if !defined(__has_feature)
#define __has_feature(x) 0
#endif
#if !defined(__has_builtin)
#define __has_builtin(x) __has_feature(x)
#endif
#if !defined(__GNUC_PREREQ__)
#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#define __GNUC_PREREQ__(maj, min)					\
    ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
#define __GNUC_PREREQ__(maj, min) 0
#endif
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Removed __PM_CLANG_ATOMICS clause, this is because
 * 1) clang understands gcc intrinsics as well;
 * 2) clang intrinsics require _Atomic quialified types while gcc ones
 *    don't.
 */
#if __GNUC_PREREQ__(4, 7)
#define	__PM_GNUC_ATOMICS
#elif defined(__GNUC__)
#define	__PM_SYNC_ATOMICS
#else
#error "pmatomic.h does not support your compiler"
#endif

/*
 * 7.17.1 Atomic lock-free macros.
 */
#if 0

#ifdef __GCC_ATOMIC_BOOL_LOCK_FREE
#define	ATOMIC_BOOL_LOCK_FREE		__GCC_ATOMIC_BOOL_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_CHAR_LOCK_FREE
#define	ATOMIC_CHAR_LOCK_FREE		__GCC_ATOMIC_CHAR_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_CHAR16_T_LOCK_FREE
#define	ATOMIC_CHAR16_T_LOCK_FREE	__GCC_ATOMIC_CHAR16_T_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_CHAR32_T_LOCK_FREE
#define	ATOMIC_CHAR32_T_LOCK_FREE	__GCC_ATOMIC_CHAR32_T_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_WCHAR_T_LOCK_FREE
#define	ATOMIC_WCHAR_T_LOCK_FREE	__GCC_ATOMIC_WCHAR_T_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_SHORT_LOCK_FREE
#define	ATOMIC_SHORT_LOCK_FREE		__GCC_ATOMIC_SHORT_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_INT_LOCK_FREE
#define	ATOMIC_INT_LOCK_FREE		__GCC_ATOMIC_INT_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_LONG_LOCK_FREE
#define	ATOMIC_LONG_LOCK_FREE		__GCC_ATOMIC_LONG_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_LLONG_LOCK_FREE
#define	ATOMIC_LLONG_LOCK_FREE		__GCC_ATOMIC_LLONG_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_POINTER_LOCK_FREE
#define	ATOMIC_POINTER_LOCK_FREE	__GCC_ATOMIC_POINTER_LOCK_FREE
#endif

#endif

/*
 * 7.17.2 Initialization.
 */
#if 0

#if defined(__PM_CLANG_ATOMICS)
#define	ATOMIC_VAR_INIT(value)		(value)
#define	atomic_init(obj, value)		__c11_atomic_init(obj, value)
#else
#define	ATOMIC_VAR_INIT(value)		{ .__val = (value) }
#define	atomic_init(obj, value)		((void)((obj)->__val = (value)))
#endif

#endif

/*
 * Clang and recent GCC both provide predefined macros for the memory
 * orderings.  If we are using a compiler that doesn't define them, use the
 * clang values - these will be ignored in the fallback path.
 */

#ifndef __ATOMIC_RELAXED
#define __ATOMIC_RELAXED		0
#endif
#ifndef __ATOMIC_CONSUME
#define __ATOMIC_CONSUME		1
#endif
#ifndef __ATOMIC_ACQUIRE
#define __ATOMIC_ACQUIRE		2
#endif
#ifndef __ATOMIC_RELEASE
#define __ATOMIC_RELEASE		3
#endif
#ifndef __ATOMIC_ACQ_REL
#define __ATOMIC_ACQ_REL		4
#endif
#ifndef __ATOMIC_SEQ_CST
#define __ATOMIC_SEQ_CST		5
#endif

/*
 * 7.17.3 Order and consistency.
 *
 * The pm_memory_order_* constants that denote the barrier behaviour of the
 * atomic operations.
 */

typedef enum {
    pm_memory_order_relaxed = __ATOMIC_RELAXED,
    pm_memory_order_consume = __ATOMIC_CONSUME,
    pm_memory_order_acquire = __ATOMIC_ACQUIRE,
    pm_memory_order_release = __ATOMIC_RELEASE,
    pm_memory_order_acq_rel = __ATOMIC_ACQ_REL,
    pm_memory_order_seq_cst = __ATOMIC_SEQ_CST
} pm_memory_order;

/*
 * 7.17.4 Fences.
 */

static __inline void
pm_atomic_thread_fence(pm_memory_order __order __attribute__((__unused__)))
{

#ifdef __PM_CLANG_ATOMICS
    __c11_atomic_thread_fence(__order);
#elif defined(__PM_GNUC_ATOMICS)
    __atomic_thread_fence(__order);
#else
    __sync_synchronize();
#endif
}

static __inline void
pm_atomic_signal_fence(pm_memory_order __order __attribute__((__unused__)))
{

#ifdef __PM_CLANG_ATOMICS
    __c11_atomic_signal_fence(__order);
#elif defined(__PM_GNUC_ATOMICS)
    __atomic_signal_fence(__order);
#else
    __asm volatile ("" ::: "memory");
#endif
}

/*
 * 7.17.5 Lock-free property.
 */
#if 0

#if defined(_KERNEL)
/* Atomics in kernelspace are always lock-free. */
#define	atomic_is_lock_free(obj) \
    ((void)(obj), (bool)1)
#elif defined(__PM_CLANG_ATOMICS)
#define	atomic_is_lock_free(obj) \
    __atomic_is_lock_free(sizeof(*(obj)), obj)
#elif defined(__PM_GNUC_ATOMICS)
#define	atomic_is_lock_free(obj) \
    __atomic_is_lock_free(sizeof((obj)->__val), &(obj)->__val)
#else
#define	atomic_is_lock_free(obj) \
    ((void)(obj), sizeof((obj)->__val) <= sizeof(void *))
#endif

#endif

/*
 * 7.17.6 Atomic integer types.
 */
#if 0

typedef _Atomic(bool)			atomic_bool;
typedef _Atomic(char)			atomic_char;
typedef _Atomic(signed char)		atomic_schar;
typedef _Atomic(unsigned char)		atomic_uchar;
typedef _Atomic(short)			atomic_short;
typedef _Atomic(unsigned short)		atomic_ushort;
typedef _Atomic(int)			atomic_int;
typedef _Atomic(unsigned int)		atomic_uint;
typedef _Atomic(long)			atomic_long;
typedef _Atomic(unsigned long)		atomic_ulong;
typedef _Atomic(long long)		atomic_llong;
typedef _Atomic(unsigned long long)	atomic_ullong;
typedef _Atomic(__char16_t)		atomic_char16_t;
typedef _Atomic(__char32_t)		atomic_char32_t;
typedef _Atomic(___wchar_t)		atomic_wchar_t;
typedef _Atomic(__int_least8_t)		atomic_int_least8_t;
typedef _Atomic(__uint_least8_t)	atomic_uint_least8_t;
typedef _Atomic(__int_least16_t)	atomic_int_least16_t;
typedef _Atomic(__uint_least16_t)	atomic_uint_least16_t;
typedef _Atomic(__int_least32_t)	atomic_int_least32_t;
typedef _Atomic(__uint_least32_t)	atomic_uint_least32_t;
typedef _Atomic(__int_least64_t)	atomic_int_least64_t;
typedef _Atomic(__uint_least64_t)	atomic_uint_least64_t;
typedef _Atomic(__int_fast8_t)		atomic_int_fast8_t;
typedef _Atomic(__uint_fast8_t)		atomic_uint_fast8_t;
typedef _Atomic(__int_fast16_t)		atomic_int_fast16_t;
typedef _Atomic(__uint_fast16_t)	atomic_uint_fast16_t;
typedef _Atomic(__int_fast32_t)		atomic_int_fast32_t;
typedef _Atomic(__uint_fast32_t)	atomic_uint_fast32_t;
typedef _Atomic(__int_fast64_t)		atomic_int_fast64_t;
typedef _Atomic(__uint_fast64_t)	atomic_uint_fast64_t;
typedef _Atomic(__intptr_t)		atomic_intptr_t;
typedef _Atomic(__uintptr_t)		atomic_uintptr_t;
typedef _Atomic(__size_t)		atomic_size_t;
typedef _Atomic(__ptrdiff_t)		atomic_ptrdiff_t;
typedef _Atomic(__intmax_t)		atomic_intmax_t;
typedef _Atomic(__uintmax_t)		atomic_uintmax_t;

#endif

/*
 * 7.17.7 Operations on atomic types.
 */

/*
 * Compiler-specific operations.
 */

#if defined(__PM_CLANG_ATOMICS)
#define	pm_atomic_compare_exchange_strong_explicit(object, expected,	\
    desired, success, failure)						\
    __c11_atomic_compare_exchange_strong(object, expected, desired,	\
        success, failure)
#define	pm_atomic_compare_exchange_weak_explicit(object, expected,		\
    desired, success, failure)						\
    __c11_atomic_compare_exchange_weak(object, expected, desired,	\
        success, failure)
#define	pm_atomic_exchange_explicit(object, desired, order)		\
    __c11_atomic_exchange(object, desired, order)
#define	pm_atomic_fetch_add_explicit(object, operand, order)		\
    __c11_atomic_fetch_add(object, operand, order)
#define	pm_atomic_fetch_and_explicit(object, operand, order)		\
    __c11_atomic_fetch_and(object, operand, order)
#define	pm_atomic_fetch_or_explicit(object, operand, order)		\
    __c11_atomic_fetch_or(object, operand, order)
#define	pm_atomic_fetch_sub_explicit(object, operand, order)		\
    __c11_atomic_fetch_sub(object, operand, order)
#define	pm_atomic_fetch_xor_explicit(object, operand, order)		\
    __c11_atomic_fetch_xor(object, operand, order)
#define	pm_atomic_load_explicit(object, order)				\
    __c11_atomic_load(object, order)
#define	pm_atomic_store_explicit(object, desired, order)			\
    __c11_atomic_store(object, desired, order)
#elif defined(__PM_GNUC_ATOMICS)
#define	pm_atomic_compare_exchange_strong_explicit(object, expected,	\
    desired, success, failure)						\
    __atomic_compare_exchange_n(object, expected,		\
        desired, 0, success, failure)
#define	pm_atomic_compare_exchange_weak_explicit(object, expected,		\
    desired, success, failure)						\
    __atomic_compare_exchange_n(object, expected,		\
        desired, 1, success, failure)
#define	pm_atomic_exchange_explicit(object, desired, order)		\
    __atomic_exchange_n(object, desired, order)
#define	pm_atomic_fetch_add_explicit(object, operand, order)		\
    __atomic_fetch_add(object, operand, order)
#define	pm_atomic_fetch_and_explicit(object, operand, order)		\
    __atomic_fetch_and(object, operand, order)
#define	pm_atomic_fetch_or_explicit(object, operand, order)		\
    __atomic_fetch_or(object, operand, order)
#define	pm_atomic_fetch_sub_explicit(object, operand, order)		\
    __atomic_fetch_sub(object, operand, order)
#define	pm_atomic_fetch_xor_explicit(object, operand, order)		\
    __atomic_fetch_xor(object, operand, order)
#define	pm_atomic_load_explicit(object, order)				\
    __atomic_load_n(object, order)
#define	pm_atomic_store_explicit(object, desired, order)			\
    __atomic_store_n(object, desired, order)
#else
#define	__pm_atomic_apply_stride(object, operand) \
    (((__typeof__(*(object)))0) + (operand))
#define	pm_atomic_compare_exchange_strong_explicit(object, expected,	\
    desired, success, failure)	__extension__ ({			\
    __typeof__(expected) __ep = (expected);				\
    __typeof__(*__ep) __e = *__ep;					\
    (void)(success); (void)(failure);				\
    (bool)((*__ep = __sync_val_compare_and_swap(object,	\
        __e, desired)) == __e);					\
})
#define	pm_atomic_compare_exchange_weak_explicit(object, expected,		\
    desired, success, failure)						\
    pm_atomic_compare_exchange_strong_explicit(object, expected,	\
        desired, success, failure)
#if __has_builtin(__sync_swap)
/* Clang provides a full-barrier atomic exchange - use it if available. */
#define	pm_atomic_exchange_explicit(object, desired, order)		\
    ((void)(order), __sync_swap(object, desired))
#else
/*
 * __sync_lock_test_and_set() is only an acquire barrier in theory (although in
 * practice it is usually a full barrier) so we need an explicit barrier before
 * it.
 */
#define	pm_atomic_exchange_explicit(object, desired, order)		\
__extension__ ({							\
    __typeof__(object) __o = (object);				\
    __typeof__(desired) __d = (desired);				\
    (void)(order);							\
    __sync_synchronize();						\
    __sync_lock_test_and_set(__o, __d);			\
})
#endif
#define	pm_atomic_fetch_add_explicit(object, operand, order)		\
    ((void)(order), __sync_fetch_and_add(object,		\
        __pm_atomic_apply_stride(object, operand)))
#define	pm_atomic_fetch_and_explicit(object, operand, order)		\
    ((void)(order), __sync_fetch_and_and(object, operand))
#define	pm_atomic_fetch_or_explicit(object, operand, order)		\
    ((void)(order), __sync_fetch_and_or(object, operand))
#define	pm_atomic_fetch_sub_explicit(object, operand, order)		\
    ((void)(order), __sync_fetch_and_sub(object,		\
        __pm_atomic_apply_stride(object, operand)))
#define	pm_atomic_fetch_xor_explicit(object, operand, order)		\
    ((void)(order), __sync_fetch_and_xor(object, operand))
#define	pm_atomic_load_explicit(object, order)				\
    ((void)(order), __sync_fetch_and_add(object, 0))
#define	pm_atomic_store_explicit(object, desired, order)			\
    ((void)pm_atomic_exchange_explicit(object, desired, order))
#endif

/*
 * Convenience functions.
 *
 * Don't provide these in kernel space. In kernel space, we should be
 * disciplined enough to always provide explicit barriers.
 */

#ifndef _KERNEL
#define	pm_atomic_compare_exchange_strong(object, expected, desired)	\
    pm_atomic_compare_exchange_strong_explicit(object, expected,	\
        desired, pm_memory_order_seq_cst, pm_memory_order_seq_cst)
#define	pm_atomic_compare_exchange_weak(object, expected, desired)		\
    pm_atomic_compare_exchange_weak_explicit(object, expected,		\
        desired, pm_memory_order_seq_cst, pm_memory_order_seq_cst)
#define	pm_atomic_exchange(object, desired)				\
    pm_atomic_exchange_explicit(object, desired, pm_memory_order_seq_cst)
#define	pm_atomic_fetch_add(object, operand)				\
    pm_atomic_fetch_add_explicit(object, operand, pm_memory_order_seq_cst)
#define	pm_atomic_fetch_and(object, operand)				\
    pm_atomic_fetch_and_explicit(object, operand, pm_memory_order_seq_cst)
#define	pm_atomic_fetch_or(object, operand)				\
    pm_atomic_fetch_or_explicit(object, operand, pm_memory_order_seq_cst)
#define	pm_atomic_fetch_sub(object, operand)				\
    pm_atomic_fetch_sub_explicit(object, operand, pm_memory_order_seq_cst)
#define	pm_atomic_fetch_xor(object, operand)				\
    pm_atomic_fetch_xor_explicit(object, operand, pm_memory_order_seq_cst)
#define	pm_atomic_load(object)						\
    pm_atomic_load_explicit(object, pm_memory_order_seq_cst)
#define	pm_atomic_store(object, desired)					\
    pm_atomic_store_explicit(object, desired, pm_memory_order_seq_cst)
#endif /* !_KERNEL */

/*
 * 7.17.8 Atomic flag type and operations.
 *
 * XXX: Assume atomic_bool can be used as an atomic_flag. Is there some
 * kind of compiler built-in type we could use?
 */
#if 0

typedef struct {
    atomic_bool	__flag;
} atomic_flag;

#define	ATOMIC_FLAG_INIT		{ ATOMIC_VAR_INIT(0) }

static __inline bool
atomic_flag_test_and_set_explicit(volatile atomic_flag *__object,
    pm_memory_order __order)
{
    return (pm_atomic_exchange_explicit(&__object->__flag, 1, __order));
}

static __inline void
atomic_flag_clear_explicit(volatile atomic_flag *__object, pm_memory_order __order)
{

    pm_atomic_store_explicit(&__object->__flag, 0, __order);
}

#ifndef _KERNEL
static __inline bool
atomic_flag_test_and_set(volatile atomic_flag *__object)
{

    return (atomic_flag_test_and_set_explicit(__object,
        pm_memory_order_seq_cst));
}

static __inline void
atomic_flag_clear(volatile atomic_flag *__object)
{

    atomic_flag_clear_explicit(__object, pm_memory_order_seq_cst);
}
#endif /* !_KERNEL */

#endif

#endif /* !_STDATOMIC_H_ */


/*
   COPY: file name = include/tarantool/tnt_io.h.
   Copying Date = 2017-09-14.
   Changes: Removed #includes.
*/
#ifndef TNT_IO_H_INCLUDED
#define TNT_IO_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

//#include <sys/uio.h>
//#include <tarantool/tnt_net.h>

/**
 * \internal
 * \file tnt_io.h
 * \brief Basic network layer io
 */

enum tnt_error
tnt_io_connect(struct tnt_stream_net *s);
void
tnt_io_close(struct tnt_stream_net *s);

ssize_t
tnt_io_flush(struct tnt_stream_net *s);

ssize_t
tnt_io_send_raw(struct tnt_stream_net *s, const char *buf,
        size_t size, int all);
ssize_t
tnt_io_sendv_raw(struct tnt_stream_net *s, struct iovec *iov,
         int count, int all);
ssize_t
tnt_io_recv_raw(struct tnt_stream_net *s, char *buf, size_t size, int all);

ssize_t
tnt_io_send(struct tnt_stream_net *s, const char *buf, size_t size);
ssize_t
tnt_io_sendv(struct tnt_stream_net *s, struct iovec *iov, int count);
ssize_t
tnt_io_recv(struct tnt_stream_net *s, char *buf, size_t size);

#endif /* TNT_IO_H_INCLUDED */


/*
   COPY: file name = include/tarantool/tnt_object.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/

#ifndef TNT_OBJECT_H_INCLUDED
#define TNT_OBJECT_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file tnt_object.h
 * \brief Object for manipulating msgpack objects
 */

#include <stdarg.h>

/**
 * \brief for internal use
 */
struct tnt_sbo_stack {
    size_t   offset;
    uint32_t size;
    int8_t   type;
};

/**
 * \brief type of packing msgpack array/map
 *
 * - TNT_SBO_SIMPLE - without packing, demanding size to be specified
 * - TNT_SBO_SPARSE - 5 bytes always allocated for map/array, size is ignored
 * - TNT_SBO_PACKED - 1 byte is alloced for map/array, if needed more, then
 *                    everything is moved to n bytes, when called
 *                    "tnt_object_container_close"
 */
enum tnt_sbo_type {
    TNT_SBO_SIMPLE = 0,
    TNT_SBO_SPARSE,
    TNT_SBO_PACKED,
};

struct tnt_sbuf_object {
    struct tnt_sbo_stack *stack;
    uint8_t stack_size;
    uint8_t stack_alloc;
    enum tnt_sbo_type type;
};

#define TNT_OBJ_CAST(SB) ((struct tnt_sbuf_object *)(SB)->subdata)
#define TNT_SOBJ_CAST(S) TNT_OBJ_CAST(TNT_SBUF_CAST(S))

/**
 * \brief Set type of packing for objects
 *
 * Type must be set before first value was written
 *
 * \param s    tnt_object instance
 * \param type type of packing
 *
 * \returns status of operation
 * \retval  -1 (something was written before
 * \retval   0 success
 */
int
tnt_object_type(struct tnt_stream *s, enum tnt_sbo_type type);

/**
 * \brief create and initialize tnt_object
 *
 * tnt_object is used to create msgpack values: keys/tuples/args for
 * passing them into tnt_request or tnt_<operation>
 * if stream object is NULL, then new stream object will be created
 *
 * \param s object pointer
 *
 * \returns object pointer
 * \retval NULL error
 */
struct tnt_stream *
tnt_object(struct tnt_stream *s);

/**
 * \brief Add nil to a stream object
 */
ssize_t
tnt_object_add_nil(struct tnt_stream *s);

/**
 * \brief Add integer to a stream object
 */
ssize_t
tnt_object_add_int(struct tnt_stream *s, int64_t value);

/**
 * \brief Add unsigned integer to a stream object
 */
ssize_t
tnt_object_add_uint(struct tnt_stream *s, uint64_t value);

/**
 * \brief Add string to a stream object
 */
ssize_t
tnt_object_add_str(struct tnt_stream *s, const char *str, uint32_t len);

/**
 * \brief Add null terminated string to a stream object
 */
ssize_t
tnt_object_add_strz(struct tnt_stream *s, const char *strz);

/**
 * \brief Add binary object to a stream object
 */
ssize_t
tnt_object_add_bin(struct tnt_stream *s, const void *bin, uint32_t len);

/**
 * \brief Add boolean to a stream object
 */
ssize_t
tnt_object_add_bool(struct tnt_stream *s, char value);

/**
 * \brief Add floating value to a stream object
 */
ssize_t
tnt_object_add_float(struct tnt_stream *s, float value);

/**
 * \brief Add double precision floating value to a stream object
 */
ssize_t
tnt_object_add_double(struct tnt_stream *s, double value);

/**
 * \brief Append array header to stream object
 * \sa tnt_sbo_type
 */
ssize_t
tnt_object_add_array(struct tnt_stream *s, uint32_t size);

/**
 * \brief Append map header to stream object
 * \sa tnt_sbo_type
 */
ssize_t
tnt_object_add_map(struct tnt_stream *s, uint32_t size);

/**
 * \brief Close array/map in case TNT_SBO_PACKED/TNT_SBO_SPARSE were used
 * \sa tnt_sbo_type
 */
ssize_t
tnt_object_container_close(struct tnt_stream *s);

/**
 * \brief create immutable tnt_object from given buffer
 */
struct tnt_stream *
tnt_object_as(struct tnt_stream *s, char *buf, size_t buf_len);

/**
 * \brief verify that object is valid msgpack structure
 * \param s object pointer
 * \param type -1 on check without validating type, otherwise `enum mp_type`
 */
int
tnt_object_verify(struct tnt_stream *s, int8_t type);

/**
 * \brief reset tnt_object to basic state
 * this function doesn't deallocate memory, but instead it simply sets all
 * pointers to beginning
 */
int
tnt_object_reset(struct tnt_stream *s);

/**
 * \brief create tnt_object from format string/values (va_list variation)
 *
 * \code{.c}
 * \*to get a msgpack array of two items: number 42 and map (0->"false, 2->"true")*\
 * tnt_object_format(s, "[%d {%d%s%d%s}]", 42, 0, "false", 1, "true");
 * \endcode
 *
 * \param s   tnt_object instance
 * \param fmt zero-end string, containing structure of resulting
 * msgpack and types of next arguments.
 * Format can contain '[' and ']' pairs, defining arrays,
 * '{' and '}' pairs, defining maps, and format specifiers, described below:
 * %d, %i - int
 * %u - unsigned int
 * %ld, %li - long
 * %lu - unsigned long
 * %lld, %lli - long long
 * %llu - unsigned long long
 * %hd, %hi - short
 * %hu - unsigned short
 * %hhd, %hhi - char (as number)
 * %hhu - unsigned char (as number)
 * %f - float
 * %lf - double
 * %b - bool
 * %s - zero-end string
 * %.*s - string with specified length
 * %% is ignored
 * %'smth else' assert and undefined behaviour
 * NIL - a nil value
 * all other symbols are ignored.
 *
 * \sa tnt_object_vformat
 * \sa tnt_object_format
 */
ssize_t
tnt_object_format(struct tnt_stream *s, const char *fmt, ...);

/**
 * \brief create tnt_object from format string/values
 * \sa tnt_object_vformat
 * \sa tnt_object_format
 */
ssize_t
tnt_object_vformat(struct tnt_stream *s, const char *fmt, va_list vl);

#endif /* TNT_OBJECT_H_INCLUDED */

/*
   COPY: file name = include/tarantool/tnt_buf.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/


#ifndef TNT_BUF_H_INCLUDED
#define TNT_BUF_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file tnt_buf.h
 * \brief basic buffer structure
 */

/*!
 * Type for resize function
 */
typedef char *(*tnt_buf_resize_t)(struct tnt_stream *, size_t);

/*!
 * Stream buffer substructure
 */
struct tnt_stream_buf {
    char   *data;   /*!< buffer data */
    size_t  size;   /*!< buffer used */
    size_t  alloc;  /*!< current buffer size */
    size_t  rdoff;  /*!< read offset */
    char *(*resize)(struct tnt_stream *, size_t); /*!< resize function */
    void  (*free)(struct tnt_stream *); /*!< custom free function */
    void   *subdata; /*!< subclass */
    int     as;      /*!< constructed from user's string */
};

/* buffer stream accessors */

/*!
 * \brief cast tnt_stream to tnt_stream_buf structure
 */
#define TNT_SBUF_CAST(S) ((struct tnt_stream_buf *)(S)->data)
/*!
 * \brief get data field from tnt_stream_buf
 */
#define TNT_SBUF_DATA(S) TNT_SBUF_CAST(S)->data
/*!
 * \brief get size field from tnt_stream_buf
 */
#define TNT_SBUF_SIZE(S) TNT_SBUF_CAST(S)->size

/**
 * \brief Allocate and init stream buffer object
 *
 * if stream pointer is NULL, then new stream will be created.
 *
 * \param   s pointer to allocated stream buffer
 *
 * \returns pointer to newly allocated sbuf object
 * \retval  NULL memory allocation failure
 */
struct tnt_stream *
tnt_buf(struct tnt_stream *s);

struct tnt_stream *
tnt_buf_as(struct tnt_stream *s, char *buf, size_t buf_len);

#endif /* TNT_BUF_H_INCLUDED */

/*
   COPY: file name = include/tarantool/tnt_schema.h.
   Copying Date = 2017-09-14.
   Changes: none.
*/

#ifndef TNT_SCHEMA_H_INCLUDED
#define TNT_SCHEMA_H_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


/**
 * \internal
 * \file tnt_schema.h
 * \brief Tarantool schema
 */

struct mh_assoc_t;

/**
 * \internal
 * \brief index value information
 */
struct tnt_schema_ival {
    const char *name;
    uint32_t    name_len;
    uint32_t    number;
};

/**
 * \internal
 * \brief space value information
 */
struct tnt_schema_sval {
    char              *name;
    uint32_t           name_len;
    uint32_t           number;
    struct mh_assoc_t *index;
};

/**
 * \brief Schema of tarantool instance
 */
struct tnt_schema {
    struct mh_assoc_t *space_hash; /*!< hash with spaces */
    int alloc; /*!< allocation mark */
};

/**
 * \brief Add spaces definitions to schema
 *
 * must be used with response's data of select from 280/281 spaces
 * (_space/_vspace)
 *
 * \param sch  schema pointer
 * \param data pointer to data
 * \param dlen data size
 *
 * \returns status
 * \retval  -1 failed parsing/oom
 * \retval  0 ok
 */
int
tnt_schema_add_spaces(struct tnt_schema *sch, struct tnt_reply *r);

/**
 * \brief Add indexes definitions to schema
 *
 * must be used with response's data of select from 288/289 indexes
 * (_index/_vindex)
 *
 * \param sch  schema pointer
 * \param data pointer to data
 * \param dlen data size
 *
 * \returns status
 * \retval  -1 failed parsing/oom
 * \retval  0 ok
 */
int
tnt_schema_add_indexes(struct tnt_schema *sch, struct tnt_reply *r);

/**
 * \brief Get spaceno by space name
 *
 * \param sch   schema pointer
 * \param sstr  space name
 * \param sslen space name len
 *
 * \returns space id
 * \retval -1 error, space not found
 */
int32_t
tnt_schema_stosid (struct tnt_schema *sch, const char *sstr, uint32_t sslen);

/**
 * \brief Get indexno by space no and index name
 *
 * \param sch   schema pointer
 * \param sno   space no
 * \param istr  index name
 * \param islen index name len
 *
 * \returns index id
 * \retval -1 error, index/space not found
 */
int32_t
tnt_schema_stoiid (struct tnt_schema *sch, uint32_t sno, const char *istr,
           uint32_t islen);

/**
 * \brief Create and init schema object
 *
 * if schema pointer is NULL, then new schema object will be created
 *
 * \param sch schema pointer
 * \returns new schema object
 * \retval  NULL oom
 */
struct tnt_schema *
tnt_schema_new(struct tnt_schema *sch);

/**
 * \brief Reset schema to default state (empty)
 * \param sch schema pointer
 */
void
tnt_schema_flush(struct tnt_schema *sch);

/**
 * \brief Reset and free schema
 * \param sch schema pointer
 */
void
tnt_schema_free(struct tnt_schema *sch);

ssize_t
tnt_get_space(struct tnt_stream *s);

ssize_t
tnt_get_index(struct tnt_stream *s);

#endif /* TNT_SCHEMA_H_INCLUDED */

#ifdef OCELOT_THIRD_PARTY_CODE

/*
   COPY: file name = third_party/base64.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef BASE64_C_INCLUDED
#define BASE64_C_INCLUDED
/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
//#include "base64.h"
/*
 * This is part of the libb64 project, and has been placed in the
 * public domain. For details, see
 * http://sourceforge.net/projects/libb64
 */

/* {{{ encode */

enum base64_encodestep { step_A, step_B, step_C };

struct base64_encodestate {
    enum base64_encodestep step;
    char result;
    int stepcount;
};

static inline void
base64_encodestate_init(struct base64_encodestate *state)
{
    state->step = step_A;
    state->result = 0;
    state->stepcount = 0;
}

static inline char
base64_encode_value(char value)
{
    static const char encoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned codepos = (unsigned) value;
    if (codepos > sizeof(encoding) - 1)
        return '=';
    return encoding[codepos];
}

static int
base64_encode_block(const char *in_bin, int in_len,
            char *out_base64, int out_len,
            struct base64_encodestate *state)
{
    const char *const in_end = in_bin + in_len;
    const char *in_pos = in_bin;
    char *out_pos = out_base64;
    char *out_end = out_base64  + out_len;
    char result;
    char fragment;

    result = state->result;

    switch (state->step)
    {
        while (1)
        {
    case step_A:
            if (in_pos == in_end || out_pos >= out_end) {
                state->result = result;
                state->step = step_A;
                return out_pos - out_base64;
            }
            fragment = *in_pos++;
            result = (fragment & 0x0fc) >> 2;
            *out_pos++ = base64_encode_value(result);
            result = (fragment & 0x003) << 4;
    case step_B:
            if (in_pos == in_end || out_pos >= out_end) {
                state->result = result;
                state->step = step_B;
                return out_pos - out_base64;
            }
            fragment = *in_pos++;
            result |= (fragment & 0x0f0) >> 4;
            *out_pos++ = base64_encode_value(result);
            result = (fragment & 0x00f) << 2;
    case step_C:
            if (in_pos == in_end || out_pos + 2 >= out_end) {
                state->result = result;
                state->step = step_C;
                return out_pos - out_base64;
            }
            fragment = *in_pos++;
            result |= (fragment & 0x0c0) >> 6;
            *out_pos++ = base64_encode_value(result);
            result  = (fragment & 0x03f) >> 0;
            *out_pos++ = base64_encode_value(result);

            /*
             * Each full step (A->B->C) yields
             * 4 characters.
             */
            if (++state->stepcount * 4 == BASE64_CHARS_PER_LINE) {
                if (out_pos >= out_end)
                    return out_pos - out_base64;
                *out_pos++ = '\n';
                state->stepcount = 0;
            }
        }
    }
    /* control should not reach here */
    return out_pos - out_base64;
}

static int
base64_encode_blockend(char *out_base64, int out_len,
               struct base64_encodestate *state)
{
    char *out_pos = out_base64;
    char *out_end = out_base64 + out_len;

    switch (state->step) {
    case step_B:
        if (out_pos + 2 >= out_end)
            return out_pos - out_base64;
        *out_pos++ = base64_encode_value(state->result);
        *out_pos++ = '=';
        *out_pos++ = '=';
        break;
    case step_C:
        if (out_pos + 1 >= out_end)
            return out_pos - out_base64;
        *out_pos++ = base64_encode_value(state->result);
        *out_pos++ = '=';
        break;
    case step_A:
        break;
    }
    if (out_pos >= out_end)
        return out_pos - out_base64;
#if 0
    /* Sometimes the output is useful without a newline. */
    *out_pos++ = '\n';
    if (out_pos >= out_end)
        return out_pos - out_base64;
#endif
    *out_pos = '\0';
    return out_pos - out_base64;
}

int
base64_encode(const char *in_bin, int in_len,
          char *out_base64, int out_len)
{
    struct base64_encodestate state;
    base64_encodestate_init(&state);
    int res = base64_encode_block(in_bin, in_len, out_base64,
                      out_len, &state);
    return res + base64_encode_blockend(out_base64 + res, out_len - res,
                        &state);
}

/* }}} */

/* {{{ decode */

enum base64_decodestep { step_a, step_b, step_c, step_d };

struct base64_decodestate
{
    enum base64_decodestep step;
    char result;
};

static char
base64_decode_value(char value)
{
    static const char decoding[] = {
        62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
        59, 60, 61, -1, -1, -1, -2, -1, -1, -1,  0,  1,
        2,   3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13,
        14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
        -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
        44, 45, 46, 47, 48, 49, 50, 51
    };
    static const char decoding_size = sizeof(decoding);
    int codepos = (signed char) value;
    codepos -= 43;
    if (codepos < 0 || codepos > decoding_size)
        return -1;
    return decoding[codepos];
}

static inline void
base64_decodestate_init(struct base64_decodestate *state)
{
    state->step = step_a;
    state->result = 0;
}

static int
base64_decode_block(const char *in_base64, int in_len,
            char *out_bin, int out_len,
            struct base64_decodestate *state)
{
    const char *in_pos = in_base64;
    const char *in_end = in_base64 + in_len;
    char *out_pos = out_bin;
    char *out_end = out_bin + out_len;
    char fragment;

    *out_pos = state->result;

    switch (state->step)
    {
        while (1)
        {
    case step_a:
            do {
                if (in_pos == in_end || out_pos >= out_end)
                {
                    state->step = step_a;
                    state->result = *out_pos;
                    return out_pos - out_bin;
                }
                fragment = base64_decode_value(*in_pos++);
            } while (fragment < 0);
            *out_pos    = (fragment & 0x03f) << 2;
    case step_b:
            do {
                if (in_pos == in_end || out_pos >= out_end)
                {
                    state->step = step_b;
                    state->result = *out_pos;
                    return out_pos - out_bin;
                }
                fragment = base64_decode_value(*in_pos++);
            } while (fragment < 0);
            *out_pos++ |= (fragment & 0x030) >> 4;
            if (out_pos < out_end)
                *out_pos = (fragment & 0x00f) << 4;
    case step_c:
            do {
                if (in_pos == in_end || out_pos >= out_end)
                {
                    state->step = step_c;
                    state->result = *out_pos;
                    return out_pos - out_bin;
                }
                fragment = base64_decode_value(*in_pos++);
            } while (fragment < 0);
            *out_pos++ |= (fragment & 0x03c) >> 2;
            if (out_pos < out_end)
                *out_pos = (fragment & 0x003) << 6;
    case step_d:
            do {
                if (in_pos == in_end || out_pos >= out_end)
                {
                    state->step = step_d;
                    state->result = *out_pos;
                    return out_pos - out_bin;
                }
                fragment = base64_decode_value(*in_pos++);
            } while (fragment < 0);
            *out_pos++   |= (fragment & 0x03f);
        }
    }
    /* control should not reach here */
    return out_pos - out_bin;
}



int
base64_decode(const char *in_base64, int in_len,
          char *out_bin, int out_len)
{
    struct base64_decodestate state;
    base64_decodestate_init(&state);
    return base64_decode_block(in_base64, in_len,
                   out_bin, out_len, &state);
}

/* }}} */

#endif /* BASE64_C_INCLUDED */

/*
   COPY: file name = third_party/PMurHash.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef PMURHASH_C_INCLUDED
#define PMURHASH_C_INCLUDED
/*-----------------------------------------------------------------------------
 * MurmurHash3 was written by Austin Appleby, and is placed in the public
 * domain.
 *
 * This implementation was written by Shane Day, and is also public domain.
 *
 * This is a portable ANSI C implementation of MurmurHash3_x86_32 (Murmur3A)
 * with support for progressive processing.
 */

/*-----------------------------------------------------------------------------

If you want to understand the MurmurHash algorithm you would be much better
off reading the original source. Just point your browser at:
http://code.google.com/p/smhasher/source/browse/trunk/MurmurHash3.cpp


What this version provides?

1. Progressive data feeding. Useful when the entire payload to be hashed
does not fit in memory or when the data is streamed through the application.
Also useful when hashing a number of strings with a common prefix. A partial
hash of a prefix string can be generated and reused for each suffix string.

2. Portability. Plain old C so that it should compile on any old compiler.
Both CPU endian and access-alignment neutral, but avoiding inefficient code
when possible depending on CPU capabilities.

3. Drop in. I personally like nice self contained public domain code, making it
easy to pilfer without loads of refactoring to work properly in the existing
application code & makefile structure and mucking around with licence files.
Just copy PMurHash.h and PMurHash.c and you're ready to go.


How does it work?

We can only process entire 32 bit chunks of input, except for the very end
that may be shorter. So along with the partial hash we need to give back to
the caller a carry containing up to 3 bytes that we were unable to process.
This carry also needs to record the number of bytes the carry holds. I use
the low 2 bits as a count (0..3) and the carry bytes are shifted into the
high byte in stream order.

To handle endianess I simply use a macro that reads a uint32_t and define
that macro to be a direct read on little endian machines, a read and swap
on big endian machines, or a byte-by-byte read if the endianess is unknown.

-----------------------------------------------------------------------------*/


//#include "PMurHash.h"

/* I used ugly type names in the header to avoid potential conflicts with
 * application or system typedefs & defines. Since I'm not including any more
 * headers below here I can rename these so that the code reads like C99 */
#undef uint32_t
#define uint32_t MH_UINT32
#undef uint8_t
#define uint8_t  MH_UINT8

/* MSVC warnings we choose to ignore */
#if defined(_MSC_VER)
  #pragma warning(disable: 4127) /* conditional expression is constant */
#endif

/*-----------------------------------------------------------------------------
 * Endianess, misalignment capabilities and util macros
 *
 * The following 3 macros are defined in this section. The other macros defined
 * are only needed to help derive these 3.
 *
 * READ_UINT32(x)   Read a little endian unsigned 32-bit int
 * UNALIGNED_SAFE   Defined if READ_UINT32 works on non-word boundaries
 * ROTL32(x,r)      Rotate x left by r bits
 */

/* Convention is to define __BYTE_ORDER == to one of these values */
#if !defined(__BIG_ENDIAN)
  #define __BIG_ENDIAN 4321
#endif
#if !defined(__LITTLE_ENDIAN)
  #define __LITTLE_ENDIAN 1234
#endif

/* I386 */
#if defined(_M_IX86) || defined(__i386__) || defined(__i386) || defined(i386)
  #define __BYTE_ORDER __LITTLE_ENDIAN
  #define UNALIGNED_SAFE
#endif

/* gcc 'may' define __LITTLE_ENDIAN__ or __BIG_ENDIAN__ to 1 (Note the trailing __),
 * or even _LITTLE_ENDIAN or _BIG_ENDIAN (Note the single _ prefix) */
#if !defined(__BYTE_ORDER)
  #if defined(__LITTLE_ENDIAN__) && __LITTLE_ENDIAN__==1 || defined(_LITTLE_ENDIAN) && _LITTLE_ENDIAN==1
    #define __BYTE_ORDER __LITTLE_ENDIAN
  #elif defined(__BIG_ENDIAN__) && __BIG_ENDIAN__==1 || defined(_BIG_ENDIAN) && _BIG_ENDIAN==1
    #define __BYTE_ORDER __BIG_ENDIAN
  #endif
#endif

/* gcc (usually) defines xEL/EB macros for ARM and MIPS endianess */
#if !defined(__BYTE_ORDER)
  #if defined(__ARMEL__) || defined(__MIPSEL__)
    #define __BYTE_ORDER __LITTLE_ENDIAN
  #endif
  #if defined(__ARMEB__) || defined(__MIPSEB__)
    #define __BYTE_ORDER __BIG_ENDIAN
  #endif
#endif

/* Now find best way we can to READ_UINT32 */
#if __BYTE_ORDER==__LITTLE_ENDIAN
  /* CPU endian matches murmurhash algorithm, so read 32-bit word directly */
  #define READ_UINT32(ptr)   (*((uint32_t*)(ptr)))
#elif __BYTE_ORDER==__BIG_ENDIAN
  /* TODO: Add additional cases below where a compiler provided bswap32 is available */
  #if defined(__GNUC__) && (__GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=3))
    #define READ_UINT32(ptr)   (__builtin_bswap32(*((uint32_t*)(ptr))))
  #else
    /* Without a known fast bswap32 we're just as well off doing this */
    #define READ_UINT32(ptr)   (ptr[0]|ptr[1]<<8|ptr[2]<<16|ptr[3]<<24)
    #define UNALIGNED_SAFE
  #endif
#else
  /* Unknown endianess so last resort is to read individual bytes */
  #define READ_UINT32(ptr)   (ptr[0]|ptr[1]<<8|ptr[2]<<16|ptr[3]<<24)

  /* Since we're not doing word-reads we can skip the messing about with realignment */
  #define UNALIGNED_SAFE
#endif

/* Find best way to ROTL32 */
#if defined(_MSC_VER)
  #include <stdlib.h>  /* Microsoft put _rotl declaration in here */
  #define ROTL32(x,r)  _rotl(x,r)
#else
  /* gcc recognises this code and generates a rotate instruction for CPUs with one */
  #define ROTL32(x,r)  (((uint32_t)x << r) | ((uint32_t)x >> (32 - r)))
#endif


/*-----------------------------------------------------------------------------
 * Core murmurhash algorithm macros */

#define C1  (0xcc9e2d51)
#define C2  (0x1b873593)

/* This is the main processing body of the algorithm. It operates
 * on each full 32-bits of input. */
#define DOBLOCK(h1, k1) do{ \
        k1 *= C1; \
        k1 = ROTL32(k1,15); \
        k1 *= C2; \
        \
        h1 ^= k1; \
        h1 = ROTL32(h1,13); \
        h1 = h1*5+0xe6546b64; \
    }while(0)


/* Append unaligned bytes to carry, forcing hash churn if we have 4 bytes */
/* cnt=bytes to process, h1=name of h1 var, c=carry, n=bytes in c, ptr/len=payload */
#define DOBYTES(cnt, h1, c, n, ptr, len) do{ \
    int _i = cnt; \
    while(_i--) { \
        c = c>>8 | *ptr++<<24; \
        n++; len--; \
        if(n==4) { \
            DOBLOCK(h1, c); \
            n = 0; \
        } \
    } }while(0)

/*---------------------------------------------------------------------------*/

/* Main hashing function. Initialise carry to 0 and h1 to 0 or an initial seed
 * if wanted. Both ph1 and pcarry are required arguments. */
void PMurHash32_Process(uint32_t *ph1, uint32_t *pcarry, const void *key, int len)
{
  uint32_t h1 = *ph1;
  uint32_t c = *pcarry;

  const uint8_t *ptr = (uint8_t*)key;
  const uint8_t *end;

  /* Extract carry count from low 2 bits of c value */
  int n = c & 3;

#if defined(UNALIGNED_SAFE)
  /* This CPU handles unaligned word access */

  /* Consume any carry bytes */
  int i = (4-n) & 3;
  if(i && i <= len) {
    DOBYTES(i, h1, c, n, ptr, len);
  }

  /* Process 32-bit chunks */
  end = ptr + len/4*4;
  for( ; ptr < end ; ptr+=4) {
    uint32_t k1 = READ_UINT32(ptr);
    DOBLOCK(h1, k1);
  }

#else /*UNALIGNED_SAFE*/
  /* This CPU does not handle unaligned word access */

  /* Consume enough so that the next data byte is word aligned */
  int i = -(long)ptr & 3;
  if(i && i <= len) {
      DOBYTES(i, h1, c, n, ptr, len);
  }

  /* We're now aligned. Process in aligned blocks. Specialise for each possible carry count */
  end = ptr + len/4*4;
  switch(n) { /* how many bytes in c */
  case 0: /* c=[----]  w=[3210]  b=[3210]=w            c'=[----] */
    for( ; ptr < end ; ptr+=4) {
      uint32_t k1 = READ_UINT32(ptr);
      DOBLOCK(h1, k1);
    }
    break;
  case 1: /* c=[0---]  w=[4321]  b=[3210]=c>>24|w<<8   c'=[4---] */
    for( ; ptr < end ; ptr+=4) {
      uint32_t k1 = c>>24;
      c = READ_UINT32(ptr);
      k1 |= c<<8;
      DOBLOCK(h1, k1);
    }
    break;
  case 2: /* c=[10--]  w=[5432]  b=[3210]=c>>16|w<<16  c'=[54--] */
    for( ; ptr < end ; ptr+=4) {
      uint32_t k1 = c>>16;
      c = READ_UINT32(ptr);
      k1 |= c<<16;
      DOBLOCK(h1, k1);
    }
    break;
  case 3: /* c=[210-]  w=[6543]  b=[3210]=c>>8|w<<24   c'=[654-] */
    for( ; ptr < end ; ptr+=4) {
      uint32_t k1 = c>>8;
      c = READ_UINT32(ptr);
      k1 |= c<<24;
      DOBLOCK(h1, k1);
    }
  }
#endif /*UNALIGNED_SAFE*/

  /* Advance over whole 32-bit chunks, possibly leaving 1..3 bytes */
  len -= len/4*4;

  /* Append any remaining bytes into carry */
  DOBYTES(len, h1, c, n, ptr, len);

  /* Copy out new running hash and carry */
  *ph1 = h1;
  *pcarry = (c & ~0xff) | n;
}

/*---------------------------------------------------------------------------*/

/* Finalize a hash. To match the original Murmur3A the total_length must be provided */
uint32_t PMurHash32_Result(uint32_t h, uint32_t carry, uint32_t total_length)
{
  uint32_t k1;
  int n = carry & 3;
  if(n) {
    k1 = carry >> (4-n)*8;
    k1 *= C1; k1 = ROTL32(k1,15); k1 *= C2; h ^= k1;
  }
  h ^= total_length;

  /* fmix */
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

/*---------------------------------------------------------------------------*/

/* Murmur3A compatable all-at-once */
uint32_t PMurHash32(uint32_t seed, const void *key, int len)
{
  uint32_t h1=seed, carry=0;
  PMurHash32_Process(&h1, &carry, key, len);
  return PMurHash32_Result(h1, carry, len);
}

/*---------------------------------------------------------------------------*/

/* Provide an API suitable for smhasher */
void PMurHash32_test(const void *key, int len, uint32_t seed, void *out)
{
  uint32_t h1=seed, carry=0;
  const uint8_t *ptr = (uint8_t*)key;
  const uint8_t *end = ptr + len;

#if 0 /* Exercise the progressive processing */
  while(ptr < end) {
    //const uint8_t *mid = ptr + rand()%(end-ptr)+1;
    const uint8_t *mid = ptr + (rand()&0xF);
    mid = mid<end?mid:end;
    PMurHash32_Process(&h1, &carry, ptr, mid-ptr);
    ptr = mid;
  }
#else
  PMurHash32_Process(&h1, &carry, ptr, (int)(end-ptr));
#endif
  h1 = PMurHash32_Result(h1, carry, len);
  *(uint32_t*)out = h1;
}

/*---------------------------------------------------------------------------*/

#endif /* PMURHASH_C_INCLUDED */

/*
   COPY: file name = third_party/sha1.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef SHA1_C_INCLUDED
#define SHA1_C_INCLUDED

/* from valgrind tests */

/* ================ sha1.c ================ */
/*
SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain

Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

/* #define LITTLE_ENDIAN * This should be #define'd already, if true. */
/* #define SHA1HANDSOFF * Copies data before messing with it. */

#define SHA1HANDSOFF

#include <stdio.h>
#include <string.h>
#include <sys/types.h>	/* for u_int*_t */
//#include "sha1.h"

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))

#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);


/* Hash a single 512-bit block. This is the core of the algorithm. */

void SHA1Transform(uint32_t state[5], const unsigned char buffer[64])
{
    uint32_t a, b, c, d, e;
    typedef union {
        unsigned char c[64];
        uint32_t l[16];
    } CHAR64LONG16;
#ifdef SHA1HANDSOFF
    CHAR64LONG16 block[1];  /* use array to appear as a pointer */
    memcpy(block, buffer, 64);
#else
    /* The following had better never be used because it causes the
     * pointer-to-const buffer to be cast into a pointer to non-const.
     * And the result is written through.  I threw a "const" in, hoping
     * this will cause a diagnostic.
     */
    CHAR64LONG16* block = (const CHAR64LONG16*)buffer;
#endif
    /* Copy context->state[] to working vars */
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
    R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
    R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
    R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
    R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
    R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
    R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
    R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
    R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
    R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
    R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
    R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
    R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
    R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
    R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
    R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
    R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
    R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
    R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
    R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
    /* Add the working vars back into context.state[] */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    /* Wipe variables */
    a = b = c = d = e = 0;
#ifdef SHA1HANDSOFF
    memset(block, '\0', sizeof(block));
#endif
}


/* SHA1Init - Initialize new context */

void SHA1Init(SHA1_CTX* context)
{
    /* SHA1 initialization constants */
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}


/* Run your data through this. */

void SHA1Update(SHA1_CTX* context, const unsigned char* data, uint32_t len)
{
    uint32_t i, j;

    j = context->count[0];
    if ((context->count[0] += len << 3) < j)
        context->count[1]++;
    context->count[1] += (len>>29);
    j = (j >> 3) & 63;
    if ((j + len) > 63) {
        memcpy(&context->buffer[j], data, (i = 64-j));
        SHA1Transform(context->state, context->buffer);
        for ( ; i + 63 < len; i += 64) {
            SHA1Transform(context->state, &data[i]);
        }
        j = 0;
    }
    else i = 0;
    memcpy(&context->buffer[j], &data[i], len - i);
}


/* Add padding and return the message digest. */

void SHA1Final(unsigned char digest[20], SHA1_CTX* context)
{
    unsigned i;
    unsigned char finalcount[8];
    unsigned char c;

#if 0	/* untested "improvement" by DHR */
    /* Convert context->count to a sequence of bytes
     * in finalcount.  Second element first, but
     * big-endian order within element.
     * But we do it all backwards.
     */
    unsigned char *fcp = &finalcount[8];

    for (i = 0; i < 2; i++)
       {
        uint32_t t = context->count[i];
        int j;

        for (j = 0; j < 4; t >>= 8, j++)
              *--fcp = (unsigned char) t;
    }
#else
    for (i = 0; i < 8; i++) {
        finalcount[i] = (unsigned char)((context->count[(i >= 4 ? 0 : 1)]
         >> ((3-(i & 3)) * 8) ) & 255);  /* Endian independent */
    }
#endif
    c = 0200;
    SHA1Update(context, &c, 1);
    while ((context->count[0] & 504) != 448) {
    c = 0000;
        SHA1Update(context, &c, 1);
    }
    SHA1Update(context, finalcount, 8);  /* Should cause a SHA1Transform() */
    for (i = 0; i < 20; i++) {
        digest[i] = (unsigned char)
         ((context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
    }
    /* Wipe variables */
    memset(context, '\0', sizeof(*context));
    memset(&finalcount, '\0', sizeof(finalcount));
}
/* ================ end of sha1.c ================ */

#if 0
#define BUFSIZE 4096

int
main(int argc, char **argv)
{
    SHA1_CTX ctx;
    unsigned char hash[20], buf[BUFSIZE];
    int i;

    for(i=0;i<BUFSIZE;i++)
        buf[i] = i;

    SHA1Init(&ctx);
    for(i=0;i<1000;i++)
        SHA1Update(&ctx, buf, BUFSIZE);
    SHA1Final(hash, &ctx);

    printf("SHA1=");
    for(i=0;i<20;i++)
        printf("%02x", hash[i]);
    printf("\n");
    return 0;
}

#endif


#endif /* SHA1_C_INCLUDED */

/*
   COPY: file name = third_party/uri.c
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef URI_C_INCLUDED
#define URI_C_INCLUDED
//#line 1 "../../src/uri.rl"
/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
//#include "uri.h"
#include <string.h>
#include <stdio.h> /* snprintf */
int
uri_parse(struct uri *uri, const char *p)
{
    const char *pe = p + strlen(p);
    const char *eof = pe;
    int cs;
    memset(uri, 0, sizeof(*uri));

    if (p == pe)
        return -1;

    const char *s = NULL, *login = NULL, *scheme = NULL;
    size_t login_len = 0, scheme_len = 0;


//#line 50 "../../src/uri.c"
static const int uri_start = 144;
static const int uri_first_final = 144;
static const int uri_error = 0;

static const int uri_en_main = 144;


//#line 58 "../../src/uri.c"
    {
    cs = uri_start;
    }

//#line 63 "../../src/uri.c"
    {
    if ( p == pe )
        goto _test_eof;
    switch ( cs )
    {
case 144:
    switch( (*p) ) {
        case 33: goto tr150;
        case 35: goto tr151;
        case 37: goto tr152;
        case 47: goto tr153;
        case 59: goto tr150;
        case 61: goto tr150;
        case 63: goto tr155;
        case 64: goto st204;
        case 91: goto st38;
        case 95: goto tr150;
        case 117: goto tr158;
        case 126: goto tr150;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto tr150;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto tr157;
        } else if ( (*p) >= 65 )
            goto tr157;
    } else
        goto tr154;
    goto st0;
st0:
cs = 0;
    goto _out;
tr150:
//#line 137 "../../src/uri.rl"
    { s = p; }
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st145;
st145:
    if ( ++p == pe )
        goto _test_eof145;
case 145:
//#line 109 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 47: goto tr161;
        case 58: goto tr162;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st145;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st145;
    } else
        goto st145;
    goto st0;
tr151:
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st146;
tr159:
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st146;
tr170:
//#line 68 "../../src/uri.rl"
    { s = p; }
//#line 69 "../../src/uri.rl"
    { uri->query = s; uri->query_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st146;
tr172:
//#line 69 "../../src/uri.rl"
    { uri->query = s; uri->query_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st146;
tr175:
//#line 131 "../../src/uri.rl"
    { s = p; }
//#line 132 "../../src/uri.rl"
    { uri->service = s; uri->service_len = p - s; }
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st146;
tr185:
//#line 132 "../../src/uri.rl"
    { uri->service = s; uri->service_len = p - s; }
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st146;
tr200:
//#line 100 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;
               uri->host_hint = 1; }
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st146;
tr209:
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st146;
tr314:
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 128 "../../src/uri.rl"
    { s = p;}
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st146;
tr318:
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st146;
tr323:
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st146;
st146:
    if ( ++p == pe )
        goto _test_eof146;
case 146:
//#line 281 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto tr165;
        case 37: goto tr166;
        case 61: goto tr165;
        case 95: goto tr165;
        case 126: goto tr165;
    }
    if ( (*p) < 63 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto tr165;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto tr165;
    } else
        goto tr165;
    goto st0;
tr165:
//#line 72 "../../src/uri.rl"
    { s = p; }
    goto st147;
st147:
    if ( ++p == pe )
        goto _test_eof147;
case 147:
//#line 306 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st147;
        case 37: goto st1;
        case 61: goto st147;
        case 95: goto st147;
        case 126: goto st147;
    }
    if ( (*p) < 63 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st147;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st147;
    } else
        goto st147;
    goto st0;
tr166:
//#line 72 "../../src/uri.rl"
    { s = p; }
    goto st1;
st1:
    if ( ++p == pe )
        goto _test_eof1;
case 1:
//#line 331 "../../src/uri.c"
    switch( (*p) ) {
        case 37: goto st147;
        case 117: goto st2;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st147;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st147;
    } else
        goto st147;
    goto st0;
st2:
    if ( ++p == pe )
        goto _test_eof2;
case 2:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st3;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st3;
    } else
        goto st3;
    goto st0;
st3:
    if ( ++p == pe )
        goto _test_eof3;
case 3:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st4;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st4;
    } else
        goto st4;
    goto st0;
st4:
    if ( ++p == pe )
        goto _test_eof4;
case 4:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st5;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st5;
    } else
        goto st5;
    goto st0;
st5:
    if ( ++p == pe )
        goto _test_eof5;
case 5:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st147;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st147;
    } else
        goto st147;
    goto st0;
tr152:
//#line 137 "../../src/uri.rl"
    { s = p; }
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st6;
st6:
    if ( ++p == pe )
        goto _test_eof6;
case 6:
//#line 407 "../../src/uri.c"
    switch( (*p) ) {
        case 37: goto st145;
        case 117: goto st7;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st145;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st145;
    } else
        goto st145;
    goto st0;
st7:
    if ( ++p == pe )
        goto _test_eof7;
case 7:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st8;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st8;
    } else
        goto st8;
    goto st0;
st8:
    if ( ++p == pe )
        goto _test_eof8;
case 8:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st9;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st9;
    } else
        goto st9;
    goto st0;
st9:
    if ( ++p == pe )
        goto _test_eof9;
case 9:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st10;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st10;
    } else
        goto st10;
    goto st0;
st10:
    if ( ++p == pe )
        goto _test_eof10;
case 10:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st145;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st145;
    } else
        goto st145;
    goto st0;
tr161:
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
//#line 161 "../../src/uri.rl"
    { s = p; }
    goto st148;
tr177:
//#line 131 "../../src/uri.rl"
    { s = p; }
//#line 132 "../../src/uri.rl"
    { uri->service = s; uri->service_len = p - s; }
//#line 161 "../../src/uri.rl"
    { s = p; }
    goto st148;
tr186:
//#line 132 "../../src/uri.rl"
    { uri->service = s; uri->service_len = p - s; }
//#line 161 "../../src/uri.rl"
    { s = p; }
    goto st148;
tr201:
//#line 100 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;
               uri->host_hint = 1; }
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
//#line 161 "../../src/uri.rl"
    { s = p; }
    goto st148;
tr210:
//#line 161 "../../src/uri.rl"
    { s = p; }
    goto st148;
st148:
    if ( ++p == pe )
        goto _test_eof148;
case 148:
//#line 510 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st148;
        case 35: goto tr151;
        case 37: goto st11;
        case 61: goto st148;
        case 63: goto tr155;
        case 95: goto st148;
        case 126: goto st148;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st148;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st148;
    } else
        goto st148;
    goto st0;
st11:
    if ( ++p == pe )
        goto _test_eof11;
case 11:
    switch( (*p) ) {
        case 37: goto st148;
        case 117: goto st12;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st148;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st148;
    } else
        goto st148;
    goto st0;
st12:
    if ( ++p == pe )
        goto _test_eof12;
case 12:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st13;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st13;
    } else
        goto st13;
    goto st0;
st13:
    if ( ++p == pe )
        goto _test_eof13;
case 13:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st14;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st14;
    } else
        goto st14;
    goto st0;
st14:
    if ( ++p == pe )
        goto _test_eof14;
case 14:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st15;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st15;
    } else
        goto st15;
    goto st0;
st15:
    if ( ++p == pe )
        goto _test_eof15;
case 15:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st148;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st148;
    } else
        goto st148;
    goto st0;
tr155:
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st149;
tr163:
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st149;
tr179:
//#line 131 "../../src/uri.rl"
    { s = p; }
//#line 132 "../../src/uri.rl"
    { uri->service = s; uri->service_len = p - s; }
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st149;
tr188:
//#line 132 "../../src/uri.rl"
    { uri->service = s; uri->service_len = p - s; }
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st149;
tr204:
//#line 100 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;
               uri->host_hint = 1; }
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st149;
tr212:
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st149;
tr317:
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 128 "../../src/uri.rl"
    { s = p;}
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st149;
tr320:
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st149;
tr325:
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
//#line 185 "../../src/uri.rl"
    { s = p; }
    goto st149;
st149:
    if ( ++p == pe )
        goto _test_eof149;
case 149:
//#line 734 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto tr169;
        case 35: goto tr170;
        case 37: goto tr171;
        case 61: goto tr169;
        case 95: goto tr169;
        case 126: goto tr169;
    }
    if ( (*p) < 63 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto tr169;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto tr169;
    } else
        goto tr169;
    goto st0;
tr169:
//#line 68 "../../src/uri.rl"
    { s = p; }
    goto st150;
st150:
    if ( ++p == pe )
        goto _test_eof150;
case 150:
//#line 760 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st150;
        case 35: goto tr172;
        case 37: goto st16;
        case 61: goto st150;
        case 95: goto st150;
        case 126: goto st150;
    }
    if ( (*p) < 63 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st150;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st150;
    } else
        goto st150;
    goto st0;
tr171:
//#line 68 "../../src/uri.rl"
    { s = p; }
    goto st16;
st16:
    if ( ++p == pe )
        goto _test_eof16;
case 16:
//#line 786 "../../src/uri.c"
    switch( (*p) ) {
        case 37: goto st150;
        case 117: goto st17;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st150;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st150;
    } else
        goto st150;
    goto st0;
st17:
    if ( ++p == pe )
        goto _test_eof17;
case 17:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st18;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st18;
    } else
        goto st18;
    goto st0;
st18:
    if ( ++p == pe )
        goto _test_eof18;
case 18:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st19;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st19;
    } else
        goto st19;
    goto st0;
st19:
    if ( ++p == pe )
        goto _test_eof19;
case 19:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st20;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st20;
    } else
        goto st20;
    goto st0;
st20:
    if ( ++p == pe )
        goto _test_eof20;
case 20:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st150;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st150;
    } else
        goto st150;
    goto st0;
tr162:
//#line 138 "../../src/uri.rl"
    { login = s; login_len = p - s; }
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
    goto st151;
tr240:
//#line 138 "../../src/uri.rl"
    { login = s; login_len = p - s; }
//#line 100 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;
               uri->host_hint = 1; }
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
    goto st151;
st151:
    if ( ++p == pe )
        goto _test_eof151;
case 151:
//#line 871 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto tr174;
        case 35: goto tr175;
        case 37: goto tr176;
        case 47: goto tr177;
        case 59: goto tr174;
        case 61: goto tr174;
        case 63: goto tr179;
        case 95: goto tr174;
        case 126: goto tr174;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto tr174;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto tr180;
        } else if ( (*p) >= 65 )
            goto tr180;
    } else
        goto tr178;
    goto st0;
tr174:
//#line 141 "../../src/uri.rl"
    { s = p; }
    goto st21;
st21:
    if ( ++p == pe )
        goto _test_eof21;
case 21:
//#line 903 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st21;
        case 37: goto st22;
        case 59: goto st21;
        case 61: goto st21;
        case 64: goto tr23;
        case 95: goto st21;
        case 126: goto st21;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st21;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st21;
        } else if ( (*p) >= 65 )
            goto st21;
    } else
        goto st21;
    goto st0;
tr176:
//#line 141 "../../src/uri.rl"
    { s = p; }
    goto st22;
st22:
    if ( ++p == pe )
        goto _test_eof22;
case 22:
//#line 933 "../../src/uri.c"
    switch( (*p) ) {
        case 37: goto st21;
        case 117: goto st23;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st21;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st21;
    } else
        goto st21;
    goto st0;
st23:
    if ( ++p == pe )
        goto _test_eof23;
case 23:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st24;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st24;
    } else
        goto st24;
    goto st0;
st24:
    if ( ++p == pe )
        goto _test_eof24;
case 24:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st25;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st25;
    } else
        goto st25;
    goto st0;
st25:
    if ( ++p == pe )
        goto _test_eof25;
case 25:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st26;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st26;
    } else
        goto st26;
    goto st0;
st26:
    if ( ++p == pe )
        goto _test_eof26;
case 26:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st21;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st21;
    } else
        goto st21;
    goto st0;
tr23:
//#line 142 "../../src/uri.rl"
    { uri->password = s; uri->password_len = p - s; }
//#line 146 "../../src/uri.rl"
    { uri->login = login; uri->login_len = login_len; }
    goto st27;
tr164:
//#line 138 "../../src/uri.rl"
    { login = s; login_len = p - s; }
//#line 146 "../../src/uri.rl"
    { uri->login = login; uri->login_len = login_len; }
    goto st27;
st27:
    if ( ++p == pe )
        goto _test_eof27;
case 27:
//#line 1015 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto tr28;
        case 37: goto tr29;
        case 47: goto tr30;
        case 59: goto tr28;
        case 61: goto tr28;
        case 91: goto st38;
        case 95: goto tr28;
        case 117: goto tr33;
        case 126: goto tr28;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto tr28;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto tr28;
        } else if ( (*p) >= 65 )
            goto tr28;
    } else
        goto tr31;
    goto st0;
tr28:
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st152;
st152:
    if ( ++p == pe )
        goto _test_eof152;
case 152:
//#line 1047 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 47: goto tr161;
        case 58: goto tr182;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st152;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st152;
    } else
        goto st152;
    goto st0;
tr29:
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st28;
st28:
    if ( ++p == pe )
        goto _test_eof28;
case 28:
//#line 1076 "../../src/uri.c"
    switch( (*p) ) {
        case 37: goto st152;
        case 117: goto st29;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st152;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st152;
    } else
        goto st152;
    goto st0;
st29:
    if ( ++p == pe )
        goto _test_eof29;
case 29:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st30;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st30;
    } else
        goto st30;
    goto st0;
st30:
    if ( ++p == pe )
        goto _test_eof30;
case 30:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st31;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st31;
    } else
        goto st31;
    goto st0;
st31:
    if ( ++p == pe )
        goto _test_eof31;
case 31:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st32;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st32;
    } else
        goto st32;
    goto st0;
st32:
    if ( ++p == pe )
        goto _test_eof32;
case 32:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st152;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st152;
    } else
        goto st152;
    goto st0;
tr182:
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
    goto st153;
tr203:
//#line 100 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;
               uri->host_hint = 1; }
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
    goto st153;
st153:
    if ( ++p == pe )
        goto _test_eof153;
case 153:
//#line 1157 "../../src/uri.c"
    switch( (*p) ) {
        case 35: goto tr175;
        case 47: goto tr177;
        case 63: goto tr179;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto tr183;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto tr184;
    } else
        goto tr184;
    goto st0;
tr183:
//#line 131 "../../src/uri.rl"
    { s = p; }
    goto st154;
st154:
    if ( ++p == pe )
        goto _test_eof154;
case 154:
//#line 1180 "../../src/uri.c"
    switch( (*p) ) {
        case 35: goto tr185;
        case 47: goto tr186;
        case 63: goto tr188;
    }
    if ( 48 <= (*p) && (*p) <= 57 )
        goto st154;
    goto st0;
tr184:
//#line 131 "../../src/uri.rl"
    { s = p; }
    goto st155;
st155:
    if ( ++p == pe )
        goto _test_eof155;
case 155:
//#line 1197 "../../src/uri.c"
    switch( (*p) ) {
        case 35: goto tr185;
        case 47: goto tr186;
        case 63: goto tr188;
    }
    if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st155;
    } else if ( (*p) >= 65 )
        goto st155;
    goto st0;
tr30:
//#line 182 "../../src/uri.rl"
    { s = p; }
    goto st156;
st156:
    if ( ++p == pe )
        goto _test_eof156;
case 156:
//#line 1217 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st157;
        case 37: goto st33;
        case 61: goto st157;
        case 95: goto st157;
        case 126: goto st157;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st157;
    } else if ( (*p) > 59 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st157;
        } else if ( (*p) >= 64 )
            goto st157;
    } else
        goto st157;
    goto st0;
st157:
    if ( ++p == pe )
        goto _test_eof157;
case 157:
    switch( (*p) ) {
        case 33: goto st157;
        case 37: goto st33;
        case 61: goto st157;
        case 95: goto st157;
        case 126: goto st157;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st157;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st157;
    } else
        goto st157;
    goto st0;
st33:
    if ( ++p == pe )
        goto _test_eof33;
case 33:
    switch( (*p) ) {
        case 37: goto st157;
        case 117: goto st34;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st157;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st157;
    } else
        goto st157;
    goto st0;
st34:
    if ( ++p == pe )
        goto _test_eof34;
case 34:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st35;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st35;
    } else
        goto st35;
    goto st0;
st35:
    if ( ++p == pe )
        goto _test_eof35;
case 35:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st36;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st36;
    } else
        goto st36;
    goto st0;
st36:
    if ( ++p == pe )
        goto _test_eof36;
case 36:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st37;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st37;
    } else
        goto st37;
    goto st0;
st37:
    if ( ++p == pe )
        goto _test_eof37;
case 37:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st157;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st157;
    } else
        goto st157;
    goto st0;
tr31:
//#line 99 "../../src/uri.rl"
    { s = p; }
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st158;
st158:
    if ( ++p == pe )
        goto _test_eof158;
case 158:
//#line 1336 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 46: goto st159;
        case 47: goto tr161;
        case 58: goto tr182;
        case 59: goto st152;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st152;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st152;
        } else if ( (*p) >= 65 )
            goto st152;
    } else
        goto st171;
    goto st0;
st159:
    if ( ++p == pe )
        goto _test_eof159;
case 159:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 47: goto tr161;
        case 58: goto tr182;
        case 59: goto st152;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st152;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st152;
        } else if ( (*p) >= 65 )
            goto st152;
    } else
        goto st160;
    goto st0;
st160:
    if ( ++p == pe )
        goto _test_eof160;
case 160:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 46: goto st161;
        case 47: goto tr161;
        case 58: goto tr182;
        case 59: goto st152;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st152;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st152;
        } else if ( (*p) >= 65 )
            goto st152;
    } else
        goto st169;
    goto st0;
st161:
    if ( ++p == pe )
        goto _test_eof161;
case 161:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 47: goto tr161;
        case 58: goto tr182;
        case 59: goto st152;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st152;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st152;
        } else if ( (*p) >= 65 )
            goto st152;
    } else
        goto st162;
    goto st0;
st162:
    if ( ++p == pe )
        goto _test_eof162;
case 162:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 46: goto st163;
        case 47: goto tr161;
        case 58: goto tr182;
        case 59: goto st152;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st152;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st152;
        } else if ( (*p) >= 65 )
            goto st152;
    } else
        goto st167;
    goto st0;
st163:
    if ( ++p == pe )
        goto _test_eof163;
case 163:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 47: goto tr161;
        case 58: goto tr182;
        case 59: goto st152;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st152;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st152;
        } else if ( (*p) >= 65 )
            goto st152;
    } else
        goto st164;
    goto st0;
st164:
    if ( ++p == pe )
        goto _test_eof164;
case 164:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr200;
        case 37: goto st28;
        case 47: goto tr201;
        case 58: goto tr203;
        case 59: goto st152;
        case 61: goto st152;
        case 63: goto tr204;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st152;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st152;
        } else if ( (*p) >= 65 )
            goto st152;
    } else
        goto st165;
    goto st0;
st165:
    if ( ++p == pe )
        goto _test_eof165;
case 165:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr200;
        case 37: goto st28;
        case 47: goto tr201;
        case 58: goto tr203;
        case 59: goto st152;
        case 61: goto st152;
        case 63: goto tr204;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st152;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st152;
        } else if ( (*p) >= 65 )
            goto st152;
    } else
        goto st166;
    goto st0;
st166:
    if ( ++p == pe )
        goto _test_eof166;
case 166:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr200;
        case 37: goto st28;
        case 47: goto tr201;
        case 58: goto tr203;
        case 61: goto st152;
        case 63: goto tr204;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st152;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st152;
    } else
        goto st152;
    goto st0;
st167:
    if ( ++p == pe )
        goto _test_eof167;
case 167:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 46: goto st163;
        case 47: goto tr161;
        case 58: goto tr182;
        case 59: goto st152;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st152;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st152;
        } else if ( (*p) >= 65 )
            goto st152;
    } else
        goto st168;
    goto st0;
st168:
    if ( ++p == pe )
        goto _test_eof168;
case 168:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 46: goto st163;
        case 47: goto tr161;
        case 58: goto tr182;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st152;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st152;
    } else
        goto st152;
    goto st0;
st169:
    if ( ++p == pe )
        goto _test_eof169;
case 169:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 46: goto st161;
        case 47: goto tr161;
        case 58: goto tr182;
        case 59: goto st152;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st152;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st152;
        } else if ( (*p) >= 65 )
            goto st152;
    } else
        goto st170;
    goto st0;
st170:
    if ( ++p == pe )
        goto _test_eof170;
case 170:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 46: goto st161;
        case 47: goto tr161;
        case 58: goto tr182;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st152;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st152;
    } else
        goto st152;
    goto st0;
st171:
    if ( ++p == pe )
        goto _test_eof171;
case 171:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 46: goto st159;
        case 47: goto tr161;
        case 58: goto tr182;
        case 59: goto st152;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st152;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st152;
        } else if ( (*p) >= 65 )
            goto st152;
    } else
        goto st172;
    goto st0;
st172:
    if ( ++p == pe )
        goto _test_eof172;
case 172:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 46: goto st159;
        case 47: goto tr161;
        case 58: goto tr182;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st152;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st152;
    } else
        goto st152;
    goto st0;
st38:
    if ( ++p == pe )
        goto _test_eof38;
case 38:
    if ( (*p) == 58 )
        goto tr45;
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto tr44;
    } else if ( (*p) >= 48 )
        goto tr44;
    goto st0;
tr44:
//#line 106 "../../src/uri.rl"
    { s = p; }
    goto st39;
st39:
    if ( ++p == pe )
        goto _test_eof39;
case 39:
//#line 1766 "../../src/uri.c"
    if ( (*p) == 58 )
        goto st43;
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st40;
    } else if ( (*p) >= 48 )
        goto st40;
    goto st0;
st40:
    if ( ++p == pe )
        goto _test_eof40;
case 40:
    if ( (*p) == 58 )
        goto st43;
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st41;
    } else if ( (*p) >= 48 )
        goto st41;
    goto st0;
st41:
    if ( ++p == pe )
        goto _test_eof41;
case 41:
    if ( (*p) == 58 )
        goto st43;
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st42;
    } else if ( (*p) >= 48 )
        goto st42;
    goto st0;
st42:
    if ( ++p == pe )
        goto _test_eof42;
case 42:
    if ( (*p) == 58 )
        goto st43;
    goto st0;
st43:
    if ( ++p == pe )
        goto _test_eof43;
case 43:
    switch( (*p) ) {
        case 58: goto st48;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st44;
    } else if ( (*p) >= 48 )
        goto st44;
    goto st0;
st44:
    if ( ++p == pe )
        goto _test_eof44;
case 44:
    switch( (*p) ) {
        case 58: goto st48;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st45;
    } else if ( (*p) >= 48 )
        goto st45;
    goto st0;
st45:
    if ( ++p == pe )
        goto _test_eof45;
case 45:
    switch( (*p) ) {
        case 58: goto st48;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st46;
    } else if ( (*p) >= 48 )
        goto st46;
    goto st0;
st46:
    if ( ++p == pe )
        goto _test_eof46;
case 46:
    switch( (*p) ) {
        case 58: goto st48;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st47;
    } else if ( (*p) >= 48 )
        goto st47;
    goto st0;
st47:
    if ( ++p == pe )
        goto _test_eof47;
case 47:
    switch( (*p) ) {
        case 58: goto st48;
        case 93: goto tr52;
    }
    goto st0;
st48:
    if ( ++p == pe )
        goto _test_eof48;
case 48:
    switch( (*p) ) {
        case 58: goto st53;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st49;
    } else if ( (*p) >= 48 )
        goto st49;
    goto st0;
st49:
    if ( ++p == pe )
        goto _test_eof49;
case 49:
    switch( (*p) ) {
        case 58: goto st53;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st50;
    } else if ( (*p) >= 48 )
        goto st50;
    goto st0;
st50:
    if ( ++p == pe )
        goto _test_eof50;
case 50:
    switch( (*p) ) {
        case 58: goto st53;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st51;
    } else if ( (*p) >= 48 )
        goto st51;
    goto st0;
st51:
    if ( ++p == pe )
        goto _test_eof51;
case 51:
    switch( (*p) ) {
        case 58: goto st53;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st52;
    } else if ( (*p) >= 48 )
        goto st52;
    goto st0;
st52:
    if ( ++p == pe )
        goto _test_eof52;
case 52:
    switch( (*p) ) {
        case 58: goto st53;
        case 93: goto tr52;
    }
    goto st0;
st53:
    if ( ++p == pe )
        goto _test_eof53;
case 53:
    switch( (*p) ) {
        case 58: goto st58;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st54;
    } else if ( (*p) >= 48 )
        goto st54;
    goto st0;
st54:
    if ( ++p == pe )
        goto _test_eof54;
case 54:
    switch( (*p) ) {
        case 58: goto st58;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st55;
    } else if ( (*p) >= 48 )
        goto st55;
    goto st0;
st55:
    if ( ++p == pe )
        goto _test_eof55;
case 55:
    switch( (*p) ) {
        case 58: goto st58;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st56;
    } else if ( (*p) >= 48 )
        goto st56;
    goto st0;
st56:
    if ( ++p == pe )
        goto _test_eof56;
case 56:
    switch( (*p) ) {
        case 58: goto st58;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st57;
    } else if ( (*p) >= 48 )
        goto st57;
    goto st0;
st57:
    if ( ++p == pe )
        goto _test_eof57;
case 57:
    switch( (*p) ) {
        case 58: goto st58;
        case 93: goto tr52;
    }
    goto st0;
st58:
    if ( ++p == pe )
        goto _test_eof58;
case 58:
    switch( (*p) ) {
        case 58: goto st63;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st59;
    } else if ( (*p) >= 48 )
        goto st59;
    goto st0;
st59:
    if ( ++p == pe )
        goto _test_eof59;
case 59:
    switch( (*p) ) {
        case 58: goto st63;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st60;
    } else if ( (*p) >= 48 )
        goto st60;
    goto st0;
st60:
    if ( ++p == pe )
        goto _test_eof60;
case 60:
    switch( (*p) ) {
        case 58: goto st63;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st61;
    } else if ( (*p) >= 48 )
        goto st61;
    goto st0;
st61:
    if ( ++p == pe )
        goto _test_eof61;
case 61:
    switch( (*p) ) {
        case 58: goto st63;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st62;
    } else if ( (*p) >= 48 )
        goto st62;
    goto st0;
st62:
    if ( ++p == pe )
        goto _test_eof62;
case 62:
    switch( (*p) ) {
        case 58: goto st63;
        case 93: goto tr52;
    }
    goto st0;
st63:
    if ( ++p == pe )
        goto _test_eof63;
case 63:
    switch( (*p) ) {
        case 58: goto st68;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st64;
    } else if ( (*p) >= 48 )
        goto st64;
    goto st0;
st64:
    if ( ++p == pe )
        goto _test_eof64;
case 64:
    switch( (*p) ) {
        case 58: goto st68;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st65;
    } else if ( (*p) >= 48 )
        goto st65;
    goto st0;
st65:
    if ( ++p == pe )
        goto _test_eof65;
case 65:
    switch( (*p) ) {
        case 58: goto st68;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st66;
    } else if ( (*p) >= 48 )
        goto st66;
    goto st0;
st66:
    if ( ++p == pe )
        goto _test_eof66;
case 66:
    switch( (*p) ) {
        case 58: goto st68;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st67;
    } else if ( (*p) >= 48 )
        goto st67;
    goto st0;
st67:
    if ( ++p == pe )
        goto _test_eof67;
case 67:
    switch( (*p) ) {
        case 58: goto st68;
        case 93: goto tr52;
    }
    goto st0;
st68:
    if ( ++p == pe )
        goto _test_eof68;
case 68:
    switch( (*p) ) {
        case 58: goto st73;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st69;
    } else if ( (*p) >= 48 )
        goto st69;
    goto st0;
st69:
    if ( ++p == pe )
        goto _test_eof69;
case 69:
    switch( (*p) ) {
        case 58: goto st73;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st70;
    } else if ( (*p) >= 48 )
        goto st70;
    goto st0;
st70:
    if ( ++p == pe )
        goto _test_eof70;
case 70:
    switch( (*p) ) {
        case 58: goto st73;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st71;
    } else if ( (*p) >= 48 )
        goto st71;
    goto st0;
st71:
    if ( ++p == pe )
        goto _test_eof71;
case 71:
    switch( (*p) ) {
        case 58: goto st73;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st72;
    } else if ( (*p) >= 48 )
        goto st72;
    goto st0;
st72:
    if ( ++p == pe )
        goto _test_eof72;
case 72:
    switch( (*p) ) {
        case 58: goto st73;
        case 93: goto tr52;
    }
    goto st0;
st73:
    if ( ++p == pe )
        goto _test_eof73;
case 73:
    switch( (*p) ) {
        case 58: goto st78;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st74;
    } else if ( (*p) >= 48 )
        goto st74;
    goto st0;
st74:
    if ( ++p == pe )
        goto _test_eof74;
case 74:
    switch( (*p) ) {
        case 58: goto st78;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st75;
    } else if ( (*p) >= 48 )
        goto st75;
    goto st0;
st75:
    if ( ++p == pe )
        goto _test_eof75;
case 75:
    switch( (*p) ) {
        case 58: goto st78;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st76;
    } else if ( (*p) >= 48 )
        goto st76;
    goto st0;
st76:
    if ( ++p == pe )
        goto _test_eof76;
case 76:
    switch( (*p) ) {
        case 58: goto st78;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st77;
    } else if ( (*p) >= 48 )
        goto st77;
    goto st0;
st77:
    if ( ++p == pe )
        goto _test_eof77;
case 77:
    switch( (*p) ) {
        case 58: goto st78;
        case 93: goto tr52;
    }
    goto st0;
st78:
    if ( ++p == pe )
        goto _test_eof78;
case 78:
    if ( (*p) == 93 )
        goto tr52;
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st79;
    } else if ( (*p) >= 48 )
        goto st79;
    goto st0;
st79:
    if ( ++p == pe )
        goto _test_eof79;
case 79:
    if ( (*p) == 93 )
        goto tr52;
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st80;
    } else if ( (*p) >= 48 )
        goto st80;
    goto st0;
st80:
    if ( ++p == pe )
        goto _test_eof80;
case 80:
    if ( (*p) == 93 )
        goto tr52;
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st81;
    } else if ( (*p) >= 48 )
        goto st81;
    goto st0;
st81:
    if ( ++p == pe )
        goto _test_eof81;
case 81:
    if ( (*p) == 93 )
        goto tr52;
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st82;
    } else if ( (*p) >= 48 )
        goto st82;
    goto st0;
st82:
    if ( ++p == pe )
        goto _test_eof82;
case 82:
    if ( (*p) == 93 )
        goto tr52;
    goto st0;
tr52:
//#line 107 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;
                   uri->host_hint = 2; }
    goto st173;
st173:
    if ( ++p == pe )
        goto _test_eof173;
case 173:
//#line 2325 "../../src/uri.c"
    switch( (*p) ) {
        case 35: goto tr209;
        case 47: goto tr210;
        case 58: goto st153;
        case 63: goto tr212;
    }
    goto st0;
tr45:
//#line 106 "../../src/uri.rl"
    { s = p; }
    goto st83;
st83:
    if ( ++p == pe )
        goto _test_eof83;
case 83:
//#line 2341 "../../src/uri.c"
    switch( (*p) ) {
        case 58: goto st84;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st44;
    } else if ( (*p) >= 48 )
        goto st44;
    goto st0;
st84:
    if ( ++p == pe )
        goto _test_eof84;
case 84:
    switch( (*p) ) {
        case 58: goto st53;
        case 93: goto tr52;
        case 102: goto st85;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 101 )
            goto st49;
    } else if ( (*p) >= 48 )
        goto st49;
    goto st0;
st85:
    if ( ++p == pe )
        goto _test_eof85;
case 85:
    switch( (*p) ) {
        case 58: goto st53;
        case 93: goto tr52;
        case 102: goto st86;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 101 )
            goto st50;
    } else if ( (*p) >= 48 )
        goto st50;
    goto st0;
st86:
    if ( ++p == pe )
        goto _test_eof86;
case 86:
    switch( (*p) ) {
        case 58: goto st53;
        case 93: goto tr52;
        case 102: goto st87;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 101 )
            goto st51;
    } else if ( (*p) >= 48 )
        goto st51;
    goto st0;
st87:
    if ( ++p == pe )
        goto _test_eof87;
case 87:
    switch( (*p) ) {
        case 58: goto st53;
        case 93: goto tr52;
        case 102: goto st88;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 101 )
            goto st52;
    } else if ( (*p) >= 48 )
        goto st52;
    goto st0;
st88:
    if ( ++p == pe )
        goto _test_eof88;
case 88:
    switch( (*p) ) {
        case 58: goto st89;
        case 93: goto tr52;
    }
    goto st0;
st89:
    if ( ++p == pe )
        goto _test_eof89;
case 89:
    switch( (*p) ) {
        case 58: goto st58;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st54;
    } else if ( (*p) >= 48 )
        goto st90;
    goto st0;
st90:
    if ( ++p == pe )
        goto _test_eof90;
case 90:
    switch( (*p) ) {
        case 46: goto st91;
        case 58: goto st58;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st55;
    } else if ( (*p) >= 48 )
        goto st102;
    goto st0;
st91:
    if ( ++p == pe )
        goto _test_eof91;
case 91:
    if ( 48 <= (*p) && (*p) <= 57 )
        goto st92;
    goto st0;
st92:
    if ( ++p == pe )
        goto _test_eof92;
case 92:
    if ( (*p) == 46 )
        goto st93;
    if ( 48 <= (*p) && (*p) <= 57 )
        goto st100;
    goto st0;
st93:
    if ( ++p == pe )
        goto _test_eof93;
case 93:
    if ( 48 <= (*p) && (*p) <= 57 )
        goto st94;
    goto st0;
st94:
    if ( ++p == pe )
        goto _test_eof94;
case 94:
    if ( (*p) == 46 )
        goto st95;
    if ( 48 <= (*p) && (*p) <= 57 )
        goto st98;
    goto st0;
st95:
    if ( ++p == pe )
        goto _test_eof95;
case 95:
    if ( 48 <= (*p) && (*p) <= 57 )
        goto st96;
    goto st0;
st96:
    if ( ++p == pe )
        goto _test_eof96;
case 96:
    if ( (*p) == 93 )
        goto tr52;
    if ( 48 <= (*p) && (*p) <= 57 )
        goto st97;
    goto st0;
st97:
    if ( ++p == pe )
        goto _test_eof97;
case 97:
    if ( (*p) == 93 )
        goto tr52;
    if ( 48 <= (*p) && (*p) <= 57 )
        goto st82;
    goto st0;
st98:
    if ( ++p == pe )
        goto _test_eof98;
case 98:
    if ( (*p) == 46 )
        goto st95;
    if ( 48 <= (*p) && (*p) <= 57 )
        goto st99;
    goto st0;
st99:
    if ( ++p == pe )
        goto _test_eof99;
case 99:
    if ( (*p) == 46 )
        goto st95;
    goto st0;
st100:
    if ( ++p == pe )
        goto _test_eof100;
case 100:
    if ( (*p) == 46 )
        goto st93;
    if ( 48 <= (*p) && (*p) <= 57 )
        goto st101;
    goto st0;
st101:
    if ( ++p == pe )
        goto _test_eof101;
case 101:
    if ( (*p) == 46 )
        goto st93;
    goto st0;
st102:
    if ( ++p == pe )
        goto _test_eof102;
case 102:
    switch( (*p) ) {
        case 46: goto st91;
        case 58: goto st58;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st56;
    } else if ( (*p) >= 48 )
        goto st103;
    goto st0;
st103:
    if ( ++p == pe )
        goto _test_eof103;
case 103:
    switch( (*p) ) {
        case 46: goto st91;
        case 58: goto st58;
        case 93: goto tr52;
    }
    if ( (*p) > 57 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st57;
    } else if ( (*p) >= 48 )
        goto st57;
    goto st0;
tr33:
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st174;
st174:
    if ( ++p == pe )
        goto _test_eof174;
case 174:
//#line 2577 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 47: goto tr161;
        case 58: goto tr182;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 110: goto st175;
        case 126: goto st152;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st152;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st152;
    } else
        goto st152;
    goto st0;
st175:
    if ( ++p == pe )
        goto _test_eof175;
case 175:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 47: goto tr161;
        case 58: goto tr182;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 105: goto st176;
        case 126: goto st152;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st152;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st152;
    } else
        goto st152;
    goto st0;
st176:
    if ( ++p == pe )
        goto _test_eof176;
case 176:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 47: goto tr161;
        case 58: goto tr182;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 120: goto st177;
        case 126: goto st152;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st152;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st152;
    } else
        goto st152;
    goto st0;
st177:
    if ( ++p == pe )
        goto _test_eof177;
case 177:
    switch( (*p) ) {
        case 33: goto st152;
        case 35: goto tr159;
        case 37: goto st28;
        case 47: goto tr216;
        case 58: goto tr182;
        case 61: goto st152;
        case 63: goto tr163;
        case 95: goto st152;
        case 126: goto st152;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st152;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st152;
    } else
        goto st152;
    goto st0;
tr216:
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
//#line 161 "../../src/uri.rl"
    { s = p; }
    goto st178;
st178:
    if ( ++p == pe )
        goto _test_eof178;
case 178:
//#line 2683 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st148;
        case 35: goto tr151;
        case 37: goto st11;
        case 58: goto st179;
        case 61: goto st148;
        case 63: goto tr155;
        case 95: goto st148;
        case 126: goto st148;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st148;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st148;
    } else
        goto st148;
    goto st0;
st179:
    if ( ++p == pe )
        goto _test_eof179;
case 179:
    switch( (*p) ) {
        case 33: goto tr218;
        case 35: goto tr151;
        case 37: goto tr219;
        case 47: goto tr220;
        case 58: goto tr221;
        case 61: goto tr218;
        case 63: goto tr155;
        case 95: goto tr218;
        case 126: goto tr218;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto tr218;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto tr218;
    } else
        goto tr218;
    goto st0;
tr218:
//#line 128 "../../src/uri.rl"
    { s = p;}
    goto st180;
st180:
    if ( ++p == pe )
        goto _test_eof180;
case 180:
//#line 2735 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st180;
        case 35: goto tr151;
        case 37: goto st104;
        case 47: goto st181;
        case 58: goto tr223;
        case 61: goto st180;
        case 63: goto tr155;
        case 95: goto st180;
        case 126: goto st180;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st180;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st180;
    } else
        goto st180;
    goto st0;
tr219:
//#line 128 "../../src/uri.rl"
    { s = p;}
    goto st104;
st104:
    if ( ++p == pe )
        goto _test_eof104;
case 104:
//#line 2764 "../../src/uri.c"
    switch( (*p) ) {
        case 37: goto st180;
        case 117: goto st105;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st180;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st180;
    } else
        goto st180;
    goto st0;
st105:
    if ( ++p == pe )
        goto _test_eof105;
case 105:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st106;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st106;
    } else
        goto st106;
    goto st0;
st106:
    if ( ++p == pe )
        goto _test_eof106;
case 106:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st107;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st107;
    } else
        goto st107;
    goto st0;
st107:
    if ( ++p == pe )
        goto _test_eof107;
case 107:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st108;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st108;
    } else
        goto st108;
    goto st0;
st108:
    if ( ++p == pe )
        goto _test_eof108;
case 108:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st180;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st180;
    } else
        goto st180;
    goto st0;
tr226:
//#line 161 "../../src/uri.rl"
    { s = p; }
    goto st181;
tr220:
//#line 128 "../../src/uri.rl"
    { s = p;}
    goto st181;
st181:
    if ( ++p == pe )
        goto _test_eof181;
case 181:
//#line 2842 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st181;
        case 35: goto tr151;
        case 37: goto st109;
        case 58: goto tr225;
        case 61: goto st181;
        case 63: goto tr155;
        case 95: goto st181;
        case 126: goto st181;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st181;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st181;
    } else
        goto st181;
    goto st0;
st109:
    if ( ++p == pe )
        goto _test_eof109;
case 109:
    switch( (*p) ) {
        case 37: goto st181;
        case 117: goto st110;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st181;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st181;
    } else
        goto st181;
    goto st0;
st110:
    if ( ++p == pe )
        goto _test_eof110;
case 110:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st111;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st111;
    } else
        goto st111;
    goto st0;
st111:
    if ( ++p == pe )
        goto _test_eof111;
case 111:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st112;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st112;
    } else
        goto st112;
    goto st0;
st112:
    if ( ++p == pe )
        goto _test_eof112;
case 112:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st113;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st113;
    } else
        goto st113;
    goto st0;
st113:
    if ( ++p == pe )
        goto _test_eof113;
case 113:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st181;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st181;
    } else
        goto st181;
    goto st0;
tr225:
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
    goto st182;
st182:
    if ( ++p == pe )
        goto _test_eof182;
case 182:
//#line 2954 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st181;
        case 35: goto tr209;
        case 37: goto st109;
        case 47: goto tr226;
        case 58: goto tr225;
        case 61: goto st181;
        case 63: goto tr212;
        case 95: goto st181;
        case 126: goto st181;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st181;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st181;
    } else
        goto st181;
    goto st0;
tr223:
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
    goto st183;
tr221:
//#line 128 "../../src/uri.rl"
    { s = p;}
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
    goto st183;
st183:
    if ( ++p == pe )
        goto _test_eof183;
case 183:
//#line 3019 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st148;
        case 35: goto tr209;
        case 37: goto st11;
        case 47: goto tr210;
        case 61: goto st148;
        case 63: goto tr212;
        case 95: goto st148;
        case 126: goto st148;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st148;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st148;
    } else
        goto st148;
    goto st0;
tr178:
//#line 141 "../../src/uri.rl"
    { s = p; }
//#line 131 "../../src/uri.rl"
    { s = p; }
    goto st184;
st184:
    if ( ++p == pe )
        goto _test_eof184;
case 184:
//#line 3049 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st21;
        case 35: goto tr185;
        case 37: goto st22;
        case 47: goto tr186;
        case 59: goto st21;
        case 61: goto st21;
        case 63: goto tr188;
        case 64: goto tr23;
        case 95: goto st21;
        case 126: goto st21;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st21;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st21;
        } else if ( (*p) >= 65 )
            goto st21;
    } else
        goto st184;
    goto st0;
tr180:
//#line 141 "../../src/uri.rl"
    { s = p; }
//#line 131 "../../src/uri.rl"
    { s = p; }
    goto st185;
st185:
    if ( ++p == pe )
        goto _test_eof185;
case 185:
//#line 3084 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st21;
        case 35: goto tr185;
        case 37: goto st22;
        case 47: goto tr186;
        case 59: goto st21;
        case 61: goto st21;
        case 63: goto tr188;
        case 64: goto tr23;
        case 95: goto st21;
        case 126: goto st21;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 57 )
            goto st21;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st185;
    } else
        goto st185;
    goto st0;
tr153:
//#line 182 "../../src/uri.rl"
    { s = p; }
    goto st186;
st186:
    if ( ++p == pe )
        goto _test_eof186;
case 186:
//#line 3114 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st187;
        case 35: goto tr151;
        case 37: goto st114;
        case 61: goto st187;
        case 63: goto tr155;
        case 95: goto st187;
        case 126: goto st187;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st187;
    } else if ( (*p) > 59 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st187;
        } else if ( (*p) >= 64 )
            goto st187;
    } else
        goto st187;
    goto st0;
st187:
    if ( ++p == pe )
        goto _test_eof187;
case 187:
    switch( (*p) ) {
        case 33: goto st187;
        case 35: goto tr151;
        case 37: goto st114;
        case 61: goto st187;
        case 63: goto tr155;
        case 95: goto st187;
        case 126: goto st187;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st187;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st187;
    } else
        goto st187;
    goto st0;
st114:
    if ( ++p == pe )
        goto _test_eof114;
case 114:
    switch( (*p) ) {
        case 37: goto st187;
        case 117: goto st115;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st187;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st187;
    } else
        goto st187;
    goto st0;
st115:
    if ( ++p == pe )
        goto _test_eof115;
case 115:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st116;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st116;
    } else
        goto st116;
    goto st0;
st116:
    if ( ++p == pe )
        goto _test_eof116;
case 116:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st117;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st117;
    } else
        goto st117;
    goto st0;
st117:
    if ( ++p == pe )
        goto _test_eof117;
case 117:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st118;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st118;
    } else
        goto st118;
    goto st0;
st118:
    if ( ++p == pe )
        goto _test_eof118;
case 118:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st187;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st187;
    } else
        goto st187;
    goto st0;
tr154:
//#line 137 "../../src/uri.rl"
    { s = p; }
//#line 99 "../../src/uri.rl"
    { s = p; }
//#line 92 "../../src/uri.rl"
    { s = p; }
//#line 178 "../../src/uri.rl"
    { uri->service = p; }
    goto st188;
st188:
    if ( ++p == pe )
        goto _test_eof188;
case 188:
//#line 3241 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 46: goto st189;
        case 47: goto tr161;
        case 58: goto tr162;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st145;
        } else if ( (*p) >= 65 )
            goto st145;
    } else
        goto st201;
    goto st0;
st189:
    if ( ++p == pe )
        goto _test_eof189;
case 189:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 47: goto tr161;
        case 58: goto tr162;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st145;
        } else if ( (*p) >= 65 )
            goto st145;
    } else
        goto st190;
    goto st0;
st190:
    if ( ++p == pe )
        goto _test_eof190;
case 190:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 46: goto st191;
        case 47: goto tr161;
        case 58: goto tr162;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st145;
        } else if ( (*p) >= 65 )
            goto st145;
    } else
        goto st199;
    goto st0;
st191:
    if ( ++p == pe )
        goto _test_eof191;
case 191:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 47: goto tr161;
        case 58: goto tr162;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st145;
        } else if ( (*p) >= 65 )
            goto st145;
    } else
        goto st192;
    goto st0;
st192:
    if ( ++p == pe )
        goto _test_eof192;
case 192:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 46: goto st193;
        case 47: goto tr161;
        case 58: goto tr162;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st145;
        } else if ( (*p) >= 65 )
            goto st145;
    } else
        goto st197;
    goto st0;
st193:
    if ( ++p == pe )
        goto _test_eof193;
case 193:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 47: goto tr161;
        case 58: goto tr162;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st145;
        } else if ( (*p) >= 65 )
            goto st145;
    } else
        goto st194;
    goto st0;
st194:
    if ( ++p == pe )
        goto _test_eof194;
case 194:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr200;
        case 37: goto st6;
        case 47: goto tr201;
        case 58: goto tr240;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr204;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st145;
        } else if ( (*p) >= 65 )
            goto st145;
    } else
        goto st195;
    goto st0;
st195:
    if ( ++p == pe )
        goto _test_eof195;
case 195:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr200;
        case 37: goto st6;
        case 47: goto tr201;
        case 58: goto tr240;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr204;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st145;
        } else if ( (*p) >= 65 )
            goto st145;
    } else
        goto st196;
    goto st0;
st196:
    if ( ++p == pe )
        goto _test_eof196;
case 196:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr200;
        case 37: goto st6;
        case 47: goto tr201;
        case 58: goto tr240;
        case 61: goto st145;
        case 63: goto tr204;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st145;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st145;
    } else
        goto st145;
    goto st0;
st197:
    if ( ++p == pe )
        goto _test_eof197;
case 197:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 46: goto st193;
        case 47: goto tr161;
        case 58: goto tr162;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st145;
        } else if ( (*p) >= 65 )
            goto st145;
    } else
        goto st198;
    goto st0;
st198:
    if ( ++p == pe )
        goto _test_eof198;
case 198:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 46: goto st193;
        case 47: goto tr161;
        case 58: goto tr162;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st145;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st145;
    } else
        goto st145;
    goto st0;
st199:
    if ( ++p == pe )
        goto _test_eof199;
case 199:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 46: goto st191;
        case 47: goto tr161;
        case 58: goto tr162;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st145;
        } else if ( (*p) >= 65 )
            goto st145;
    } else
        goto st200;
    goto st0;
st200:
    if ( ++p == pe )
        goto _test_eof200;
case 200:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 46: goto st191;
        case 47: goto tr161;
        case 58: goto tr162;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st145;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st145;
    } else
        goto st145;
    goto st0;
st201:
    if ( ++p == pe )
        goto _test_eof201;
case 201:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 46: goto st189;
        case 47: goto tr161;
        case 58: goto tr162;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st145;
        } else if ( (*p) >= 65 )
            goto st145;
    } else
        goto st202;
    goto st0;
st202:
    if ( ++p == pe )
        goto _test_eof202;
case 202:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 46: goto st189;
        case 47: goto tr161;
        case 58: goto tr162;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st145;
        } else if ( (*p) >= 65 )
            goto st145;
    } else
        goto st203;
    goto st0;
st203:
    if ( ++p == pe )
        goto _test_eof203;
case 203:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 47: goto tr161;
        case 58: goto tr162;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st145;
        } else if ( (*p) >= 65 )
            goto st145;
    } else
        goto st203;
    goto st0;
st204:
    if ( ++p == pe )
        goto _test_eof204;
case 204:
    switch( (*p) ) {
        case 35: goto tr151;
        case 47: goto st148;
        case 63: goto tr155;
    }
    goto st0;
tr157:
//#line 151 "../../src/uri.rl"
    { s = p; }
//#line 137 "../../src/uri.rl"
    { s = p; }
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st205;
st205:
    if ( ++p == pe )
        goto _test_eof205;
case 205:
//#line 3721 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 43: goto st205;
        case 47: goto tr161;
        case 58: goto tr247;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 45 ) {
        if ( 36 <= (*p) && (*p) <= 44 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st205;
        } else if ( (*p) >= 65 )
            goto st205;
    } else
        goto st205;
    goto st0;
tr247:
//#line 153 "../../src/uri.rl"
    {scheme = s; scheme_len = p - s; }
//#line 138 "../../src/uri.rl"
    { login = s; login_len = p - s; }
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
    goto st206;
st206:
    if ( ++p == pe )
        goto _test_eof206;
case 206:
//#line 3760 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto tr174;
        case 35: goto tr175;
        case 37: goto tr176;
        case 47: goto tr248;
        case 59: goto tr174;
        case 61: goto tr174;
        case 63: goto tr179;
        case 95: goto tr174;
        case 126: goto tr174;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto tr174;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto tr180;
        } else if ( (*p) >= 65 )
            goto tr180;
    } else
        goto tr178;
    goto st0;
tr248:
//#line 169 "../../src/uri.rl"
    { uri->scheme = scheme; uri->scheme_len = scheme_len;}
//#line 131 "../../src/uri.rl"
    { s = p; }
//#line 132 "../../src/uri.rl"
    { uri->service = s; uri->service_len = p - s; }
//#line 161 "../../src/uri.rl"
    { s = p; }
    goto st207;
st207:
    if ( ++p == pe )
        goto _test_eof207;
case 207:
//#line 3798 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st148;
        case 35: goto tr151;
        case 37: goto st11;
        case 47: goto st208;
        case 61: goto st148;
        case 63: goto tr155;
        case 95: goto st148;
        case 126: goto st148;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st148;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st148;
    } else
        goto st148;
    goto st0;
st208:
    if ( ++p == pe )
        goto _test_eof208;
case 208:
    switch( (*p) ) {
        case 33: goto tr250;
        case 35: goto tr151;
        case 37: goto tr251;
        case 47: goto st148;
        case 58: goto st148;
        case 59: goto tr250;
        case 61: goto tr250;
        case 63: goto tr155;
        case 64: goto st148;
        case 91: goto st38;
        case 95: goto tr250;
        case 117: goto tr253;
        case 126: goto tr250;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto tr250;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto tr250;
        } else if ( (*p) >= 65 )
            goto tr250;
    } else
        goto tr252;
    goto st0;
tr250:
//#line 137 "../../src/uri.rl"
    { s = p; }
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st209;
st209:
    if ( ++p == pe )
        goto _test_eof209;
case 209:
//#line 3859 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 47: goto tr161;
        case 58: goto tr255;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st209;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st209;
    } else
        goto st209;
    goto st0;
tr251:
//#line 137 "../../src/uri.rl"
    { s = p; }
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st119;
st119:
    if ( ++p == pe )
        goto _test_eof119;
case 119:
//#line 3891 "../../src/uri.c"
    switch( (*p) ) {
        case 37: goto st209;
        case 117: goto st120;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st209;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st209;
    } else
        goto st209;
    goto st0;
st120:
    if ( ++p == pe )
        goto _test_eof120;
case 120:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st121;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st121;
    } else
        goto st121;
    goto st0;
st121:
    if ( ++p == pe )
        goto _test_eof121;
case 121:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st122;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st122;
    } else
        goto st122;
    goto st0;
st122:
    if ( ++p == pe )
        goto _test_eof122;
case 122:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st123;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st123;
    } else
        goto st123;
    goto st0;
st123:
    if ( ++p == pe )
        goto _test_eof123;
case 123:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st209;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st209;
    } else
        goto st209;
    goto st0;
tr255:
//#line 138 "../../src/uri.rl"
    { login = s; login_len = p - s; }
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
    goto st210;
tr303:
//#line 138 "../../src/uri.rl"
    { login = s; login_len = p - s; }
//#line 100 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;
               uri->host_hint = 1; }
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
    goto st210;
st210:
    if ( ++p == pe )
        goto _test_eof210;
case 210:
//#line 3976 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto tr257;
        case 35: goto tr175;
        case 37: goto tr258;
        case 47: goto tr177;
        case 58: goto st148;
        case 59: goto tr257;
        case 61: goto tr257;
        case 63: goto tr179;
        case 64: goto st148;
        case 95: goto tr257;
        case 126: goto tr257;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto tr257;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto tr260;
        } else if ( (*p) >= 65 )
            goto tr260;
    } else
        goto tr259;
    goto st0;
tr257:
//#line 141 "../../src/uri.rl"
    { s = p; }
    goto st211;
st211:
    if ( ++p == pe )
        goto _test_eof211;
case 211:
//#line 4010 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st211;
        case 35: goto tr151;
        case 37: goto st124;
        case 47: goto st148;
        case 58: goto st148;
        case 61: goto st211;
        case 63: goto tr155;
        case 64: goto tr262;
        case 95: goto st211;
        case 126: goto st211;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st211;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st211;
    } else
        goto st211;
    goto st0;
tr258:
//#line 141 "../../src/uri.rl"
    { s = p; }
    goto st124;
st124:
    if ( ++p == pe )
        goto _test_eof124;
case 124:
//#line 4040 "../../src/uri.c"
    switch( (*p) ) {
        case 37: goto st211;
        case 117: goto st125;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st211;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st211;
    } else
        goto st211;
    goto st0;
st125:
    if ( ++p == pe )
        goto _test_eof125;
case 125:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st126;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st126;
    } else
        goto st126;
    goto st0;
st126:
    if ( ++p == pe )
        goto _test_eof126;
case 126:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st127;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st127;
    } else
        goto st127;
    goto st0;
st127:
    if ( ++p == pe )
        goto _test_eof127;
case 127:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st128;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st128;
    } else
        goto st128;
    goto st0;
st128:
    if ( ++p == pe )
        goto _test_eof128;
case 128:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st211;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st211;
    } else
        goto st211;
    goto st0;
tr262:
//#line 142 "../../src/uri.rl"
    { uri->password = s; uri->password_len = p - s; }
//#line 146 "../../src/uri.rl"
    { uri->login = login; uri->login_len = login_len; }
    goto st212;
tr256:
//#line 138 "../../src/uri.rl"
    { login = s; login_len = p - s; }
//#line 146 "../../src/uri.rl"
    { uri->login = login; uri->login_len = login_len; }
    goto st212;
st212:
    if ( ++p == pe )
        goto _test_eof212;
case 212:
//#line 4122 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto tr263;
        case 35: goto tr151;
        case 37: goto tr264;
        case 47: goto st148;
        case 58: goto st148;
        case 59: goto tr263;
        case 61: goto tr263;
        case 63: goto tr155;
        case 64: goto st148;
        case 91: goto st38;
        case 95: goto tr263;
        case 117: goto tr266;
        case 126: goto tr263;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto tr263;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto tr263;
        } else if ( (*p) >= 65 )
            goto tr263;
    } else
        goto tr265;
    goto st0;
tr263:
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st213;
st213:
    if ( ++p == pe )
        goto _test_eof213;
case 213:
//#line 4158 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 47: goto tr161;
        case 58: goto tr268;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st213;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st213;
    } else
        goto st213;
    goto st0;
tr264:
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st129;
st129:
    if ( ++p == pe )
        goto _test_eof129;
case 129:
//#line 4188 "../../src/uri.c"
    switch( (*p) ) {
        case 37: goto st213;
        case 117: goto st130;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st213;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st213;
    } else
        goto st213;
    goto st0;
st130:
    if ( ++p == pe )
        goto _test_eof130;
case 130:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st131;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st131;
    } else
        goto st131;
    goto st0;
st131:
    if ( ++p == pe )
        goto _test_eof131;
case 131:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st132;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st132;
    } else
        goto st132;
    goto st0;
st132:
    if ( ++p == pe )
        goto _test_eof132;
case 132:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st133;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st133;
    } else
        goto st133;
    goto st0;
st133:
    if ( ++p == pe )
        goto _test_eof133;
case 133:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st213;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st213;
    } else
        goto st213;
    goto st0;
tr268:
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
    goto st214;
tr283:
//#line 100 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;
               uri->host_hint = 1; }
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
    goto st214;
st214:
    if ( ++p == pe )
        goto _test_eof214;
case 214:
//#line 4269 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st148;
        case 35: goto tr175;
        case 37: goto st11;
        case 47: goto tr177;
        case 61: goto st148;
        case 63: goto tr179;
        case 64: goto st148;
        case 95: goto st148;
        case 126: goto st148;
    }
    if ( (*p) < 58 ) {
        if ( (*p) > 46 ) {
            if ( 48 <= (*p) && (*p) <= 57 )
                goto tr269;
        } else if ( (*p) >= 36 )
            goto st148;
    } else if ( (*p) > 59 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto tr270;
        } else if ( (*p) >= 65 )
            goto tr270;
    } else
        goto st148;
    goto st0;
tr269:
//#line 131 "../../src/uri.rl"
    { s = p; }
    goto st215;
st215:
    if ( ++p == pe )
        goto _test_eof215;
case 215:
//#line 4304 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st148;
        case 35: goto tr185;
        case 37: goto st11;
        case 47: goto tr186;
        case 61: goto st148;
        case 63: goto tr188;
        case 95: goto st148;
        case 126: goto st148;
    }
    if ( (*p) < 58 ) {
        if ( (*p) > 46 ) {
            if ( 48 <= (*p) && (*p) <= 57 )
                goto st215;
        } else if ( (*p) >= 36 )
            goto st148;
    } else if ( (*p) > 59 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st148;
        } else if ( (*p) >= 64 )
            goto st148;
    } else
        goto st148;
    goto st0;
tr270:
//#line 131 "../../src/uri.rl"
    { s = p; }
    goto st216;
st216:
    if ( ++p == pe )
        goto _test_eof216;
case 216:
//#line 4338 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st148;
        case 35: goto tr185;
        case 37: goto st11;
        case 47: goto tr186;
        case 61: goto st148;
        case 63: goto tr188;
        case 64: goto st148;
        case 95: goto st148;
        case 126: goto st148;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st148;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st216;
    } else
        goto st216;
    goto st0;
tr265:
//#line 99 "../../src/uri.rl"
    { s = p; }
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st217;
st217:
    if ( ++p == pe )
        goto _test_eof217;
case 217:
//#line 4369 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 46: goto st218;
        case 47: goto tr161;
        case 58: goto tr268;
        case 59: goto st213;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st213;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st213;
        } else if ( (*p) >= 65 )
            goto st213;
    } else
        goto st230;
    goto st0;
st218:
    if ( ++p == pe )
        goto _test_eof218;
case 218:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 47: goto tr161;
        case 58: goto tr268;
        case 59: goto st213;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st213;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st213;
        } else if ( (*p) >= 65 )
            goto st213;
    } else
        goto st219;
    goto st0;
st219:
    if ( ++p == pe )
        goto _test_eof219;
case 219:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 46: goto st220;
        case 47: goto tr161;
        case 58: goto tr268;
        case 59: goto st213;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st213;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st213;
        } else if ( (*p) >= 65 )
            goto st213;
    } else
        goto st228;
    goto st0;
st220:
    if ( ++p == pe )
        goto _test_eof220;
case 220:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 47: goto tr161;
        case 58: goto tr268;
        case 59: goto st213;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st213;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st213;
        } else if ( (*p) >= 65 )
            goto st213;
    } else
        goto st221;
    goto st0;
st221:
    if ( ++p == pe )
        goto _test_eof221;
case 221:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 46: goto st222;
        case 47: goto tr161;
        case 58: goto tr268;
        case 59: goto st213;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st213;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st213;
        } else if ( (*p) >= 65 )
            goto st213;
    } else
        goto st226;
    goto st0;
st222:
    if ( ++p == pe )
        goto _test_eof222;
case 222:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 47: goto tr161;
        case 58: goto tr268;
        case 59: goto st213;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st213;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st213;
        } else if ( (*p) >= 65 )
            goto st213;
    } else
        goto st223;
    goto st0;
st223:
    if ( ++p == pe )
        goto _test_eof223;
case 223:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr200;
        case 37: goto st129;
        case 47: goto tr201;
        case 58: goto tr283;
        case 59: goto st213;
        case 61: goto st213;
        case 63: goto tr204;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st213;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st213;
        } else if ( (*p) >= 65 )
            goto st213;
    } else
        goto st224;
    goto st0;
st224:
    if ( ++p == pe )
        goto _test_eof224;
case 224:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr200;
        case 37: goto st129;
        case 47: goto tr201;
        case 58: goto tr283;
        case 59: goto st213;
        case 61: goto st213;
        case 63: goto tr204;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st213;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st213;
        } else if ( (*p) >= 65 )
            goto st213;
    } else
        goto st225;
    goto st0;
st225:
    if ( ++p == pe )
        goto _test_eof225;
case 225:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr200;
        case 37: goto st129;
        case 47: goto tr201;
        case 58: goto tr283;
        case 61: goto st213;
        case 63: goto tr204;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st213;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st213;
    } else
        goto st213;
    goto st0;
st226:
    if ( ++p == pe )
        goto _test_eof226;
case 226:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 46: goto st222;
        case 47: goto tr161;
        case 58: goto tr268;
        case 59: goto st213;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st213;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st213;
        } else if ( (*p) >= 65 )
            goto st213;
    } else
        goto st227;
    goto st0;
st227:
    if ( ++p == pe )
        goto _test_eof227;
case 227:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 46: goto st222;
        case 47: goto tr161;
        case 58: goto tr268;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st213;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st213;
    } else
        goto st213;
    goto st0;
st228:
    if ( ++p == pe )
        goto _test_eof228;
case 228:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 46: goto st220;
        case 47: goto tr161;
        case 58: goto tr268;
        case 59: goto st213;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st213;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st213;
        } else if ( (*p) >= 65 )
            goto st213;
    } else
        goto st229;
    goto st0;
st229:
    if ( ++p == pe )
        goto _test_eof229;
case 229:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 46: goto st220;
        case 47: goto tr161;
        case 58: goto tr268;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st213;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st213;
    } else
        goto st213;
    goto st0;
st230:
    if ( ++p == pe )
        goto _test_eof230;
case 230:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 46: goto st218;
        case 47: goto tr161;
        case 58: goto tr268;
        case 59: goto st213;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st213;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st213;
        } else if ( (*p) >= 65 )
            goto st213;
    } else
        goto st231;
    goto st0;
st231:
    if ( ++p == pe )
        goto _test_eof231;
case 231:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 46: goto st218;
        case 47: goto tr161;
        case 58: goto tr268;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st213;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st213;
    } else
        goto st213;
    goto st0;
tr266:
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st232;
st232:
    if ( ++p == pe )
        goto _test_eof232;
case 232:
//#line 4802 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 47: goto tr161;
        case 58: goto tr268;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 110: goto st233;
        case 126: goto st213;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st213;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st213;
    } else
        goto st213;
    goto st0;
st233:
    if ( ++p == pe )
        goto _test_eof233;
case 233:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 47: goto tr161;
        case 58: goto tr268;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 105: goto st234;
        case 126: goto st213;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st213;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st213;
    } else
        goto st213;
    goto st0;
st234:
    if ( ++p == pe )
        goto _test_eof234;
case 234:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 47: goto tr161;
        case 58: goto tr268;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 120: goto st235;
        case 126: goto st213;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st213;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st213;
    } else
        goto st213;
    goto st0;
st235:
    if ( ++p == pe )
        goto _test_eof235;
case 235:
    switch( (*p) ) {
        case 33: goto st213;
        case 35: goto tr159;
        case 37: goto st129;
        case 47: goto tr216;
        case 58: goto tr268;
        case 61: goto st213;
        case 63: goto tr163;
        case 64: goto st148;
        case 95: goto st213;
        case 126: goto st213;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st213;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st213;
    } else
        goto st213;
    goto st0;
tr259:
//#line 141 "../../src/uri.rl"
    { s = p; }
//#line 131 "../../src/uri.rl"
    { s = p; }
    goto st236;
st236:
    if ( ++p == pe )
        goto _test_eof236;
case 236:
//#line 4912 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st211;
        case 35: goto tr185;
        case 37: goto st124;
        case 47: goto tr186;
        case 58: goto st148;
        case 59: goto st211;
        case 61: goto st211;
        case 63: goto tr188;
        case 64: goto tr262;
        case 95: goto st211;
        case 126: goto st211;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st211;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st211;
        } else if ( (*p) >= 65 )
            goto st211;
    } else
        goto st236;
    goto st0;
tr260:
//#line 141 "../../src/uri.rl"
    { s = p; }
//#line 131 "../../src/uri.rl"
    { s = p; }
    goto st237;
st237:
    if ( ++p == pe )
        goto _test_eof237;
case 237:
//#line 4948 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st211;
        case 35: goto tr185;
        case 37: goto st124;
        case 47: goto tr186;
        case 58: goto st148;
        case 61: goto st211;
        case 63: goto tr188;
        case 64: goto tr262;
        case 95: goto st211;
        case 126: goto st211;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st211;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st237;
    } else
        goto st237;
    goto st0;
tr252:
//#line 137 "../../src/uri.rl"
    { s = p; }
//#line 99 "../../src/uri.rl"
    { s = p; }
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st238;
st238:
    if ( ++p == pe )
        goto _test_eof238;
case 238:
//#line 4982 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 46: goto st239;
        case 47: goto tr161;
        case 58: goto tr255;
        case 59: goto st209;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st209;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st209;
        } else if ( (*p) >= 65 )
            goto st209;
    } else
        goto st251;
    goto st0;
st239:
    if ( ++p == pe )
        goto _test_eof239;
case 239:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 47: goto tr161;
        case 58: goto tr255;
        case 59: goto st209;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st209;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st209;
        } else if ( (*p) >= 65 )
            goto st209;
    } else
        goto st240;
    goto st0;
st240:
    if ( ++p == pe )
        goto _test_eof240;
case 240:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 46: goto st241;
        case 47: goto tr161;
        case 58: goto tr255;
        case 59: goto st209;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st209;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st209;
        } else if ( (*p) >= 65 )
            goto st209;
    } else
        goto st249;
    goto st0;
st241:
    if ( ++p == pe )
        goto _test_eof241;
case 241:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 47: goto tr161;
        case 58: goto tr255;
        case 59: goto st209;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st209;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st209;
        } else if ( (*p) >= 65 )
            goto st209;
    } else
        goto st242;
    goto st0;
st242:
    if ( ++p == pe )
        goto _test_eof242;
case 242:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 46: goto st243;
        case 47: goto tr161;
        case 58: goto tr255;
        case 59: goto st209;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st209;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st209;
        } else if ( (*p) >= 65 )
            goto st209;
    } else
        goto st247;
    goto st0;
st243:
    if ( ++p == pe )
        goto _test_eof243;
case 243:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 47: goto tr161;
        case 58: goto tr255;
        case 59: goto st209;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st209;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st209;
        } else if ( (*p) >= 65 )
            goto st209;
    } else
        goto st244;
    goto st0;
st244:
    if ( ++p == pe )
        goto _test_eof244;
case 244:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr200;
        case 37: goto st119;
        case 47: goto tr201;
        case 58: goto tr303;
        case 59: goto st209;
        case 61: goto st209;
        case 63: goto tr204;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st209;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st209;
        } else if ( (*p) >= 65 )
            goto st209;
    } else
        goto st245;
    goto st0;
st245:
    if ( ++p == pe )
        goto _test_eof245;
case 245:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr200;
        case 37: goto st119;
        case 47: goto tr201;
        case 58: goto tr303;
        case 59: goto st209;
        case 61: goto st209;
        case 63: goto tr204;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 46 )
            goto st209;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st209;
        } else if ( (*p) >= 65 )
            goto st209;
    } else
        goto st246;
    goto st0;
st246:
    if ( ++p == pe )
        goto _test_eof246;
case 246:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr200;
        case 37: goto st119;
        case 47: goto tr201;
        case 58: goto tr303;
        case 61: goto st209;
        case 63: goto tr204;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st209;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st209;
    } else
        goto st209;
    goto st0;
st247:
    if ( ++p == pe )
        goto _test_eof247;
case 247:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 46: goto st243;
        case 47: goto tr161;
        case 58: goto tr255;
        case 59: goto st209;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st209;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st209;
        } else if ( (*p) >= 65 )
            goto st209;
    } else
        goto st248;
    goto st0;
st248:
    if ( ++p == pe )
        goto _test_eof248;
case 248:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 46: goto st243;
        case 47: goto tr161;
        case 58: goto tr255;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st209;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st209;
    } else
        goto st209;
    goto st0;
st249:
    if ( ++p == pe )
        goto _test_eof249;
case 249:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 46: goto st241;
        case 47: goto tr161;
        case 58: goto tr255;
        case 59: goto st209;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st209;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st209;
        } else if ( (*p) >= 65 )
            goto st209;
    } else
        goto st250;
    goto st0;
st250:
    if ( ++p == pe )
        goto _test_eof250;
case 250:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 46: goto st241;
        case 47: goto tr161;
        case 58: goto tr255;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st209;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st209;
    } else
        goto st209;
    goto st0;
st251:
    if ( ++p == pe )
        goto _test_eof251;
case 251:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 46: goto st239;
        case 47: goto tr161;
        case 58: goto tr255;
        case 59: goto st209;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 48 ) {
        if ( 36 <= (*p) && (*p) <= 45 )
            goto st209;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st209;
        } else if ( (*p) >= 65 )
            goto st209;
    } else
        goto st252;
    goto st0;
st252:
    if ( ++p == pe )
        goto _test_eof252;
case 252:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 46: goto st239;
        case 47: goto tr161;
        case 58: goto tr255;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st209;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st209;
    } else
        goto st209;
    goto st0;
tr253:
//#line 137 "../../src/uri.rl"
    { s = p; }
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st253;
st253:
    if ( ++p == pe )
        goto _test_eof253;
case 253:
//#line 5417 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 47: goto tr161;
        case 58: goto tr255;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 110: goto st254;
        case 126: goto st209;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st209;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st209;
    } else
        goto st209;
    goto st0;
st254:
    if ( ++p == pe )
        goto _test_eof254;
case 254:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 47: goto tr161;
        case 58: goto tr255;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 105: goto st255;
        case 126: goto st209;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st209;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st209;
    } else
        goto st209;
    goto st0;
st255:
    if ( ++p == pe )
        goto _test_eof255;
case 255:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 47: goto tr161;
        case 58: goto tr255;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 120: goto st256;
        case 126: goto st209;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st209;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st209;
    } else
        goto st209;
    goto st0;
st256:
    if ( ++p == pe )
        goto _test_eof256;
case 256:
    switch( (*p) ) {
        case 33: goto st209;
        case 35: goto tr159;
        case 37: goto st119;
        case 47: goto tr311;
        case 58: goto tr255;
        case 61: goto st209;
        case 63: goto tr163;
        case 64: goto tr256;
        case 95: goto st209;
        case 126: goto st209;
    }
    if ( (*p) < 65 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st209;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st209;
    } else
        goto st209;
    goto st0;
tr311:
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
//#line 161 "../../src/uri.rl"
    { s = p; }
    goto st257;
st257:
    if ( ++p == pe )
        goto _test_eof257;
case 257:
//#line 5527 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st148;
        case 35: goto tr151;
        case 37: goto st11;
        case 58: goto st258;
        case 61: goto st148;
        case 63: goto tr155;
        case 95: goto st148;
        case 126: goto st148;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st148;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st148;
    } else
        goto st148;
    goto st0;
st258:
    if ( ++p == pe )
        goto _test_eof258;
case 258:
    switch( (*p) ) {
        case 33: goto tr313;
        case 35: goto tr314;
        case 37: goto tr315;
        case 47: goto tr316;
        case 58: goto tr221;
        case 61: goto tr313;
        case 63: goto tr317;
        case 95: goto tr313;
        case 126: goto tr313;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto tr313;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto tr313;
    } else
        goto tr313;
    goto st0;
tr313:
//#line 128 "../../src/uri.rl"
    { s = p;}
    goto st259;
st259:
    if ( ++p == pe )
        goto _test_eof259;
case 259:
//#line 5579 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st259;
        case 35: goto tr318;
        case 37: goto st134;
        case 47: goto st260;
        case 58: goto tr223;
        case 61: goto st259;
        case 63: goto tr320;
        case 95: goto st259;
        case 126: goto st259;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st259;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st259;
    } else
        goto st259;
    goto st0;
tr315:
//#line 128 "../../src/uri.rl"
    { s = p;}
    goto st134;
st134:
    if ( ++p == pe )
        goto _test_eof134;
case 134:
//#line 5608 "../../src/uri.c"
    switch( (*p) ) {
        case 37: goto st259;
        case 117: goto st135;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st259;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st259;
    } else
        goto st259;
    goto st0;
st135:
    if ( ++p == pe )
        goto _test_eof135;
case 135:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st136;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st136;
    } else
        goto st136;
    goto st0;
st136:
    if ( ++p == pe )
        goto _test_eof136;
case 136:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st137;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st137;
    } else
        goto st137;
    goto st0;
st137:
    if ( ++p == pe )
        goto _test_eof137;
case 137:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st138;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st138;
    } else
        goto st138;
    goto st0;
st138:
    if ( ++p == pe )
        goto _test_eof138;
case 138:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st259;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st259;
    } else
        goto st259;
    goto st0;
tr324:
//#line 161 "../../src/uri.rl"
    { s = p; }
    goto st260;
tr316:
//#line 128 "../../src/uri.rl"
    { s = p;}
    goto st260;
st260:
    if ( ++p == pe )
        goto _test_eof260;
case 260:
//#line 5686 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st260;
        case 35: goto tr318;
        case 37: goto st139;
        case 58: goto tr322;
        case 61: goto st260;
        case 63: goto tr320;
        case 95: goto st260;
        case 126: goto st260;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st260;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st260;
    } else
        goto st260;
    goto st0;
st139:
    if ( ++p == pe )
        goto _test_eof139;
case 139:
    switch( (*p) ) {
        case 37: goto st260;
        case 117: goto st140;
    }
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st260;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st260;
    } else
        goto st260;
    goto st0;
st140:
    if ( ++p == pe )
        goto _test_eof140;
case 140:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st141;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st141;
    } else
        goto st141;
    goto st0;
st141:
    if ( ++p == pe )
        goto _test_eof141;
case 141:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st142;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st142;
    } else
        goto st142;
    goto st0;
st142:
    if ( ++p == pe )
        goto _test_eof142;
case 142:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st143;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st143;
    } else
        goto st143;
    goto st0;
st143:
    if ( ++p == pe )
        goto _test_eof143;
case 143:
    if ( (*p) < 65 ) {
        if ( 48 <= (*p) && (*p) <= 57 )
            goto st260;
    } else if ( (*p) > 70 ) {
        if ( 97 <= (*p) && (*p) <= 102 )
            goto st260;
    } else
        goto st260;
    goto st0;
tr322:
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
    goto st261;
st261:
    if ( ++p == pe )
        goto _test_eof261;
case 261:
//#line 5798 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st260;
        case 35: goto tr323;
        case 37: goto st139;
        case 47: goto tr324;
        case 58: goto tr322;
        case 61: goto st260;
        case 63: goto tr325;
        case 95: goto st260;
        case 126: goto st260;
    }
    if ( (*p) < 64 ) {
        if ( 36 <= (*p) && (*p) <= 59 )
            goto st260;
    } else if ( (*p) > 90 ) {
        if ( 97 <= (*p) && (*p) <= 122 )
            goto st260;
    } else
        goto st260;
    goto st0;
tr158:
//#line 151 "../../src/uri.rl"
    { s = p; }
//#line 137 "../../src/uri.rl"
    { s = p; }
//#line 92 "../../src/uri.rl"
    { s = p; }
    goto st262;
st262:
    if ( ++p == pe )
        goto _test_eof262;
case 262:
//#line 5831 "../../src/uri.c"
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 43: goto st205;
        case 47: goto tr161;
        case 58: goto tr247;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 110: goto st263;
        case 126: goto st145;
    }
    if ( (*p) < 45 ) {
        if ( 36 <= (*p) && (*p) <= 44 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st205;
        } else if ( (*p) >= 65 )
            goto st205;
    } else
        goto st205;
    goto st0;
st263:
    if ( ++p == pe )
        goto _test_eof263;
case 263:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 43: goto st205;
        case 47: goto tr161;
        case 58: goto tr247;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 105: goto st264;
        case 126: goto st145;
    }
    if ( (*p) < 45 ) {
        if ( 36 <= (*p) && (*p) <= 44 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st205;
        } else if ( (*p) >= 65 )
            goto st205;
    } else
        goto st205;
    goto st0;
st264:
    if ( ++p == pe )
        goto _test_eof264;
case 264:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 43: goto st205;
        case 47: goto tr161;
        case 58: goto tr247;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 120: goto st265;
        case 126: goto st145;
    }
    if ( (*p) < 45 ) {
        if ( 36 <= (*p) && (*p) <= 44 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st205;
        } else if ( (*p) >= 65 )
            goto st205;
    } else
        goto st205;
    goto st0;
st265:
    if ( ++p == pe )
        goto _test_eof265;
case 265:
    switch( (*p) ) {
        case 33: goto st145;
        case 35: goto tr159;
        case 37: goto st6;
        case 43: goto st205;
        case 47: goto tr311;
        case 58: goto tr247;
        case 59: goto st145;
        case 61: goto st145;
        case 63: goto tr163;
        case 64: goto tr164;
        case 95: goto st145;
        case 126: goto st145;
    }
    if ( (*p) < 45 ) {
        if ( 36 <= (*p) && (*p) <= 44 )
            goto st145;
    } else if ( (*p) > 57 ) {
        if ( (*p) > 90 ) {
            if ( 97 <= (*p) && (*p) <= 122 )
                goto st205;
        } else if ( (*p) >= 65 )
            goto st205;
    } else
        goto st205;
    goto st0;
    }
    _test_eof145: cs = 145; goto _test_eof;
    _test_eof146: cs = 146; goto _test_eof;
    _test_eof147: cs = 147; goto _test_eof;
    _test_eof1: cs = 1; goto _test_eof;
    _test_eof2: cs = 2; goto _test_eof;
    _test_eof3: cs = 3; goto _test_eof;
    _test_eof4: cs = 4; goto _test_eof;
    _test_eof5: cs = 5; goto _test_eof;
    _test_eof6: cs = 6; goto _test_eof;
    _test_eof7: cs = 7; goto _test_eof;
    _test_eof8: cs = 8; goto _test_eof;
    _test_eof9: cs = 9; goto _test_eof;
    _test_eof10: cs = 10; goto _test_eof;
    _test_eof148: cs = 148; goto _test_eof;
    _test_eof11: cs = 11; goto _test_eof;
    _test_eof12: cs = 12; goto _test_eof;
    _test_eof13: cs = 13; goto _test_eof;
    _test_eof14: cs = 14; goto _test_eof;
    _test_eof15: cs = 15; goto _test_eof;
    _test_eof149: cs = 149; goto _test_eof;
    _test_eof150: cs = 150; goto _test_eof;
    _test_eof16: cs = 16; goto _test_eof;
    _test_eof17: cs = 17; goto _test_eof;
    _test_eof18: cs = 18; goto _test_eof;
    _test_eof19: cs = 19; goto _test_eof;
    _test_eof20: cs = 20; goto _test_eof;
    _test_eof151: cs = 151; goto _test_eof;
    _test_eof21: cs = 21; goto _test_eof;
    _test_eof22: cs = 22; goto _test_eof;
    _test_eof23: cs = 23; goto _test_eof;
    _test_eof24: cs = 24; goto _test_eof;
    _test_eof25: cs = 25; goto _test_eof;
    _test_eof26: cs = 26; goto _test_eof;
    _test_eof27: cs = 27; goto _test_eof;
    _test_eof152: cs = 152; goto _test_eof;
    _test_eof28: cs = 28; goto _test_eof;
    _test_eof29: cs = 29; goto _test_eof;
    _test_eof30: cs = 30; goto _test_eof;
    _test_eof31: cs = 31; goto _test_eof;
    _test_eof32: cs = 32; goto _test_eof;
    _test_eof153: cs = 153; goto _test_eof;
    _test_eof154: cs = 154; goto _test_eof;
    _test_eof155: cs = 155; goto _test_eof;
    _test_eof156: cs = 156; goto _test_eof;
    _test_eof157: cs = 157; goto _test_eof;
    _test_eof33: cs = 33; goto _test_eof;
    _test_eof34: cs = 34; goto _test_eof;
    _test_eof35: cs = 35; goto _test_eof;
    _test_eof36: cs = 36; goto _test_eof;
    _test_eof37: cs = 37; goto _test_eof;
    _test_eof158: cs = 158; goto _test_eof;
    _test_eof159: cs = 159; goto _test_eof;
    _test_eof160: cs = 160; goto _test_eof;
    _test_eof161: cs = 161; goto _test_eof;
    _test_eof162: cs = 162; goto _test_eof;
    _test_eof163: cs = 163; goto _test_eof;
    _test_eof164: cs = 164; goto _test_eof;
    _test_eof165: cs = 165; goto _test_eof;
    _test_eof166: cs = 166; goto _test_eof;
    _test_eof167: cs = 167; goto _test_eof;
    _test_eof168: cs = 168; goto _test_eof;
    _test_eof169: cs = 169; goto _test_eof;
    _test_eof170: cs = 170; goto _test_eof;
    _test_eof171: cs = 171; goto _test_eof;
    _test_eof172: cs = 172; goto _test_eof;
    _test_eof38: cs = 38; goto _test_eof;
    _test_eof39: cs = 39; goto _test_eof;
    _test_eof40: cs = 40; goto _test_eof;
    _test_eof41: cs = 41; goto _test_eof;
    _test_eof42: cs = 42; goto _test_eof;
    _test_eof43: cs = 43; goto _test_eof;
    _test_eof44: cs = 44; goto _test_eof;
    _test_eof45: cs = 45; goto _test_eof;
    _test_eof46: cs = 46; goto _test_eof;
    _test_eof47: cs = 47; goto _test_eof;
    _test_eof48: cs = 48; goto _test_eof;
    _test_eof49: cs = 49; goto _test_eof;
    _test_eof50: cs = 50; goto _test_eof;
    _test_eof51: cs = 51; goto _test_eof;
    _test_eof52: cs = 52; goto _test_eof;
    _test_eof53: cs = 53; goto _test_eof;
    _test_eof54: cs = 54; goto _test_eof;
    _test_eof55: cs = 55; goto _test_eof;
    _test_eof56: cs = 56; goto _test_eof;
    _test_eof57: cs = 57; goto _test_eof;
    _test_eof58: cs = 58; goto _test_eof;
    _test_eof59: cs = 59; goto _test_eof;
    _test_eof60: cs = 60; goto _test_eof;
    _test_eof61: cs = 61; goto _test_eof;
    _test_eof62: cs = 62; goto _test_eof;
    _test_eof63: cs = 63; goto _test_eof;
    _test_eof64: cs = 64; goto _test_eof;
    _test_eof65: cs = 65; goto _test_eof;
    _test_eof66: cs = 66; goto _test_eof;
    _test_eof67: cs = 67; goto _test_eof;
    _test_eof68: cs = 68; goto _test_eof;
    _test_eof69: cs = 69; goto _test_eof;
    _test_eof70: cs = 70; goto _test_eof;
    _test_eof71: cs = 71; goto _test_eof;
    _test_eof72: cs = 72; goto _test_eof;
    _test_eof73: cs = 73; goto _test_eof;
    _test_eof74: cs = 74; goto _test_eof;
    _test_eof75: cs = 75; goto _test_eof;
    _test_eof76: cs = 76; goto _test_eof;
    _test_eof77: cs = 77; goto _test_eof;
    _test_eof78: cs = 78; goto _test_eof;
    _test_eof79: cs = 79; goto _test_eof;
    _test_eof80: cs = 80; goto _test_eof;
    _test_eof81: cs = 81; goto _test_eof;
    _test_eof82: cs = 82; goto _test_eof;
    _test_eof173: cs = 173; goto _test_eof;
    _test_eof83: cs = 83; goto _test_eof;
    _test_eof84: cs = 84; goto _test_eof;
    _test_eof85: cs = 85; goto _test_eof;
    _test_eof86: cs = 86; goto _test_eof;
    _test_eof87: cs = 87; goto _test_eof;
    _test_eof88: cs = 88; goto _test_eof;
    _test_eof89: cs = 89; goto _test_eof;
    _test_eof90: cs = 90; goto _test_eof;
    _test_eof91: cs = 91; goto _test_eof;
    _test_eof92: cs = 92; goto _test_eof;
    _test_eof93: cs = 93; goto _test_eof;
    _test_eof94: cs = 94; goto _test_eof;
    _test_eof95: cs = 95; goto _test_eof;
    _test_eof96: cs = 96; goto _test_eof;
    _test_eof97: cs = 97; goto _test_eof;
    _test_eof98: cs = 98; goto _test_eof;
    _test_eof99: cs = 99; goto _test_eof;
    _test_eof100: cs = 100; goto _test_eof;
    _test_eof101: cs = 101; goto _test_eof;
    _test_eof102: cs = 102; goto _test_eof;
    _test_eof103: cs = 103; goto _test_eof;
    _test_eof174: cs = 174; goto _test_eof;
    _test_eof175: cs = 175; goto _test_eof;
    _test_eof176: cs = 176; goto _test_eof;
    _test_eof177: cs = 177; goto _test_eof;
    _test_eof178: cs = 178; goto _test_eof;
    _test_eof179: cs = 179; goto _test_eof;
    _test_eof180: cs = 180; goto _test_eof;
    _test_eof104: cs = 104; goto _test_eof;
    _test_eof105: cs = 105; goto _test_eof;
    _test_eof106: cs = 106; goto _test_eof;
    _test_eof107: cs = 107; goto _test_eof;
    _test_eof108: cs = 108; goto _test_eof;
    _test_eof181: cs = 181; goto _test_eof;
    _test_eof109: cs = 109; goto _test_eof;
    _test_eof110: cs = 110; goto _test_eof;
    _test_eof111: cs = 111; goto _test_eof;
    _test_eof112: cs = 112; goto _test_eof;
    _test_eof113: cs = 113; goto _test_eof;
    _test_eof182: cs = 182; goto _test_eof;
    _test_eof183: cs = 183; goto _test_eof;
    _test_eof184: cs = 184; goto _test_eof;
    _test_eof185: cs = 185; goto _test_eof;
    _test_eof186: cs = 186; goto _test_eof;
    _test_eof187: cs = 187; goto _test_eof;
    _test_eof114: cs = 114; goto _test_eof;
    _test_eof115: cs = 115; goto _test_eof;
    _test_eof116: cs = 116; goto _test_eof;
    _test_eof117: cs = 117; goto _test_eof;
    _test_eof118: cs = 118; goto _test_eof;
    _test_eof188: cs = 188; goto _test_eof;
    _test_eof189: cs = 189; goto _test_eof;
    _test_eof190: cs = 190; goto _test_eof;
    _test_eof191: cs = 191; goto _test_eof;
    _test_eof192: cs = 192; goto _test_eof;
    _test_eof193: cs = 193; goto _test_eof;
    _test_eof194: cs = 194; goto _test_eof;
    _test_eof195: cs = 195; goto _test_eof;
    _test_eof196: cs = 196; goto _test_eof;
    _test_eof197: cs = 197; goto _test_eof;
    _test_eof198: cs = 198; goto _test_eof;
    _test_eof199: cs = 199; goto _test_eof;
    _test_eof200: cs = 200; goto _test_eof;
    _test_eof201: cs = 201; goto _test_eof;
    _test_eof202: cs = 202; goto _test_eof;
    _test_eof203: cs = 203; goto _test_eof;
    _test_eof204: cs = 204; goto _test_eof;
    _test_eof205: cs = 205; goto _test_eof;
    _test_eof206: cs = 206; goto _test_eof;
    _test_eof207: cs = 207; goto _test_eof;
    _test_eof208: cs = 208; goto _test_eof;
    _test_eof209: cs = 209; goto _test_eof;
    _test_eof119: cs = 119; goto _test_eof;
    _test_eof120: cs = 120; goto _test_eof;
    _test_eof121: cs = 121; goto _test_eof;
    _test_eof122: cs = 122; goto _test_eof;
    _test_eof123: cs = 123; goto _test_eof;
    _test_eof210: cs = 210; goto _test_eof;
    _test_eof211: cs = 211; goto _test_eof;
    _test_eof124: cs = 124; goto _test_eof;
    _test_eof125: cs = 125; goto _test_eof;
    _test_eof126: cs = 126; goto _test_eof;
    _test_eof127: cs = 127; goto _test_eof;
    _test_eof128: cs = 128; goto _test_eof;
    _test_eof212: cs = 212; goto _test_eof;
    _test_eof213: cs = 213; goto _test_eof;
    _test_eof129: cs = 129; goto _test_eof;
    _test_eof130: cs = 130; goto _test_eof;
    _test_eof131: cs = 131; goto _test_eof;
    _test_eof132: cs = 132; goto _test_eof;
    _test_eof133: cs = 133; goto _test_eof;
    _test_eof214: cs = 214; goto _test_eof;
    _test_eof215: cs = 215; goto _test_eof;
    _test_eof216: cs = 216; goto _test_eof;
    _test_eof217: cs = 217; goto _test_eof;
    _test_eof218: cs = 218; goto _test_eof;
    _test_eof219: cs = 219; goto _test_eof;
    _test_eof220: cs = 220; goto _test_eof;
    _test_eof221: cs = 221; goto _test_eof;
    _test_eof222: cs = 222; goto _test_eof;
    _test_eof223: cs = 223; goto _test_eof;
    _test_eof224: cs = 224; goto _test_eof;
    _test_eof225: cs = 225; goto _test_eof;
    _test_eof226: cs = 226; goto _test_eof;
    _test_eof227: cs = 227; goto _test_eof;
    _test_eof228: cs = 228; goto _test_eof;
    _test_eof229: cs = 229; goto _test_eof;
    _test_eof230: cs = 230; goto _test_eof;
    _test_eof231: cs = 231; goto _test_eof;
    _test_eof232: cs = 232; goto _test_eof;
    _test_eof233: cs = 233; goto _test_eof;
    _test_eof234: cs = 234; goto _test_eof;
    _test_eof235: cs = 235; goto _test_eof;
    _test_eof236: cs = 236; goto _test_eof;
    _test_eof237: cs = 237; goto _test_eof;
    _test_eof238: cs = 238; goto _test_eof;
    _test_eof239: cs = 239; goto _test_eof;
    _test_eof240: cs = 240; goto _test_eof;
    _test_eof241: cs = 241; goto _test_eof;
    _test_eof242: cs = 242; goto _test_eof;
    _test_eof243: cs = 243; goto _test_eof;
    _test_eof244: cs = 244; goto _test_eof;
    _test_eof245: cs = 245; goto _test_eof;
    _test_eof246: cs = 246; goto _test_eof;
    _test_eof247: cs = 247; goto _test_eof;
    _test_eof248: cs = 248; goto _test_eof;
    _test_eof249: cs = 249; goto _test_eof;
    _test_eof250: cs = 250; goto _test_eof;
    _test_eof251: cs = 251; goto _test_eof;
    _test_eof252: cs = 252; goto _test_eof;
    _test_eof253: cs = 253; goto _test_eof;
    _test_eof254: cs = 254; goto _test_eof;
    _test_eof255: cs = 255; goto _test_eof;
    _test_eof256: cs = 256; goto _test_eof;
    _test_eof257: cs = 257; goto _test_eof;
    _test_eof258: cs = 258; goto _test_eof;
    _test_eof259: cs = 259; goto _test_eof;
    _test_eof134: cs = 134; goto _test_eof;
    _test_eof135: cs = 135; goto _test_eof;
    _test_eof136: cs = 136; goto _test_eof;
    _test_eof137: cs = 137; goto _test_eof;
    _test_eof138: cs = 138; goto _test_eof;
    _test_eof260: cs = 260; goto _test_eof;
    _test_eof139: cs = 139; goto _test_eof;
    _test_eof140: cs = 140; goto _test_eof;
    _test_eof141: cs = 141; goto _test_eof;
    _test_eof142: cs = 142; goto _test_eof;
    _test_eof143: cs = 143; goto _test_eof;
    _test_eof261: cs = 261; goto _test_eof;
    _test_eof262: cs = 262; goto _test_eof;
    _test_eof263: cs = 263; goto _test_eof;
    _test_eof264: cs = 264; goto _test_eof;
    _test_eof265: cs = 265; goto _test_eof;

    _test_eof: {}
    if ( p == eof )
    {
    switch ( cs ) {
    case 150:
//#line 69 "../../src/uri.rl"
    { uri->query = s; uri->query_len = p - s; }
    break;
    case 147:
//#line 73 "../../src/uri.rl"
    { uri->fragment = s; uri->fragment_len = p - s; }
    break;
    case 156:
    case 157:
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
    break;
    case 144:
    case 148:
    case 178:
    case 179:
    case 180:
    case 181:
    case 204:
    case 207:
    case 208:
    case 211:
    case 212:
    case 257:
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
    break;
    case 149:
//#line 68 "../../src/uri.rl"
    { s = p; }
//#line 69 "../../src/uri.rl"
    { uri->query = s; uri->query_len = p - s; }
    break;
    case 146:
//#line 72 "../../src/uri.rl"
    { s = p; }
//#line 73 "../../src/uri.rl"
    { uri->fragment = s; uri->fragment_len = p - s; }
    break;
    case 173:
    case 182:
    case 183:
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
    break;
    case 186:
    case 187:
    case 259:
    case 260:
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
    break;
    case 145:
    case 152:
    case 158:
    case 159:
    case 160:
    case 161:
    case 162:
    case 163:
    case 167:
    case 168:
    case 169:
    case 170:
    case 171:
    case 172:
    case 174:
    case 175:
    case 176:
    case 177:
    case 189:
    case 190:
    case 191:
    case 192:
    case 193:
    case 197:
    case 198:
    case 199:
    case 200:
    case 205:
    case 209:
    case 213:
    case 217:
    case 218:
    case 219:
    case 220:
    case 221:
    case 222:
    case 226:
    case 227:
    case 228:
    case 229:
    case 230:
    case 231:
    case 232:
    case 233:
    case 234:
    case 235:
    case 238:
    case 239:
    case 240:
    case 241:
    case 242:
    case 243:
    case 247:
    case 248:
    case 249:
    case 250:
    case 251:
    case 252:
    case 253:
    case 254:
    case 255:
    case 256:
    case 262:
    case 263:
    case 264:
    case 265:
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
    break;
    case 154:
    case 155:
    case 184:
    case 185:
    case 215:
    case 216:
    case 236:
    case 237:
//#line 132 "../../src/uri.rl"
    { uri->service = s; uri->service_len = p - s; }
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
    break;
    case 261:
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
    break;
    case 258:
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 128 "../../src/uri.rl"
    { s = p;}
//#line 111 "../../src/uri.rl"
    {
            /*
             * This action is also called for path_* terminals.
             * I absolute have no idea why. Please don't blame
             * and fix grammar if you have a LOT of free time.
             */
            if (uri->host_hint != 3) {
                uri->host_hint = 3;
                uri->host = URI_HOST_UNIX;
                uri->host_len = strlen(URI_HOST_UNIX);
                uri->service = s; uri->service_len = p - s;
                /* a workaround for grammar limitations */
                uri->path = NULL;
                uri->path_len = 0;
            };
        }
    break;
    case 188:
    case 201:
    case 202:
    case 203:
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
//#line 179 "../../src/uri.rl"
    { uri->service_len = p - uri->service;
               uri->host = NULL; uri->host_len = 0; }
    break;
    case 164:
    case 165:
    case 166:
    case 194:
    case 195:
    case 196:
    case 223:
    case 224:
    case 225:
    case 244:
    case 245:
    case 246:
//#line 100 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;
               uri->host_hint = 1; }
//#line 93 "../../src/uri.rl"
    { uri->host = s; uri->host_len = p - s;}
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
    break;
    case 151:
    case 153:
    case 206:
    case 210:
    case 214:
//#line 131 "../../src/uri.rl"
    { s = p; }
//#line 132 "../../src/uri.rl"
    { uri->service = s; uri->service_len = p - s; }
//#line 161 "../../src/uri.rl"
    { s = p; }
//#line 165 "../../src/uri.rl"
    { uri->path = s; uri->path_len = p - s; }
    break;
//#line 6492 "../../src/uri.c"
    }
    }

    _out: {}
    }

//#line 192 "../../src/uri.rl"


    if (uri->path_len == 0)
        uri->path = NULL;
    if (uri->service_len == 0)
        uri->service = NULL;
    if (uri->service_len >= URI_MAXSERVICE)
        return -1;
    if (uri->host_len >= URI_MAXHOST)
        return -1;

    (void)uri_first_final;
    (void)uri_error;
    (void)uri_en_main;
    (void)eof;

    return cs >= uri_first_final ? 0 : -1;
}

char *
uri_format(const struct uri *uri)
{
    static char buf[1024];
    /* very primitive implementation suitable for our needs */
    snprintf(buf, sizeof(buf), "%.*s:%.*s",
         (int) uri->host_len, uri->host != NULL ? uri->host : "*",
         (int) uri->service_len, uri->service);
    return buf;
}
/* vim: set ft=ragel: */

#endif /* URI_C_INCLUDED */

/*
   COPY: file name = third_party/msgpuck/hints.c
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef HINTS_C_INCLUDED
/*
 * Copyright (c) 2013-2017 MsgPuck Authors
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

//#include "msgpuck.h"

/**
 * This lookup table used by mp_sizeof() to determine enum mp_type by the first
 * byte of MsgPack element.
 */
const enum mp_type mp_type_hint[256]= {
    /* {{{ MP_UINT (fixed) */
    /* 0x00 */ MP_UINT,
    /* 0x01 */ MP_UINT,
    /* 0x02 */ MP_UINT,
    /* 0x03 */ MP_UINT,
    /* 0x04 */ MP_UINT,
    /* 0x05 */ MP_UINT,
    /* 0x06 */ MP_UINT,
    /* 0x07 */ MP_UINT,
    /* 0x08 */ MP_UINT,
    /* 0x09 */ MP_UINT,
    /* 0x0a */ MP_UINT,
    /* 0x0b */ MP_UINT,
    /* 0x0c */ MP_UINT,
    /* 0x0d */ MP_UINT,
    /* 0x0e */ MP_UINT,
    /* 0x0f */ MP_UINT,
    /* 0x10 */ MP_UINT,
    /* 0x11 */ MP_UINT,
    /* 0x12 */ MP_UINT,
    /* 0x13 */ MP_UINT,
    /* 0x14 */ MP_UINT,
    /* 0x15 */ MP_UINT,
    /* 0x16 */ MP_UINT,
    /* 0x17 */ MP_UINT,
    /* 0x18 */ MP_UINT,
    /* 0x19 */ MP_UINT,
    /* 0x1a */ MP_UINT,
    /* 0x1b */ MP_UINT,
    /* 0x1c */ MP_UINT,
    /* 0x1d */ MP_UINT,
    /* 0x1e */ MP_UINT,
    /* 0x1f */ MP_UINT,
    /* 0x20 */ MP_UINT,
    /* 0x21 */ MP_UINT,
    /* 0x22 */ MP_UINT,
    /* 0x23 */ MP_UINT,
    /* 0x24 */ MP_UINT,
    /* 0x25 */ MP_UINT,
    /* 0x26 */ MP_UINT,
    /* 0x27 */ MP_UINT,
    /* 0x28 */ MP_UINT,
    /* 0x29 */ MP_UINT,
    /* 0x2a */ MP_UINT,
    /* 0x2b */ MP_UINT,
    /* 0x2c */ MP_UINT,
    /* 0x2d */ MP_UINT,
    /* 0x2e */ MP_UINT,
    /* 0x2f */ MP_UINT,
    /* 0x30 */ MP_UINT,
    /* 0x31 */ MP_UINT,
    /* 0x32 */ MP_UINT,
    /* 0x33 */ MP_UINT,
    /* 0x34 */ MP_UINT,
    /* 0x35 */ MP_UINT,
    /* 0x36 */ MP_UINT,
    /* 0x37 */ MP_UINT,
    /* 0x38 */ MP_UINT,
    /* 0x39 */ MP_UINT,
    /* 0x3a */ MP_UINT,
    /* 0x3b */ MP_UINT,
    /* 0x3c */ MP_UINT,
    /* 0x3d */ MP_UINT,
    /* 0x3e */ MP_UINT,
    /* 0x3f */ MP_UINT,
    /* 0x40 */ MP_UINT,
    /* 0x41 */ MP_UINT,
    /* 0x42 */ MP_UINT,
    /* 0x43 */ MP_UINT,
    /* 0x44 */ MP_UINT,
    /* 0x45 */ MP_UINT,
    /* 0x46 */ MP_UINT,
    /* 0x47 */ MP_UINT,
    /* 0x48 */ MP_UINT,
    /* 0x49 */ MP_UINT,
    /* 0x4a */ MP_UINT,
    /* 0x4b */ MP_UINT,
    /* 0x4c */ MP_UINT,
    /* 0x4d */ MP_UINT,
    /* 0x4e */ MP_UINT,
    /* 0x4f */ MP_UINT,
    /* 0x50 */ MP_UINT,
    /* 0x51 */ MP_UINT,
    /* 0x52 */ MP_UINT,
    /* 0x53 */ MP_UINT,
    /* 0x54 */ MP_UINT,
    /* 0x55 */ MP_UINT,
    /* 0x56 */ MP_UINT,
    /* 0x57 */ MP_UINT,
    /* 0x58 */ MP_UINT,
    /* 0x59 */ MP_UINT,
    /* 0x5a */ MP_UINT,
    /* 0x5b */ MP_UINT,
    /* 0x5c */ MP_UINT,
    /* 0x5d */ MP_UINT,
    /* 0x5e */ MP_UINT,
    /* 0x5f */ MP_UINT,
    /* 0x60 */ MP_UINT,
    /* 0x61 */ MP_UINT,
    /* 0x62 */ MP_UINT,
    /* 0x63 */ MP_UINT,
    /* 0x64 */ MP_UINT,
    /* 0x65 */ MP_UINT,
    /* 0x66 */ MP_UINT,
    /* 0x67 */ MP_UINT,
    /* 0x68 */ MP_UINT,
    /* 0x69 */ MP_UINT,
    /* 0x6a */ MP_UINT,
    /* 0x6b */ MP_UINT,
    /* 0x6c */ MP_UINT,
    /* 0x6d */ MP_UINT,
    /* 0x6e */ MP_UINT,
    /* 0x6f */ MP_UINT,
    /* 0x70 */ MP_UINT,
    /* 0x71 */ MP_UINT,
    /* 0x72 */ MP_UINT,
    /* 0x73 */ MP_UINT,
    /* 0x74 */ MP_UINT,
    /* 0x75 */ MP_UINT,
    /* 0x76 */ MP_UINT,
    /* 0x77 */ MP_UINT,
    /* 0x78 */ MP_UINT,
    /* 0x79 */ MP_UINT,
    /* 0x7a */ MP_UINT,
    /* 0x7b */ MP_UINT,
    /* 0x7c */ MP_UINT,
    /* 0x7d */ MP_UINT,
    /* 0x7e */ MP_UINT,
    /* 0x7f */ MP_UINT,
    /* }}} */

    /* {{{ MP_MAP (fixed) */
    /* 0x80 */ MP_MAP,
    /* 0x81 */ MP_MAP,
    /* 0x82 */ MP_MAP,
    /* 0x83 */ MP_MAP,
    /* 0x84 */ MP_MAP,
    /* 0x85 */ MP_MAP,
    /* 0x86 */ MP_MAP,
    /* 0x87 */ MP_MAP,
    /* 0x88 */ MP_MAP,
    /* 0x89 */ MP_MAP,
    /* 0x8a */ MP_MAP,
    /* 0x8b */ MP_MAP,
    /* 0x8c */ MP_MAP,
    /* 0x8d */ MP_MAP,
    /* 0x8e */ MP_MAP,
    /* 0x8f */ MP_MAP,
    /* }}} */

    /* {{{ MP_ARRAY (fixed) */
    /* 0x90 */ MP_ARRAY,
    /* 0x91 */ MP_ARRAY,
    /* 0x92 */ MP_ARRAY,
    /* 0x93 */ MP_ARRAY,
    /* 0x94 */ MP_ARRAY,
    /* 0x95 */ MP_ARRAY,
    /* 0x96 */ MP_ARRAY,
    /* 0x97 */ MP_ARRAY,
    /* 0x98 */ MP_ARRAY,
    /* 0x99 */ MP_ARRAY,
    /* 0x9a */ MP_ARRAY,
    /* 0x9b */ MP_ARRAY,
    /* 0x9c */ MP_ARRAY,
    /* 0x9d */ MP_ARRAY,
    /* 0x9e */ MP_ARRAY,
    /* 0x9f */ MP_ARRAY,
    /* }}} */

    /* {{{ MP_STR (fixed) */
    /* 0xa0 */ MP_STR,
    /* 0xa1 */ MP_STR,
    /* 0xa2 */ MP_STR,
    /* 0xa3 */ MP_STR,
    /* 0xa4 */ MP_STR,
    /* 0xa5 */ MP_STR,
    /* 0xa6 */ MP_STR,
    /* 0xa7 */ MP_STR,
    /* 0xa8 */ MP_STR,
    /* 0xa9 */ MP_STR,
    /* 0xaa */ MP_STR,
    /* 0xab */ MP_STR,
    /* 0xac */ MP_STR,
    /* 0xad */ MP_STR,
    /* 0xae */ MP_STR,
    /* 0xaf */ MP_STR,
    /* 0xb0 */ MP_STR,
    /* 0xb1 */ MP_STR,
    /* 0xb2 */ MP_STR,
    /* 0xb3 */ MP_STR,
    /* 0xb4 */ MP_STR,
    /* 0xb5 */ MP_STR,
    /* 0xb6 */ MP_STR,
    /* 0xb7 */ MP_STR,
    /* 0xb8 */ MP_STR,
    /* 0xb9 */ MP_STR,
    /* 0xba */ MP_STR,
    /* 0xbb */ MP_STR,
    /* 0xbc */ MP_STR,
    /* 0xbd */ MP_STR,
    /* 0xbe */ MP_STR,
    /* 0xbf */ MP_STR,
    /* }}} */

    /* {{{ MP_NIL, MP_BOOL */
    /* 0xc0 */ MP_NIL,
    /* 0xc1 */ MP_EXT, /* never used */
    /* 0xc2 */ MP_BOOL,
    /* 0xc3 */ MP_BOOL,
    /* }}} */

    /* {{{ MP_BIN */
    /* 0xc4 */ MP_BIN,   /* MP_BIN(8)  */
    /* 0xc5 */ MP_BIN,   /* MP_BIN(16) */
    /* 0xc6 */ MP_BIN,   /* MP_BIN(32) */
    /* }}} */

    /* {{{ MP_EXT */
    /* 0xc7 */ MP_EXT,
    /* 0xc8 */ MP_EXT,
    /* 0xc9 */ MP_EXT,
    /* }}} */

    /* {{{ MP_FLOAT, MP_DOUBLE */
    /* 0xca */ MP_FLOAT,
    /* 0xcb */ MP_DOUBLE,
    /* }}} */

    /* {{{ MP_UINT */
    /* 0xcc */ MP_UINT,
    /* 0xcd */ MP_UINT,
    /* 0xce */ MP_UINT,
    /* 0xcf */ MP_UINT,
    /* }}} */

    /* {{{ MP_INT */
    /* 0xd0 */ MP_INT,   /* MP_INT (8)  */
    /* 0xd1 */ MP_INT,   /* MP_INT (16) */
    /* 0xd2 */ MP_INT,   /* MP_INT (32) */
    /* 0xd3 */ MP_INT,   /* MP_INT (64) */
    /* }}} */

    /* {{{ MP_EXT */
    /* 0xd4 */ MP_EXT,   /* MP_INT (8)    */
    /* 0xd5 */ MP_EXT,   /* MP_INT (16)   */
    /* 0xd6 */ MP_EXT,   /* MP_INT (32)   */
    /* 0xd7 */ MP_EXT,   /* MP_INT (64)   */
    /* 0xd8 */ MP_EXT,   /* MP_INT (127)  */
    /* }}} */

    /* {{{ MP_STR */
    /* 0xd9 */ MP_STR,   /* MP_STR(8)  */
    /* 0xda */ MP_STR,   /* MP_STR(16) */
    /* 0xdb */ MP_STR,   /* MP_STR(32) */
    /* }}} */

    /* {{{ MP_ARRAY */
    /* 0xdc */ MP_ARRAY, /* MP_ARRAY(16)  */
    /* 0xdd */ MP_ARRAY, /* MP_ARRAY(32)  */
    /* }}} */

    /* {{{ MP_MAP */
    /* 0xde */ MP_MAP,   /* MP_MAP (16) */
    /* 0xdf */ MP_MAP,   /* MP_MAP (32) */
    /* }}} */

    /* {{{ MP_INT */
    /* 0xe0 */ MP_INT,
    /* 0xe1 */ MP_INT,
    /* 0xe2 */ MP_INT,
    /* 0xe3 */ MP_INT,
    /* 0xe4 */ MP_INT,
    /* 0xe5 */ MP_INT,
    /* 0xe6 */ MP_INT,
    /* 0xe7 */ MP_INT,
    /* 0xe8 */ MP_INT,
    /* 0xe9 */ MP_INT,
    /* 0xea */ MP_INT,
    /* 0xeb */ MP_INT,
    /* 0xec */ MP_INT,
    /* 0xed */ MP_INT,
    /* 0xee */ MP_INT,
    /* 0xef */ MP_INT,
    /* 0xf0 */ MP_INT,
    /* 0xf1 */ MP_INT,
    /* 0xf2 */ MP_INT,
    /* 0xf3 */ MP_INT,
    /* 0xf4 */ MP_INT,
    /* 0xf5 */ MP_INT,
    /* 0xf6 */ MP_INT,
    /* 0xf7 */ MP_INT,
    /* 0xf8 */ MP_INT,
    /* 0xf9 */ MP_INT,
    /* 0xfa */ MP_INT,
    /* 0xfb */ MP_INT,
    /* 0xfc */ MP_INT,
    /* 0xfd */ MP_INT,
    /* 0xfe */ MP_INT,
    /* 0xff */ MP_INT
    /* }}} */
};

/**
 * This lookup table used by mp_next() and mp_check() to determine
 * size of MsgPack element by its first byte.
 * A positive value contains size of the element (excluding the first byte).
 * A negative value means the element is compound (e.g. array or map)
 * of size (-n).
 * MP_HINT_* values used for special cases handled by switch() statement.
 */
const int8_t mp_parser_hint[256] = {
    /* {{{ MP_UINT(fixed) **/
    /* 0x00 */ 0,
    /* 0x01 */ 0,
    /* 0x02 */ 0,
    /* 0x03 */ 0,
    /* 0x04 */ 0,
    /* 0x05 */ 0,
    /* 0x06 */ 0,
    /* 0x07 */ 0,
    /* 0x08 */ 0,
    /* 0x09 */ 0,
    /* 0x0a */ 0,
    /* 0x0b */ 0,
    /* 0x0c */ 0,
    /* 0x0d */ 0,
    /* 0x0e */ 0,
    /* 0x0f */ 0,
    /* 0x10 */ 0,
    /* 0x11 */ 0,
    /* 0x12 */ 0,
    /* 0x13 */ 0,
    /* 0x14 */ 0,
    /* 0x15 */ 0,
    /* 0x16 */ 0,
    /* 0x17 */ 0,
    /* 0x18 */ 0,
    /* 0x19 */ 0,
    /* 0x1a */ 0,
    /* 0x1b */ 0,
    /* 0x1c */ 0,
    /* 0x1d */ 0,
    /* 0x1e */ 0,
    /* 0x1f */ 0,
    /* 0x20 */ 0,
    /* 0x21 */ 0,
    /* 0x22 */ 0,
    /* 0x23 */ 0,
    /* 0x24 */ 0,
    /* 0x25 */ 0,
    /* 0x26 */ 0,
    /* 0x27 */ 0,
    /* 0x28 */ 0,
    /* 0x29 */ 0,
    /* 0x2a */ 0,
    /* 0x2b */ 0,
    /* 0x2c */ 0,
    /* 0x2d */ 0,
    /* 0x2e */ 0,
    /* 0x2f */ 0,
    /* 0x30 */ 0,
    /* 0x31 */ 0,
    /* 0x32 */ 0,
    /* 0x33 */ 0,
    /* 0x34 */ 0,
    /* 0x35 */ 0,
    /* 0x36 */ 0,
    /* 0x37 */ 0,
    /* 0x38 */ 0,
    /* 0x39 */ 0,
    /* 0x3a */ 0,
    /* 0x3b */ 0,
    /* 0x3c */ 0,
    /* 0x3d */ 0,
    /* 0x3e */ 0,
    /* 0x3f */ 0,
    /* 0x40 */ 0,
    /* 0x41 */ 0,
    /* 0x42 */ 0,
    /* 0x43 */ 0,
    /* 0x44 */ 0,
    /* 0x45 */ 0,
    /* 0x46 */ 0,
    /* 0x47 */ 0,
    /* 0x48 */ 0,
    /* 0x49 */ 0,
    /* 0x4a */ 0,
    /* 0x4b */ 0,
    /* 0x4c */ 0,
    /* 0x4d */ 0,
    /* 0x4e */ 0,
    /* 0x4f */ 0,
    /* 0x50 */ 0,
    /* 0x51 */ 0,
    /* 0x52 */ 0,
    /* 0x53 */ 0,
    /* 0x54 */ 0,
    /* 0x55 */ 0,
    /* 0x56 */ 0,
    /* 0x57 */ 0,
    /* 0x58 */ 0,
    /* 0x59 */ 0,
    /* 0x5a */ 0,
    /* 0x5b */ 0,
    /* 0x5c */ 0,
    /* 0x5d */ 0,
    /* 0x5e */ 0,
    /* 0x5f */ 0,
    /* 0x60 */ 0,
    /* 0x61 */ 0,
    /* 0x62 */ 0,
    /* 0x63 */ 0,
    /* 0x64 */ 0,
    /* 0x65 */ 0,
    /* 0x66 */ 0,
    /* 0x67 */ 0,
    /* 0x68 */ 0,
    /* 0x69 */ 0,
    /* 0x6a */ 0,
    /* 0x6b */ 0,
    /* 0x6c */ 0,
    /* 0x6d */ 0,
    /* 0x6e */ 0,
    /* 0x6f */ 0,
    /* 0x70 */ 0,
    /* 0x71 */ 0,
    /* 0x72 */ 0,
    /* 0x73 */ 0,
    /* 0x74 */ 0,
    /* 0x75 */ 0,
    /* 0x76 */ 0,
    /* 0x77 */ 0,
    /* 0x78 */ 0,
    /* 0x79 */ 0,
    /* 0x7a */ 0,
    /* 0x7b */ 0,
    /* 0x7c */ 0,
    /* 0x7d */ 0,
    /* 0x7e */ 0,
    /* 0x7f */ 0,
    /* }}} */

    /* {{{ MP_MAP (fixed) */
    /* 0x80 */  0, /* empty map - just skip one byte */
    /* 0x81 */ -2, /* 2 elements follow */
    /* 0x82 */ -4,
    /* 0x83 */ -6,
    /* 0x84 */ -8,
    /* 0x85 */ -10,
    /* 0x86 */ -12,
    /* 0x87 */ -14,
    /* 0x88 */ -16,
    /* 0x89 */ -18,
    /* 0x8a */ -20,
    /* 0x8b */ -22,
    /* 0x8c */ -24,
    /* 0x8d */ -26,
    /* 0x8e */ -28,
    /* 0x8f */ -30,
    /* }}} */

    /* {{{ MP_ARRAY (fixed) */
    /* 0x90 */  0,  /* empty array - just skip one byte */
    /* 0x91 */ -1,  /* 1 element follows */
    /* 0x92 */ -2,
    /* 0x93 */ -3,
    /* 0x94 */ -4,
    /* 0x95 */ -5,
    /* 0x96 */ -6,
    /* 0x97 */ -7,
    /* 0x98 */ -8,
    /* 0x99 */ -9,
    /* 0x9a */ -10,
    /* 0x9b */ -11,
    /* 0x9c */ -12,
    /* 0x9d */ -13,
    /* 0x9e */ -14,
    /* 0x9f */ -15,
    /* }}} */

    /* {{{ MP_STR (fixed) */
    /* 0xa0 */ 0,
    /* 0xa1 */ 1,
    /* 0xa2 */ 2,
    /* 0xa3 */ 3,
    /* 0xa4 */ 4,
    /* 0xa5 */ 5,
    /* 0xa6 */ 6,
    /* 0xa7 */ 7,
    /* 0xa8 */ 8,
    /* 0xa9 */ 9,
    /* 0xaa */ 10,
    /* 0xab */ 11,
    /* 0xac */ 12,
    /* 0xad */ 13,
    /* 0xae */ 14,
    /* 0xaf */ 15,
    /* 0xb0 */ 16,
    /* 0xb1 */ 17,
    /* 0xb2 */ 18,
    /* 0xb3 */ 19,
    /* 0xb4 */ 20,
    /* 0xb5 */ 21,
    /* 0xb6 */ 22,
    /* 0xb7 */ 23,
    /* 0xb8 */ 24,
    /* 0xb9 */ 25,
    /* 0xba */ 26,
    /* 0xbb */ 27,
    /* 0xbc */ 28,
    /* 0xbd */ 29,
    /* 0xbe */ 30,
    /* 0xbf */ 31,
    /* }}} */

    /* {{{ MP_NIL, MP_BOOL */
    /* 0xc0 */ 0, /* MP_NIL */
    /* 0xc1 */ 0, /* never used */
    /* 0xc2 */ 0, /* MP_BOOL*/
    /* 0xc3 */ 0, /* MP_BOOL*/
    /* }}} */

    /* {{{ MP_BIN */
    /* 0xc4 */ MP_HINT_STR_8,  /* MP_BIN (8)  */
    /* 0xc5 */ MP_HINT_STR_16, /* MP_BIN (16) */
    /* 0xc6 */ MP_HINT_STR_32, /* MP_BIN (32) */
    /* }}} */

    /* {{{ MP_EXT */
    /* 0xc7 */ MP_HINT_EXT_8,    /* MP_EXT (8)  */
    /* 0xc8 */ MP_HINT_EXT_16,   /* MP_EXT (16) */
    /* 0xc9 */ MP_HINT_EXT_32,   /* MP_EXT (32) */
    /* }}} */

    /* {{{ MP_FLOAT, MP_DOUBLE */
    /* 0xca */ sizeof(float),    /* MP_FLOAT */
    /* 0xcb */ sizeof(double),   /* MP_DOUBLE */
    /* }}} */

    /* {{{ MP_UINT */
    /* 0xcc */ sizeof(uint8_t),  /* MP_UINT (8)  */
    /* 0xcd */ sizeof(uint16_t), /* MP_UINT (16) */
    /* 0xce */ sizeof(uint32_t), /* MP_UINT (32) */
    /* 0xcf */ sizeof(uint64_t), /* MP_UINT (64) */
    /* }}} */

    /* {{{ MP_INT */
    /* 0xd0 */ sizeof(uint8_t),  /* MP_INT (8)  */
    /* 0xd1 */ sizeof(uint16_t), /* MP_INT (8)  */
    /* 0xd2 */ sizeof(uint32_t), /* MP_INT (8)  */
    /* 0xd3 */ sizeof(uint64_t), /* MP_INT (8)  */
    /* }}} */

    /* {{{ MP_EXT (fixext) */
    /* 0xd4 */ 2,  /* MP_EXT (fixext 8)   */
    /* 0xd5 */ 3,  /* MP_EXT (fixext 16)  */
    /* 0xd6 */ 5,  /* MP_EXT (fixext 32)  */
    /* 0xd7 */ 9,  /* MP_EXT (fixext 64)  */
    /* 0xd8 */ 17, /* MP_EXT (fixext 128) */
    /* }}} */

    /* {{{ MP_STR */
    /* 0xd9 */ MP_HINT_STR_8,      /* MP_STR (8) */
    /* 0xda */ MP_HINT_STR_16,     /* MP_STR (16) */
    /* 0xdb */ MP_HINT_STR_32,     /* MP_STR (32) */
    /* }}} */

    /* {{{ MP_ARRAY */
    /* 0xdc */ MP_HINT_ARRAY_16,   /* MP_ARRAY (16) */
    /* 0xdd */ MP_HINT_ARRAY_32,   /* MP_ARRAY (32) */
    /* }}} */

    /* {{{ MP_MAP */
    /* 0xde */ MP_HINT_MAP_16,     /* MP_MAP (16) */
    /* 0xdf */ MP_HINT_MAP_32,     /* MP_MAP (32) */
    /* }}} */

    /* {{{ MP_INT (fixed) */
    /* 0xe0 */ 0,
    /* 0xe1 */ 0,
    /* 0xe2 */ 0,
    /* 0xe3 */ 0,
    /* 0xe4 */ 0,
    /* 0xe5 */ 0,
    /* 0xe6 */ 0,
    /* 0xe7 */ 0,
    /* 0xe8 */ 0,
    /* 0xe9 */ 0,
    /* 0xea */ 0,
    /* 0xeb */ 0,
    /* 0xec */ 0,
    /* 0xed */ 0,
    /* 0xee */ 0,
    /* 0xef */ 0,
    /* 0xf0 */ 0,
    /* 0xf1 */ 0,
    /* 0xf2 */ 0,
    /* 0xf3 */ 0,
    /* 0xf4 */ 0,
    /* 0xf5 */ 0,
    /* 0xf6 */ 0,
    /* 0xf7 */ 0,
    /* 0xf8 */ 0,
    /* 0xf9 */ 0,
    /* 0xfa */ 0,
    /* 0xfb */ 0,
    /* 0xfc */ 0,
    /* 0xfd */ 0,
    /* 0xfe */ 0,
    /* 0xff */ 0
    /* }}} */
};

const char *mp_char2escape[128] = {
    "\\u0000", "\\u0001", "\\u0002", "\\u0003",
    "\\u0004", "\\u0005", "\\u0006", "\\u0007",
    "\\b", "\\t", "\\n", "\\u000b",
    "\\f", "\\r", "\\u000e", "\\u000f",
    "\\u0010", "\\u0011", "\\u0012", "\\u0013",
    "\\u0014", "\\u0015", "\\u0016", "\\u0017",
    "\\u0018", "\\u0019", "\\u001a", "\\u001b",
    "\\u001c", "\\u001d", "\\u001e", "\\u001f",
    NULL, NULL, "\\\"", NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, "\\/",
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, "\\\\", NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, "\\u007f"
};

#define HINTS_C_INCLUDED
#endif /* HINTS_C_INCLUDED */

/*
   COPY: file name = third_party/msgpuck/msgpuck.c
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef MSGPUCK_C_INCLUDED
#define MSGPUCK_C_INCLUDED
/*
 * Copyright (c) 2013-2017 MsgPuck Authors
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define MP_LIBRARY 1
//#include "msgpuck.h"

size_t
mp_vformat(char *data, size_t data_size, const char *format, va_list vl)
{
    size_t result = 0;
    const char *f = NULL;

    for (f = format; *f; f++) {
        if (f[0] == '[') {
            uint32_t size = 0;
            int level = 1;
            const char *e = NULL;

            for (e = f + 1; level && *e; e++) {
                if (*e == '[' || *e == '{') {
                    if (level == 1)
                        size++;
                    level++;
                } else if (*e == ']' || *e == '}') {
                    level--;
                    /* opened '[' must be closed by ']' */
                    assert(level || *e == ']');
                } else if (*e == '%') {
                    if (e[1] == '%')
                        e++;
                    else if (level == 1)
                        size++;
                } else if (*e == 'N' && e[1] == 'I'
                       && e[2] == 'L' && level == 1) {
                    size++;
                }
            }
            /* opened '[' must be closed */
            assert(level == 0);
            result += mp_sizeof_array(size);
            if (result <= data_size)
                data = mp_encode_array(data, size);
        } else if (f[0] == '{') {
            uint32_t count = 0;
            int level = 1;
            const char *e = NULL;

            for (e = f + 1; level && *e; e++) {
                if (*e == '[' || *e == '{') {
                    if (level == 1)
                        count++;
                    level++;
                } else if (*e == ']' || *e == '}') {
                    level--;
                    /* opened '{' must be closed by '}' */
                    assert(level || *e == '}');
                } else if (*e == '%') {
                    if (e[1] == '%')
                        e++;
                    else if (level == 1)
                        count++;
                } else if (*e == 'N' && e[1] == 'I'
                       && e[2] == 'L' && level == 1) {
                    count++;
                }
            }
            /* opened '{' must be closed */
            assert(level == 0);
            /* since map is a pair list, count must be even */
            assert(count % 2 == 0);
            uint32_t size = count / 2;
            result += mp_sizeof_map(size);
            if (result <= data_size)
                data = mp_encode_map(data, size);
        } else if (f[0] == '%') {
            f++;
            assert(f[0]);
            int64_t int_value = 0;
            int int_status = 0; /* 1 - signed, 2 - unsigned */

            if (f[0] == 'd' || f[0] == 'i') {
                int_value = va_arg(vl, int);
                int_status = 1;
            } else if (f[0] == 'u') {
                int_value = va_arg(vl, unsigned int);
                int_status = 2;
            } else if (f[0] == 's') {
                const char *str = va_arg(vl, const char *);
                uint32_t len = (uint32_t)strlen(str);
                result += mp_sizeof_str(len);
                if (result <= data_size)
                    data = mp_encode_str(data, str, len);
            } else if (f[0] == '.' && f[1] == '*' && f[2] == 's') {
                uint32_t len = va_arg(vl, uint32_t);
                const char *str = va_arg(vl, const char *);
                result += mp_sizeof_str(len);
                if (result <= data_size)
                    data = mp_encode_str(data, str, len);
                f += 2;
            } else if (f[0] == 'p') {
                const char *p = va_arg(vl, const char *);
                const char *end = p;
                mp_next(&end);
                uint32_t len = end - p;
                result += len;
                if (result <= data_size) {
                    memcpy(data, p, len);
                    data += len;
                }
            } else if (f[0] == '.' && f[1] == '*' && f[2] == 'p') {
                uint32_t len = va_arg(vl, uint32_t);
                const char *p = va_arg(vl, const char *);
                assert(len > 0);
                result += len;
                if (result <= data_size) {
                    memcpy(data, p, len);
                    data += len;
                }
                f += 2;
            } else if(f[0] == 'f') {
                float v = (float)va_arg(vl, double);
                result += mp_sizeof_float(v);
                if (result <= data_size)
                    data = mp_encode_float(data, v);
            } else if(f[0] == 'l' && f[1] == 'f') {
                double v = va_arg(vl, double);
                result += mp_sizeof_double(v);
                if (result <= data_size)
                    data = mp_encode_double(data, v);
                f++;
            } else if(f[0] == 'b') {
                bool v = (bool)va_arg(vl, int);
                result += mp_sizeof_bool(v);
                if (result <= data_size)
                    data = mp_encode_bool(data, v);
            } else if (f[0] == 'l'
                   && (f[1] == 'd' || f[1] == 'i')) {
                int_value = va_arg(vl, long);
                int_status = 1;
                f++;
            } else if (f[0] == 'l' && f[1] == 'u') {
                int_value = va_arg(vl, unsigned long);
                int_status = 2;
                f++;
            } else if (f[0] == 'l' && f[1] == 'l'
                   && (f[2] == 'd' || f[2] == 'i')) {
                int_value = va_arg(vl, long long);
                int_status = 1;
                f += 2;
            } else if (f[0] == 'l' && f[1] == 'l' && f[2] == 'u') {
                int_value = va_arg(vl, unsigned long long);
                int_status = 2;
                f += 2;
            } else if (f[0] == 'h'
                   && (f[1] == 'd' || f[1] == 'i')) {
                int_value = va_arg(vl, int);
                int_status = 1;
                f++;
            } else if (f[0] == 'h' && f[1] == 'u') {
                int_value = va_arg(vl, unsigned int);
                int_status = 2;
                f++;
            } else if (f[0] == 'h' && f[1] == 'h'
                   && (f[2] == 'd' || f[2] == 'i')) {
                int_value = va_arg(vl, int);
                int_status = 1;
                f += 2;
            } else if (f[0] == 'h' && f[1] == 'h' && f[2] == 'u') {
                int_value = va_arg(vl, unsigned int);
                int_status = 2;
                f += 2;
            } else if (f[0] != '%') {
                /* unexpected format specifier */
                assert(false);
            }

            if (int_status == 1 && int_value < 0) {
                result += mp_sizeof_int(int_value);
                if (result <= data_size)
                    data = mp_encode_int(data, int_value);
            } else if(int_status) {
                result += mp_sizeof_uint(int_value);
                if (result <= data_size)
                    data = mp_encode_uint(data, int_value);
            }
        } else if (f[0] == 'N' && f[1] == 'I' && f[2] == 'L') {
            result += mp_sizeof_nil();
            if (result <= data_size)
                data = mp_encode_nil(data);
            f += 2;
        }
    }
    return result;
}

size_t
mp_format(char *data, size_t data_size, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    size_t res = mp_vformat(data, data_size, format, args);
    va_end(args);
    return res;
}

#define MP_PRINT(SELF, PRINTF) \
{										\
    switch (mp_typeof(**data)) {						\
    case MP_NIL:								\
        mp_decode_nil(data);						\
        PRINTF("null");							\
        break;								\
    case MP_UINT:								\
        PRINTF("%llu", (unsigned long long) mp_decode_uint(data));	\
        break;								\
    case MP_INT:								\
        PRINTF("%lld", (long long) mp_decode_int(data));		\
        break;								\
    case MP_STR:								\
    case MP_BIN:								\
    {									\
        uint32_t len = mp_typeof(**data) == MP_STR ?			\
            mp_decode_strl(data) : mp_decode_binl(data);		\
        PRINTF("\"");							\
        const char *s;							\
        for (s = *data; s < *data + len; s++) {				\
            unsigned char c = (unsigned char ) *s;			\
            if (c < 128 && mp_char2escape[c] != NULL) {		\
                /* Escape character */				\
                PRINTF("%s", mp_char2escape[c]);		\
            } else {						\
                PRINTF("%c", c);				\
            }							\
        }								\
        PRINTF("\"");							\
        *data += len;							\
        break;								\
    }									\
    case MP_ARRAY:								\
    {									\
        uint32_t count = mp_decode_array(data);				\
        PRINTF("[");							\
        uint32_t i;							\
        for (i = 0; i < count; i++) {					\
            if (i)							\
                PRINTF(", ");					\
            SELF(data);						\
        }								\
        PRINTF("]");							\
        break;								\
    }									\
    case MP_MAP:								\
    {									\
        uint32_t count = mp_decode_map(data);				\
        PRINTF("{");							\
        uint32_t i;							\
        for (i = 0; i < count; i++) {					\
            if (i)							\
                PRINTF(", ");					\
            SELF(data);						\
            PRINTF(": ");						\
            SELF(data);						\
        }								\
        PRINTF("}");							\
        break;								\
    }									\
    case MP_BOOL:								\
        PRINTF(mp_decode_bool(data) ? "true" : "false");		\
        break;								\
    case MP_FLOAT:								\
        PRINTF("%g", mp_decode_float(data));				\
        break;								\
    case MP_DOUBLE:								\
        PRINTF("%lg", mp_decode_double(data));				\
        break;								\
    case MP_EXT:								\
        mp_next(data);							\
        PRINTF("undefined");						\
        break;								\
    default:								\
        mp_unreachable();						\
        return -1;							\
    }									\
}

static inline int
mp_fprint_internal(FILE *file, const char **data)
{
    int total_bytes = 0;
#define HANDLE(FUN, ...) do {							\
    int bytes = FUN(file, __VA_ARGS__);					\
    if (mp_unlikely(bytes < 0))						\
        return -1;							\
    total_bytes += bytes;							\
} while (0)
#define PRINT(...) HANDLE(fprintf, __VA_ARGS__)
#define SELF(...) HANDLE(mp_fprint_internal, __VA_ARGS__)
MP_PRINT(SELF, PRINT)
#undef HANDLE
#undef SELF
#undef PRINT
    return total_bytes;
}

int
mp_fprint(FILE *file, const char *data)
{
    if (!file)
        file = stdout;
    int res = mp_fprint_internal(file, &data);
    return res;
}

static inline int
mp_snprint_internal(char *buf, int size, const char **data)
{
    int total_bytes = 0;
#define HANDLE(FUN, ...) do {							\
    int bytes = FUN(buf, size, __VA_ARGS__);				\
    if (mp_unlikely(bytes < 0))						\
        return -1;							\
    total_bytes += bytes;							\
    if (bytes < size) {							\
        buf += bytes;							\
        size -= bytes;							\
    } else {								\
        /* Calculate the number of bytes needed */			\
        buf = NULL;							\
        size = 0;							\
    }									\
} while (0)
#define PRINT(...) HANDLE(snprintf, __VA_ARGS__)
#define SELF(...) HANDLE(mp_snprint_internal, __VA_ARGS__)
MP_PRINT(SELF, PRINT)
#undef HANDLE
#undef SELF
#undef PRINT
    return total_bytes;
}
#undef MP_PRINT

int
mp_snprint(char *buf, int size, const char *data)
{
    return mp_snprint_internal(buf, size, &data);
}

#endif

/*
   COPY: file name = tnt/tnt_assoc.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
   The MH_SOURCE section of tnt/tnt_assoc.h is copied to tnt/tnt_assoc.c.
*/
#ifndef TNT_ASSOC_C_INCLUDED
#define TNT_ASSOC_C_INCLUDED
#include 		      <stdint.h>
#include                      <string.h>
#include                      <stdio.h>

//#include		      <tarantool/tnt_mem.h>

#define MH_INCREMENTAL_RESIZE 1
#define MH_SOURCE             1
//#include                      "tnt_assoc.h"
#ifdef MH_SOURCE

void __attribute__((noinline))
_mh(del_resize)(struct _mh(t) *h, mh_int_t x,
        mh_arg_t arg)
{
    struct _mh(t) *s = h->shadow;
    uint32_t y = _mh(get)(s, (const mh_node_t *) &(h->p[x]),
                  arg);
    _mh(del)(s, y, arg);
    _mh(resize)(h, arg);
}

struct _mh(t) *
_mh(new)()
{
    struct _mh(t) *h = (struct _mh(t) *) MH_CALLOC(1, sizeof(*h));
    if (!h) return NULL;
    memset(h, 0, sizeof(struct _mh(t)));
    h->shadow = (struct _mh(t) *) MH_CALLOC(1, sizeof(*h));
    if (!h->shadow) goto error;
    h->prime = 0;
    h->n_buckets = __ac_prime_list[h->prime];
    h->p = (mh_node_t *) MH_CALLOC(h->n_buckets, sizeof(mh_node_t));
    if (!h->p) goto error;
    h->b = (mh_int_t *) MH_CALLOC(h->n_buckets / 16 + 1, sizeof(mh_int_t));
    if (!h->b) goto error;
    h->upper_bound = h->n_buckets * MH_DENSITY;
    return h;
error:
    _mh(delete)(h);
    return NULL;
}

void
_mh(clear)(struct _mh(t) *h)
{
    MH_FREE(h->p);
    h->prime = 0;
    h->n_buckets = __ac_prime_list[h->prime];
    h->p = (mh_node_t *) MH_CALLOC(h->n_buckets, sizeof(mh_node_t));
    h->upper_bound = h->n_buckets * MH_DENSITY;
}

void
_mh(delete)(struct _mh(t) *h)
{
    MH_FREE(h->shadow);
    MH_FREE(h->b);
    MH_FREE(h->p);
    MH_FREE(h);
}

/** Calculate hash size. */
size_t
_mh(memsize)(struct _mh(t) *h)
{
    size_t sz = 2 * sizeof(struct _mh(t));

    sz += h->n_buckets * sizeof(mh_node_t);
    sz += (h->n_buckets / 16 + 1) * sizeof(mh_int_t);
    if (h->resize_position) {
        h = h->shadow;
        sz += h->n_buckets * sizeof(mh_node_t);
        sz += (h->n_buckets / 16 + 1) * sizeof(mh_int_t);
    }
    return sz;
}

void
_mh(resize)(struct _mh(t) *h,
        mh_arg_t arg)
{
    struct _mh(t) *s = h->shadow;
#if MH_INCREMENTAL_RESIZE
    mh_int_t  batch = h->batch;
#endif
    mh_int_t i = 0;
    for (i = h->resize_position; i < h->n_buckets; i++) {
#if MH_INCREMENTAL_RESIZE
        if (batch-- == 0) {
            h->resize_position = i;
            return;
        }
#endif
        if (!mh_exist(h, i))
            continue;
        mh_int_t n = put_slot(s, mh_node(h, i), arg);
        s->p[n] = h->p[i];
        mh_setexist(s, n);
        s->n_dirty++;
    }
    MH_FREE(h->p);
    MH_FREE(h->b);
    s->size = h->size;
    memcpy(h, s, sizeof(*h));
    h->resize_cnt++;
}

int
_mh(start_resize)(struct _mh(t) *h, mh_int_t buckets, mh_int_t batch,
          mh_arg_t arg)
{
    if (h->resize_position) {
        /* resize has already been started */
        return 0;
    }
    if (buckets < h->n_buckets) {
        /* hash size is already greater than requested */
        return 0;
    }
    while (h->prime < __ac_HASH_PRIME_SIZE) {
        if (__ac_prime_list[h->prime] >= buckets)
            break;
        h->prime += 1;
    }

    h->batch = batch > 0 ? batch : h->n_buckets / (256 * 1024);
    if (h->batch < 256) {
        /*
         * Minimal batch must be greater or equal to
         * 1 / (1 - f), where f is upper bound percent
         * = MH_DENSITY
         */
        h->batch = 256;
    }

    struct _mh(t) *s = h->shadow;
    memcpy(s, h, sizeof(*h));
    s->resize_position = 0;
    s->n_buckets = __ac_prime_list[h->prime];
    s->upper_bound = s->n_buckets * MH_DENSITY;
    s->n_dirty = 0;
    s->p = (mh_node_t *) MH_CALLOC(s->n_buckets, sizeof(mh_node_t));
    if (s->p == NULL)
        return -1;
    s->b = (mh_int_t *) MH_CALLOC(s->n_buckets / 16 + 1, sizeof(mh_int_t));
    if (s->b == NULL) {
        MH_FREE(s->p);
        s->p = NULL;
        return -1;
    }
    _mh(resize)(h, arg);

    return 0;
}

void
_mh(reserve)(struct _mh(t) *h, mh_int_t size,
         mh_arg_t arg)
{
    _mh(start_resize)(h, size/MH_DENSITY, h->size, arg);
}

#ifndef mh_stat
#define mh_stat(buf, h) ({						\
        tbuf_printf(buf, "  n_buckets: %" PRIu32 CRLF		\
                "  n_dirty: %" PRIu32 CRLF			\
                "  size: %" PRIu32 CRLF			\
                "  resize_cnt: %" PRIu32 CRLF		\
                "  resize_position: %" PRIu32 CRLF,		\
                h->n_buckets,				\
                h->n_dirty,					\
                h->size,					\
                h->resize_cnt,				\
                h->resize_position);			\
            })
#endif

#ifdef MH_DEBUG
void
_mh(dump)(struct _mh(t) *h)
{
    printf("slots:\n");
    int k = 0, i = 0;
    for(i = 0; i < h->n_buckets; i++) {
        if (mh_dirty(h, i) || mh_exist(h, i)) {
            printf("   [%i] ", i);
            if (mh_exist(h, i)) {
                /* TODO(roman): fix this printf */
                printf("   -> %p", h->p[i]);
                k++;
            }
            if (mh_dirty(h, i))
                printf(" dirty");
            printf("\n");
        }
    }
    printf("end(%i)\n", k);
}
#endif

#endif

#if defined(MH_SOURCE) || defined(MH_UNDEF)
#undef MH_CALLOC
#undef MH_FREE
#undef MH_HEADER
#undef mh_node_t
#undef mh_arg_t
#undef mh_key_t
#undef mh_name
#undef mh_hash
#undef mh_hash_key
#undef mh_eq
#undef mh_eq_key
#undef mh_node
#undef mh_dirty
#undef mh_place
#undef mh_setdirty
#undef mh_setexist
#undef mh_setvalue
#undef mh_unlikely
#undef slot
#undef slot_and_dirty
#undef MH_DENSITY
#endif

#undef mh_cat
#undef mh_ecat
#undef _mh


#endif /* TNT_ASSOC_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_auth.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef TNT_AUTH_C_INCLUDED
#define TNT_AUTH_C_INCLUDED
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>

//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_ping.h>

//#include <tarantool/tnt_net.h>

//#include <sha1.h>
//#include <base64.h>

//#include "tnt_proto_internal.h"

static inline void
tnt_xor(unsigned char *to, const unsigned char *left,
    const unsigned char *right, uint32_t len)
{
    const uint8_t *end = to + len;
    while (to < end)
        *to++= *left++ ^ *right++;
}
static inline void
tnt_scramble_prepare(void *out, const void *salt, const void *pass, int plen)
{
    unsigned char hash1[TNT_SCRAMBLE_SIZE];
    unsigned char hash2[TNT_SCRAMBLE_SIZE];
    SHA1_CTX ctx;

    SHA1Init(&ctx);
    SHA1Update(&ctx, (const unsigned char *) pass, plen);
    SHA1Final(hash1, &ctx);

    SHA1Init(&ctx);
    SHA1Update(&ctx, hash1, TNT_SCRAMBLE_SIZE);
    SHA1Final(hash2, &ctx);

    SHA1Init(&ctx);
    SHA1Update(&ctx, (const unsigned char *) salt, TNT_SCRAMBLE_SIZE);
    SHA1Update(&ctx, hash2, TNT_SCRAMBLE_SIZE);
    SHA1Final((unsigned char *) out, &ctx);

    tnt_xor((unsigned char *) out, hash1, (const unsigned char *) out,
           TNT_SCRAMBLE_SIZE);
}


ssize_t
tnt_auth_raw(struct tnt_stream *s, const char *user, int ulen,
         const char *pass, int plen, const char *base64_salt)
{
    struct tnt_iheader hdr;
    struct iovec v[6]; int v_sz = 5;
    char *data = NULL, *body_start = NULL;
    int guest = !user || (ulen == 5 && !strncmp(user, "guest", 5));
    if (guest) {
        user = "guest";
        ulen = 5;
    }
    encode_header(&hdr, TNT_OP_AUTH, s->reqid++);
    v[1].iov_base = (void *)hdr.header;
    v[1].iov_len  = hdr.end - hdr.header;
    char body[64]; data = body; body_start = data;

    data = mp_encode_map(data, 2);
    data = mp_encode_uint(data, TNT_USERNAME);
    data = mp_encode_strl(data, ulen);
    v[2].iov_base = body_start;
    v[2].iov_len  = data - body_start;
    v[3].iov_base = (void *)user;
    v[3].iov_len  = ulen;
    body_start = data;
    data = mp_encode_uint(data, TNT_TUPLE);
    char salt[64], scramble[TNT_SCRAMBLE_SIZE];
    if (!guest) {
        data = mp_encode_array(data, 2);
        data = mp_encode_str(data, "chap-sha1", strlen("chap-sha1"));
        data = mp_encode_strl(data, TNT_SCRAMBLE_SIZE);
        base64_decode(base64_salt, TNT_SALT_SIZE, salt, 64);
        tnt_scramble_prepare(scramble, salt, pass, plen);
        v[5].iov_base = scramble;
        v[5].iov_len  = TNT_SCRAMBLE_SIZE;
        v_sz++;
    } else {
        data = mp_encode_array(data, 0);
    }
    v[4].iov_base = body_start;
    v[4].iov_len  = data - body_start;

    size_t package_len = 0;
    for (int i = 1; i < v_sz; ++i) {
        package_len += v[i].iov_len;
    }
    char len_prefix[9];
    char *len_end = mp_encode_luint32(len_prefix, package_len);
    v[0].iov_base = len_prefix;
    v[0].iov_len = len_end - len_prefix;
    return s->writev(s, v, v_sz);
}

ssize_t
tnt_auth(struct tnt_stream *s, const char *user, int ulen,
     const char *pass, int plen)
{
    return tnt_auth_raw(s, user, ulen, pass, plen,
                TNT_SNET_CAST(s)->greeting + TNT_VERSION_SIZE);
}

ssize_t
tnt_deauth(struct tnt_stream *s)
{
    return tnt_auth(s, NULL, 0, NULL, 0);
}

#endif /* TNT_AUTH_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_buf.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
            rename tnt_reply() xtnt_reply()
*/
#ifndef TNT_BUF_C_INCLUDED
#define TNT_BUF_C_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//#include <tarantool/tnt_mem.h>
//#include <tarantool/tnt_proto.h>
//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_buf.h>

static void tnt_buf_free(struct tnt_stream *s) {
    struct tnt_stream_buf *sb = TNT_SBUF_CAST(s);
    if (!sb->as && sb->data)
        tnt_mem_free(sb->data);
    if (sb->free)
        sb->free(s);
    tnt_mem_free(s->data);
    s->data = NULL;
}

static ssize_t
tnt_buf_read(struct tnt_stream *s, char *buf, size_t size) {
    struct tnt_stream_buf *sb = TNT_SBUF_CAST(s);
    if (sb->data == NULL)
        return 0;
    if (sb->size == sb->rdoff)
        return 0;
    size_t avail = sb->size - sb->rdoff;
    if (size > avail)
        size = avail;
    memcpy(sb->data + sb->rdoff, buf, size);
    sb->rdoff += size;
    return size;
}

static char* tnt_buf_resize(struct tnt_stream *s, size_t size) {
    struct tnt_stream_buf *sb = TNT_SBUF_CAST(s);
    size_t off = sb->size;
    size_t nsize = off + size;
    char *nd = (char *)tnt_mem_realloc(sb->data, nsize);
    if (nd == NULL) {
        tnt_mem_free(sb->data);
        return NULL;
    }
    sb->data = nd;
    sb->alloc = nsize;
    return sb->data + off;
}

static ssize_t
tnt_buf_write(struct tnt_stream *s, const char *buf, size_t size) {
    if (TNT_SBUF_CAST(s)->as == 1) return -1;
    char *p = TNT_SBUF_CAST(s)->resize(s, size);
    if (p == NULL)
        return -1;
    memcpy(p, buf, size);
    TNT_SBUF_CAST(s)->size += size;
    s->wrcnt++;
    return size;
}

static ssize_t
tnt_buf_writev(struct tnt_stream *s, struct iovec *iov, int count) {
    if (TNT_SBUF_CAST(s)->as == 1) return -1;
    size_t size = 0;
    int i;
    for (i = 0 ; i < count ; i++)
        size += iov[i].iov_len;
    char *p = TNT_SBUF_CAST(s)->resize(s, size);
    if (p == NULL)
        return -1;
    for (i = 0 ; i < count ; i++) {
        memcpy(p, iov[i].iov_base, iov[i].iov_len);
        p += iov[i].iov_len;
    }
    TNT_SBUF_CAST(s)->size += size;
    s->wrcnt++;
    return size;
}

static int
tnt_buf_reply(struct tnt_stream *s, struct tnt_reply *r) {
    struct tnt_stream_buf *sb = TNT_SBUF_CAST(s);
    if (sb->data == NULL)
        return -1;
    if (sb->size == sb->rdoff)
        return 1;
    size_t off = 0;
    int rc = xtnt_reply(r, sb->data + sb->rdoff, sb->size - sb->rdoff, &off);
    if (rc == 0)
        sb->rdoff += off;
    return rc;
}

struct tnt_stream *tnt_buf(struct tnt_stream *s) {
    int allocated = s == NULL;
    s = tnt_stream_init(s);
    if (s == NULL)
        return NULL;
    /* allocating stream data */
    s->data = tnt_mem_alloc(sizeof(struct tnt_stream_buf));
    if (s->data == NULL) {
        if (allocated)
            tnt_stream_free(s);
        return NULL;
    }
    /* initializing interfaces */
    s->read       = tnt_buf_read;
    s->read_reply = tnt_buf_reply;
    s->write      = tnt_buf_write;
    s->writev     = tnt_buf_writev;
    s->free       = tnt_buf_free;
    /* initializing internal data */
    struct tnt_stream_buf *sb = TNT_SBUF_CAST(s);
    sb->rdoff   = 0;
    sb->size    = 0;
    sb->alloc   = 0;
    sb->data    = NULL;
    sb->resize  = tnt_buf_resize;
    sb->free    = NULL;
    sb->subdata = NULL;
    sb->as      = 0;
    return s;
}

struct tnt_stream *tnt_buf_as(struct tnt_stream *s, char *buf, size_t buf_len)
{
    if (s == NULL) {
        s = tnt_buf(s);
        if (s == NULL)
            return NULL;
    }
    struct tnt_stream_buf *sb = TNT_SBUF_CAST(s);

    sb->data = buf;
    sb->size = buf_len;
    sb->alloc = buf_len;
    sb->as = 1;

    return s;
}

#endif /* TNT_BUF_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_mem.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef TNT_MEM_C_INCLUDED
#define TNT_MEM_C_INCLUDED
/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include <tarantool/tnt_mem.h>

static void *custom_realloc(void *ptr, size_t size) {
    if (!ptr) {
        if (!size)
            return NULL;
        return calloc(1, size);
    }
    if (size)
        return realloc(ptr, size);
    free(ptr);
    return NULL;
}

/*
static void *(*_tnt_realloc)(void *ptr, size_t size) =
    (void *(*)(void*, size_t))realloc;
*/

static void *(*_tnt_realloc)(void *ptr, size_t size) = custom_realloc;

void *tnt_mem_init(tnt_allocator_t alloc) {
    void *ptr = (void *)_tnt_realloc;
    if (alloc)
        _tnt_realloc = alloc;
    return ptr;
}

void *tnt_mem_alloc(size_t size) {
    return _tnt_realloc(NULL, size);
}

void *tnt_mem_realloc(void *ptr, size_t size) {
    return _tnt_realloc(ptr, size);
}

char *tnt_mem_dup(char *sz) {
    size_t len = strlen(sz);
    char *szp = (char *)tnt_mem_alloc(len + 1);
    if (szp == NULL)
        return NULL;
    memcpy(szp, sz, len + 1);
    return szp;
}

void tnt_mem_free(void *ptr) {
    _tnt_realloc(ptr, 0);
}

#endif /* TNT_MEM_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_iob.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef TNT_IOB_C_INCLUDED
#define TNT_IOB_C_INCLUDED
/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
//#include <sys/uio.h>

//#include <tarantool/tnt_mem.h>
//#include <tarantool/tnt_iob.h>

int
tnt_iob_init(struct tnt_iob *iob, size_t size,
         tnt_iob_tx_t tx,
         tnt_iob_txv_t txv, void *ptr)
{
    iob->tx = tx;
    iob->txv = txv;
    iob->ptr = ptr;
    iob->size = size;
    iob->off = 0;
    iob->top = 0;
    iob->buf = NULL;
    if (size > 0) {
        iob->buf = (char *)tnt_mem_alloc(size);
        if (iob->buf == NULL)
            return -1;
        memset(iob->buf, 0, size);
    }
    return 0;
}

void
tnt_iob_clear(struct tnt_iob *iob)
{
    iob->top = 0;
    iob->off = 0;
}

void
tnt_iob_free(struct tnt_iob *iob)
{
    if (iob->buf)
        tnt_mem_free(iob->buf);
}

#endif /* TNT_IOB_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_iter.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef TNT_ITER_C_INCLUDED
#define TNT_ITER_C_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//#include <msgpuck.h>

//#include <tarantool/tnt_mem.h>
//#include <tarantool/tnt_proto.h>
//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_buf.h>
//#include <tarantool/tnt_iter.h>

static struct tnt_iter *tnt_iter_init(struct tnt_iter *i) {
    int alloc = (i == NULL);
    if (alloc) {
        i = (tnt_iter *)(tnt_reply *)tnt_mem_alloc(sizeof(struct tnt_iter));
        if (i == NULL)
            return NULL;
    }
    memset(i, 0, sizeof(struct tnt_iter));
    i->status = TNT_ITER_OK;
    i->alloc = alloc;
    return i;
}

static int tnt_iter_array_next(struct tnt_iter *i) {
    struct tnt_iter_array *itr = TNT_IARRAY(i);
    itr->cur_index++;
    if ((uint32_t)itr->cur_index >= itr->elem_count) {
        i->status = TNT_ITER_FAIL;
        return 0;
    }
    if (itr->cur_index == 0)
        itr->elem = itr->first_elem;
    else
        itr->elem = itr->elem_end;
    itr->elem_end = itr->elem;
    mp_next(&itr->elem_end);
    return 1;
}

static void tnt_iter_array_rewind(struct tnt_iter *i) {
    struct tnt_iter_array *itr = TNT_IARRAY(i);
    itr->cur_index = -1;
    itr->elem = NULL;
    itr->elem_end = NULL;
    i->status = TNT_ITER_OK;
}

struct tnt_iter *
tnt_iter_array_object(struct tnt_iter *i, struct tnt_stream *data)
{
    return tnt_iter_array(i, TNT_SBUF_DATA(data), TNT_SBUF_SIZE(data));
}

struct tnt_iter *
tnt_iter_array(struct tnt_iter *i, const char *data, size_t size)
{
    const char *tmp_data = data;
    if (mp_check(&tmp_data, data + size) != 0)
        return NULL;
    if (!data || !size || mp_typeof(*data) != MP_ARRAY)
        return NULL;
    i = tnt_iter_init(i);
    if (i == NULL)
        return NULL;
    i->type = TNT_ITER_ARRAY;
    i->next = tnt_iter_array_next;
    i->rewind = tnt_iter_array_rewind;
    i->free = NULL;
    struct tnt_iter_array *itr = TNT_IARRAY(i);
    itr->data = data;
    itr->first_elem = data;
    itr->elem_count = mp_decode_array(&itr->first_elem);
    itr->cur_index = -1;
    return i;
}


static int tnt_iter_map_next(struct tnt_iter *i) {
    struct tnt_iter_map *itr = TNT_IMAP(i);
    itr->cur_index++;
    if ((uint32_t)itr->cur_index >= itr->pair_count) {
        i->status = TNT_ITER_FAIL;
        return 0;
    }
    if (itr->cur_index == 0)
        itr->key = itr->first_key;
    else
        itr->key = itr->value_end;
    itr->key_end = itr->key;
    mp_next(&itr->key_end);
    itr->value = itr->key_end;
    itr->value_end = itr->value;
    mp_next(&itr->value_end);
    return 1;

}

static void tnt_iter_map_rewind(struct tnt_iter *i) {
    struct tnt_iter_map *itr = TNT_IMAP(i);
    itr->cur_index = -1;
    itr->key = NULL;
    itr->key_end = NULL;
    itr->value = NULL;
    itr->value_end = NULL;
    i->status = TNT_ITER_OK;
}

struct tnt_iter *
tnt_iter_map_object(struct tnt_iter *i, struct tnt_stream *data)
{
    return tnt_iter_map(i, TNT_SBUF_DATA(data), TNT_SBUF_SIZE(data));
}

struct tnt_iter *
tnt_iter_map(struct tnt_iter *i, const char *data, size_t size)
{
    const char *tmp_data = data;
    if (mp_check(&tmp_data, data + size) != 0)
        return NULL;
    if (!data || !size || mp_typeof(*data) != MP_MAP)
        return NULL;
    i = tnt_iter_init(i);
    if (i == NULL)
        return NULL;
    i->type = TNT_ITER_MAP;
    i->next = tnt_iter_map_next;
    i->rewind = tnt_iter_map_rewind;
    i->free = NULL;
    struct tnt_iter_map *itr = TNT_IMAP(i);
    itr->data = data;
    itr->first_key = data;
    itr->pair_count = mp_decode_map(&itr->first_key);
    itr->cur_index = -1;
    return i;
}

static int tnt_iter_reply_next(struct tnt_iter *i) {
    struct tnt_iter_reply *ir = TNT_IREPLY(i);
    tnt_reply_free(&ir->r);
    tnt_reply_init(&ir->r);
    int rc = ir->s->read_reply(ir->s, &ir->r);
    if (rc == -1) {
        i->status = TNT_ITER_FAIL;
        return 0;
    }
    return (rc == 1 /* finish */ ) ? 0 : 1;
}

static void tnt_iter_reply_free(struct tnt_iter *i) {
    struct tnt_iter_reply *ir = TNT_IREPLY(i);
    tnt_reply_free(&ir->r);
}

struct tnt_iter *tnt_iter_reply(struct tnt_iter *i, struct tnt_stream *s) {
    i = tnt_iter_init(i);
    if (i == NULL)
        return NULL;
    i->type = TNT_ITER_REPLY;
    i->next = tnt_iter_reply_next;
    i->rewind = NULL;
    i->free = tnt_iter_reply_free;
    struct tnt_iter_reply *ir = TNT_IREPLY(i);
    ir->s = s;
    tnt_reply_init(&ir->r);
    return i;
}

/*static int tnt_iter_request_next(struct tnt_iter *i) {
    struct tnt_iter_request *ir = TNT_IREQUEST(i);
    tnt_request_free(&ir->r);
    tnt_request_init(&ir->r);
    int rc = ir->s->read_request(ir->s, &ir->r);
    if (rc == -1) {
        i->status = TNT_ITER_FAIL;
        return 0;
    }
    return (rc == 1) ? 0 : 1;
}

static void tnt_iter_request_free(struct tnt_iter *i) {
    struct tnt_iter_request *ir = TNT_IREQUEST(i);
    tnt_request_free(&ir->r);
}*/

/*struct tnt_iter *tnt_iter_request(struct tnt_iter *i, struct tnt_stream *s) {
    i = tnt_iter_init(i);
    if (i == NULL)
        return NULL;
    i->type = TNT_ITER_REQUEST;
    i->next = tnt_iter_request_next;
    i->rewind = NULL;
    i->free = tnt_iter_request_free;
    struct tnt_iter_request *ir = TNT_IREQUEST(i);
    ir->s = s;
    tnt_request_init(&ir->r);
    return i;
}

static int tnt_iter_storage_next(struct tnt_iter *i) {
    struct tnt_iter_storage *is = TNT_ISTORAGE(i);
    tnt_tuple_free(&is->t);
    tnt_tuple_init(&is->t);

    int rc = is->s->read_tuple(is->s, &is->t);
    if (rc == -1) {
        i->status = TNT_ITER_FAIL;
        return 0;
    }
    return (rc == 1) ? 0 : 1;
}

static void tnt_iter_storage_free(struct tnt_iter *i) {
    struct tnt_iter_storage *is = TNT_ISTORAGE(i);
    tnt_tuple_free(&is->t);
}*/

/*
 * tnt_iter_storage()
 *
 * initialize tuple storage iterator;
 * create and initialize storage iterator;
 *
 * i - tuple storage iterator pointer, maybe NULL
 * s - stream pointer
 *
 * if stream iterator pointer is NULL, then new stream
 * iterator will be created.
 *
 * return stream iterator pointer, or NULL on error.
*/
/*struct tnt_iter *tnt_iter_storage(struct tnt_iter *i, struct tnt_stream *s) {
    i = tnt_iter_init(i);
    if (i == NULL)
        return NULL;
    i->type = TNT_ITER_STORAGE;
    i->next = tnt_iter_storage_next;
    i->rewind = NULL;
    i->free = tnt_iter_storage_free;
    struct tnt_iter_storage *is = TNT_ISTORAGE(i);
    is->s = s;
    tnt_tuple_init(&is->t);
    return i;
}*/

void tnt_iter_free(struct tnt_iter *i) {
    if (i->free)
        i->free(i);
    if (i->alloc)
        tnt_mem_free(i);
}

int tnt_next(struct tnt_iter *i) {
    return i->next(i);
}

void tnt_rewind(struct tnt_iter *i) {
    i->status = TNT_ITER_OK;
    if (i->rewind)
        i->rewind(i);
}

#endif /* TNT_ITER_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_reply.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
            In tnt_reply_from(), put early definition to avoid
            "error: crosses initialization".
            Rename tnt_reply() xtnt_reply().
*/
#ifndef TNT_REPLY_C_INCLUDED
/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <sys/types.h>

//#include <msgpuck.h>

//#include <tarantool/tnt_mem.h>
//#include <tarantool/tnt_proto.h>
//#include <tarantool/tnt_reply.h>

struct tnt_reply *tnt_reply_init(struct tnt_reply *r) {
    int alloc = (r == NULL);
    if (alloc) {
        r = (tnt_reply *)tnt_mem_alloc(sizeof(struct tnt_reply));
        if (!r) return NULL;
    }
    memset(r, 0, sizeof(struct tnt_reply));
    r->alloc = alloc;
    return r;
}

void tnt_reply_free(struct tnt_reply *r) {
    if (r->buf) {
        tnt_mem_free((void *)r->buf);
        r->buf = NULL;
    }
    if (r->alloc) tnt_mem_free(r);
}

int tnt_reply_from(struct tnt_reply *r, tnt_reply_t rcv, void *ptr) {
    size_t size;
    size_t hdr_length;
    /* cleanup, before processing response */
    int alloc = r->alloc;
    memset(r, 0 , sizeof(struct tnt_reply));
    r->alloc = alloc;
    /* reading iproto header */
    char length[TNT_REPLY_IPROTO_HDR_SIZE]; const char *data = (const char *)length;
    if (rcv(ptr, length, sizeof(length)) == -1)
        goto rollback;
    if (mp_typeof(*length) != MP_UINT)
        goto rollback;
    size = mp_decode_uint(&data);
    r->buf = (const char *)tnt_mem_alloc(size);
    r->buf_size = size;
    if (r->buf == NULL)
        goto rollback;
    if(rcv(ptr, (char *)r->buf, size) == -1)
        goto rollback;
    if (tnt_reply_hdr0(r, r->buf, r->buf_size, &hdr_length) != 0)
        goto rollback;
    if (size == (size_t)hdr_length)
        return 0; /* no body */
    if (tnt_reply_body0(r, r->buf + hdr_length, r->buf_size - hdr_length, NULL) != 0)
        goto rollback;

    return 0;
rollback:
    if (r->buf) tnt_mem_free((void *)r->buf);
    alloc = r->alloc;
    memset(r, 0, sizeof(struct tnt_reply));
    r->alloc = alloc;
    return -1;
}

static ssize_t tnt_reply_cb(void *ptr[2], char *buf, ssize_t size) {
    char *src = (char *)ptr[0];
    ssize_t *off = (ssize_t *)ptr[1];
    memcpy(buf, src + *off, size);
    *off += size;
    return size;
}

static int
tnt_reply_len(const char *buf, size_t size, size_t *len)
{
    if (size < TNT_REPLY_IPROTO_HDR_SIZE) {
        *len = TNT_REPLY_IPROTO_HDR_SIZE - size;
        return 1;
    }
    const char *p = buf;
    if (mp_typeof(*p) != MP_UINT)
        return -1;
    size_t length = mp_decode_uint(&p);
    if (size < length + TNT_REPLY_IPROTO_HDR_SIZE) {
        *len = (length + TNT_REPLY_IPROTO_HDR_SIZE) - size;
        return 1;
    }
    *len = length + TNT_REPLY_IPROTO_HDR_SIZE;
    return 0;
}

int
tnt_reply_hdr0(struct tnt_reply *r, const char *buf, size_t size, size_t *off) {
    const char *test = buf;
    const char *p = buf;
    if (mp_check(&test, p + size))
        return -1;
    if (mp_typeof(*p) != MP_MAP)
        return -1;

    uint32_t n = mp_decode_map(&p);
    uint64_t sync = 0, code = 0, schema_id = 0, bitmap = 0;
    while (n-- > 0) {
        if (mp_typeof(*p) != MP_UINT)
            return -1;
        uint32_t key = mp_decode_uint(&p);
        if (mp_typeof(*p) != MP_UINT)
            return -1;
        switch (key) {
        case TNT_SYNC:
            sync = mp_decode_uint(&p);
            break;
        case TNT_CODE:
            code = mp_decode_uint(&p);
            break;
        case TNT_SCHEMA_ID:
            schema_id = mp_decode_uint(&p);
            break;
        default:
            return -1;
        }
        bitmap |= (1ULL << key);
    }
    if (r) {
        r->sync = sync;
        r->code = code & ((1 << 15) - 1);
        r->schema_id = schema_id;
        r->bitmap = bitmap;
    }
    if (off)
        *off = p - buf;
    return 0;
}

int
tnt_reply_body0(struct tnt_reply *r, const char *buf, size_t size, size_t *off) {
    const char *test = buf;
    const char *p = buf;
    if (mp_check(&test, p + size))
        return -1;
    if (mp_typeof(*p) != MP_MAP)
        return -1;
    const char *error = NULL, *error_end = NULL,
           *data = NULL, *data_end = NULL,
           *metadata = NULL, *metadata_end = NULL,
           *sqlinfo = NULL, *sqlinfo_end = NULL;
    uint64_t bitmap = 0;
    uint32_t n = mp_decode_map(&p);
    while (n-- > 0) {
        uint32_t key = mp_decode_uint(&p);
        switch (key) {
        case TNT_ERROR: {
            if (mp_typeof(*p) != MP_STR)
                return -1;
            uint32_t elen = 0;
            error = mp_decode_str(&p, &elen);
            error_end = error + elen;
            break;
        }
        case TNT_DATA: {
            if (mp_typeof(*p) != MP_ARRAY)
                return -1;
            data = p;
            mp_next(&p);
            data_end = p;
            break;
        }
        case TNT_METADATA: {
            if (mp_typeof(*p) != MP_ARRAY)
                return -1;
            metadata = p;
            mp_next(&p);
            metadata_end = p;
            break;
        }
        case TNT_SQL_INFO: {
            if (mp_typeof(*p) != MP_MAP)
                return -1;
            sqlinfo = p;
            mp_next(&p);
            sqlinfo_end = p;
            break;
        }
        default: {
            mp_next(&p);
            break;
        }
        }
        bitmap |= (1ULL << key);
    }
    if (r) {
        r->error = error;
        r->error_end = error_end;
        r->data = data;
        r->data_end = data_end;
        r->metadata = metadata;
        r->metadata_end = metadata_end;
        r->sqlinfo = sqlinfo;
        r->sqlinfo_end = sqlinfo_end;
        r->bitmap |= bitmap;
    }
    if (off)
        *off = p - buf;
    return 0;
}

int
xtnt_reply(struct tnt_reply *r, char *buf, size_t size, size_t *off) {
    /* supplied buffer must contain full reply,
     * if it doesn't then returning count of bytes
     * needed to process */
    size_t length;
    switch (tnt_reply_len(buf, size, &length)) {
    case 0:
        break;
    case 1:
        if (off)
            *off = length;
        return 1;
    default:
        return -1;
    }
    if (r == NULL) {
        if (off)
            *off = length;
        return 0;
    }
    size_t offv = 0;
    void *ptr[2] = { buf, &offv };
    int rc = tnt_reply_from(r, (tnt_reply_t)tnt_reply_cb, ptr);
    if (off)
        *off = offv;
    return rc;
}

int
tnt_reply0(struct tnt_reply *r, const char *buf, size_t size, size_t *off) {
    /* supplied buffer must contain full reply,
     * if it doesn't then returning count of bytes
     * needed to process */
    size_t length;
    switch (tnt_reply_len(buf, size, &length)) {
    case 0:
        break;
    case 1:
        if (off)
            *off = length;
        return 1;
    default:
        return -1;
    }
    if (r == NULL) {
        if (off)
            *off = length;
        return 0;
    }
    const char *data = buf + TNT_REPLY_IPROTO_HDR_SIZE;
    size_t data_length = length - TNT_REPLY_IPROTO_HDR_SIZE;
    size_t hdr_length;
    if (tnt_reply_hdr0(r, data, data_length, &hdr_length) != 0)
        return -1;
    if (data_length != hdr_length) {
        if (tnt_reply_body0(r, data + hdr_length, data_length - hdr_length, NULL) != 0)
            return -1;
    }
    if (off)
        *off = length;
    return 0;
}

#define TNT_REPLY_C_INCLUDED
#endif

/*
   COPY: file name = tnt/tnt_stream.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef TNT_STREAM_C_INCLUDED
#define TNT_STREAM_C_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/types.h>

//#include <tarantool/tnt_mem.h>
//#include <tarantool/tnt_proto.h>
//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>

uint32_t
tnt_stream_reqid(struct tnt_stream *s, uint32_t reqid)
{
    uint32_t old = s->reqid;
    s->reqid = reqid;
    return old;
}

struct tnt_stream*
tnt_stream_init(struct tnt_stream *s)
{
    int alloc = (s == NULL);
    if (alloc) {
        s = (tnt_stream *)tnt_mem_alloc(sizeof(struct tnt_stream));
        if (s == NULL)
            return NULL;
    }
    memset(s, 0, sizeof(struct tnt_stream));
    s->alloc = alloc;
    return s;
}

void tnt_stream_free(struct tnt_stream *s) {
    if (s == NULL)
        return;
    if (s->free)
        s->free(s);
    if (s->alloc)
        tnt_mem_free(s);
}

#endif /* TNT_STREAM_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_request.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
   Replaced:
           req = tnt_mem_alloc(sizeof(struct tnt_request));
   With:
           req = (tnt_request*) tnt_mem_alloc(sizeof(struct tnt_request));
   Replaced:
           mp_encode_luint32(v[0].iov_base, plen);
   With:
           mp_encode_luint32((char*)v[0].iov_base, plen);
*/
#ifndef TNT_REQUEST_C_INCLUDED
#define TNT_REQUEST_C_INCLUDED

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

//#include <msgpuck.h>

//#include <tarantool/tnt_mem.h>
//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_net.h>
//#include <tarantool/tnt_object.h>
//#include <tarantool/tnt_buf.h>
//#include <tarantool/tnt_proto.h>
//#include <tarantool/tnt_schema.h>

//#include <tarantool/tnt_request.h>

//#include "tnt_proto_internal.h"

struct tnt_request *tnt_request_init(struct tnt_request *req) {
    int alloc = (req == NULL);
    if (req == NULL) {
        req = (tnt_request*) tnt_mem_alloc(sizeof(struct tnt_request));
        if (!req) return NULL;
    }
    memset(req, 0, sizeof(struct tnt_request));
    req->limit = UINT32_MAX;
    req->alloc = alloc;
    return req;
};

void tnt_request_free(struct tnt_request *req) {
    if (req->key_object)
        tnt_stream_free(req->key_object);
    req->key_object = NULL;
    if (req->tuple_object)
        tnt_stream_free(req->tuple_object);
    req->tuple_object = NULL;
    if (req->alloc) tnt_mem_free(req);
}

#define TNT_REQUEST_CUSTOM(NM, CNM)				\
struct tnt_request *tnt_request_##NM(struct tnt_request *req) {	\
    req = tnt_request_init(req);				\
    if (req) {						\
        req->hdr.type = TNT_OP_##CNM;			\
    }							\
    return req;						\
}

TNT_REQUEST_CUSTOM(select, SELECT);
TNT_REQUEST_CUSTOM(insert, INSERT);
TNT_REQUEST_CUSTOM(replace, REPLACE);
TNT_REQUEST_CUSTOM(update, UPDATE);
TNT_REQUEST_CUSTOM(delete, DELETE);
TNT_REQUEST_CUSTOM(call, CALL);
TNT_REQUEST_CUSTOM(call_16, CALL_16);
TNT_REQUEST_CUSTOM(auth, AUTH);
TNT_REQUEST_CUSTOM(eval, EVAL);
TNT_REQUEST_CUSTOM(upsert, UPSERT);
TNT_REQUEST_CUSTOM(ping, PING);

#undef TNT_REQUEST_CUSTOM

int tnt_request_set_space(struct tnt_request *req, uint32_t space)
{
    req->space_id = space;
    return 0;
}

int tnt_request_set_index(struct tnt_request *req, uint32_t index)
{
    req->index_id = index;
    return 0;
}

int tnt_request_set_offset(struct tnt_request *req, uint32_t offset)
{
    req->offset = offset;
    return 0;
}

int tnt_request_set_limit(struct tnt_request *req, uint32_t limit)
{
    req->limit = limit;
    return 0;
}

int
tnt_request_set_iterator(struct tnt_request *req, enum tnt_iterator_t iterator)
{
    req->iterator = iterator;
    return 0;
}

int tnt_request_set_index_base(struct tnt_request *req, uint32_t index_base)
{
    req->index_base = index_base;
    return 0;
}

int tnt_request_set_key(struct tnt_request *req, struct tnt_stream *s)
{
    req->key     = TNT_SBUF_DATA(s);
    req->key_end = req->key + TNT_SBUF_SIZE(s);
    return 0;
}

int tnt_request_set_key_format(struct tnt_request *req, const char *fmt, ...)
{
    if (req->key_object)
        tnt_object_reset(req->key_object);
    else
        req->key_object = tnt_object(NULL);
    if (!req->key_object)
        return -1;
    va_list args;
    va_start(args, fmt);
    ssize_t res = tnt_object_vformat(req->key_object, fmt, args);
    va_end(args);
    if (res == -1)
        return -1;
    return tnt_request_set_key(req, req->key_object);
}

int
tnt_request_set_func(struct tnt_request *req, const char *func,
             uint32_t flen)
{
    if (!is_call(req->hdr.type))
        return -1;
    if (!func)
        return -1;
    req->key = func; req->key_end = req->key + flen;
    return 0;
}

int
tnt_request_set_funcz(struct tnt_request *req, const char *func)
{
    if (!is_call(req->hdr.type))
        return -1;
    if (!func)
        return -1;
    req->key = func; req->key_end = req->key + strlen(req->key);
    return 0;
}

int
tnt_request_set_expr(struct tnt_request *req, const char *expr,
             uint32_t elen)
{
    if (req->hdr.type != TNT_OP_EVAL)
        return -1;
    if (!expr)
        return -1;
    req->key = expr; req->key_end = req->key + elen;
    return 0;
}

int
tnt_request_set_exprz(struct tnt_request *req, const char *expr)
{
    if (req->hdr.type != TNT_OP_EVAL)
        return -1;
    if (!expr)
        return -1;
    req->key = expr; req->key_end = req->key + strlen(req->key);
    return 0;
}

int
tnt_request_set_ops(struct tnt_request *req, struct tnt_stream *s)
{
    if (req->hdr.type == TNT_OP_UPDATE) {
        req->tuple     = TNT_SBUF_DATA(s);
        req->tuple_end = req->tuple + TNT_SBUF_SIZE(s);
        return 0;
    } else if (req->hdr.type == TNT_OP_UPSERT) {
        req->key     = TNT_SBUF_DATA(s);
        req->key_end = req->key + TNT_SBUF_SIZE(s);
        return 0;
    }
    return -1;
}

int tnt_request_set_tuple(struct tnt_request *req, struct tnt_stream *s)
{
    req->tuple     = TNT_SBUF_DATA(s);
    req->tuple_end = req->tuple + TNT_SBUF_SIZE(s);
    return 0;
}

int tnt_request_set_tuple_format(struct tnt_request *req, const char *fmt, ...)
{
    if (req->tuple_object)
        tnt_object_reset(req->tuple_object);
    else
        req->tuple_object = tnt_object(NULL);
    if (!req->tuple_object)
        return -1;
    va_list args;
    va_start(args, fmt);
    ssize_t res = tnt_object_vformat(req->tuple_object, fmt, args);
    va_end(args);
    if (res == -1)
        return -1;
    return tnt_request_set_tuple(req, req->tuple_object);
}

int
tnt_request_writeout(struct tnt_stream *s, struct tnt_request *req,
             uint64_t *sync) {
    enum tnt_request_t tp = req->hdr.type;
    if (sync != NULL && *sync == INT64_MAX &&
        (s->reqid & INT64_MAX) == INT64_MAX) {
        s->reqid = 0;
    }
    req->hdr.sync = s->reqid++;
    /* header */
    /* int (9) + 1 + sync + 1 + op */
    struct iovec v[10]; int v_sz = 0;
    char header[128];
    char *pos = header + 9;
    char *begin = pos;
    v[v_sz].iov_base = begin;
    v[v_sz++].iov_len  = 0;
    pos = mp_encode_map(pos, 2);              /* 1 */
    pos = mp_encode_uint(pos, TNT_CODE);      /* 1 */
    pos = mp_encode_uint(pos, req->hdr.type); /* 1 */
    pos = mp_encode_uint(pos, TNT_SYNC);      /* 1 */
    pos = mp_encode_uint(pos, req->hdr.sync); /* 9 */
    char *map = pos++;                        /* 1 */
    size_t nd = 0;
    if (tp < TNT_OP_CALL_16 || tp == TNT_OP_UPSERT) {
        pos = mp_encode_uint(pos, TNT_SPACE);     /* 1 */
        pos = mp_encode_uint(pos, req->space_id); /* 5 */
        nd += 1;
    }
    if (req->index_id && (tp == TNT_OP_SELECT ||
                  tp == TNT_OP_UPDATE ||
                  tp == TNT_OP_DELETE)) {
        pos = mp_encode_uint(pos, TNT_INDEX);     /* 1 */
        pos = mp_encode_uint(pos, req->index_id); /* 5 */
        nd += 1;
    }
    if (tp == TNT_OP_SELECT) {
        pos = mp_encode_uint(pos, TNT_LIMIT);  /* 1 */
        pos = mp_encode_uint(pos, req->limit); /* 5 */
        nd += 1;
    }
    if (req->offset && tp == TNT_OP_SELECT) {
        pos = mp_encode_uint(pos, TNT_OFFSET);  /* 1 */
        pos = mp_encode_uint(pos, req->offset); /* 5 */
        nd += 1;
    }
    if (req->iterator && tp == TNT_OP_SELECT) {
        pos = mp_encode_uint(pos, TNT_ITERATOR);  /* 1 */
        pos = mp_encode_uint(pos, req->iterator); /* 1 */
        nd += 1;
    }
    if (req->key) {
        switch (tp) {
        case TNT_OP_EVAL:
            pos = mp_encode_uint(pos, TNT_EXPRESSION);          /* 1 */
            pos = mp_encode_strl(pos, req->key_end - req->key); /* 5 */
            break;
        case TNT_OP_CALL_16:
        case TNT_OP_CALL:
            pos = mp_encode_uint(pos, TNT_FUNCTION);            /* 1 */
            pos = mp_encode_strl(pos, req->key_end - req->key); /* 5 */
            break;
        case TNT_OP_SELECT:
        case TNT_OP_UPDATE:
        case TNT_OP_DELETE:
            pos = mp_encode_uint(pos, TNT_KEY); /* 1 */
            break;
        case TNT_OP_UPSERT:
            pos = mp_encode_uint(pos, TNT_OPS); /* 1 */
            break;
        default:
            return -1;
        }
        v[v_sz].iov_base  = begin;
        v[v_sz++].iov_len = pos - begin;
        begin = pos;
        v[v_sz].iov_base  = (void *)req->key;
        v[v_sz++].iov_len = req->key_end - req->key;
        nd += 1;
    }
    if (req->tuple) {
        pos = mp_encode_uint(pos, TNT_TUPLE); /* 1 */
        v[v_sz].iov_base  = begin;
        v[v_sz++].iov_len = pos - begin;
        begin = pos;
        v[v_sz].iov_base  = (void *)req->tuple;
        v[v_sz++].iov_len = req->tuple_end - req->tuple;
        nd += 1;
    }
    if (req->index_base && (tp == TNT_OP_UPDATE || tp == TNT_OP_UPSERT)) {
        pos = mp_encode_uint(pos, TNT_INDEX_BASE);  /* 1 */
        pos = mp_encode_uint(pos, req->index_base); /* 1 */
        nd += 1;
    }
    assert(mp_sizeof_map(nd) == 1);
    if (pos != begin) {
        v[v_sz].iov_base  = begin;
        v[v_sz++].iov_len = pos - begin;
    }
    mp_encode_map(map, nd);

    size_t plen = 0;
    for (int i = 1; i < v_sz; ++i) plen += v[i].iov_len;
    size_t hlen = mp_sizeof_luint32(plen);
#ifdef OCELOT_OS_FREEBSD
    v[0].iov_base = (char*)(v[0].iov_base) - hlen;
#else
    v[0].iov_base -= hlen;
#endif
    v[0].iov_len  += hlen;
    mp_encode_luint32((char*)v[0].iov_base, plen);
    ssize_t rv = s->writev(s, v, v_sz);
    if (rv == -1)
        return -1;
    if (sync != NULL)
        *sync = req->hdr.sync;
    return 0;
}

int64_t
tnt_request_compile(struct tnt_stream *s, struct tnt_request *req) {
    uint64_t sync = INT64_MAX;
    if (tnt_request_writeout(s, req, &sync) == -1)
        return -1;
    return sync;
}

#endif /* TNT_REQUEST_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_call.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef TNT_CALL_C_INCLUDED
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

//#include <msgpuck.h>

//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_buf.h>
//#include <tarantool/tnt_object.h>
//#include <tarantool/tnt_call.h>

//#include "tnt_proto_internal.h"

static ssize_t
tnt_rpc_base(struct tnt_stream *s, const char *proc, size_t proc_len,
         struct tnt_stream *args, enum tnt_request_t op)
{
    if (!proc || proc_len == 0)
        return -1;
    if (tnt_object_verify(args, MP_ARRAY))
        return -1;
    uint32_t fld = (is_call(op) ? TNT_FUNCTION : TNT_EXPRESSION);
    struct tnt_iheader hdr;
    struct iovec v[6]; int v_sz = 6;
    char *data = NULL, *body_start = NULL;
    encode_header(&hdr, op, s->reqid++);
    v[1].iov_base = (void *)hdr.header;
    v[1].iov_len  = hdr.end - hdr.header;
    char body[64]; body_start = body; data = body;

    data = mp_encode_map(data, 2);
    data = mp_encode_uint(data, fld);
    data = mp_encode_strl(data, proc_len);
    v[2].iov_base = body_start;
    v[2].iov_len  = data - body_start;
    v[3].iov_base = (void *)proc;
    v[3].iov_len  = proc_len;
    body_start = data;
    data = mp_encode_uint(data, TNT_TUPLE);
    v[4].iov_base = body_start;
    v[4].iov_len  = data - body_start;
    v[5].iov_base = TNT_SBUF_DATA(args);
    v[5].iov_len  = TNT_SBUF_SIZE(args);

    size_t package_len = 0;
    for (int i = 1; i < v_sz; ++i)
        package_len += v[i].iov_len;
    char len_prefix[9];
    char *len_end = mp_encode_luint32(len_prefix, package_len);
    v[0].iov_base = len_prefix;
    v[0].iov_len = len_end - len_prefix;
    return s->writev(s, v, v_sz);
}

ssize_t
tnt_call(struct tnt_stream *s, const char *proc, size_t proc_len,
     struct tnt_stream *args)
{
    return tnt_rpc_base(s, proc, proc_len, args, TNT_OP_CALL);
}

ssize_t
tnt_call_16(struct tnt_stream *s, const char *proc, size_t proc_len,
        struct tnt_stream *args)
{
    return tnt_rpc_base(s, proc, proc_len, args, TNT_OP_CALL_16);
}

ssize_t
tnt_eval(struct tnt_stream *s, const char *proc, size_t proc_len,
     struct tnt_stream *args)
{
    return tnt_rpc_base(s, proc, proc_len, args, TNT_OP_EVAL);
}

#define TNT_CALL_C_INCLUDED
#endif /* TNT_CALL_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_delete.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef TNT_DELETE_C_INCLUDED
#define TNT_DELETE_C_INCLUDED


#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

//#include <msgpuck.h>
//#include <sys/types.h>

//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_buf.h>
//#include <tarantool/tnt_object.h>
//#include <tarantool/tnt_delete.h>

//#include "tnt_proto_internal.h"

ssize_t
tnt_delete(struct tnt_stream *s, uint32_t space, uint32_t index,
       struct tnt_stream *key)
{
    if (tnt_object_verify(key, MP_ARRAY))
        return -1;
    struct tnt_iheader hdr;
    struct iovec v[4]; int v_sz = 4;
    char *data = NULL;
    encode_header(&hdr, TNT_OP_DELETE, s->reqid++);
    v[1].iov_base = (void *)hdr.header;
    v[1].iov_len  = hdr.end - hdr.header;
    char body[64]; data = body;

    data = mp_encode_map(data, 3);
    data = mp_encode_uint(data, TNT_SPACE);
    data = mp_encode_uint(data, space);
    data = mp_encode_uint(data, TNT_INDEX);
    data = mp_encode_uint(data, index);
    data = mp_encode_uint(data, TNT_KEY);
    v[2].iov_base = body;
    v[2].iov_len  = data - body;
    v[3].iov_base = TNT_SBUF_DATA(key);
    v[3].iov_len  = TNT_SBUF_SIZE(key);

    size_t package_len = 0;
    for (int i = 1; i < v_sz; ++i)
        package_len += v[i].iov_len;
    char len_prefix[9];
    char *len_end = mp_encode_luint32(len_prefix, package_len);
    v[0].iov_base = len_prefix;
    v[0].iov_len = len_end - len_prefix;
    return s->writev(s, v, v_sz);
}

#endif /* TNT_DELETE_C_INCLUDED */

/*
  COPY: file name = tnt/tnt_execute.c.
  Copying date = 2020-09-23.
  Changes: added ifndef...#endif. commented out #includes.
*/
#ifndef TNT_EXECUTE_C_INCLUDED
#define TNT_EXECUTE_C_INCLUDED
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

//#include <msgpuck.h>

//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_buf.h>
//#include <tarantool/tnt_object.h>
//#include <tarantool/tnt_execute.h>

//#include "tnt_proto_internal.h"

ssize_t
tnt_execute(struct tnt_stream *s, const char *expr, size_t elen,
        struct tnt_stream *params)
{
    if (!expr || elen == 0)
        return -1;
    if (tnt_object_verify(params, MP_ARRAY))
        return -1;
    struct tnt_iheader hdr;
    struct iovec v[6];
    int v_sz = 6;
    char *data = NULL, *body_start = NULL;
    encode_header(&hdr, TNT_OP_EXECUTE, s->reqid++);
    v[1].iov_base = (void *) hdr.header;
    v[1].iov_len = hdr.end - hdr.header;
    char body[64];
    body_start = body;
    data = body;

    data = mp_encode_map(data, 2);
    data = mp_encode_uint(data, TNT_SQL_TEXT);
    data = mp_encode_strl(data, elen);
    v[2].iov_base = body_start;
    v[2].iov_len = data - body_start;
    v[3].iov_base = (void *) expr;
    v[3].iov_len = elen;
    body_start = data;
    data = mp_encode_uint(data, TNT_SQL_BIND);
    v[4].iov_base = body_start;
    v[4].iov_len = data - body_start;
    v[5].iov_base = TNT_SBUF_DATA(params);
    v[5].iov_len = TNT_SBUF_SIZE(params);

    size_t package_len = 0;
    for (int i = 1; i < v_sz; ++i)
        package_len += v[i].iov_len;
    char len_prefix[9];
    char *len_end = mp_encode_luint32(len_prefix, package_len);
    v[0].iov_base = len_prefix;
    v[0].iov_len = len_end - len_prefix;
    return s->writev(s, v, v_sz);
}
#endif /* TNT_EXECUTE_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_insert.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef TNT_INSERT_C_INCLUDED
#define TNT_INSERT_C_INCLUDED


#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

//#include <msgpuck.h>

//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_buf.h>
//#include <tarantool/tnt_object.h>
//#include <tarantool/tnt_insert.h>

//#include "tnt_proto_internal.h"

static ssize_t
tnt_store_base(struct tnt_stream *s, uint32_t space, struct tnt_stream *tuple,
           enum tnt_request_t op)
{
    if (tnt_object_verify(tuple, MP_ARRAY))
        return -1;
    struct tnt_iheader hdr;
    struct iovec v[4]; int v_sz = 4;
    char *data = NULL;
    encode_header(&hdr, op, s->reqid++);
    v[1].iov_base = (void *)hdr.header;
    v[1].iov_len  = hdr.end - hdr.header;
    char body[64]; data = body;

    data = mp_encode_map(data, 2);
    data = mp_encode_uint(data, TNT_SPACE);
    data = mp_encode_uint(data, space);
    data = mp_encode_uint(data, TNT_TUPLE);
    v[2].iov_base = body;
    v[2].iov_len  = data - body;
    v[3].iov_base = TNT_SBUF_DATA(tuple);
    v[3].iov_len  = TNT_SBUF_SIZE(tuple);

    size_t package_len = 0;
    for (int i = 1; i < v_sz; ++i)
        package_len += v[i].iov_len;
    char len_prefix[9];
    char *len_end = mp_encode_luint32(len_prefix, package_len);
    v[0].iov_base = len_prefix;
    v[0].iov_len = len_end - len_prefix;
    return s->writev(s, v, v_sz);
}

ssize_t
tnt_insert(struct tnt_stream *s, uint32_t space, struct tnt_stream *tuple)
{
    return tnt_store_base(s, space, tuple, TNT_OP_INSERT);
}

ssize_t
tnt_replace(struct tnt_stream *s, uint32_t space, struct tnt_stream *tuple)
{
    return tnt_store_base(s, space, tuple, TNT_OP_REPLACE);
}
#endif /* TNT_INSERT_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_opt.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef TNT_OPT_C_INCLUDED
#define TNT_OPT_C_INCLUDED
/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//#include <sys/uio.h>

#include <sys/types.h>
#include <sys/time.h>

//#include <uri.h>

//#include <tarantool/tnt_mem.h>
//#include <tarantool/tnt_net.h>

int
tnt_opt_init(struct tnt_opt *opt)
{
    memset(opt, 0, sizeof(struct tnt_opt));
    opt->recv_buf = 16384;
    opt->send_buf = 16384;
    opt->tmout_connect.tv_sec = 16;
    opt->tmout_connect.tv_usec = 0;
    opt->uri = (uri *)tnt_mem_alloc(sizeof(struct uri));
    if (!opt->uri) return -1;
    return 0;
}

void
tnt_opt_free(struct tnt_opt *opt)
{
    if (opt->uristr)
        tnt_mem_free((void *)opt->uristr);
    tnt_mem_free((void *)opt->uri);
}

int
tnt_opt_set(struct tnt_opt *opt, enum tnt_opt_type name, va_list args)
{
    struct timeval *tvp;
    switch (name) {
    case TNT_OPT_URI:
        if (opt->uristr) tnt_mem_free((void *)opt->uristr);
        opt->uristr = tnt_mem_dup(va_arg(args, char*));
        if (opt->uristr == NULL)
            return TNT_EMEMORY;
        if (uri_parse(opt->uri, opt->uristr) == -1)
            return TNT_EFAIL;
        break;
    case TNT_OPT_TMOUT_CONNECT:
        tvp = va_arg(args, struct timeval*);
        memcpy(&opt->tmout_connect, tvp, sizeof(struct timeval));
        break;
    case TNT_OPT_TMOUT_RECV:
        tvp = va_arg(args, struct timeval*);
        memcpy(&opt->tmout_recv, tvp, sizeof(struct timeval));
        break;
    case TNT_OPT_TMOUT_SEND:
        tvp = va_arg(args, struct timeval*);
        memcpy(&opt->tmout_send, tvp, sizeof(struct timeval));
        break;
    case TNT_OPT_SEND_CB:
        opt->send_cb = va_arg(args, void*);
        break;
    case TNT_OPT_SEND_CBV:
        opt->send_cbv = va_arg(args, void*);
        break;
    case TNT_OPT_SEND_CB_ARG:
        opt->send_cb_arg = va_arg(args, void*);
        break;
    case TNT_OPT_SEND_BUF:
        opt->send_buf = va_arg(args, int);
        break;
    case TNT_OPT_RECV_CB:
        opt->recv_cb = va_arg(args, void*);
        break;
    case TNT_OPT_RECV_CB_ARG:
        opt->recv_cb_arg = va_arg(args, void*);
        break;
    case TNT_OPT_RECV_BUF:
        opt->recv_buf = va_arg(args, int);
        break;
    default:
        return TNT_EFAIL;
    }
    return TNT_EOK;
}

#endif /* TNT_OPT_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_schema.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
            In tnt_schema_add_space() and tnt_schema_add_index()
            put "const char *name_tmp" and "uint32_t tuple_len"
            etc. at routine start to avoid
            "error: crosses initialization".
*/
#ifndef TNT_SCHEMA_C_INCLUDED
#define TNT_SCHEMA_C_INCLUDED
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <inttypes.h>
#include <assert.h>
#include <stdint.h>

//#include <msgpuck.h>

//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_buf.h>
//#include <tarantool/tnt_object.h>
//#include <tarantool/tnt_proto.h>
//#include <tarantool/tnt_schema.h>
//#include <tarantool/tnt_mem.h>
//#include <tarantool/tnt_select.h>

//#include "tnt_assoc.h"

static inline void
tnt_schema_ival_free(struct tnt_schema_ival *val) {
    if (val) tnt_mem_free((void *)val->name);
    tnt_mem_free(val);
}

static inline void
tnt_schema_index_free(struct mh_assoc_t *schema) {
    mh_int_t pos = 0;
    mh_int_t index_slot = 0;
    mh_foreach(schema, pos) {
        struct tnt_schema_ival *ival =
            (tnt_schema_ival *)(*mh_assoc_node(schema, pos))->data;
        struct assoc_val *av1 = NULL, *av2 = NULL;
        do {
            struct assoc_key key_number = {
                (const char *)(void *)&(ival->number),
                sizeof(uint32_t)
            };
            index_slot = mh_assoc_find(schema, &key_number, NULL);
            if (index_slot == mh_end(schema))
                break;
            av1 = *mh_assoc_node(schema, index_slot);
            mh_assoc_del(schema, index_slot, NULL);
        } while (0);
        do {
            struct assoc_key key_string = {
                ival->name,
                ival->name_len
            };
            index_slot = mh_assoc_find(schema, &key_string, NULL);
            if (index_slot == mh_end(schema))
                break;
            av2 = *mh_assoc_node(schema, index_slot);
            mh_assoc_del(schema, index_slot, NULL);
        } while (0);
        tnt_schema_ival_free(ival);
        if (av1) tnt_mem_free((void *)av1);
        if (av2) tnt_mem_free((void *)av2);
    }
}

static inline void
tnt_schema_sval_free(struct tnt_schema_sval *val) {
    if (val) {
        tnt_mem_free(val->name);
        if (val->index) {
            tnt_schema_index_free(val->index);
            mh_assoc_delete(val->index);
        }
    }
    tnt_mem_free(val);
}

static inline void
tnt_schema_space_free(struct mh_assoc_t *schema) {
    mh_int_t pos = 0;
    mh_int_t space_slot = 0;
    mh_foreach(schema, pos) {
        struct tnt_schema_sval *sval = NULL;
        sval = (tnt_schema_sval *)(*mh_assoc_node(schema, pos))->data;
        struct assoc_val *av1 = NULL, *av2 = NULL;
        do {
            struct assoc_key key_number = {
                (const char *)(void *)&(sval->number),
                sizeof(uint32_t)
            };
            space_slot = mh_assoc_find(schema, &key_number, NULL);
            if (space_slot == mh_end(schema))
                break;
            av1 = *mh_assoc_node(schema, space_slot);
            mh_assoc_del(schema, space_slot, NULL);
        } while (0);
        do {
            struct assoc_key key_string = {
                sval->name,
                sval->name_len
            };
            space_slot = mh_assoc_find(schema, &key_string, NULL);
            if (space_slot == mh_end(schema))
                break;
            av2 = *mh_assoc_node(schema, space_slot);
            mh_assoc_del(schema, space_slot, NULL);
        } while (0);
        tnt_schema_sval_free(sval);
        if (av1) tnt_mem_free((void *)av1);
        if (av2) tnt_mem_free((void *)av2);
    }
}

static inline int
tnt_schema_add_space(struct mh_assoc_t *schema, const char **data)
{
    const char *name_tmp;
    uint32_t tuple_len;
    struct tnt_schema_sval *space = NULL;
    struct assoc_val *space_string = NULL, *space_number = NULL;
    const char *tuple = *data;
    if (mp_typeof(*tuple) != MP_ARRAY)
        goto error;
    tuple_len = mp_decode_array(&tuple); (void )tuple_len;
    space = (tnt_schema_sval *)tnt_mem_alloc(sizeof(struct tnt_schema_sval));
    if (!space)
        goto error;
    memset(space, 0, sizeof(struct tnt_schema_sval));
    if (mp_typeof(*tuple) != MP_UINT)
        goto error;
    space->number = mp_decode_uint(&tuple);
    mp_next(&tuple); /* skip owner id */
    if (mp_typeof(*tuple) != MP_STR)
        goto error;
    name_tmp = mp_decode_str(&tuple, &space->name_len);
    space->name = (char *)tnt_mem_alloc(space->name_len);
    if (!space->name)
        goto error;
    memcpy(space->name, name_tmp, space->name_len);

    space->index = mh_assoc_new();
    if (!space->index)
        goto error;
    space_string = (assoc_val *)tnt_mem_alloc(sizeof(struct assoc_val));
    if (!space_string)
        goto error;
    space_string->key.id     = space->name;
    space_string->key.id_len = space->name_len;
    space_string->data = space;
    space_number = (assoc_val *)tnt_mem_alloc(sizeof(struct assoc_val));
    if (!space_number)
        goto error;
    space_number->key.id = (const char *)(void *)&(space->number);
    space_number->key.id_len = sizeof(space->number);
    space_number->data = space;
    mh_assoc_put(schema, (const struct assoc_val **)&space_string,
             NULL, NULL);
    mh_assoc_put(schema, (const struct assoc_val **)&space_number,
             NULL, NULL);
    mp_next(data);
    return 0;
error:
    mp_next(data);
    tnt_schema_sval_free(space);
    if (space_string) tnt_mem_free(space_string);
    if (space_number) tnt_mem_free(space_number);
    return -1;
}

int tnt_schema_add_spaces(struct tnt_schema *schema_obj, struct tnt_reply *r) {
    struct mh_assoc_t *schema = schema_obj->space_hash;
    const char *tuple = r->data;
    if (mp_check(&tuple, tuple + (r->data_end - r->data)))
        return -1;
    tuple = r->data;
    if (mp_typeof(*tuple) != MP_ARRAY)
        return -1;
    uint32_t space_count = mp_decode_array(&tuple);
    while (space_count-- > 0) {
        if (tnt_schema_add_space(schema, &tuple))
            return -1;
    }
    return 0;
}

static inline int
tnt_schema_add_index(struct mh_assoc_t *schema, const char **data) {
    const char *name_tmp;
    mh_int_t space_slot;
    int64_t tuple_len;
    uint32_t space_number;
    struct assoc_key space_key = {
        (const char *)(void *)&(space_number),
        sizeof(uint32_t)
    };
    const struct tnt_schema_sval *space = NULL;
    struct tnt_schema_ival *index = NULL;
    struct assoc_val *index_number = NULL, *index_string = NULL;
    const char *tuple = *data;
    if (mp_typeof(*tuple) != MP_ARRAY)
        goto error;
    tuple_len = mp_decode_array(&tuple); (void )tuple_len;
    space_number = mp_decode_uint(&tuple);
    if (mp_typeof(*tuple) != MP_UINT)
        goto error;
    space_slot = mh_assoc_find(schema, &space_key, NULL);
    if (space_slot == mh_end(schema))
        return -1;
    space = (const tnt_schema_sval *)(*mh_assoc_node(schema, space_slot))->data;
    index = (tnt_schema_ival *)tnt_mem_alloc(sizeof(struct tnt_schema_ival));
    if (!index)
        goto error;
    memset(index, 0, sizeof(struct tnt_schema_ival));
    if (mp_typeof(*tuple) != MP_UINT)
        goto error;
    index->number = mp_decode_uint(&tuple);
    if (mp_typeof(*tuple) != MP_STR)
        goto error;
    name_tmp = mp_decode_str(&tuple, &index->name_len);
    index->name = (char *)tnt_mem_alloc(index->name_len);
    if (!index->name)
        goto error;
    memcpy((void *)index->name, name_tmp, index->name_len);

    index_string = (assoc_val *)tnt_mem_alloc(sizeof(struct assoc_val));
    if (!index_string) goto error;
    index_string->key.id     = index->name;
    index_string->key.id_len = index->name_len;
    index_string->data = index;
    index_number = (assoc_val *)tnt_mem_alloc(sizeof(struct assoc_val));
    if (!index_number) goto error;
    index_number->key.id     = (const char *)(void *)&(index->number);
    index_number->key.id_len = sizeof(uint32_t);
    index_number->data = index;
    mh_assoc_put(space->index, (const struct assoc_val **)&index_string,
             NULL, NULL);
    mh_assoc_put(space->index, (const struct assoc_val **)&index_number,
             NULL, NULL);
    mp_next(data);
    return 0;
error:
    mp_next(data);
    if (index_string) tnt_mem_free(index_string);
    if (index_number) tnt_mem_free(index_number);
    tnt_schema_ival_free(index);
    return -1;
}

int tnt_schema_add_indexes(struct tnt_schema *schema_obj, struct tnt_reply *r) {
    struct mh_assoc_t *schema = schema_obj->space_hash;
    const char *tuple = r->data;
    if (mp_check(&tuple, tuple + (r->data_end - r->data)))
        return -1;
    tuple = r->data;
    if (mp_typeof(*tuple) != MP_ARRAY)
        return -1;
    uint32_t space_count = mp_decode_array(&tuple);
    while (space_count-- > 0) {
        if (tnt_schema_add_index(schema, &tuple))
            return -1;
    }
    return 0;
}

int32_t tnt_schema_stosid(struct tnt_schema *schema_obj, const char *name,
              uint32_t name_len) {
    struct mh_assoc_t *schema = schema_obj->space_hash;
    struct assoc_key space_key = {name, name_len};
    mh_int_t space_slot = mh_assoc_find(schema, &space_key, NULL);
    if (space_slot == mh_end(schema))
        return -1;
    const struct tnt_schema_sval *space =
        (const tnt_schema_sval *)(*mh_assoc_node(schema, space_slot))->data;
    return space->number;
}

int32_t tnt_schema_stoiid(struct tnt_schema *schema_obj, uint32_t sid,
              const char *name, uint32_t name_len) {
    struct mh_assoc_t *schema = schema_obj->space_hash;
    struct assoc_key space_key = {(const char *)(void *)&sid, sizeof(uint32_t)};
    mh_int_t space_slot = mh_assoc_find(schema, &space_key, NULL);
    if (space_slot == mh_end(schema))
        return -1;
    const struct tnt_schema_sval *space =
        (const tnt_schema_sval *)(*mh_assoc_node(schema, space_slot))->data;
    struct assoc_key index_key = {name, name_len};
    mh_int_t index_slot = mh_assoc_find(space->index, &index_key, NULL);
    if (index_slot == mh_end(space->index))
        return -1;
    const struct tnt_schema_ival *index =
        (const tnt_schema_ival *)(*mh_assoc_node(space->index, index_slot))->data;
    return index->number;
}

struct tnt_schema *tnt_schema_new(struct tnt_schema *s) {
    int alloc = (s == NULL);
    if (!s) {
        s = (tnt_schema *)tnt_mem_alloc(sizeof(struct tnt_schema));
        if (!s) return NULL;
    }
    s->space_hash = mh_assoc_new();
    s->alloc = alloc;
    return s;
}

void tnt_schema_flush(struct tnt_schema *obj) {
    tnt_schema_space_free(obj->space_hash);
}

void tnt_schema_free(struct tnt_schema *obj) {
    if (obj == NULL)
        return;
    tnt_schema_space_free(obj->space_hash);
    mh_assoc_delete(obj->space_hash);
}

ssize_t
tnt_get_space(struct tnt_stream *s)
{
    struct tnt_stream *obj = tnt_object(NULL);
    if (obj == NULL)
        return -1;

    tnt_object_add_array(obj, 0);
    ssize_t retval = tnt_select(s, tnt_vsp_space, tnt_vin_name,
                    UINT32_MAX, 0, TNT_ITER_ALL, obj);
    tnt_stream_free(obj);
    return retval;
}

ssize_t
tnt_get_index(struct tnt_stream *s)
{
    struct tnt_stream *obj = tnt_object(NULL);
    if (obj == NULL)
        return -1;

    tnt_object_add_array(obj, 0);
    ssize_t retval = tnt_select(s, tnt_vsp_index, tnt_vin_name,
                    UINT32_MAX, 0, TNT_ITER_ALL, obj);
    tnt_stream_free(obj);
    return retval;
}

#endif /* TNT_SCHEMA_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_object.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
            In tnt_object() put some early definitions to avoid
            "error: crosses initialization".
*/
#ifndef TNT_OBJECT_C_INCLUDED
#define TNT_OBJECT_C_INCLUDED
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

//#include <msgpuck.h>

//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_buf.h>
//#include <tarantool/tnt_object.h>
//#include <tarantool/tnt_mem.h>

static void
tnt_sbuf_object_free(struct tnt_stream *s)
{
    struct tnt_sbuf_object *sbo = TNT_SOBJ_CAST(s);
    if (sbo->stack) tnt_mem_free(sbo->stack);
    sbo->stack = NULL;
    tnt_mem_free(sbo);
}

int
tnt_object_type(struct tnt_stream *s, enum tnt_sbo_type type)
{
    if (s->wrcnt > 0) return -1;
    TNT_SOBJ_CAST(s)->type = type;
    return 0;
};

static int
tnt_sbuf_object_grow_stack(struct tnt_sbuf_object *sbo)
{
    if (sbo->stack_alloc == 128) return -1;
    uint8_t new_stack_alloc = 2 * sbo->stack_alloc;
    struct tnt_sbo_stack *stack = (tnt_sbo_stack *)tnt_mem_alloc(new_stack_alloc * sizeof(
                struct tnt_sbo_stack));
    if (!stack) return -1;
    sbo->stack_alloc = new_stack_alloc;
    sbo->stack = stack;
    return 0;
}

static char *
tnt_sbuf_object_resize(struct tnt_stream *s, size_t size) {
    struct tnt_stream_buf *sb = TNT_SBUF_CAST(s);
    if (sb->size + size > sb->alloc) {
        size_t newsize = 2 * (sb->alloc);
        if (newsize < sb->size + size)
            newsize = sb->size + size;
        char *nd = (char *)tnt_mem_realloc(sb->data, newsize);
        if (nd == NULL) {
            tnt_mem_free(sb->data);
            return NULL;
        }
        sb->data = nd;
        sb->alloc = newsize;
    }
    return sb->data + sb->size;
}

struct tnt_stream *
tnt_object(struct tnt_stream *s)
{
    struct tnt_stream_buf *sb;
    struct tnt_sbuf_object *sbo;
    if ((s = tnt_buf(s)) == NULL)
        goto error;

    sb = TNT_SBUF_CAST(s);
    sb->resize = tnt_sbuf_object_resize;
    sb->free = tnt_sbuf_object_free;

    sbo = (tnt_sbuf_object *)tnt_mem_alloc(sizeof(struct tnt_sbuf_object));
    if (sbo == NULL)
        goto error;
    sb->subdata = sbo;
    sbo->stack_size = 0;
    sbo->stack_alloc = 8;
    sbo->stack = (tnt_sbo_stack *)tnt_mem_alloc(sbo->stack_alloc *
            sizeof(struct tnt_sbo_stack));
    if (sbo->stack == NULL)
        goto error;
    tnt_object_type(s, TNT_SBO_SIMPLE);

    return s;
error:
    tnt_stream_free(s);
    return NULL;
}

ssize_t
tnt_object_add_nil (struct tnt_stream *s)
{
    struct tnt_sbuf_object *sbo = TNT_SOBJ_CAST(s);
    if (sbo->stack_size > 0)
        sbo->stack[sbo->stack_size - 1].size += 1;
    char data[2]; char *end = mp_encode_nil(data);
    return s->write(s, data, end - data);
}

ssize_t
tnt_object_add_uint(struct tnt_stream *s, uint64_t value)
{
    struct tnt_sbuf_object *sbo = TNT_SOBJ_CAST(s);
    if (sbo->stack_size > 0)
        sbo->stack[sbo->stack_size - 1].size += 1;
    char data[10], *end;
    end = mp_encode_uint(data, value);
    return s->write(s, data, end - data);
}

ssize_t
tnt_object_add_int (struct tnt_stream *s, int64_t value)
{
    struct tnt_sbuf_object *sbo = TNT_SOBJ_CAST(s);
    if (sbo->stack_size > 0)
        sbo->stack[sbo->stack_size - 1].size += 1;
    char data[10], *end;
    if (value < 0)
        end = mp_encode_int(data, value);
    else
        end = mp_encode_uint(data, value);
    return s->write(s, data, end - data);
}

ssize_t
tnt_object_add_str (struct tnt_stream *s, const char *str, uint32_t len)
{
    struct tnt_sbuf_object *sbo = TNT_SOBJ_CAST(s);
    if (sbo->stack_size > 0)
        sbo->stack[sbo->stack_size - 1].size += 1;
    struct iovec v[2]; int v_sz = 2;
    char data[6], *end;
    end = mp_encode_strl(data, len);
    v[0].iov_base = data;
    v[0].iov_len  = end - data;
    v[1].iov_base = (void *)str;
    v[1].iov_len  = len;
    return s->writev(s, v, v_sz);
}

ssize_t
tnt_object_add_strz (struct tnt_stream *s, const char *strz)
{
    uint32_t len = strlen(strz);
    return tnt_object_add_str(s, strz, len);
}

ssize_t
tnt_object_add_bin (struct tnt_stream *s, const void *bin, uint32_t len)
{
    struct tnt_sbuf_object *sbo = TNT_SOBJ_CAST(s);
    if (sbo->stack_size > 0)
        sbo->stack[sbo->stack_size - 1].size += 1;
    struct iovec v[2]; int v_sz = 2;
    char data[6], *end;
    end = mp_encode_binl(data, len);
    v[0].iov_base = data;
    v[0].iov_len  = end - data;
    v[1].iov_base = (void *)bin;
    v[1].iov_len  = len;
    return s->writev(s, v, v_sz);
}

ssize_t
tnt_object_add_bool (struct tnt_stream *s, char value)
{
    struct tnt_sbuf_object *sbo = TNT_SOBJ_CAST(s);
    if (sbo->stack_size > 0)
        sbo->stack[sbo->stack_size - 1].size += 1;
    char data[2], *end;
    end = mp_encode_bool(data, value != 0);
    return s->write(s, data, end - data);
}

ssize_t
tnt_object_add_float (struct tnt_stream *s, float value)
{
    struct tnt_sbuf_object *sbo = TNT_SOBJ_CAST(s);
    if (sbo->stack_size > 0)
        sbo->stack[sbo->stack_size - 1].size += 1;
    char data[6], *end;
    end = mp_encode_float(data, value);
    return s->write(s, data, end - data);
}

ssize_t
tnt_object_add_double (struct tnt_stream *s, double value)
{
    struct tnt_sbuf_object *sbo = TNT_SOBJ_CAST(s);
    if (sbo->stack_size > 0)
        sbo->stack[sbo->stack_size - 1].size += 1;
    char data[10], *end;
    end = mp_encode_double(data, value);
    return s->write(s, data, end - data);
}

static char *
mp_encode_array32(char *data, uint32_t size)
{
    data = mp_store_u8(data, 0xdd);
    return mp_store_u32(data, size);
}

ssize_t
tnt_object_add_array (struct tnt_stream *s, uint32_t size)
{
    struct tnt_sbuf_object *sbo = TNT_SOBJ_CAST(s);
    if (sbo->stack_size > 0)
        sbo->stack[sbo->stack_size - 1].size += 1;
    char data[6], *end;
    struct tnt_stream_buf  *sb  = TNT_SBUF_CAST(s);
    if (sbo->stack_size == sbo->stack_alloc)
        if (tnt_sbuf_object_grow_stack(sbo) == -1)
            return -1;
    sbo->stack[sbo->stack_size].size = 0;
    sbo->stack[sbo->stack_size].offset = sb->size;
    sbo->stack[sbo->stack_size].type = MP_ARRAY;
    sbo->stack_size += 1;
    if (TNT_SOBJ_CAST(s)->type == TNT_SBO_SIMPLE) {
        end = mp_encode_array(data, size);
    } else if (TNT_SOBJ_CAST(s)->type == TNT_SBO_SPARSE) {
        end = mp_encode_array32(data, 0);
    } else if (TNT_SOBJ_CAST(s)->type == TNT_SBO_PACKED) {
        end = mp_encode_array(data, 0);
    } else {
        return -1;
    }
    ssize_t rv = s->write(s, data, end - data);
    return rv;
}

static char *
mp_encode_map32(char *data, uint32_t size)
{
    data = mp_store_u8(data, 0xdf);
    return mp_store_u32(data, size);
}

ssize_t
tnt_object_add_map (struct tnt_stream *s, uint32_t size)
{
    struct tnt_sbuf_object *sbo = TNT_SOBJ_CAST(s);
    if (sbo->stack_size > 0)
        sbo->stack[sbo->stack_size - 1].size += 1;
    char data[6], *end;
    struct tnt_stream_buf  *sb  = TNT_SBUF_CAST(s);
    if (sbo->stack_size == sbo->stack_alloc)
        if (tnt_sbuf_object_grow_stack(sbo) == -1)
            return -1;
    sbo->stack[sbo->stack_size].size = 0;
    sbo->stack[sbo->stack_size].offset = sb->size;
    sbo->stack[sbo->stack_size].type = MP_MAP;
    sbo->stack_size += 1;
    if (TNT_SOBJ_CAST(s)->type == TNT_SBO_SIMPLE) {
        end = mp_encode_map(data, size);
    } else if (TNT_SOBJ_CAST(s)->type == TNT_SBO_SPARSE) {
        end = mp_encode_map32(data, 0);
    } else if (TNT_SOBJ_CAST(s)->type == TNT_SBO_PACKED) {
        end = mp_encode_map(data, 0);
    } else {
        return -1;
    }
    ssize_t rv = s->write(s, data, end - data);
    return rv;
}

ssize_t
tnt_object_container_close (struct tnt_stream *s)
{
    struct tnt_stream_buf   *sb = TNT_SBUF_CAST(s);
    struct tnt_sbuf_object *sbo = TNT_SOBJ_CAST(s);
    if (sbo->stack_size == 0) return -1;
    size_t       size   = sbo->stack[sbo->stack_size - 1].size;
    enum mp_type type   = (enum mp_type)sbo->stack[sbo->stack_size - 1].type;
    size_t       offset = sbo->stack[sbo->stack_size - 1].offset;
    if (type == MP_MAP && size % 2) return -1;
    sbo->stack_size -= 1;
    char *lenp = sb->data + offset;
    if (sbo->type == TNT_SBO_SIMPLE) {
        return 0;
    } else if (sbo->type == TNT_SBO_SPARSE) {
        if (type == MP_MAP)
            mp_encode_map32(lenp, size/2);
        else
            mp_encode_array32(lenp, size);
        return 0;
    } else if (sbo->type == TNT_SBO_PACKED) {
        size_t sz = 0;
        if (type == MP_MAP)
            sz = mp_sizeof_map(size/2);
        else
            sz = mp_sizeof_array(size);
        if (sz > 1) {
            if (!sb->resize(s, sz - 1))
                return -1;
            lenp = sb->data + offset;
            memmove(lenp + sz, lenp + 1, sb->size - offset - 1);
        }
        if (type == MP_MAP) {
            mp_encode_map(sb->data + offset, size/2);
        } else {
            mp_encode_array(sb->data + offset, size);
        }
        sb->size += (sz - 1);
        return 0;
    }
    return -1;
}

int tnt_object_verify(struct tnt_stream *obj, int8_t type)
{
    const char *pos = TNT_SBUF_DATA(obj);
    const char *end = pos + TNT_SBUF_SIZE(obj);
    if (type >= 0 && mp_typeof(*pos) != (uint8_t) type) return -1;
    if (mp_check(&pos, end)) return -1;
    if (pos < end) return -1;
    return 0;
}

ssize_t tnt_object_vformat(struct tnt_stream *s, const char *fmt, va_list vl)
{
    if (tnt_object_type(s, TNT_SBO_PACKED) == -1)
        return -1;
    ssize_t result = 0, rv = 0;

    for (const char *f = fmt; *f; f++) {
        if (f[0] == '[') {
            if ((rv = tnt_object_add_array(s, 0)) == -1)
                return -1;
            result += rv;
        } else if (f[0] == '{') {
            if ((rv = tnt_object_add_map(s, 0)) == -1)
                return -1;
            result += rv;
        } else if (f[0] == ']' || f[0] == '}') {
            if ((rv = tnt_object_container_close(s)) == -1)
                return -1;
            result += rv;
        } else if (f[0] == '%') {
            f++;
            assert(f[0]);
            int64_t int_value = 0;
            int int_status = 0; /* 1 - signed, 2 - unsigned */

            if (f[0] == 'd' || f[0] == 'i') {
                int_value = va_arg(vl, int);
                int_status = 1;
            } else if (f[0] == 'u') {
                int_value = va_arg(vl, unsigned int);
                int_status = 2;
            } else if (f[0] == 's') {
                const char *str = va_arg(vl, const char *);
                uint32_t len = (uint32_t)strlen(str);
                if ((rv = tnt_object_add_str(s, str, len)) == -1)
                    return -1;
                result += rv;
            } else if (f[0] == '.' && f[1] == '*' && f[2] == 's') {
                uint32_t len = va_arg(vl, uint32_t);
                const char *str = va_arg(vl, const char *);
                if ((rv = tnt_object_add_str(s, str, len)) == -1)
                    return -1;
                result += rv;
                f += 2;
            } else if (f[0] == 'f') {
                float v = (float)va_arg(vl, double);
                if ((rv = tnt_object_add_float(s, v)) == -1)
                    return -1;
                result += rv;
            } else if (f[0] == 'l' && f[1] == 'f') {
                double v = va_arg(vl, double);
                if ((rv = tnt_object_add_double(s, v)) == -1)
                    return -1;
                result += rv;
                f++;
            } else if (f[0] == 'b') {
                bool v = (bool)va_arg(vl, int);
                if ((rv = tnt_object_add_bool(s, v)) == -1)
                    return -1;
                result += rv;
            } else if (f[0] == 'l'
                   && (f[1] == 'd' || f[1] == 'i')) {
                int_value = va_arg(vl, long);
                int_status = 1;
                f++;
            } else if (f[0] == 'l' && f[1] == 'u') {
                int_value = va_arg(vl, unsigned long);
                int_status = 2;
                f++;
            } else if (f[0] == 'l' && f[1] == 'l'
                   && (f[2] == 'd' || f[2] == 'i')) {
                int_value = va_arg(vl, long long);
                int_status = 1;
                f += 2;
            } else if (f[0] == 'l' && f[1] == 'l' && f[2] == 'u') {
                int_value = va_arg(vl, unsigned long long);
                int_status = 2;
                f += 2;
            } else if (f[0] == 'h'
                   && (f[1] == 'd' || f[1] == 'i')) {
                int_value = va_arg(vl, int);
                int_status = 1;
                f++;
            } else if (f[0] == 'h' && f[1] == 'u') {
                int_value = va_arg(vl, unsigned int);
                int_status = 2;
                f++;
            } else if (f[0] == 'h' && f[1] == 'h'
                   && (f[2] == 'd' || f[2] == 'i')) {
                int_value = va_arg(vl, int);
                int_status = 1;
                f += 2;
            } else if (f[0] == 'h' && f[1] == 'h' && f[2] == 'u') {
                int_value = va_arg(vl, unsigned int);
                int_status = 2;
                f += 2;
            } else if (f[0] != '%') {
                /* unexpected format specifier */
                assert(false);
            }

            if (int_status) {
                if ((rv = tnt_object_add_int(s, int_value)) == -1)
                    return -1;
                result += rv;
            }
        } else if (f[0] == 'N' && f[1] == 'I' && f[2] == 'L') {
            if ((rv = tnt_object_add_nil(s)) == -1)
                return -1;
            result += rv;
            f += 2;
        }
    }
    return result;
}

ssize_t tnt_object_format(struct tnt_stream *s, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ssize_t res = tnt_object_vformat(s, fmt, args);
    va_end(args);
    return res;
}

struct tnt_stream *tnt_object_as(struct tnt_stream *s, char *buf,
                 size_t buf_len)
{
    if (s == NULL) {
        s = tnt_object(s);
        if (s == NULL)
            return NULL;
    }
    struct tnt_stream_buf *sb = TNT_SBUF_CAST(s);

    sb->data = buf;
    sb->size = buf_len;
    sb->alloc = buf_len;
    sb->as = 1;

    return s;
}

int tnt_object_reset(struct tnt_stream *s)
{
    struct tnt_stream_buf *sb = TNT_SBUF_CAST(s);
    struct tnt_sbuf_object *sbo = TNT_SOBJ_CAST(s);

    s->reqid = 0;
    s->wrcnt = 0;
    sb->size = 0;
    sb->rdoff = 0;
    sbo->stack_size = 0;
    sbo->type = TNT_SBO_SIMPLE;

    return 0;
}

#endif /* TNT_OBJECT_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_io.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
   Added #define IOV_MAX 1024 because limits.h is already included.
   BIG Change: tnt_io_nonblock() had to be rewritten because MinGW
   won't handle fcntl at all. The stuff inside #ifdef _WIN32 ... #else
   is my best guess, but I really don't know this stuff very well.
   And "s->errno_ = WSAGetLastError();" is almost certainly the
   wrong value -- I'm just hoping nobody cares about result details.
   Re setsockopt|getsockopt: Microsoft has const char* but POSIX
   usually has void*, so for all getsockopt|setsockopt calls:
     &avg to static_cast<void*>(&avg)
     &opt to static_cast<*void*>(&opt)
     &s->opt.tmout_send to static_cast<void*>(&s->opt.tmout_send)
     &s->opt.tmout_recv to static_cast<void*>(&s->opt.tmout_recv)
   Re writev: I wrote pseudo_writev, I hope it does the same thing.
   Anything inside #ifdef WIN32 ... #endif is added by Peter Gulutzan.
   Todo: We call WSAStartup every time we connect, never WSACleanup.
*/
#ifndef TNT_IO_C_INCLUDED
#define TNT_IO_C_INCLUDED
/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/* need this to get IOV_MAX on some platforms. */
#ifndef __need_IOV_MAX
#define __need_IOV_MAX
#endif
#include <limits.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include <sys/time.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <sys/uio.h>
//#include <netinet/in.h>
//#include <sys/un.h>
//#include <netinet/tcp.h>
//#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

//#include <tarantool/tnt_net.h>
//#include <tarantool/tnt_io.h>

//#include <uri.h>

#ifndef IOV_MAX
#define IOV_MAX 1024
#endif

#if !defined(MIN)
#	define MIN(a, b) (a) < (b) ? (a) : (b)
#endif /* !defined(MIN) */

static enum tnt_error
tnt_io_resolve(struct sockaddr_in *addr,
           const char *hostname, unsigned short port)
{
    memset(addr, 0, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    struct addrinfo *addr_info = NULL;
    if (getaddrinfo(hostname, NULL, NULL, &addr_info) == 0 &&
        addr_info != NULL) {
        memcpy(&addr->sin_addr,
               (void*)&((struct sockaddr_in *)addr_info->ai_addr)->sin_addr,
               sizeof(addr->sin_addr));
        freeaddrinfo(addr_info);
        return TNT_EOK;
    }
    if (addr_info)
        freeaddrinfo(addr_info);
    return TNT_ERESOLVE;
}

static enum tnt_error
tnt_io_nonblock(struct tnt_stream_net *s, int set)
{
#ifdef _WIN32
   unsigned long mode;
   if (set) mode = 1;
   else mode = 0;
   if (ioctlsocket(s->fd, FIONBIO, &mode) != 0)
   {
       s->errno_ = WSAGetLastError();
       return TNT_ESYSTEM;
   }
   return TNT_EOK;
#else
    int flags = fcntl(s->fd, F_GETFL);
    if (flags == -1) {
        s->errno_ = errno;
        return TNT_ESYSTEM;
    }
    if (set)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;
    if (fcntl(s->fd, F_SETFL, flags) == -1) {
        s->errno_ = errno;
        return TNT_ESYSTEM;
    }
    return TNT_EOK;
#endif
}

static enum tnt_error
tnt_io_connect_do(struct tnt_stream_net *s, struct sockaddr *addr,
               socklen_t addr_size)
{
    /* setting nonblock */
    enum tnt_error result = tnt_io_nonblock(s, 1);
    if (result != TNT_EOK)
        return result;

    if (connect(s->fd, (struct sockaddr*)addr, addr_size) != -1)
        return TNT_EOK;
#ifdef _WIN32
    errno = WSAGetLastError();
#endif
#ifdef _WIN32
    if ((errno == WSAEINPROGRESS) || (errno == WSAEWOULDBLOCK)) {
#else
    if (errno == EINPROGRESS) {
#endif
        /** waiting for connection while handling signal events */
        const int64_t micro = 1000000;
        int64_t tmout_usec = s->opt.tmout_connect.tv_sec * micro;
        /* get start connect time */
        struct timeval start_connect;
        if (gettimeofday(&start_connect, NULL) == -1) {
            s->errno_ = errno;
            return TNT_ESYSTEM;
        }
        /* set initial timer */
        struct timeval tmout;
        memcpy(&tmout, &s->opt.tmout_connect, sizeof(tmout));
        while (1) {
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(s->fd, &fds);
            int ret = select(s->fd + 1, NULL, &fds, NULL, &tmout);
            if (ret == -1) {
#ifdef _WIN32
                errno = WSAGetLastError();
#endif
#ifdef _WIN32
                if ((errno == WSAEINTR) || (errno == WSATRY_AGAIN)) {
#else
                if (errno == EINTR || errno == EAGAIN) {
#endif
                    /* get current time */
                    struct timeval curr;
                    if (gettimeofday(&curr, NULL) == -1) {
                        s->errno_ = errno;
                        return TNT_ESYSTEM;
                    }
                    /* calculate timeout last time */
                    int64_t passd_usec = (curr.tv_sec - start_connect.tv_sec) * micro +
                        (curr.tv_usec - start_connect.tv_usec);
                    int64_t curr_tmeout = passd_usec - tmout_usec;
                    if (curr_tmeout <= 0) {
                        /* timeout */
                        return TNT_ETMOUT;
                    }
                    tmout.tv_sec = curr_tmeout / micro;
                    tmout.tv_usec = curr_tmeout % micro;
                } else {
                    s->errno_ = errno;
                    return TNT_ESYSTEM;
                }
            } else if (ret == 0) {
                /* timeout */
                return TNT_ETMOUT;
            } else {
                /* we have a event on socket */
                break;
            }
        }
        /* checking error status */
        int opt = 0;
        socklen_t len = sizeof(opt);
#ifdef _WIN32
        if ((getsockopt(s->fd, SOL_SOCKET, SO_ERROR,
                static_cast<void*>(&opt), &len) == -1) || opt) {
            errno = WSAGetLastError();
            s->errno_ = (opt) ? opt : errno;
#else
        if ((getsockopt(s->fd, SOL_SOCKET, SO_ERROR,
                &opt, &len) == -1) || opt) {
            s->errno_ = (opt) ? opt : errno;
#endif
            return TNT_ESYSTEM;
        }
    } else {
        s->errno_ = errno;
        return TNT_ESYSTEM;
    }

    /* setting block */
    result = tnt_io_nonblock(s, 0);
    if (result != TNT_EOK)
        return result;
    return TNT_EOK;
}

static enum tnt_error
tnt_io_connect_tcp(struct tnt_stream_net *s, const char *host, int port)
{
    /* resolving address */
    struct sockaddr_in addr;
    enum tnt_error result = tnt_io_resolve(&addr, host, port);
    if (result != TNT_EOK)
        return result;

    return tnt_io_connect_do(s, (struct sockaddr *)&addr, sizeof(addr));
}

static enum tnt_error
tnt_io_connect_unix(struct tnt_stream_net *s, const char *path)
{
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);
    if (connect(s->fd, (struct sockaddr*)&addr, sizeof(addr)) != -1)
        return TNT_EOK;
#ifdef _WIN32
    errno = WSAGetLastError();
#endif
    s->errno_ = errno;
    return TNT_ESYSTEM;
}

static enum tnt_error tnt_io_xbufmax(struct tnt_stream_net *s, int opt, int min) {
    int max = 128 * 1024 * 1024;
    if (min == 0)
        min = 16384;
    unsigned int avg = 0;
    while (min <= max) {
        avg = ((unsigned int)(min + max)) / 2;
#ifdef _WIN32
        if (setsockopt(s->fd, SOL_SOCKET, opt, static_cast<void*>(&avg), sizeof(avg)) == 0)
            min = avg + 1;
#else
        if (setsockopt(s->fd, SOL_SOCKET, opt, &avg, sizeof(avg)) == 0)
            min = avg + 1;
#endif
        else
            max = avg - 1;
    }
    return TNT_EOK;
}

static enum tnt_error tnt_io_setopts(struct tnt_stream_net *s) {
    int opt = 1;
    if (s->opt.uri->host_hint != URI_UNIX) {
#ifdef _WIN32
        if (setsockopt(s->fd, IPPROTO_TCP, TCP_NODELAY, static_cast<void*>(&opt), sizeof(opt)) == -1)
            goto error;
#else
        if (setsockopt(s->fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) == -1)
            goto error;
#endif
    }

    tnt_io_xbufmax(s, SO_SNDBUF, s->opt.send_buf);
    tnt_io_xbufmax(s, SO_RCVBUF, s->opt.recv_buf);
#ifdef _WIN32
    if (setsockopt(s->fd, SOL_SOCKET, SO_SNDTIMEO,
               static_cast<void*>(&s->opt.tmout_send), sizeof(s->opt.tmout_send)) == -1)
#else
    if (setsockopt(s->fd, SOL_SOCKET, SO_SNDTIMEO,
               &s->opt.tmout_send, sizeof(s->opt.tmout_send)) == -1)
#endif
    goto error;
#ifdef _WIN32
    if (setsockopt(s->fd, SOL_SOCKET, SO_RCVTIMEO,
               static_cast<void*>(&s->opt.tmout_recv), sizeof(s->opt.tmout_recv)) == -1)
#else
    if (setsockopt(s->fd, SOL_SOCKET, SO_RCVTIMEO,
               &s->opt.tmout_recv, sizeof(s->opt.tmout_recv)) == -1)
#endif
    goto error;
    return TNT_EOK;
error:
#ifdef _WIN32
    errno = WSAGetLastError();
#endif
    s->errno_ = errno;
    return TNT_ESYSTEM;
}

static int tnt_io_htopf(int host_hint) {
    switch(host_hint) {
    case URI_NAME:
    case URI_IPV4:
        return PF_INET;
    case URI_IPV6:
        return PF_INET6;
    case URI_UNIX:
        return PF_UNIX;
    default:
        return -1;
    }
}

enum tnt_error
tnt_io_connect(struct tnt_stream_net *s)
{
#ifdef _WIN32
  WSADATA wsadata;
  int iresult = WSAStartup(MAKEWORD(2, 2), &wsadata);
  if(iresult != 0)
  {
     printf("WSAStartup failed.\n");
     exit(0);
  }
#endif
    struct uri *uri = s->opt.uri;
    s->fd = socket(tnt_io_htopf(uri->host_hint), SOCK_STREAM, 0);
    if (s->fd < 0) {
#ifdef _WIN32
        errno = WSAGetLastError();
#endif
        s->errno_ = errno;
        return TNT_ESYSTEM;
    }
    enum tnt_error result = tnt_io_setopts(s);
    if (result != TNT_EOK)
        goto out;
    switch (uri->host_hint) {
    case URI_NAME:
    case URI_IPV4:
    case URI_IPV6: {
        char host[128];
        memcpy(host, uri->host, uri->host_len);
        host[uri->host_len] = '\0';
        uint32_t port = 3301;
        if (uri->service)
            port = strtol(uri->service, NULL, 10);
        result = tnt_io_connect_tcp(s, host, port);
        break;
    }
    case URI_UNIX: {
        char service[128];
        memcpy(service, uri->service, uri->service_len);
        service[uri->service_len] = '\0';
        result = tnt_io_connect_unix(s, service);
        break;
    }
    default:
        result = TNT_EFAIL;
    }
    if (result != TNT_EOK)
        goto out;
    s->connected = 1;
    return TNT_EOK;
out:
    tnt_io_close(s);
    return result;
}

void tnt_io_close(struct tnt_stream_net *s)
{
    if (s->fd > 0) {
        close(s->fd);
        s->fd = -1;
    }
    s->connected = 0;
}

ssize_t tnt_io_flush(struct tnt_stream_net *s) {
    if (s->sbuf.off == 0)
        return 0;
    ssize_t rc = tnt_io_send_raw(s, s->sbuf.buf, s->sbuf.off, 1);
    if (rc == -1)
        return -1;
    s->sbuf.off = 0;
    return rc;
}

ssize_t
tnt_io_send_raw(struct tnt_stream_net *s, const char *buf, size_t size, int all)
{
    size_t off = 0;
    do {
        ssize_t r;
        if (s->sbuf.tx) {
            r = s->sbuf.tx(&s->sbuf, buf + off, size - off);
        } else {
            do {
                r = send(s->fd, buf + off, size - off, 0);
#ifdef _WIN32
            } while (r == -1 && (WSAGetLastError() == WSAEINTR));
#else
            } while (r == -1 && (errno == EINTR));
#endif
        }
        if (r <= 0) {
            s->error = TNT_ESYSTEM;
#ifdef _WIN32
            errno = WSAGetLastError();
#endif
            s->errno_ = errno;
            return -1;
        }
        off += r;
    } while (off != size && all);
    return off;
}

#ifdef _WIN32
/*
  Addition for ocelotgui 1.0.9 on 2019-07-05
  MinGW lacks writev() so combine input buffers and call write().
*/
ssize_t
pseudo_writev(int fd, const struct iovec *iov, int iovcnt)
{
  ssize_t r;
  char *combined_buffer;
  int combined_buffer_size= 0, offset= 0, i;
  for(i= 0; i < iovcnt; ++i) combined_buffer_size+= iov[i].iov_len;
  combined_buffer= (char*)malloc(combined_buffer_size);
  if (combined_buffer == NULL) return -1000; /* -1000 later causes errno= ENOMEM */
  for (i= 0; i < iovcnt; ++i)
  {
    memcpy(combined_buffer + offset, iov[i].iov_base, iov[i].iov_len);
    offset+= iov[i].iov_len;
  }
  r= send(fd, combined_buffer, combined_buffer_size, 0);
  free(combined_buffer);
  return r;
}
#endif

ssize_t
tnt_io_sendv_raw(struct tnt_stream_net *s, struct iovec *iov, int count, int all)
{
    size_t total = 0;
    while (count > 0) {
        ssize_t r;
        if (s->sbuf.txv) {
            r = s->sbuf.txv(&s->sbuf, iov, MIN(count, IOV_MAX));
        } else {
            do {
#ifdef _WIN32
                r = pseudo_writev(s->fd, iov, count);
#else
                r = writev(s->fd, iov, count);
#endif
#ifdef _WIN32
            } while (r == -1 && (WSAGetLastError() == WSAEINTR));
#else
            } while (r == -1 && (errno == EINTR));
#endif
        }
        if (r <= 0) {
            s->error = TNT_ESYSTEM;
#ifdef _WIN32
            if (r == -1000) errno= ENOMEM; /* special value returned by pseudo_writev */
            else errno = WSAGetLastError();
            /* todo: maybe if r<0 and errno==0 we should set errno= something not 0 */
#endif
            s->errno_ = errno;
            return -1;
        }
        total += r;
        if (!all)
            break;
        while (count > 0) {
            if (iov->iov_len > (size_t)r) {
#ifdef OCELOT_OS_FREEBSD
                iov->iov_base = (char*)(iov->iov_base) + r;
#else
                iov->iov_base += r;
#endif
                iov->iov_len -= r;
                break;
            } else {
                r -= iov->iov_len;
                iov++;
                count--;
            }
        }
    }
    return total;
}

ssize_t
tnt_io_send(struct tnt_stream_net *s, const char *buf, size_t size)
{
    if (s->sbuf.buf == NULL)
        return tnt_io_send_raw(s, buf, size, 1);
    if (size > s->sbuf.size) {
        s->error = TNT_EBIG;
        return -1;
    }
    if ((s->sbuf.off + size) <= s->sbuf.size) {
        memcpy(s->sbuf.buf + s->sbuf.off, buf, size);
        s->sbuf.off += size;
        return size;
    }
    ssize_t r = tnt_io_send_raw(s, s->sbuf.buf, s->sbuf.off, 1);
    if (r == -1)
        return -1;
    s->sbuf.off = size;
    memcpy(s->sbuf.buf, buf, size);
    return size;
}

inline static void
tnt_io_sendv_put(struct tnt_stream_net *s, struct iovec *iov, int count) {
    int i;
    for (i = 0 ; i < count ; i++) {
        memcpy(s->sbuf.buf + s->sbuf.off,
               iov[i].iov_base,
               iov[i].iov_len);
        s->sbuf.off += iov[i].iov_len;
    }
}

ssize_t
tnt_io_sendv(struct tnt_stream_net *s, struct iovec *iov, int count)
{
    if (s->sbuf.buf == NULL)
        return tnt_io_sendv_raw(s, iov, count, 1);
    size_t size = 0;
    int i;
    for (i = 0 ; i < count ; i++)
        size += iov[i].iov_len;
    if (size > s->sbuf.size) {
        s->error = TNT_EBIG;
        return -1;
    }
    if ((s->sbuf.off + size) <= s->sbuf.size) {
        tnt_io_sendv_put(s, iov, count);
        return size;
    }
    ssize_t r = tnt_io_send_raw(s, s->sbuf.buf, s->sbuf.off, 1);
    if (r == -1)
        return -1;
    s->sbuf.off = 0;
    tnt_io_sendv_put(s, iov, count);
    return size;
}

ssize_t
tnt_io_recv_raw(struct tnt_stream_net *s, char *buf, size_t size, int all)
{
    size_t off = 0;
    do {
        ssize_t r;
        if (s->rbuf.tx) {
            r = s->rbuf.tx(&s->rbuf, buf + off, size - off);
        } else {
            do {
                r = recv(s->fd, buf + off, size - off, 0);
#ifdef _WIN32
            } while (r == -1 && (WSAGetLastError() == WSAEINTR));
#else
            } while (r == -1 && (errno == EINTR));
#endif
        }
        if (r <= 0) {
            s->error = TNT_ESYSTEM;
#ifdef _WIN32
            errno = WSAGetLastError();
#endif
            s->errno_ = errno;
            return -1;
        }
        off += r;
    } while (off != size && all);
    return off;
}

ssize_t
tnt_io_recv(struct tnt_stream_net *s, char *buf, size_t size)
{
    if (s->rbuf.buf == NULL)
        return tnt_io_recv_raw(s, buf, size, 1);
    size_t lv, rv, off = 0, left = size;
    while (1) {
        if ((s->rbuf.off + left) <= s->rbuf.top) {
            memcpy(buf + off, s->rbuf.buf + s->rbuf.off, left);
            s->rbuf.off += left;
            return size;
        }

        lv = s->rbuf.top - s->rbuf.off;
        rv = left - lv;
        if (lv) {
            memcpy(buf + off, s->rbuf.buf + s->rbuf.off, lv);
            off += lv;
        }

        s->rbuf.off = 0;
        ssize_t top = tnt_io_recv_raw(s, s->rbuf.buf, s->rbuf.size, 0);
        if (top <= 0) {
#ifdef _WIN32
            errno = WSAGetLastError();
#endif
            s->errno_ = errno;
            s->error = TNT_ESYSTEM;
            return -1;
        }

        s->rbuf.top = top;
        if (rv <= s->rbuf.top) {
            memcpy(buf + off, s->rbuf.buf, rv);
            s->rbuf.off = rv;
            return size;
        }
        left -= lv;
    }
    return -1;
}
#define TNT_IO_C_INCLUDED
#endif /* TNT_IO_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_net.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef TNT_NET_C_INCLUDED
#define TNT_NET_C_INCLUDED

/*
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

//#include <sys/uio.h>

//#include <uri.h>

//#include <tarantool/tnt_proto.h>
//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_object.h>
//#include <tarantool/tnt_mem.h>
//#include <tarantool/tnt_schema.h>
//#include <tarantool/tnt_select.h>
//#include <tarantool/tnt_iter.h>
//#include <tarantool/tnt_auth.h>

//#include <tarantool/tnt_net.h>
//#include <tarantool/tnt_io.h>

//#include "pmatomic.h"

static void tnt_net_free(struct tnt_stream *s) {
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    tnt_io_close(sn);
    tnt_mem_free(sn->greeting);
    tnt_iob_free(&sn->sbuf);
    tnt_iob_free(&sn->rbuf);
    tnt_opt_free(&sn->opt);
    tnt_schema_free(sn->schema);
    tnt_mem_free(sn->schema);
    tnt_mem_free(s->data);
    s->data = NULL;
}

static ssize_t
tnt_net_read(struct tnt_stream *s, char *buf, size_t size) {
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    /* read doesn't touches wrcnt */
    return tnt_io_recv(sn, buf, size);
}

static ssize_t
tnt_net_write(struct tnt_stream *s, const char *buf, size_t size) {
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    ssize_t rc = tnt_io_send(sn, buf, size);
    if (rc != -1)
        pm_atomic_fetch_add(&s->wrcnt, 1);
    return rc;
}

static ssize_t
tnt_net_writev(struct tnt_stream *s, struct iovec *iov, int count) {
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    ssize_t rc = tnt_io_sendv(sn, iov, count);
    if (rc != -1)
        pm_atomic_fetch_add(&s->wrcnt, 1);
    return rc;
}

static ssize_t
tnt_net_recv_cb(struct tnt_stream *s, char *buf, ssize_t size) {
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    return tnt_io_recv(sn, buf, size);
}

static int
tnt_net_reply(struct tnt_stream *s, struct tnt_reply *r) {
    if (pm_atomic_load(&s->wrcnt) == 0)
        return 1;
    pm_atomic_fetch_sub(&s->wrcnt, 1);
    return tnt_reply_from(r, (tnt_reply_t)tnt_net_recv_cb, s);
}

struct tnt_stream *tnt_net(struct tnt_stream *s) {
    s = tnt_stream_init(s);
    if (s == NULL)
        return NULL;
    /* allocating stream data */
    s->data = tnt_mem_alloc(sizeof(struct tnt_stream_net));
    if (s->data == NULL) {
        tnt_stream_free(s);
        return NULL;
    }
    memset(s->data, 0, sizeof(struct tnt_stream_net));
    /* initializing interfaces */
    s->read = tnt_net_read;
    s->read_reply = tnt_net_reply;
    s->write = tnt_net_write;
    s->writev = tnt_net_writev;
    s->free = tnt_net_free;
    /* initializing internal data */
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    sn->fd = -1;
    sn->greeting = (char *)tnt_mem_alloc(TNT_GREETING_SIZE);
    if (sn->greeting == NULL) {
        tnt_stream_free(s);
    }
    if (tnt_opt_init(&sn->opt) == -1) {
        tnt_stream_free(s);
    }
    return s;
}

int tnt_set(struct tnt_stream *s, int opt, ...) {
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    va_list args;
    va_start(args, opt);
    sn->error = (tnt_error)tnt_opt_set(&sn->opt,(tnt_opt_type)opt,args);
    va_end(args);
    return (sn->error == TNT_EOK) ? 0 : -1;
}

int tnt_init(struct tnt_stream *s) {
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    if ((sn->schema = tnt_schema_new(NULL)) == NULL) {
        sn->error = TNT_EMEMORY;
        return -1;
    }
    if (tnt_iob_init(&sn->sbuf, sn->opt.send_buf, (tnt_iob_tx_t)sn->opt.send_cb,
        (tnt_iob_txv_t)sn->opt.send_cbv, sn->opt.send_cb_arg) == -1) {
        sn->error = TNT_EMEMORY;
        return -1;
    }
    if (tnt_iob_init(&sn->rbuf, sn->opt.recv_buf, (tnt_iob_tx_t)sn->opt.recv_cb, NULL,
        sn->opt.recv_cb_arg) == -1) {
        sn->error = TNT_EMEMORY;
        return -1;
    }
    sn->inited = 1;
    return 0;
}

int tnt_reload_schema(struct tnt_stream *s)
{
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    if (!sn->connected || pm_atomic_load(&s->wrcnt) != 0)
        return -1;
    uint64_t oldsync = tnt_stream_reqid(s, 127);
    tnt_get_space(s);
    tnt_get_index(s);
    tnt_stream_reqid(s, oldsync);
    tnt_flush(s);
    struct tnt_iter it; tnt_iter_reply(&it, s);
    struct tnt_reply bkp; tnt_reply_init(&bkp);
    int sloaded = 0;
    while (tnt_next(&it)) {
        struct tnt_reply *r = TNT_IREPLY_PTR(&it);
        switch (r->sync) {
        case(127):
            if (r->error)
                goto error;
            tnt_schema_add_spaces(sn->schema, r);
            sloaded += 1;
            break;
        case(128):
            if (r->error)
                goto error;
            if (!(sloaded & 1)) {
                memcpy(&bkp, r, sizeof(struct tnt_reply));
                r->buf = NULL;
                break;
            }
            sloaded += 2;
            tnt_schema_add_indexes(sn->schema, r);
            break;
        default:
            goto error;
        }
    }
    if (bkp.buf) {
        tnt_schema_add_indexes(sn->schema, &bkp);
        sloaded += 2;
        tnt_reply_free(&bkp);
    }
    if (sloaded != 3) goto error;

    tnt_iter_free(&it);
    return 0;
error:
    tnt_iter_free(&it);
    return -1;
}

static int
tnt_authenticate(struct tnt_stream *s)
{
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    if (!sn->connected || pm_atomic_load(&s->wrcnt) != 0)
        return -1;
    struct uri *uri = sn->opt.uri;
    tnt_auth(s, uri->login, uri->login_len, uri->password,
         uri->password_len);
    tnt_flush(s);
    struct tnt_reply rep;
    tnt_reply_init(&rep);
    if (s->read_reply(s, &rep) == -1)
        return -1;
    if (rep.error != NULL) {
        if (TNT_REPLY_ERR(&rep) == TNT_ER_PASSWORD_MISMATCH)
            sn->error = TNT_ELOGIN;
        return -1;
    }
    tnt_reply_free(&rep);
    tnt_reload_schema(s);
    return 0;
}

int tnt_connect(struct tnt_stream *s)
{
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    if (!sn->inited) tnt_init(s);
    if (sn->connected)
        tnt_close(s);
    sn->error = tnt_io_connect(sn);
    if (sn->error != TNT_EOK)
        return -1;
    if (s->read(s, sn->greeting, TNT_GREETING_SIZE) == -1 ||
        sn->error != TNT_EOK)
        return -1;
    if (sn->opt.uri->login && sn->opt.uri->password)
        if (tnt_authenticate(s) == -1)
            return -1;
    return 0;
}

void tnt_close(struct tnt_stream *s) {
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    tnt_iob_clear(&sn->sbuf);
    tnt_iob_clear(&sn->rbuf);
    tnt_io_close(sn);
    s->wrcnt = 0;
    s->reqid = 0;
}

ssize_t tnt_flush(struct tnt_stream *s) {
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    return tnt_io_flush(sn);
}


int tnt_fd(struct tnt_stream *s) {
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    return sn->fd;
}

enum tnt_error xtnt_error(struct tnt_stream *s) {
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    return sn->error;
}

/* must be in sync with enum tnt_error */

struct tnt_error_desc {
    enum tnt_error type;
    char *desc;
};

static struct tnt_error_desc tnt_error_list[] =
{
    { TNT_EOK,      "ok"                       },
    { TNT_EFAIL,    "fail"                     },
    { TNT_EMEMORY,  "memory allocation failed" },
    { TNT_ESYSTEM,  "system error"             },
    { TNT_EBIG,     "buffer is too big"        },
    { TNT_ESIZE,    "bad buffer size"          },
    { TNT_ERESOLVE, "gethostbyname(2) failed"  },
    { TNT_ETMOUT,   "operation timeout"        },
    { TNT_EBADVAL,  "bad argument"             },
    { TNT_ELOGIN,   "failed to login"          },
    { TNT_LAST,      NULL                      }
};

char *tnt_strerror(struct tnt_stream *s)
{
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    if (sn->error == TNT_ESYSTEM) {
        static char msg[256];
        snprintf(msg, sizeof(msg), "%s (errno: %d)",
             strerror(sn->errno_), sn->errno_);
        return msg;
    }
    return tnt_error_list[(int)sn->error].desc;
}

int tnt_errno(struct tnt_stream *s) {
    struct tnt_stream_net *sn = TNT_SNET_CAST(s);
    return sn->errno_;
}

int tnt_get_spaceno(struct tnt_stream *s, const char *space,
            size_t space_len)
{
    struct tnt_schema *sch = (TNT_SNET_CAST(s))->schema;
    return tnt_schema_stosid(sch, space, space_len);
}

int tnt_get_indexno(struct tnt_stream *s, int spaceno, const char *index,
            size_t index_len)
{
    struct tnt_schema *sch = TNT_SNET_CAST(s)->schema;
    return tnt_schema_stoiid(sch, spaceno, index, index_len);
}

#endif /* TNT_NET_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_update.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef TNT_UPDATE_C_INCLUDED
#define TNT_UPDATE_C_INCLUDED
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>

//#include <tarantool/tnt_mem.h>
//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_buf.h>
//#include <tarantool/tnt_object.h>
//#include <tarantool/tnt_update.h>

//#include "tnt_proto_internal.h"

ssize_t
tnt_update(struct tnt_stream *s, uint32_t space, uint32_t index,
       struct tnt_stream *key, struct tnt_stream *ops)
{
    if (tnt_object_verify(key, MP_ARRAY))
        return -1;
    if (tnt_object_verify(ops, MP_ARRAY))
        return -1;
    struct tnt_iheader hdr;
    struct iovec v[6]; int v_sz = 6;
    char *data = NULL, *body_start = NULL;
    encode_header(&hdr, TNT_OP_UPDATE, s->reqid++);
    v[1].iov_base = (void *)hdr.header;
    v[1].iov_len  = hdr.end - hdr.header;
    char body[64]; body_start = body; data = body;

    data = mp_encode_map(data, 4);
    data = mp_encode_uint(data, TNT_SPACE);
    data = mp_encode_uint(data, space);
    data = mp_encode_uint(data, TNT_INDEX);
    data = mp_encode_uint(data, index);
    data = mp_encode_uint(data, TNT_KEY);
    v[2].iov_base = (void *)body_start;
    v[2].iov_len  = data - body_start;
    body_start = data;
    v[3].iov_base = TNT_SBUF_DATA(key);
    v[3].iov_len  = TNT_SBUF_SIZE(key);
    data = mp_encode_uint(data, TNT_TUPLE);
    v[4].iov_base = (void *)body_start;
    v[4].iov_len  = data - body_start;
    body_start = data;
    v[5].iov_base = TNT_SBUF_DATA(ops);
    v[5].iov_len  = TNT_SBUF_SIZE(ops);

    size_t package_len = 0;
    for (int i = 1; i < v_sz; ++i)
        package_len += v[i].iov_len;
    char len_prefix[9];
    char *len_end = mp_encode_luint32(len_prefix, package_len);
    v[0].iov_base = len_prefix;
    v[0].iov_len = len_end - len_prefix;
    return s->writev(s, v, v_sz);
}

ssize_t
tnt_upsert(struct tnt_stream *s, uint32_t space,
       struct tnt_stream *tuple, struct tnt_stream *ops)
{
    if (tnt_object_verify(tuple, MP_ARRAY))
        return -1;
    if (tnt_object_verify(ops, MP_ARRAY))
        return -1;
    struct tnt_iheader hdr;
    struct iovec v[6]; int v_sz = 6;
    char *data = NULL, *body_start = NULL;
    encode_header(&hdr, TNT_OP_UPSERT, s->reqid++);
    v[1].iov_base = (void *)hdr.header;
    v[1].iov_len  = hdr.end - hdr.header;
    char body[64]; body_start = body; data = body;

    data = mp_encode_map(data, 4);
    data = mp_encode_uint(data, TNT_SPACE);
    data = mp_encode_uint(data, space);
    data = mp_encode_uint(data, TNT_TUPLE);
    v[2].iov_base = (void *)body_start;
    v[2].iov_len  = data - body_start;
    body_start = data;
    v[3].iov_base = TNT_SBUF_DATA(tuple);
    v[3].iov_len  = TNT_SBUF_SIZE(tuple);
    data = mp_encode_uint(data, TNT_OPS);
    v[4].iov_base = (void *)body_start;
    v[4].iov_len  = data - body_start;
    body_start = data;
    v[5].iov_base = TNT_SBUF_DATA(ops);
    v[5].iov_len  = TNT_SBUF_SIZE(ops);

    size_t package_len = 0;
    for (int i = 1; i < v_sz; ++i)
        package_len += v[i].iov_len;
    char len_prefix[9];
    char *len_end = mp_encode_luint32(len_prefix, package_len);
    v[0].iov_base = len_prefix;
    v[0].iov_len = len_end - len_prefix;
    return s->writev(s, v, v_sz);
}

static ssize_t tnt_update_op_len(char op) {
    switch (op) {
    case (TNT_UOP_ADDITION):
    case (TNT_UOP_SUBSTRACT):
    case (TNT_UOP_AND):
    case (TNT_UOP_XOR):
    case (TNT_UOP_OR):
    case (TNT_UOP_DELETE):
    case (TNT_UOP_INSERT):
    case (TNT_UOP_ASSIGN):
        return 3;
    case (TNT_UOP_SPLICE):
        return 5;
    default:
        return -1;
    }
}

struct tnt_stream *tnt_update_container(struct tnt_stream *ops) {
    ops = tnt_object(ops);
    if (!ops) return NULL;
    tnt_object_type(ops, TNT_SBO_SPARSE);
    if (tnt_object_add_array(ops, 0) == -1) {
        tnt_stream_free(ops);
        return NULL;
    }
    return ops;
}

int tnt_update_container_close(struct tnt_stream *ops) {
    struct tnt_sbuf_object *opob = TNT_SOBJ_CAST(ops);
    opob->stack->size = ops->wrcnt - 1;
    tnt_object_container_close(ops);
    return 0;
}

int tnt_update_container_reset(struct tnt_stream *ops) {
    tnt_object_reset(ops);
    tnt_object_type(ops, TNT_SBO_SPARSE);
    if (tnt_object_add_array(ops, 0) == -1) {
        tnt_stream_free(ops);
        return -1;
    }
    return 0;
}

static ssize_t
tnt_update_op(struct tnt_stream *ops, char op, uint32_t fieldno,
          const char *opdata, size_t opdata_len) {
    struct iovec v[2]; size_t v_sz = 2;
    char body[64], *data; data = body;
    data = mp_encode_array(data, tnt_update_op_len(op));
    data = mp_encode_str(data, &op, 1);
    data = mp_encode_uint(data, fieldno);
    v[0].iov_base = body;
    v[0].iov_len  = data - body;
    v[1].iov_base = (void *)opdata;
    v[1].iov_len  = opdata_len;

    return ops->writev(ops, v, v_sz);
}

ssize_t
tnt_update_bit(struct tnt_stream *ops, uint32_t fieldno, char op,
           uint64_t value) {
    if (op != '&' && op != '^' && op != '|') return -1;
    char body[10], *data; data = body;
    data = mp_encode_uint(data, value);
    return tnt_update_op(ops, op, fieldno, body, data - body);
}

ssize_t
tnt_update_arith_int(struct tnt_stream *ops, uint32_t fieldno, char op,
             int64_t value) {
    if (op != '+' && op != '-') return -1;
    char body[10], *data; data = body;
    if (value >= 0)
        data = mp_encode_uint(data, value);
    else
        data = mp_encode_int(data, value);
    return tnt_update_op(ops, op, fieldno, body, data - body);
}

ssize_t
tnt_update_arith_float(struct tnt_stream *ops, uint32_t fieldno, char op,
               float value) {
    if (op != '+' && op != '-') return -1;
    char body[10], *data; data = body;
    data = mp_encode_float(data, value);
    return tnt_update_op(ops, op, fieldno, body, data - body);
}

ssize_t
tnt_update_arith_double(struct tnt_stream *ops, uint32_t fieldno, char op,
                double value) {
    if (op != '+' && op != '-') return -1;
    char body[10], *data; data = body;
    data = mp_encode_double(data, value);
    return tnt_update_op(ops, op, fieldno, body, data - body);
}

ssize_t
tnt_update_delete(struct tnt_stream *ops, uint32_t fieldno,
          uint32_t fieldcount) {
    char body[10], *data; data = body;
    data = mp_encode_uint(data, fieldcount);
    return tnt_update_op(ops, '#', fieldno, body, data - body);
}

ssize_t
tnt_update_insert(struct tnt_stream *ops, uint32_t fieldno,
          struct tnt_stream *val) {
    if (tnt_object_verify(val, -1))
        return -1;
    return tnt_update_op(ops, '!', fieldno, TNT_SBUF_DATA(val),
                 TNT_SBUF_SIZE(val));
}

ssize_t
tnt_update_assign(struct tnt_stream *ops, uint32_t fieldno,
          struct tnt_stream *val) {
    if (tnt_object_verify(val, -1))
        return -1;
    return tnt_update_op(ops, '=', fieldno, TNT_SBUF_DATA(val),
                 TNT_SBUF_SIZE(val));
}

ssize_t
tnt_update_splice(struct tnt_stream *ops, uint32_t fieldno,
          uint32_t position, uint32_t offset,
          const char *buffer, size_t buffer_len) {
    size_t buf_size = mp_sizeof_uint(position) +
                  mp_sizeof_uint(offset) +
              mp_sizeof_str(buffer_len);
    char *buf = (char *)tnt_mem_alloc(buf_size), *data = NULL;
    if (!buf) return -1;
    data = buf;
    data = mp_encode_uint(data, position);
    data = mp_encode_uint(data, offset);
    data = mp_encode_str(data, buffer, buffer_len);
    ssize_t retval = tnt_update_op(ops, ':', fieldno, buf, buf_size);
    tnt_mem_free(buf);
    return retval;
}

#endif /* TNT_UPDATE_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_call.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef TNT_CALL_C_INCLUDED
#define TNT_CALL_C_INCLUDED
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

//#include <msgpuck.h>

//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_buf.h>
//#include <tarantool/tnt_object.h>
//#include <tarantool/tnt_call.h>

//#include "tnt_proto_internal.h"

static ssize_t
tnt_rpc_base(struct tnt_stream *s, const char *proc, size_t proc_len,
         struct tnt_stream *args, enum tnt_request_t op)
{
    if (!proc || proc_len == 0)
        return -1;
    if (tnt_object_verify(args, MP_ARRAY))
        return -1;
    uint32_t fld = (is_call(op) ? TNT_FUNCTION : TNT_EXPRESSION);
    struct tnt_iheader hdr;
    struct iovec v[6]; int v_sz = 6;
    char *data = NULL, *body_start = NULL;
    encode_header(&hdr, op, s->reqid++);
    v[1].iov_base = (void *)hdr.header;
    v[1].iov_len  = hdr.end - hdr.header;
    char body[64]; body_start = body; data = body;

    data = mp_encode_map(data, 2);
    data = mp_encode_uint(data, fld);
    data = mp_encode_strl(data, proc_len);
    v[2].iov_base = body_start;
    v[2].iov_len  = data - body_start;
    v[3].iov_base = (void *)proc;
    v[3].iov_len  = proc_len;
    body_start = data;
    data = mp_encode_uint(data, TNT_TUPLE);
    v[4].iov_base = body_start;
    v[4].iov_len  = data - body_start;
    v[5].iov_base = TNT_SBUF_DATA(args);
    v[5].iov_len  = TNT_SBUF_SIZE(args);

    size_t package_len = 0;
    for (int i = 1; i < v_sz; ++i)
        package_len += v[i].iov_len;
    char len_prefix[9];
    char *len_end = mp_encode_luint32(len_prefix, package_len);
    v[0].iov_base = len_prefix;
    v[0].iov_len = len_end - len_prefix;
    return s->writev(s, v, v_sz);
}

ssize_t
tnt_call(struct tnt_stream *s, const char *proc, size_t proc_len,
     struct tnt_stream *args)
{
    return tnt_rpc_base(s, proc, proc_len, args, TNT_OP_CALL);
}

ssize_t
tnt_call_16(struct tnt_stream *s, const char *proc, size_t proc_len,
        struct tnt_stream *args)
{
    return tnt_rpc_base(s, proc, proc_len, args, TNT_OP_CALL_16);
}

ssize_t
tnt_eval(struct tnt_stream *s, const char *proc, size_t proc_len,
     struct tnt_stream *args)
{
    return tnt_rpc_base(s, proc, proc_len, args, TNT_OP_EVAL);
}

#endif /* TNT_CALL_C_INCLUDED */

/*
   COPY: file name = tnt/tnt_select.c.
   Copying Date = 2017-09-14.
   Changes: added #ifndef...#endif. commented out #includes.
*/
#ifndef TNT_SELECT_C_INCLUDED
#define TNT_SELECT_C_INCLUDED
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

//#include <msgpuck.h>

//#include <tarantool/tnt_reply.h>
//#include <tarantool/tnt_stream.h>
//#include <tarantool/tnt_buf.h>
//#include <tarantool/tnt_object.h>
//#include <tarantool/tnt_select.h>

//#include "tnt_proto_internal.h"

ssize_t
tnt_select(struct tnt_stream *s, uint32_t space, uint32_t index,
       uint32_t limit, uint32_t offset, uint8_t iterator,
       struct tnt_stream *key)
{
    if (tnt_object_verify(key, MP_ARRAY))
        return -1;
    struct tnt_iheader hdr;
    struct iovec v[4]; int v_sz = 4;
    char *data = NULL;
    encode_header(&hdr, TNT_OP_SELECT, s->reqid++);
    v[1].iov_base = (void *)hdr.header;
    v[1].iov_len  = hdr.end - hdr.header;
    char body[64]; data = body;

    data = mp_encode_map(data, 6);
    data = mp_encode_uint(data, TNT_SPACE);
    data = mp_encode_uint(data, space);
    data = mp_encode_uint(data, TNT_INDEX);
    data = mp_encode_uint(data, index);
    data = mp_encode_uint(data, TNT_LIMIT);
    data = mp_encode_uint(data, limit);
    data = mp_encode_uint(data, TNT_OFFSET);
    data = mp_encode_uint(data, offset);
    data = mp_encode_uint(data, TNT_ITERATOR);
    data = mp_encode_uint(data, iterator);
    data = mp_encode_uint(data, TNT_KEY);
    v[2].iov_base = body;
    v[2].iov_len  = data - body;
    v[3].iov_base = TNT_SBUF_DATA(key);
    v[3].iov_len  = TNT_SBUF_SIZE(key);

    size_t package_len = 0;
    for (int i = 1; i < v_sz; ++i)
        package_len += v[i].iov_len;
    char len_prefix[9];
    char *len_end = mp_encode_luint32(len_prefix, package_len);
    v[0].iov_base = len_prefix;
    v[0].iov_len = len_end - len_prefix;
    return s->writev(s, v, v_sz);
}

#endif /* TNT_SELECT_C_INCLUDED */

#endif /* OCELOT_THIRD_PARTY_CODE */

#pragma GCC diagnostic pop


