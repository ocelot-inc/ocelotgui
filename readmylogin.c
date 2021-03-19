/*
readmylogin.c Decrypt and display a MySQL .mylogin.cnf file.

Uses openSSL libcrypto.so library. Does not use a MySQL library.

Copyright (c) 2021 by Peter Gulutzan.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the  nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL  BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  
  To compile and link and run with Linux and gcc:
  1. Install openSSL
  2. If installation puts libcrypto.so in an unusual directory, say
     export LD_LIBRARY_PATH=/unusual-directory
  3. gcc -o readmylogin readmylogin.c -lcrypto
  
  To run, it's compulsory to specify where the file is, for example:
  ./readmylogin .mylogin.cnf

  MySQL may change file formats without notice, but the following is
  true for files produced by mysql_config_editor with MySQL 5.6:
  * First four bytes are unused, probably reserved for version number
  * Next twenty bytes are the basis of the key, to be XORed in a loop
    until a sixteen-byte key is produced.
  * The rest of the file is, repeated as necessary:
      four bytes = length of following cipher chunk, little-endian
      n bytes = cipher chunk
  * Encryption is AES 128-bit ecb.
  * Chunk lengths are always a multiple of 16 bytes (128 bits).
    Therefore there may be padding. We assume that any trailing byte
    containing a value less than '\n' is a padding byte.    

  To make the code easy to understand, all error handling code is
  reduced to "return -1;" and buffers are fixed-size.
  To make the code easy to build, the line
  #include "/usr/include/openssl/aes.h"
  is commented out, but can be uncommented if aes.h is available.
  
  This is version 1, May 21 2015.
  More up-to-date versions of this program may be available
  within the ocelotgui project https://github.com/ocelot-inc/ocelotgui
*/

#include <stdio.h>
#include <fcntl.h>
//#include "/usr/include/openssl/aes.h"

#ifndef HEADER_AES_H
#define AES_BLOCK_SIZE 16
typedef struct aes_key_st { unsigned char x[244]; } AES_KEY;
#endif

unsigned char cipher_chunk[4096], output_buffer[65536];
int fd, cipher_chunk_length, output_length= 0, i;
char key_in_file[20];
char key_after_xor[AES_BLOCK_SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
AES_KEY key_for_aes;

int main(int argc, char *argv[])
{
  if (argc < 1) return -1;
  if ((fd= open(argv[1], O_RDONLY)) == -1) return -1;
  if (lseek(fd, 4, SEEK_SET) == -1) return -1;
  if (read(fd, key_in_file, 20) != 20) return -1;
  for (i= 0; i < 20; ++i) *(key_after_xor + (i%16))^= *(key_in_file + i);
  AES_set_decrypt_key(key_after_xor, 128, &key_for_aes);
  while (read(fd, &cipher_chunk_length, 4) == 4)
  {
    if (cipher_chunk_length > sizeof(cipher_chunk)) return -1;
    if (read(fd, cipher_chunk, cipher_chunk_length) != cipher_chunk_length) return -1;
    for (i= 0; i < cipher_chunk_length; i+= AES_BLOCK_SIZE)
    {
      AES_decrypt(cipher_chunk+i, output_buffer+output_length, &key_for_aes);
      output_length+= AES_BLOCK_SIZE;
      while (*(output_buffer+(output_length-1)) < '\n') --output_length;
    }
  }
  *(output_buffer + output_length)= '\0';
  printf("%s.\n", output_buffer);
}
