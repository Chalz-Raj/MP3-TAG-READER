#include <stdio.h>
#include "encode.h"
#include<string.h>
#include "types.h"
#include<stdlib.h>
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

    int image_size = width * height *3;

    printf(" Image size is %d\n", image_size);

    // Return image capacity
    return width * height * 3; // imagwe capacity = 2000000000



    // uint image_size;

    // fseek(fptr_image,0L,SEEK_END);

    // image_size= ftell(fptr_image);
    // rewind(fptr_image);
    // return image_size;

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
    else{
        printf("file opened\n");
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w+");
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
OperationType check_operation_type(char *argv[])
{
        if((strcmp(argv[1],"-e"))==0)
        {
            return e_encode;
        }
        else if((strcmp(argv[1],"-d"))==0)
        {
            return e_decode;
        }
        else
        return e_unsupported;
}
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
    if(argc>2) // checking if there are more than 2 arguments 
    {
        if(strcmp(strstr(argv[2],"."),".bmp")==0) //checking and comparing 2nd argument has .bmp extension
        {
            encInfo->src_image_fname=argv[2]; // storing the 2nd argument into source image name
        }
        else
        return e_failure;
    }
    else
    {
        printf("Arguments not passed\n");
        return e_failure;
    }
    if(argc>3) // checking if there are more than 3 arguments
    {
        strcpy(encInfo->extn_secret_file, strstr(argv[3],".")); // checking 3rd argument for .txt extn and copying it in the extn_secret file
        
        // if((strcmp(encInfo->extn_secret_file,".txt") == 0) || (strcmp(encInfo->extn_secret_file,".c") == 0) || (strcmp(encInfo->extn_secret_file,".") == 0)) // comparing it with .txt
        // {
            encInfo->secret_fname=argv[3];  // storing the 3rd argument in secret fname
    }
    else
    {
        printf("not enough arguments\n");
        return e_failure;
    }

        if(argv[4]==NULL)  // Check if 4th argument is empty 
        {
            strcpy(encInfo->stego_image_fname,"stego.bmp"); // If its empty store stegobmp into stego_image_name
        }
        else
        {
            if(strcmp(strstr(argv[4],"."),".bmp")==0)  // checking and comparing .bmp extension
            {
                strcpy(encInfo->stego_image_fname,argv[4]); // storing it in stego_fname
            }
            else
            return e_failure;
        }
        return e_success;

}

