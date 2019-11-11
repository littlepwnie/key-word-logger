#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char* XORCipher(char* data, char* key, unsigned long dataLen, unsigned long keyLen) {
    char* output = (char*)malloc(sizeof(char)*dataLen+1);
    output[dataLen] = 0;
    for(int i = 0; i < dataLen; ++i) {
        output[i] = data[i] ^ key[i % keyLen];
    }
    return output;
}

int main(){

    char key[] = {'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'};

    FILE *fp;
    long lSize;
    char *buffer;

    fp = fopen ( "keywlogger.log" , "rb" );
    if( !fp ) perror("keywlogger.log"),exit(1);
    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );

    buffer = calloc( 1, lSize+1 );
    if( !buffer ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);
    if( 1!=fread( buffer , lSize, 1 , fp) )
    fclose(fp),free(buffer),fputs("entire read fails",stderr),exit(1);
    fclose(fp);

    unsigned long dataLen = strlen(buffer);
    unsigned long keyLen = strlen(key);

    char* xorMessage;
    xorMessage = XORCipher(buffer, key, dataLen, keyLen);
    free(buffer);
    
    FILE *file = fopen("decrypted.txt", "w");

    int results = fputs(xorMessage, file);
    if (results == EOF) {
        exit(-1);
    }
    fclose(file);

    return 0;
}
