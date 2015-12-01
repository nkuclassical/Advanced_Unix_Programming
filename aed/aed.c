/***
 *  AED 
 *  Author: Yanqiao Zhan@Stevens Institute of Technology
 *  Refer to manual of openssl
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <openssl/rand.h>

unsigned char key[EVP_MAX_KEY_LENGTH],iv[EVP_MAX_IV_LENGTH];
char data[4096];
unsigned char salt[8];
void usage(){
    printf("aed [ −deh] −p passin\n");
    printf("\
            −d Perform decryption of the input stream.\n\
            −e Perform encryption of the input stream.\n\
            −h Print a short usage and exit.\n\
            −p passin\n\
            Use the first line of the file ’passin’ (minus ’\\n’)\n\
            as the passphrase from which to derive the key\n\
            material. If this is not specified, use the value of\n\
            the AED_PASS environment variable.\n");
    exit(EXIT_FAILURE);
}

int do_crypt(FILE*infd, FILE*outfd, int do_encrypt){
    unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
    int inlen, outlen;

    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    EVP_CipherInit_ex(&ctx, EVP_bf_cbc(), NULL, NULL, NULL, do_encrypt);
    EVP_CIPHER_CTX_set_key_length(&ctx, 10);
    EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, do_encrypt);

    if(do_encrypt==1){
        fwrite("Salted__",1,8,outfd);
        fwrite(salt,1,8,outfd);
    }

    while((inlen=fread(inbuf,1,1024,infd))>0){

        if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen)){
            EVP_CIPHER_CTX_cleanup(&ctx);
            return 0;
        }
        outlen=fwrite(outbuf,1,outlen,outfd);
    }
    if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen)){
        EVP_CIPHER_CTX_cleanup(&ctx);
        return 0;
    }

    fwrite(outbuf,1,outlen,outfd);
    EVP_CIPHER_CTX_cleanup(&ctx);
    return 1;
}

void generate_data_file(char*passindir){

    FILE* fin;
    int i;
    char c;
    if((fin=fopen(passindir,"r"))==NULL){
        fprintf(stderr,"Unable to open '%s': %s\n",passindir,strerror(errno));
        exit(EXIT_FAILURE);
    }

    i=0; 
    while((c = fgetc(fin)) != EOF) {
        if (c == '\n') {
            data[i]='\0';
            break;
        }
        data[i]=c;
        i++;
    }

    fclose(fin);
}
void generate_data_char(char*passincontent){
    int i=0;
    for(i=0;i<(int)strlen(passincontent);i++){
        if(passincontent[i]=='\n'){
            data[i]='\0';
            break;
        }
        data[i]=passincontent[i];
    }
}

/**
 * int flag_p: whether -p is specified
 * passin: if flag_p is specified, then passin is the file name of passin;
 *         otherwise passin is the content of getenv("AED_PASS")
 * option: 1 means encrpyt and 0 means decrpyt
 * FILE*in: is stdin
 */
int generate_key(int flag_p,char*passin,int option,FILE*in){
    const EVP_CIPHER *cipher;
    const EVP_MD* dgst=NULL;
    int read_t;

    if(option==1){ /*encrypted, generate salt randomly*/
        RAND_bytes(salt,8);
    }else{        /*decrypt, so extract salt from stdin*/
        fread(salt,1,8,in);
        if((read_t=fread(salt,1,8,in))!=8){
            fprintf(stderr,"Can't read salt from cipher text\n");
            exit(EXIT_FAILURE);
        }
    }


    OpenSSL_add_all_algorithms();
    if((cipher=EVP_get_cipherbyname("aes-256-cbc"))==NULL){
        fprintf(stderr,"no such ciphter\n");
        return 1;
    }

    if((dgst=EVP_get_digestbyname("sha1"))==NULL){
        fprintf(stderr,"no such digest\n");
        return 1;
    }

    if(flag_p==1){ generate_data_file(passin);
    }else{
        generate_data_char(passin);
    }


    if(!EVP_BytesToKey(cipher, dgst, salt,
                (unsigned char *) data,
                strlen(data), 1, key, iv)){
        fprintf(stderr, "EVP_BytesToKey failed\n");
        return 1;
    }

    return 0;
}
int main(int argc, char *argv[]){

    char ch;
    int flag_p=0;
    int option=-1;/* 0-> Decrypt 1->Encrypt -1 -> leave the value unchanged*/
    while((ch=getopt(argc,argv,"dehp:"))!=-1){
        switch(ch){
            case 'd':
                option=0;
                break;
            case 'e':
                option=1;
                break;
            case 'h':
                usage();
                break;
            case 'p':
                flag_p=1;
                if(option==-1){
                    usage();
                    break;
                }
                generate_key(1,optarg,option,stdin);
                break;
            default:
                usage();
                break;
        }
    }
    argc -= optind;
    argv += optind;


    if(option==-1){
        usage();
    }
    if(flag_p==0){
        if(getenv("AED_PASS")==NULL){
            fprintf(stderr,"get env AED_PASS is empty\n");
            exit(EXIT_FAILURE);
        }
        generate_key(0,getenv("AED_PASS"),option,stdin);
    }

    do_crypt(stdin,stdout,option);
    return 0;
}
