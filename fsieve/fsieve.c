//
// Created by Titouan LEBOCQ on 26/04/2023.
//

#include "fsieve.h"

void child_routine_async(struct pipe pipe) {
    int pid = 0;
    struct pipe child_pipe;
    fsieve_data processed_data = recv_data_array(pipe);
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
        create_child_with_communication(child_routine_async, child_pipe);
        send_data_array(child_pipe, new_data);
        wait(NULL);
    }

    exit(EXIT_SUCCESS);
}

void child_routine_sync(struct pipe pipe) {
    int pid = 0;
    int data_buffer;
    int prime_number;
    struct pipe child_pipe;

    data_buffer = recv_data(pipe);
    if (data_buffer == END_SEQUENCE)
        exit(EXIT_SUCCESS);
    prime_number = data_buffer;
    printf("Prime number found: %d\n", prime_number);

    data_buffer = recv_data(pipe);
    if (data_buffer == END_SEQUENCE)
        exit(EXIT_SUCCESS);
    do {
        if (data_buffer % prime_number != 0) {
            if (pid == 0) { // new prime number not yet found
                child_pipe = create_pipe();
                pid = create_child_with_communication(child_routine_sync, child_pipe);
                send_data(child_pipe, data_buffer);
            } else {
                send_data(child_pipe, data_buffer);
            }
        }
        data_buffer = recv_data(pipe);
    } while (data_buffer != END_SEQUENCE);
    send_data(child_pipe, END_SEQUENCE);

    if (pid != 0) {
        close(pipe.reading_end);
        close(child_pipe.writing_end);
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

void send_data_array(struct pipe pipe, fsieve_data data) {
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

void send_data(struct pipe pipe, int data) {
    if (write(pipe.writing_end, &data, sizeof(int)) < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }
}

fsieve_data recv_data_array(struct pipe pipe) {
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

int recv_data(struct pipe pipe) {
    int buffer = 0;
    if (read(pipe.reading_end, &buffer, sizeof(int)) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    return buffer;
}