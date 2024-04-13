#pragma once

#include <stdbool.h>
#include <stdlib.h>


/**
 * some helper.
 */
#define str(x) #x
#define strname(x) str(x)

void stdout_endl();

double rand_uniform();
int rand_int(int l, int r); // [l, r)

void exit_on(int exitCode, const char * why) __attribute__((noreturn));
bool exit_if(bool condition, int exitCode, const char * why);

void todo_impl(const char * why);
#define TODO(s) todo_impl("TODO at " __FILE__ "::" strname(__LINE__) s)

char * get_current_time_str();

/**
 * go-back-n
 */
int sender_main(int argc, char ** argv);
int receiver_main(int argc, char ** argv);
int pipe_main(int argc, char ** argv);

/**
 * sender send to SENDER_SEND_SOCK_PORT, receiver receive from RECEIVER_RECV_PORT
 * to simulate loss in transmission, there will be a pipe to do packet transfer from sender to receiver.
 * It is to say, sender will send data to pipe, and receiver receives from pipe.
 */
#define SENDER_SEND_SOCK_PORT 3001
#define RECEIVER_RECV_PORT 3002
#define SENDER_RECV_SOCK_PORT 3003
#define RECEIVER_SEND_PORT 3004

size_t min_size(size_t a, size_t b);