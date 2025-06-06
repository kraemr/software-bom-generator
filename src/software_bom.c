#include <stdio.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include "../inc/software_bom.h"
#include <fcntl.h>

#define BUFFER_SIZE 16384
#define MAX_RECURSION 10

int sha256_file_evp(const char *filename, FileInformation* fileInfo);


int is_directory(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return 0;
    }
    return S_ISDIR(path_stat.st_mode);
}

int is_regular_file(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return 0; 
    }
    return S_ISREG(path_stat.st_mode);
}

// recursion is set to 0 when initially called
int get_file_hashes_directory(const char* directory,FILE * fp, int recursion) {
    if(recursion >= MAX_RECURSION) return 0;
    DIR *dir = opendir(directory);
    if (!dir) {
        perror("opendir failed");
        return 1;
    }
    struct dirent *entry;
    char fullpath[4096];
    while ((entry = readdir(dir)) != NULL) {        
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0  || strcmp(entry->d_name, "X11") == 0)
            continue;
        snprintf(fullpath, sizeof(fullpath), "%s/%s", directory, entry->d_name);        
        if(is_directory(fullpath)){            
            get_file_hashes_directory(fullpath,fp,recursion+1);
        }
        else if (is_regular_file(fullpath)) {
            FileInformation fileInfo;
            sha256_file_evp(fullpath,&fileInfo);
            memcpy(fileInfo.filePath,fullpath,strlen(fullpath)+1);//+1 for nul byte       
            unsigned int len = strlen(fileInfo.filePath)+1;
            fwrite(&len,sizeof(unsigned int),1, fp);
            fwrite(fileInfo.filePath,strlen(fileInfo.filePath)+1,1,fp);
            fwrite(fileInfo.fileHash, SHA256_DIGEST_LENGTH, 1, fp);                        
        }
    }
    closedir(dir);
    return 0;
}

int sha256_file_evp(const char *filename, FileInformation* fileInfo) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("File opening failed");
        return 1;
    }
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        perror("EVP_MD_CTX_new failed");
        fclose(file);
        return 1;
    }

    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) {
        perror("EVP_DigestInit_ex failed");
        EVP_MD_CTX_free(mdctx);
        fclose(file);
        return 1;
    }

    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (EVP_DigestUpdate(mdctx, buffer, bytes_read) != 1) {
            perror("EVP_DigestUpdate failed");
            EVP_MD_CTX_free(mdctx);
            fclose(file);
            return 1;
        }
    }

    if (ferror(file)) {
        perror("File read error");
        EVP_MD_CTX_free(mdctx);
        fclose(file);
        return 1;
    }

    unsigned int out_len = 0;
    if (EVP_DigestFinal_ex(mdctx, fileInfo->fileHash, &out_len) != 1) {
        perror("EVP_DigestFinal_ex failed");
        EVP_MD_CTX_free(mdctx);
        fclose(file);
        return 1;
    }

    EVP_MD_CTX_free(mdctx);
    fclose(file);
    return 0;
}



int main( int argc, char* argv[] ) {
    if(argc < 3){
        printf("No Directories Specified!\n");
        return 0;
    }
    const char* outputName = argv[1];
    // can be improved by forking on each initial get_file_hashes_directory call or threading
    for(int i = 2; i < argc; i++) {
        FILE * fp = fopen(outputName,"wb");
        int ret = get_file_hashes_directory(argv[i],fp,0);
        fclose(fp);
    }

    int fd = 0;
    fd = open(outputName,O_RDONLY);
    return 0;
}