#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "fsieve/fsieve.h"

int main() {
    struct pipe data_pipe;
    data_pipe = create_pipe();

    create_child_with_communication(child_routine_sync, data_pipe);

    fsieve_data sample = {0};

    int *data = calloc(sizeof(int), MAX_VAL - 2);
    for (int i = 0; i < MAX_VAL - 2; ++i) {
        data[i] = i + 3;
    }

    sample.size = MAX_VAL - 2;
    sample.data_array = data;
    sample.prime_number = 2;

    send_data_array(data_pipe, sample);
    wait(NULL);

    return EXIT_SUCCESS;
}
