#ifndef DECODE_H
#define DECODE_H

#include<stdio.h>
#include<string.h>
#include "common.h"
#include<stdlib.h>
#include "decode.h"
#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * dencoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego image details */
    char *stego_image_fname;
    FILE *stego_image;
    uint image_capacity;
    uint bits_per_pixel;
    char decode_data[MAX_IMAGE_BUF_SIZE];
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *new_secret_fname;
    FILE *new_fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    int size_extn_file;
    char secret_data[MAX_SECRET_BUF_SIZE];
    int size_secret_file;

    /* Stego Image Info */
    char *passcode;
    uint passcode_size;

} DecodeInfo;


OperationType check_operation(char *argv[]);

Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);

Status open_decode_files(DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status decode_magic_string(char * magic_string, DecodeInfo *decInfo);

Status decode_lsb_to_byte(char* image_data,char *storage);

Status decode_secret_file_extn_size(DecodeInfo *decInfo);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status open_new_file(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_the_secret_data(DecodeInfo *decInfo);



#endif