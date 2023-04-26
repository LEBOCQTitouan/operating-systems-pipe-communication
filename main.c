#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "fsieve/fsieve.h"

int main() {
    struct pipe data_pipe;
    data_pipe = create_pipe();

    create_child_with_communication(child_routine, data_pipe);

    fsieve_data sample = {0};
    int data[] = {0, 1, 2, 4, 6};

    sample.size = 5;
    sample.data_array = data;
    send_data(data_pipe, sample);

    wait(NULL);

    return EXIT_SUCCESS;
}
