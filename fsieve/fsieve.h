//
// Created by Titouan LEBOCQ on 26/04/2023.
//

#ifndef OPERATING_SYSTEM_PIPES_FSIEVE_H
#define OPERATING_SYSTEM_PIPES_FSIEVE_H

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Struct holding every data used in a pipe.
 */
typedef struct pipe {
    int reading_end;
    int writing_end;
    int *fd;        // file descriptor for the created pipe
};

/**
 * Struct holding every data used in fsieve lib.
 */
typedef struct fsieve_data {
    int size;       // size of the data array
    int *data_array;// the data array
} fsieve_data;

/**
 * Basic child routine for the fsieve library.
 * @param pipe the pipe which will be used for communication
 */
void child_routine(struct pipe pipe);

/**
 * Function creating a child wich will execute a @routine
 * @param routine the routine the child will execute
 * @param communication_pipe the communication pipe used between parent and child
 * @return the pid of the created process
 */
pid_t create_child_with_communication(void (*routine)(struct pipe), struct pipe communication_pipe);

/**
 * Function used to create a @pipe
 * @return the @pipe created
 */
struct pipe create_pipe();

/**
 * Function used to send @fsieve_data through a @pipe
 * @param pipe the communication @pipe
 * @param data the data sent
 */
void send_data(struct pipe pipe, fsieve_data data);

/**
 * Function used to receive @fsieve_data from a @pipe
 * @param pipe the communication @pipe
 * @return the data received
 */
fsieve_data recv_data(struct pipe pipe);

#endif //OPERATING_SYSTEM_PIPES_FSIEVE_H
