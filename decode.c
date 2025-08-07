#include<stdio.h>
#include<string.h>
#include "common.h"
#include "types.h"
#include<stdlib.h>
#include "decode.h"



Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->stego_image = fopen(decInfo->stego_image_fname,"r");

    if(decInfo->stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr,"Unable to open the file %s\n", decInfo->stego_image_fname);

        return e_failure;
    }
    return e_success;
}

OperationType check_operation(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1],"-d") == 0)
    {
        return e_decode;
    }
    else
    return e_unsupported;
}

Status read_and_validate_decode_args( int argc, char*argv[],DecodeInfo *decInfo)
{

    if(argc > 2)
    {
        if(strcmp(strstr(argv[2],"."),".bmp") == 0)
        {
            decInfo->stego_image_fname=argv[2];
        }
        else
        {
            printf("Enter a bmp file\n");
            return e_failure;
        }
    }
    else
    {
        printf("Not enough arguments\n");
        return e_failure;
    }
    if(argc > 3)
    {        
        decInfo->new_secret_fname = argv[3];
    }
    else
    {
        printf("not enough arguments\n");
        return e_failure;
    }
    if(argc>4)
    {
        decInfo->passcode = argv[4];
        decInfo->passcode_size = strlen(decInfo->passcode);
    }
    else
    {
        printf("Passcode argument is missing\n");
        return e_failure;
    }
    return e_success;

}

Status do_decoding(DecodeInfo *decInfo)
{
    if(open_decode_files(decInfo) == e_success)
    {
        printf("Decoding started\n");

        fseek(decInfo->stego_image,54,SEEK_SET);
        printf("stego image is at %ld\n", ftell(decInfo->stego_image));

        if(decode_magic_string(decInfo->passcode, decInfo) == e_success)
        {
            printf("Decoding magic string is done\n");

            if(decode_secret_file_extn_size(decInfo)== e_success)
            {
                printf("Decoding of secret file extn size is done\n");

                if(decode_secret_file_extn(decInfo) == e_success)
                {
                    printf("Decoding the secret file extn was successful\n");

                    if(open_new_file(decInfo) == e_success)
                    {
                        printf("new file opened\n");
                        
                        if(decode_secret_file_size(decInfo) == e_success)
                        {
                            printf("size of data of secret file is decoded\n");

                            if(decode_the_secret_data(decInfo) == e_success)
                            {
                                printf("Secret data is decoded\n");
                            }
                            else
                            {
                                printf("Secret data not decoded\n");
                            }
                        }
                        else
                        {
                            printf("Size of data in secret file is not decoded\n ");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("new file not opened\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Decoding the extn of secret file is failed\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Decoding of extn of secret file size is failed\n");
                return e_failure;
            }
        }
        else
        {
            printf("Decoding magic string was not successful\n");
            return e_failure;
        }
        return e_success;
    }
    else
    {
        printf("Error file wasn't opened\n");
        return e_failure;
    }
}

Status decode_magic_string(char * magic_string, DecodeInfo *decInfo)
{
    char magic_data[8];
    char decoded_data = 0;
    char arr[decInfo->passcode_size];

    for(uint i=0; i<decInfo->passcode_size;i++)
    {
        fread(magic_data, sizeof(char), 8, decInfo->stego_image);

        decode_lsb_to_byte(magic_data,&decoded_data);
       
        arr[i] = decoded_data;

    }
    printf("magic character is %s\n",arr);
    
    int a=ftell(decInfo->stego_image); 
    
    printf("Magic string stego image is at %d\n", a);
    
    if(strcmp(arr, decInfo->passcode) == 0)
    {

        printf("Passcode is correct\n");
        return e_success;
    }
    else
    {
        printf("Passcode is wrong\n");
        return e_failure;
    }

}
Status decode_lsb_to_byte(char* image_data,char *storage)
{
    uint i;
    *storage = 0;
    for(i=0;i<8;i++)
    {
        int mask= image_data[i] & 1;

        *storage=*storage | (mask<<(7-i));

    }
    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    int size_value=0;
    char magic_data[32];

    fread(magic_data,sizeof(char),32,decInfo->stego_image);

    int a=ftell(decInfo->stego_image);
    
    printf("After decoding Extn size of stego image is at %d\n", a);

    for(int i=0;i<32;i++)
    {
        int mask= magic_data[i] & 1;

        size_value=size_value | (mask<<(31-i));
    }
    decInfo->size_extn_file= size_value;

    printf("The size of the secret file extn is %d\n", decInfo->size_extn_file);

    
    return e_success;

}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char image_data[8];
    char decode_data = 0;
    char arr[20];

    for(uint i=0; i<decInfo->size_extn_file;i++)
    {
        fread(image_data, sizeof(char), 8, decInfo->stego_image);

        decode_lsb_to_byte(image_data,&decode_data);

        arr[i] = decode_data;
    }
    printf("extension is %s", arr);
    strcpy(decInfo->extn_secret_file,arr);
    printf("Extension of the  file is copied %s\n",decInfo->extn_secret_file);

    strcat(decInfo->new_secret_fname,decInfo->extn_secret_file);
    // decInfo->extn_secret_file = arr;
    int a=ftell(decInfo->stego_image); 
    printf("secret file extn of stego image is at %d\n", a);
    return e_success;
}

Status open_new_file(DecodeInfo *decInfo)
{
    decInfo->new_fptr_secret = fopen(decInfo->new_secret_fname,"w+");

    if(decInfo->new_fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Unable to open the file %s\n", decInfo->new_secret_fname);

        return e_failure;
    }
    return e_success;

}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    int size_value=0;
    char magic_data[32];

    fread(magic_data,sizeof(char),32,decInfo->stego_image);

    int a=ftell(decInfo->stego_image);
    
    printf("After decoding Extn size of stego image is at %d\n", a);

    for(int i=0;i<32;i++)
    {
        int mask= magic_data[i] & 1;

        size_value=size_value | (mask<<(31-i));
    }
    decInfo->size_secret_file= size_value;

    printf("The size of the secret file extn is %d\n", decInfo->size_secret_file);

    
    return e_success;
}
Status decode_the_secret_data(DecodeInfo *decInfo)
{    

    for(uint i=0; i<decInfo->size_secret_file;i++)
    {
        fread(decInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, decInfo->stego_image);

        decode_lsb_to_byte(decInfo->image_data,decInfo->decode_data);

        fprintf(decInfo->new_fptr_secret, "%s" , decInfo->decode_data);
    }
    int a=ftell(decInfo->stego_image); 
    printf("secret file extn of stego image is at %d\n", a);
    return e_success;
}