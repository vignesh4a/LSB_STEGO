#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>
#include "common.h"


/* Function Definitions */


// function to open files
Status open_files_decode(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }

    // Secret file
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    // Do Error handling
    if (decInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);

    	return e_failure;
    }

    
    // No failure return e_success
    return e_success;
}

// function to change file pointer to new file
Status open_updated_file( DecodeInfo *decInfo)
{

    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    // Do Error handling
    
    if (decInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

// function to read and validate arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{   
    // store name of sorce image
    if( strcmp(strchr(argv[2],'.'),".bmp" ) == 0 )
    {
        decInfo -> stego_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }
    // store secrest file , if no arg is passed use default file name
    if( argv[3] == NULL )
    {
        decInfo -> secret_fname = "decoded_secret_file.txt";
        strcpy(decInfo -> extn_secret_file,".txt");
    }
    else
    {
        strcpy(decInfo -> extn_secret_file, strchr(argv[3], '.'));
        decInfo -> secret_fname = argv[3];
    }

    // return success
    return e_success;    
}

// function to change extention
Status change_extn( char *fname, char *ext )
{
    // get name of file
    fname[strchr(fname,'.')-fname] = '\0';
    // concat eextention to file name
    strcat(fname,ext);

    // return success
    return e_success;
}

// function to validate magic string
Status validate_magic_string( FILE *stego_file )
{
    int len = strlen( MAGIC_STRING );
    char decode_magic_string[len+1];
    int i;
    
    // set fptr to 54th location
    fseek( stego_file, 54, SEEK_SET);

    // loop till the length of magic string
    for( i = 0 ; i < len ; i++ )
    {
        // get decoded cahracter
        decode_magic_string[i] = decode_lsb_char(stego_file);
    }
    decode_magic_string[i] = '\0';

    // compare magic string and decoded string return success if tey are same else return failure
    if( strcmp(MAGIC_STRING, decode_magic_string) == 0 )
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

// function to calculate power (used by bin_to_dec funtcion)
int power(int base, int power)
{
    int ans = 1;

    while( power-- )
        ans *= base;

    return ans;
}

// function convert binary to decimal
char bin_to_dec(char bits[])
{
    int i = 7;
    char data = 0;

    // loop till 8 bits
    for( i = 7; i >= 0 ; i-- )
    {
        // convert bin to dec
        data += power(2, i)*bits[7-i];
    }

    // return decimal data as character
    return data;
}

// function to decode lab as int
int decode_lsb_int(FILE *file)
{
    char temp[9];
    char *data = (char *)calloc(sizeof(int),sizeof(char));
    int i,j;

    // loop till size of int
    for( i = sizeof(int)-1 ; i >=  0 ; i-- )
    {
        // loop through 8 bits 
        for( j = 0 ; j < 8 ; j++ )
        {
            // get character and get last bit
            temp[j] = fgetc(file) & 1;
        } 
        temp[j] = '\0';

        // convert binary data to decimal
        data[i] = bin_to_dec( temp );
    }

    // convert succesive location of char as int and return data 
    return *(int *)data;
}

// function to decode lsb as char
char decode_lsb_char(FILE *file)
{
    char temp[9];
    int i;

    // loop through 8 bits
    for( i = 0 ; i < 8 ; i++ )
    {
        // get lsb of the ecah char
        temp[i] = fgetc(file) & 1;
    } 

    temp[i] = '\0';

    // return decimal form of binary data temp

    return bin_to_dec( temp );
}

// function to do deoding
Status do_decode( DecodeInfo *decInfo )
{
    // open the file
    if( open_files_decode( decInfo ) == e_success )
    {
        printf("Success : Files Opened\n");
        
        // validate the magic string if not equal display error and return failure
        if( validate_magic_string( decInfo -> fptr_stego_image) == e_success )
        {
            printf("Success : validated Magic string\n");
            int i,len = decode_lsb_int(decInfo -> fptr_stego_image);
            char temp_extn[len];

            // copy the extendion 
            strcpy(temp_extn,decInfo -> extn_secret_file); 

            // get the extenion encoded in stego file
            for(i = 0; i < len ; i++ )
            {
                decInfo -> extn_secret_file[i] = decode_lsb_char( decInfo -> fptr_stego_image);
            }

            // if extention missmatch set encoded extenion as new extention
            if( strcmp(temp_extn,decInfo->extn_secret_file) != 0 )
            {   
                // change exteion
                if( change_extn(decInfo -> secret_fname, decInfo -> extn_secret_file) == e_success )
                {
                    printf("Warning : Mismatch of extention overwriting to %s\n",decInfo -> secret_fname);
                    // open the new changed exteion file
                    if( open_updated_file(decInfo) == e_success )
                    {
                        printf("Success : Opened updated extention file\n");
                    }
                    else
                    {
                        printf("Error : unable to open updated file\n");
                    }
                }
                else
                {
                    printf("Error : unable to overwrite extention\n");
                    return e_failure;
                }
            }
            // get decoded size of secret file
            decInfo -> size_secret_file = decode_lsb_int(decInfo->fptr_stego_image);
            
            char secret_data[ decInfo -> size_secret_file + 1 ];

            // loop till the size of secret file
            for(i = 0 ; i < decInfo->size_secret_file ; i++ )
            {
                // store the decoded data
                secret_data[i] = decode_lsb_char(decInfo->fptr_stego_image);
                // put ecah decode char into new output file
                fputc( secret_data[i], decInfo->fptr_secret );
            }

            secret_data[i] = '\0';
            
            // display the secret data decoded
            printf("\nSecret data : %s\n",secret_data);
            
            // close the opened files
            fclose(decInfo -> fptr_secret);
            fclose(decInfo -> fptr_stego_image);
        }
        else
        {
            printf("Error : Magic string do not match\n");
            return e_failure;
        }
    }
    else
    {
        printf("Error : Unable to open Files\n");
        return e_failure;
    }
    
    // return success if allopenstion executed else return failure with approprite error message using else block
    return e_success;
}
