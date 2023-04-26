//
// Created by Titouan LEBOCQ on 26/04/2023.
//

#include "fsieve.h"

void child_routine(struct pipe pipe) {
    int pid = 0;
    struct pipe child_pipe;
    fsieve_data processed_data = recv_data(pipe);
    fsieve_data new_data = {0};

    printf("%d\n", processed_data.prime_number);

    for (int i = 0; i < processed_data.size; ++i) {
        int current = processed_data.data_array[i];
        if (current % processed_data.prime_number != 0) {
            if (new_data.prime_number == 0) { // new prime number not yet found
                new_data.prime_number = current;
            } else {
                new_data.size++;
                new_data.data_array = realloc(new_data.data_array, sizeof(int) * new_data.size);
                new_data.data_array[new_data.size - 1] = current;
            }
        }
    }
    if (new_data.prime_number > 0) {
        child_pipe = create_pipe();
        create_child_with_communication(child_routine, child_pipe);
        send_data(child_pipe, new_data);
        wait(NULL);
    }

    exit(EXIT_SUCCESS);
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
    // sending prime number
    if (write(pipe.writing_end, &data.prime_number, sizeof(int)) < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    // sending list of number
    if (write(pipe.writing_end, data.data_array, sizeof(int) * data.size) < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    // sending end sequence
    write(pipe.writing_end, &end, sizeof(int));
    close(pipe.writing_end);
}

fsieve_data recv_data(struct pipe pipe) {
    fsieve_data data = {0};

    int buffer = 0;
    if (read(pipe.reading_end, &buffer, sizeof(int)) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    if (buffer == END_SEQUENCE) // empty data
        return data;

    data.prime_number = buffer;
    if (read(pipe.reading_end, &buffer, sizeof(int)) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    while (buffer != END_SEQUENCE) {
        data.size++;
        data.data_array = realloc(data.data_array, sizeof(int) * data.size);
        data.data_array[data.size - 1] = buffer;

        if (read(pipe.reading_end, &buffer, sizeof(int)) < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
    }

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