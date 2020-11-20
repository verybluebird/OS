#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


int read_from_file(int fd, void *buf, int n) {
    int result = 1;
    int ret;
    int bytes = 0;
    while (bytes != n) {
        ret = read(fd, buf, n);

        if (ret == -1) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            } else {
                perror("Error while reading!");
                result = -1;
                return result;
            }
        }
        if (ret == 0) {
            perror("Error! End of file");
            result = -2;
            return result;
        }
        bytes += ret;
        buf += ret;
        n -= bytes;
    }
    return result;
}

int main(int argc, char *argv[]) {

    int offsets[100];
    int line_length[100];
    int fd;
    int line_number = 0;
    int need_line = 1;

    if ((fd = open(argv[1], O_RDONLY)) == -1) {

        perror("File is not opened\n");
        return -1;
    }

    int file_size = lseek(fd, (size_t) 0, SEEK_END);
    lseek(fd, (size_t) 0, SEEK_SET);

    char myfile[file_size];

    if (read_from_file(fd, myfile, file_size) == -1 || read_from_file(fd, myfile, file_size) == -2) {
        return -1;
    }
    for (int i = 0; i < file_size; i++) {

        if (myfile[i] == '\n') {

            if (line_number == 0) {
                offsets[line_number] = 0;
                line_length[line_number] = i + 1;
            } else {
                offsets[line_number] = offsets[line_number - 1] + line_length[line_number - 1];
                line_length[line_number] = i + 1 - offsets[line_number];
            }
            line_number++;
        }
    }

    printf("Count of lines : %d\n ", line_number);

    while (1) {
        printf("What line do you need? ");
        scanf("%d", &need_line);

        if (need_line == 0) {
            if (close(fd) == -1) perror("Error while closing");
            return 0;
        }

        if (need_line > line_number || need_line < 0) {
            perror("Error. Wrong number of line.");
            if (close(fd) == -1) perror("Error while closing");
            return 0;
        }

        for (int i = offsets[need_line - 1]; i < offsets[need_line - 1] + line_length[need_line - 1]; i++)
            printf("%c", myfile[i]);
    }
}
