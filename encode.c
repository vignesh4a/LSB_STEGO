#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */


uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

// function to validate and store names of file
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // store name of sorce image
    if( strcmp(strchr(argv[2],'.'),".bmp" ) == 0 )
    {
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    // store secret file name
    if( strcmp(strchr(argv[3],'.'),".txt" ) == 0 )
    {
        encInfo -> secret_fname = argv[3];
    }
    else
    {
        return e_failure;
    }

    // store strgo image name
    if( argv[4] == NULL )
    {
        encInfo -> stego_image_fname = "default.bmp";
    }
    else
    {
        encInfo -> stego_image_fname = argv[4];
    }

    // return success
    return e_success;
}


// function to copy headder of bmps
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char str[54];
    fseek(fptr_src_image, 0, SEEK_SET);
    fread(str, 54, 1, fptr_src_image);
    fwrite(str, 54, 1, fptr_dest_image);
    return e_success;
}

// function to encode data to lsb
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // loop through a byte
    for( int i = 0; i < 8; i++)
    {
        // using mask encode bit in lab of byte
        image_buffer[i] = ( image_buffer[i] & 0XFE )| (data >> (7 - i)) & 1;
    }
    // return success
    return e_success;
}


// encode data to the image data
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{   
    // loop till the size
    for(int i = 0; i < size; i++)
    {
        /*
            read data 
            encode the byte
            write to file
        */
        fread(encInfo -> image_data, 8, 1, encInfo -> fptr_src_image);
        encode_byte_to_lsb(data[i], encInfo -> image_data);
        fwrite(encInfo -> image_data, 8, 1, encInfo -> fptr_stego_image);
    }

    // return success
    return e_success;
}

// encode secret file extention
Status encode_secret_file_extn( char *file_extn, EncodeInfo *encInfo)
{
    // encode extention to image
    encode_data_to_image(file_extn, strlen(file_extn), encInfo);
    
    // retuen success
    return e_success;
}

// function to encode magic string
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    // encode the data
    encode_data_to_image(MAGIC_STRING, strlen(MAGIC_STRING), encInfo);
    
    // return success 
    return e_success;
}

// functon encode extenstion size
Status encode_extn_size(int size, EncodeInfo *encInfo)
{
    // do encodeing 
    encode_size_to_lsb(size, encInfo);
    // return success
    return e_success;
}

// function to encode size
Status encode_size_to_lsb(int size, EncodeInfo *encInfo)
{
    char str[32];
    
    // read data frm sorce image
    fread(str, 32, 1, encInfo -> fptr_src_image );
    
    // loop through int data
    for(int i  = 0 ; i < 32 ; i++ )
    {
        // encode lbs
        str[i] = (str[i] & 0xFE) | ((size >> (31 -i)) & 1); 
    }
    
    // write to stego file
    fwrite(str, 32, 1, encInfo-> fptr_stego_image);
}

// function to encode secret file size
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(file_size, encInfo);
    return e_success;
}

// function to encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // set to first char
    fseek(encInfo -> fptr_secret, 0, SEEK_SET);
    // declare character array to store data
    char str[encInfo -> size_secret_file];
    // read data from secret file
    fread(str, encInfo -> size_secret_file, 1, encInfo -> fptr_secret);
    // encode data to stego image
    encode_data_to_image(str, encInfo -> size_secret_file, encInfo);
    
    // return success
    return e_success;
}

// function to copy rest of the data as it is
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    // loop till end of file
    while( fread(&ch, 1, 1, fptr_src) > 0 )
    {
        // write data from sorce image to stego image
        fwrite(&ch, 1, 1, fptr_dest);
    }

    // return success
    return e_success;
}


// function to do encoding of data to image
Status do_encoding(EncodeInfo *encInfo)
{
    // open image 
    if( open_files(encInfo) == e_success )
    {
        printf("Success : File Opened\n");
        
        // check fif image capacity is large enough
        if( check_capacity( encInfo ) == e_success )
        {
            printf("Success : Check capacity success.\n");
            // copy the header of bmp
            if( copy_bmp_header( encInfo -> fptr_src_image, encInfo -> fptr_stego_image ) == e_success )
            {
                printf("Success : Headder copied\n");
                // encode magic string to image
                if (encode_magic_string(MAGIC_STRING, encInfo) == e_success )
                {
                    printf("Success : Magic String Encoded\n");
                    // copy extention 
                    strcpy(encInfo -> extn_secret_file,strstr(encInfo -> secret_fname, "."));
                    // encode the extention size
                    if( encode_extn_size(strlen(encInfo -> extn_secret_file), encInfo) == e_success )
                    {
                        printf("Success : encode extnsion\n");
                        // encoe the secret file extention
                        if( encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
                        {
                            printf("Success : Encodede Secret file extn\n");
                            // encode secret file extn
                            if( encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success )
                            {
                                printf("Success : Encoded secret file size\n");
                                // encode secret file data to stego image
                                if( encode_secret_file_data(encInfo) == e_success )
                                {
                                    printf("Success : Encoded Secret file data\n");
                                    // copy remaining data from sorce to stego
                                    if( copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
                                    {
                                        printf("Success : Copied remaining data\n");
                                    }
                                    else
                                    {
                                        printf("Error : Failed to copy remaining data\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Error : failed to encode secret file data\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Error : Failed to encode secret file size\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Error : Failed to encode secret file extention");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Error : encode extension\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Failed to encode magic string\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Error : unable to copy header\n");
                return e_failure;
            }
        }
        else
        {
            printf("Error : Check capacity failed.\n");
            return e_failure;
        }
        // return success if all task were done else return failure using appropriate else statement
        return e_success;
    }
    else
    {
        printf("Error : Unable to Open File.\n");
    }

    return e_failure;
}

// function to get file size
uint get_file_size(FILE *fptr)
{
    
    fseek(fptr, 0, SEEK_END);
    
    // return file size
    return ( unsigned int )ftell(fptr);
}

// function to check if data can be encoded into image
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp( encInfo -> fptr_src_image );
    encInfo -> size_secret_file = get_file_size( encInfo -> fptr_secret );

    // return success if capacity is large enough else return failure
    if( encInfo -> image_capacity > ( 16 + 32 + 32 + 32 + ( 8 * ( encInfo -> size_secret_file ) ) ) )
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }

}