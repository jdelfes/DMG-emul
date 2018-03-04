#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "utils.h"

void* map_file(const char *filename, off_t *size) {
    int fd = open(filename, O_RDONLY, 0);
    if (fd < 0) {
        fprintf(stderr, "Can't open %s -> %d\n", filename, errno);
        return NULL;
    }

    struct stat statInfo;
    if (fstat(fd, &statInfo) != 0) {
        fprintf(stderr, "Can't stat  %s -> %d\n", filename, errno);
        close(fd);
        return NULL;
    }

    void *ptr = mmap(NULL, statInfo.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        fprintf(stderr, "Can't mmap  %s -> %d\n", filename, errno);
        close(fd);
        return NULL;
    }

    close(fd);

    if (size) {
        *size = statInfo.st_size;
    }
    return ptr;
}
