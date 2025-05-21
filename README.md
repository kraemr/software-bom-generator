# software-bom-generator

generates a binary file containing the filenames and hashes of binaries,libraries etc as specified, to verify that systems run uncompromised software

# Compilation

```bash
    # you need libopenssl-dev installed
    gcc main.c -lssl -lcrypto -O2 -o software-bom
```

# Usage

```bash
    sofware-bom output_filename path_to_directory_1 path_to_directory_2
```
