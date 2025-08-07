#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<string.h>


int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    int ret,val,ret_enc,ret_dec;
    ret=check_operation_type(argv);
    if(ret==e_encode)
    {
        val=read_and_validate_encode_args(argc, argv ,&encInfo);
        if(val==e_success)
        {
            ret_enc=do_encoding(&encInfo);
            if(ret_enc == e_success)
            {
                printf("Encoding was successful\n");
            }
            else
            printf("Encoding was unsuccessful\n");
        }
        else
        printf("Enter valid arguments\n");

    }
    else if(ret == e_decode)
    {
        val = read_and_validate_decode_args(argc,argv,&decInfo);
        if(val == e_success)
        {  
            ret_dec= do_decoding(&decInfo);
            if(ret_dec == e_success)
            {
                printf("Decoding was successful\n");
            }
            else
            printf("Decoding was unsuccessful\n");
        }
        else
        printf("Enter valid arguments\n");
        
    }
    else
    printf("Error\n");

    return 0;
}
