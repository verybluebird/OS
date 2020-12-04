#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/mman.h>


int input_line(int line_number, int fd, int *offsets, int file_size, int *line_length, char *buffer) {
    fd_set set;
    int max_fd = 0;
    struct timeval timeout;
    int need_line = 1;

    printf("What line do you need?\n");
    printf("Number of lines : %d\n ", line_number);

    while (1) {

        FD_SET(0, &set); /*terminal*/
        timeout.tv_sec = 5; /* seconds */
        timeout.tv_usec = 0;  /* microseconds */

        int selectResult = select(max_fd + 1, &set, NULL, NULL, &timeout);
        /*nfds  should  be  set to the highest-numbered file descriptor in any of
       the three sets, plus 1. */


        if (selectResult == -1) {
            perror("select() error");
            if (close(fd) == -1) perror("Error while closing");
            return -1;
        }

        if (selectResult != 0) {
            if (FD_ISSET(fd, &set) != 0) { /*FD_ISSET() tests to see if a file descriptor  is  part  of  the
       set*/
                scanf("%d", &need_line);

                if (need_line == 0) {
                    if (close(fd) == -1)
                        perror("Error while closing.");
                    if (munmap(buffer, file_size) == -1)
                        perror("Error while munmap");
                    return 0;
                }

                if (need_line > line_number || need_line < 0) {
                    perror("Error! No such line!");
                    if (close(fd) == -1)
                        perror("Error while closing.");
                    return -2;
                }

                for (int i = offsets[need_line - 1]; i < offsets[need_line - 1] + line_length[need_line - 1]; i++)
                    printf("%c", buffer[i]);

            } else {
                printf("Fd absents in fd_set.");
                return -3;
            }

        }

        if (selectResult == 0) {/*zero if the timeout expires before anything interesting
       happens*/
            for (int i = 0; i < file_size; i++)
                printf("%c", buffer[i]);
            if (close(fd) == -1)
                perror("Error while closing");
            if (munmap(buffer, file_size) == -1)
                perror("Error while munmap");
            return 0;
        }
    }
}

int main(int argc, char *argv[]) {

    int offsets[100];
    int line_length[100];
    int fd;
    char *buffer;
    int line_number = 0;

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("File is not opening.\n");
        return -1;
    }

    int file_size = lseek(fd, (size_t) 0, SEEK_END); /*The file offset is set to the  size  of  the  file  plus  offset
              bytes.*/
    lseek(fd, (size_t) 0, SEEK_SET); /*The file offset is set to offset bytes.*/

    buffer = mmap(0, file_size, PROT_READ, MAP_SHARED, fd, 0);


    for (int i = 0; i < file_size; i++) {
        if (buffer[i] == '\n') {
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
    input_line(line_number, fd, offsets, file_size, line_length, buffer);
}
