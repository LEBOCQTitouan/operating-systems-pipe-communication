//
// Created by Titouan LEBOCQ on 26/04/2023.
//

#include "fsieve.h"

void child_routine(struct pipe pipe) {
    int current_pid = getpid();
    printf("[child] started successfully (%d)\n", &current_pid);
    fsieve_data data = recv_data(pipe);
    for (int i = 0; i < data.size; ++i) {
        printf("%d\n", data.data_array[i]);
    }
    printf("[child] %d ended\n", &current_pid);
}

pid_t create_child_with_communication(void (*routine)(struct pipe), struct pipe communication_pipe) {
    pid_t pid;

    if ((pid = fork()) < 0) {
        perror("fork() error");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { // starting child routine
        routine(communication_pipe);
        wait(NULL);
        exit(EXIT_SUCCESS);
    }

    return pid;
}

struct pipe create_pipe() {
    struct pipe new_pipe = {0};
    new_pipe.fd = malloc(sizeof(int) * 2);

    if (pipe(new_pipe.fd) != EXIT_SUCCESS) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    new_pipe.reading_end = new_pipe.fd[0];
    new_pipe.writing_end = new_pipe.fd[1];

    return new_pipe;
}

void send_data(struct pipe pipe, fsieve_data data) {
    int end = END_SEQUENCE;
    if (write(pipe.writing_end, data.data_array, sizeof(int) * data.size) < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    write(pipe.writing_end, &end, sizeof(int));
    // close(pipe.writing_end);
}

fsieve_data recv_data(struct pipe pipe) {
    fsieve_data data = {0};
    do {
        data.size++;
        data.data_array = realloc(data.data_array, data.size);
        if (read(pipe.reading_end, data.data_array + (data.size - 1) ,sizeof(int)) < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
    } while (data.data_array[data.size - 1] != -1);
    // close(pipe.reading_end);
    return data;
}

int log(char *str, ...) {
    va_list args;

    va_start(args, str);
    int rc = fprintf(LOG_OUTPUT, str, args);
    fprintf(LOG_OUTPUT, "\n");
    va_end(args);

    return rc;
}