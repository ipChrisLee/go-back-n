#pragma once

#include <stdbool.h>


/**
 * some helper.
 */
#define str(x) #x
#define strname(x) str(x)

int sender_main(int argc, char ** argv);
int receiver_main(int argc, char ** argv);
int pipe_main(int argc, char ** argv);

/**
 * sender send to SEND_SOCK_PORT, receiver receive from RECV_SOCK_PORT
 * to simulate loss in transmission, there will be a pipe to do packet transfer from sender to receiver.
 * It is to say, sender will send data to pipe, and receiver receives from pipe.
 */
#define SEND_SOCK_PORT 3001
#define RECV_SOCK_PORT 3002

void exit_on(int exitCode, const char * why) __attribute__((noreturn));
bool exit_if(bool condition, int exitCode, const char * why);

void todo_impl(const char * why);
#define TODO(s) todo_impl("TODO at " __FILE__ "::" strname(__LINE__) s)

char * get_current_time_str();