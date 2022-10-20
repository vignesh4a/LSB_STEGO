/*
    Name : Vignesh Uday Hegde
    Date : 10/08/2022
    Project : LSB Image Steganography

    Sample Execution:
        Encode Operation
            ./a.out -e beautiful.bmp secret.txt 
            Selected Encode Operation
            Success : Read and Validate encode arguments succeeded
            Started Encoding.....
            Success : File Opened
            width = 1024
            height = 768
            Success : Check capacity success.
            Success : Headder copied
            Success : Magic String Encoded
            Success : encode extnsion
            Success : Encodede Secret file extn
            Success : Encoded secret file size
            Success : Encoded Secret file data
            Success : Copied remaining data
            Success : Encoding is successful.

        Decode Operation :
            ./a.out -d default.bmp data.py
            Selected Decode Operation
            Success : Read and valdate of decode arguments was success.
            Decoding Started .... 
            Success : Files Opened
            Success : validated Magic string
            Warning : Mismatch of extention overwriting to data.txt
            Success : Opened updated extention file

            Secret data : My password is SECRET ;)

            Success : Decodeing was successful

        Invalid Operation :
            ./a.out default.bmp 
            Error : Invalid number of CLA
            Please pass
            For Encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]
            For decoding : ./a.out -d stego.bmp [decode.txt]

*/



#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"


int main(int agrc, char **argv)
{
    // check if appropriate number of arguments are passed or not
    if( agrc < 3 || agrc > 5)
    {
        printf("Error : Invalid number of CLA\nPlease pass\nFor Encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]\nFor decoding : ./a.out -d stego.bmp [decode.txt]\n");
        return -1;
    }

    // check opention is encode or decode
    if( check_operation_type(argv) == e_encode )
    {
        // check required amount of args are passed for the encode operation
        if( agrc < 4 || agrc > 5)
        {
            printf("Error : Invalid number of CLA\nPlease pass\nFor Encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]\nFor decoding : ./a.out -d stego.bmp [decode.txt]\n");
            return -1;
        }

        printf("Selected Encode Operation\n");

        EncodeInfo encInfo;

        // read and vlidate the arguments passed
        if( read_and_validate_encode_args(argv, &encInfo) == e_success )
        {
            printf("Success : Read and Validate encode arguments succeeded\n");
            printf("Started Encoding.....\n");

            // start encoding
            if( do_encoding(&encInfo) == e_success )
            {   
                printf("Success : Encoding is successful.\n");

            }
            else
            {   
                printf("Error : Encoding Failed.\n");
            }
        }
        // return -1 incase of error
        else
        {
            printf("Error : Read and Validate encode argument failed!\n");
            return -1;
        }
    }

    // if openrtion was decode
    else if( check_operation_type(argv) == e_decode )
    {
        printf("Selected Decode Operation\n");

        // chec if required amount of args are passed 
        if( agrc < 3 || agrc > 4)
        {
            printf("Error : Invalid number of CLA\nPlease pass\nFor Encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]\nFor decoding : ./a.out -d stego.bmp [decode.txt]\n");
            return -1;
        }

        DecodeInfo decInfo;

        // read and validate arguments passed
        if( read_and_validate_decode_args(argv, &decInfo) == e_success )
        {
            printf("Success : Read and valdate of decode arguments was success.\n");
            printf("Decoding Started .... \n");

            // start decoing 
            if( do_decode( &decInfo) == e_success )
            {
                printf("Success : Decodeing was successful\n");
            }
            else
            {
                printf("Error : Decoing was not successful\n");
                return-1;
            }
        }
        else
        {
            printf("Error : Error while reading and validating\n");
            return -1;
        }
        
    }
    else
    {
        

        printf("Error : Invlid Option\nPlease pass\nFor Encoding ./a.out -e beautiful.bmp secret.txt [stego.bmp]\nFor decoding: ./a.out -d stego.bmp [decode.txt]\n");
        return -1;
    }

    // return 0 incase of success, if error encountered return -1 using appropriate else block
    return 0;

}

// function to check openration type
OperationType check_operation_type(char *argv[])
{
    // if 1st arg is -e the return encode opeartion , if -d return decode 
    if( strcmp("-e",argv[1]) == 0 )
    {
        return e_encode;
    }
    else if( strcmp("-d",argv[1]) == 0 )
    {
        return e_decode;
    }
    
    // return unssported if invalid type is passed
    return e_unsupported;
}

