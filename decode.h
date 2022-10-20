#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h" // Contains user defined types

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    
    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} DecodeInfo;


/* read and velidate the arguments passed */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* open the files to decode */
Status open_files_decode(DecodeInfo *decInfo);

/* decode char from lsb */
char decode_lsb_char(FILE *file);

/* decode interger data from lsb */
int decode_lsb_int(FILE *file);

/* open new extention file */
Status open_updated_file( DecodeInfo *decInfo);

/* validate magic string */
Status validate_magic_string( FILE *stego_file );

/* do decoing process */
Status do_decode( DecodeInfo *decInfo );


#endif
