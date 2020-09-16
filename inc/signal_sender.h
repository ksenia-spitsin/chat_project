#ifndef SIGNAL_SENDER_H
#define SIGNAL_SENDER_H

#include <sys/types.h>
#include <unistd.h>

/**
 * @brief sends SIGCHLD to the parent process.
 * If the function fails a message describing the error will be printed to the standard error 
 */
void SendSignalToParent();

/**
 * @brief send SIGINT to _pid process.
 * If the function fails a message describing the error will be printed to the standard error 
 */
void SendSignalInit(pid_t _pid);

#endif /* SIGNAL_SENDER_H */