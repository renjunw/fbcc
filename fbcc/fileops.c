#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#include "config.h"

/**
  Reads the entire contents of the file (following the current seek
  pointer) into a buffer.  The buffer will be allocated on the heap to
  the correct size, and it is the caller's responsibility to free it.

  @param[in] fd A C \a FILE pointer that has already been opened and is
  \a seeked to the beginning of the data to be read.

  @param[out] buffer A pointer to the beginning of the buffer containing
  the file data.

  @param[out] error

  @return Non-zero if an error has occurred.
 */
__ExternC int read_all_file(char *filename, char** buffer, unsigned *size) {
    unsigned file_pos;
    unsigned file_size;
    unsigned bytes_read;

    FILE *fd = NULL;

    fd = fopen(filename, "rb");

    if(!fd)
    {
        fprintf(stderr, "Cannot open file %s\r\n", filename);
        return ERR_OPENFILE;
    }

    file_pos = ftell(fd);
    if (fseek(fd, 0, SEEK_END)) {
        fprintf(stderr, "Unable to determine size of file\r\n");
        return ERR_IO;
    }

    file_size = ftell(fd) - file_pos;

    if (fseek(fd, file_pos, SEEK_SET)) {
        fprintf(stderr, "Unable to determine size of file\r\n");
        return ERR_IO;
    }

    *buffer = malloc(file_size);
    if (*buffer == NULL) {
        fprintf(stderr, "Out of memory\r\n");
        return ERR_MEM;
    }

    bytes_read = fread(*buffer, 1, file_size, fd);
    if (bytes_read != file_size || ferror(fd)) 
    {
        fprintf(stderr, "Could not read all contents of file\r\n");
        free(*buffer); 
        *buffer = NULL;
        fclose(fd);
        return ERR_IO;
    }

    *size = file_size;
    fclose(fd);  
    return 0;
}

__ExternC char * avl_vm_get_filename(char *ori, const char *ext)
{
    int s, i;
    char * ns = NULL;
    int last_dot = 0;

    for(i = 0; ori[i]; i ++)
    {
        if(ori[i] == '.')
        {
            last_dot = i;
        }
    }
    if(!last_dot)
    {
        last_dot = i;
    }
    s = i + strlen(ext) + 2;
    if(!(ns = (char *)malloc(s)))
    {
        return NULL;
    }
    memcpy(ns, ori, last_dot);
    ns[last_dot] = '.';
    strcpy(&ns[last_dot + 1], ext);
    return ns;
}



