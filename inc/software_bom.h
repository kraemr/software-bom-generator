#include <openssl/sha.h>
#include <openssl/evp.h>
#define BINARYBOM_INVALID_FILE_PATH_SIZE -1

typedef struct FileInformation {
    unsigned int filePathLength;
    char filePath[512];
    unsigned char fileHash[SHA256_DIGEST_LENGTH];
} FileInformation;

extern int32_t readBinaryBom(int fd,FileInformation* fInfo);