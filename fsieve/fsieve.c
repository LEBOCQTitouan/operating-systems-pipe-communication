//
// Created by Titouan LEBOCQ on 26/04/2023.
//

#include "fsieve.h"

void child_routine(struct pipe pipe) {
    printf("[child] started successfully (%d)\n", getpid());
    fsieve_data data = recv_data(pipe);
    printf("[child] %d ended\n", getpid());
}

pid_t create_child_with_communication(void (*routine)(struct pipe), struct pipe communication_pipe) {
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        routine(communication_pipe);
        exit(EXIT_SUCCESS);
    }

    return pid;
}

struct pipe create_pipe() {
    struct pipe new_pipe = {0};
    int fd[2];

    if (pipe(fd) != EXIT_SUCCESS) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    new_pipe.fd = fd;
    new_pipe.reading_end = fd[1];
    new_pipe.writing_end = fd[0];

    return new_pipe;
}

void send_data(struct pipe pipe, fsieve_data data) {
    // TODO
}

fsieve_data recv_data(struct pipe pipe) {
    fsieve_data data = {0};
    // TODO
    return data;
}