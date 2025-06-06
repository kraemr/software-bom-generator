#include <openssl/sha.h>
#include <unistd.h>
#include "../inc/software_bom.h"

void print_hash(unsigned char *hash, unsigned int length) {
    for (unsigned int i = 0; i < length; i++)
        printf("%02x", hash[i]);    
}

int32_t readBinaryBom(int fd,FileInformation* fInfo) {
    const uint8_t buffer[sizeof(FileInformation)];
    ssize_t bytesRead = 0;
    bytesRead = read(fd,&fInfo->filePathLength,4);
    
    if(fInfo->filePathLength > 511) {
        return 0;
    }

    if(bytesRead == 0) {
        return 0;
    }
    bytesRead = read(fd,fInfo->filePath,fInfo->filePathLength);
    if(bytesRead == 0) {
        return 0;
    }
    bytesRead = read(fd,fInfo->fileHash,32);
    if(bytesRead == 0) {
        return 0;
    }

    fInfo->filePath[fInfo->filePathLength] = '\0';
    #ifdef PRINT_BINARY_BOM 
        for (unsigned int i = 0; i < SHA256_DIGEST_LENGTH; i++) printf("%02x", fInfo->fileHash[i]);    
        printf(" filePath : %s %d\n",fInfo->filePath, fInfo->filePathLength); 
    #endif
   
    return 1;
}