Status do_encoding(EncodeInfo *encInfo)
{

    if(open_files(encInfo) == e_success)
    {
        printf("Encoding started\n");

        printf("Checking for %s size\n", encInfo->secret_fname);
        encInfo->size_secret_file = (long)get_file_size(encInfo->fptr_secret);

        printf("secret size %ld\n", encInfo->size_secret_file);
        
        if(encInfo->size_secret_file)
        {
            printf("Done\n"); 

            printf("Checking for %s capacity to handle %s\n", encInfo->src_image_fname,encInfo->secret_fname);

            encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);

            if(check_capacity(encInfo) == e_success)
            {
                printf("Done\n");

                printf("Copying Image Header\n");

                if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
                {
                    printf("Done\n");

                    printf("Encoding Magic String\n");

                    if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                    {
                        printf("Done\n");

                        printf("Encoding %s secret file extn size\n", encInfo->secret_fname);

                        if(encode_secret_file_extn_size(encInfo) == e_success)
                        {
                            printf("Done\n");

                            printf("Encoding %s secrte file extn\n",encInfo->extn_secret_file);
                            
                            if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)
                            {
                                printf("Done\n");

                                printf("Encoding %s secret file size\n", encInfo->secret_fname);
                                
                                if(encode_secret_file_size(encInfo) == e_success)
                                {
                                    printf("Done\n");

                                    printf("Encoding %s secret file data\n", encInfo->secret_fname);
                                    if(encode_secret_file_data(encInfo) == e_success)
                                    {
                                        printf("Done\n");

                                        printf("Copy remaining data\n");

                                        if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                        {
                                            printf("Done\n");
                                        }
                                        else
                                        {
                                            printf("Error while copyinh remaining data\n");
                                            return e_failure;
                                        }
                                    }
                                    else
                                    {
                                        printf("Error while encoding secret data\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Error while encoding the size of secret data\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Error while encoding secret file extn\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Error while encoding size of secret file extrn\n");
                            return e_failure;

                        }
                    }
                    else
                    {
                        printf("error while encoding Magic String\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Errror while copying header file\n");
                    return  e_failure;
                }
            }
            else
            {
                printf("error while cheking capacity\n");
                return e_failure;
            }
        }
        else
        {
            printf("Secret file is empty\n");
            return e_failure;
        }
    }
    else
    {
        printf("Files not opened\n");
        return e_failure;
    }
    return e_success;
}
uint get_file_size(FILE *fptr) // To get the secret file size, basically checking how many characters are there in seret file
{
    uint file_size;
    
    fseek(fptr, 0, SEEK_END); // set the offset to end
    
    file_size=(uint)ftell(fptr); // this will tell how many characters are there
    
    rewind(fptr); // after checking set the offset to beginning
    
    return file_size; 
}

Status check_capacity(EncodeInfo *encInfo)
{
    
    int ext_size= sizeof(encInfo->extn_secret_file); // checking for size of extn of secret file
    
    int bytes_for_secret_file=4;
    
    printf("Extn_size is %d\n",ext_size); 
    
    long sum=54+(MAX_IMAGE_BUF_SIZE*(2+ MAX_FILE_SUFFIX + ext_size + bytes_for_secret_file + encInfo->size_secret_file)); // this will give the amount of adata we are goin to store
    
    printf("The total size required is %ld\n",sum);

    if(encInfo->image_capacity>sum) // hecking if the image capacity is more than the secret data encoding
    {
        printf("Image size is more than required\n");
        return e_success;
    }
    else
    {
        printf("Image size is less than required\n");
        return e_failure;
    }
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    fseek(fptr_src_image,0,SEEK_SET); // setting the source image offset to 0
    char header[200];
    int read=fread(header,sizeof(char),54,fptr_src_image); // copy the header data of source image to buffer
    
    int a=ftell(fptr_src_image); // this will tell where the offset of source image is
    
    printf("src image is at %d\n",a);
    if(read!=54)
    {
        printf("Read is less than 54\n");
        return e_failure;
    }
    int write=fwrite(header,sizeof(char),54,fptr_dest_image); // copy the headert data from buffer to destination image

    int b=ftell(fptr_dest_image);

    printf("dest image is at %d\n",b); // This will tell where the offet of the destination image is at

    if(write!=54)
    {
        printf("Write is less than 54\n");
        return e_failure;
    }
    return e_success;

}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int len = strlen(magic_string); // calculate the length of magic string
    char image_data[100];
    for(uint i=0;i<len;i++)
    {
        fread(image_data,sizeof(char),8, encInfo->fptr_src_image); // copy the data of source image to buffer
        
        encode_byte_to_lsb(magic_string[i],image_data); // Encode the image data with the data of magic string 

        // printf("encoded data is %d", image_data);
        
        fwrite(image_data,sizeof(char),MAX_IMAGE_BUF_SIZE,encInfo->fptr_stego_image); // Copy encoded image data to destination image


        
    }
    // check for offset if both source and desination are at same place
    int a=ftell(encInfo->fptr_src_image); 
    
    printf("Magic string src image is at %d\n", a);
    
    int b=ftell(encInfo->fptr_stego_image);
    
    printf("Magic string dest image is at %d\n", b);
    
    return e_success;

}
Status encode_byte_to_lsb(char data, char *image_buffer)
{

    for(int i=0;i<8;i++) // run the loop for 8 times
    {
        int bit = ((data >> (7-i)) & 1); // since only single characters come inside funtion, 1 character is of 1 byte
        // shift the data 1 bit at a time for 8 iterations and AND with 1, to get bit location
        // printf(" bit position during secret file extn encoding is %d\n",bit);
        if (bit == 1)
        {
            image_buffer[i] = image_buffer[i] | 1; // change the image data accordingly with the bit location
            // printf(" image buffer position during secret file extn encoding is %d\n",image_buffer[i]);
        }
        else
        {
            image_buffer[i] = image_buffer[i] & ~1;
            // printf(" image buffer during secret file extn encoding is %d\n",image_buffer[i]);
        }
    }

    return e_success;
}
// Status encode_size_to_lsb(int s, char *image_buffer)
// {

//     for(int i=0;i<32;i++)
//     {
//         int bit = ((s >> (31-i)) & 1);

//         if (bit == 1)
//         {
//             image_buffer[i] = image_buffer[i] | 1;
//         }
//         else
//         {
//             image_buffer[i] = image_buffer[i] & ~1;
//         }
//     }
//     return e_success;
    
// }
Status encode_secret_file_extn_size(EncodeInfo *encInfo)
{

    char char_data[32];

    fread(char_data,sizeof(char),32,encInfo->fptr_src_image); // copy 32 bytes of image data because we are encoding 32 bits
    
    int a=ftell(encInfo->fptr_src_image);
    
    printf("Extn size of src image is at %d\n", a);

    for(int i=0;i<32;i++) // Run for 32 times
    {
        int bit = ((sizeof(encInfo->extn_secret_file) >> (31-i)) & 1); // shift it for 32 times
    

        if (bit == 1)
        {
            char_data[i] = char_data[i] | 1;
        }
        else
        {
            char_data[i] = char_data[i] & ~1;
        }
    }

    fwrite(char_data,sizeof(char),32,encInfo->fptr_stego_image);
    
    int b=ftell(encInfo->fptr_stego_image);
        
    printf("Extn size of dest image is at %d\n", b);

    // int c = ftell(encInfo->fptr_secret);


    return e_success;


}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char xtn_data[8];
    int len= strlen(file_extn);
    printf("length is%d\n", len);

    for(uint i=0;i<len;i++)
    {
        fread(xtn_data,sizeof(char),8, encInfo->fptr_src_image);

        // printf("extn data is %X\n",xtn_data)

        encode_byte_to_lsb(encInfo->extn_secret_file[i],xtn_data);


        fwrite(xtn_data,sizeof(char),MAX_IMAGE_BUF_SIZE,encInfo->fptr_stego_image);

    }
    int a=ftell(encInfo->fptr_src_image);
    
    printf("Secret file extn of src image is at %d\n", a);

    int b=ftell(encInfo->fptr_stego_image);
        
    printf("Secret file extn of dest image is at %d\n", b);


    return e_success;
}
Status encode_secret_file_size(EncodeInfo *encInfo)
{

    char s_data[32];
    
    fread(s_data,1,32,encInfo->fptr_src_image);

    for(int i=0;i<32;i++)
    {
        int bit = ((encInfo->size_secret_file >> (31-i)) & 1);

        if (bit == 1)
        {
            s_data[i] = s_data[i] | 1;
        }
        else
        {
            s_data[i] = s_data[i] & ~1;
        }
    }
    
    fwrite(s_data,32,1,encInfo->fptr_stego_image);
    
    int a=ftell(encInfo->fptr_src_image);
    
    printf("Secret file size of src image is at %d\n", a);

    int b=ftell(encInfo->fptr_stego_image);
        
    printf("Secret file size of dest image is at %d\n", b);
 
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{

    char secret_data[encInfo->size_secret_file+1];
    
    printf("Size of the secret data to be copied is %ld\n", encInfo->size_secret_file);
    
    int ret =fread(secret_data,sizeof(char),encInfo->size_secret_file,encInfo->fptr_secret);
    
    printf("Amount of secret data read is=%d\n", ret);
    
    encode_data_to_image(secret_data, encInfo->size_secret_file, encInfo->fptr_src_image,encInfo->fptr_stego_image);

    return e_success;

}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char img_buff[MAX_IMAGE_BUF_SIZE];

    for(uint i=0;i<size;i++)
    {

        fread(img_buff, sizeof(char),MAX_IMAGE_BUF_SIZE, fptr_src_image);

        encode_byte_to_lsb(data[i],img_buff);

        fwrite(img_buff, sizeof(char),MAX_IMAGE_BUF_SIZE, fptr_stego_image);

    }
    int ret=ftell(fptr_src_image);
    printf("After copying secret data Source image is at %d\n", ret);

    int ret1=ftell(fptr_stego_image);
    printf("after copying secret data Destination image is at %d\n", ret1);
    return e_success;

}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    int ch;
    while((ch=getc(fptr_src)) != EOF)
    {
        putc(ch,fptr_dest);
    }
    int ret=ftell(fptr_src);
    printf("After copying remaining data Source image is at %d\n", ret);

    int ret1=ftell(fptr_dest);
    printf("Afte copying remaining data destination  image is at %d\n", ret1);
    
    return e_success;
}