# software-bom-generator

generates a binary file containing the filenames and hashes of binaries,libraries etc as specified, to verify that systems run uncompromised software

# Compilation

```bash
    # you need libopenssl-dev installed to compile this
    gcc main.c -lssl -lcrypto -O2 -o software-bom
```

# Usage

```bash
    # Takes about 8 seconds for 3304 files of varying sizes, some small some large
    sofware-bom output_filename path_to_directory_1 path_to_directory_2
```

# Reading The Output file

The Output file format is like this:
4 bytes -> length of filename
n bytes -> Filename
hash -> 32 bytes (SHA256_DIGEST_LENGTH)

```c

    int32_t res = 1;
    // values other than 1 or 0 indicate an error
    // 1 means we read one fileInfo
    // 0 means we are at the end of the file and read all there is
    while(res == 1){
        FileInformation fileInfo;
        res = readBinaryBom(fd, &fileInfo);
    }
```
