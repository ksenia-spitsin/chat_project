#include <signal.h> /* kill() */
#include <errno.h> /* errno */
#include <stdio.h> /* perror() */
#include <assert.h> /* assert() */
#include "signal_sender.h"

static void HandleSendSignalError(int _errorStatus);

void SendSignalToParent()
{
    pid_t parent = getppid();
    if(kill(parent, SIGCHLD) < 0)
    {
        int errorStatus = errno;
        HandleSendSignalError(errorStatus);
    }
}

void SendSignalInit(pid_t _pid)
{
    if(kill(_pid, SIGINT) < 0)
    {
        int errorStatus = errno;
        HandleSendSignalError(errorStatus);
    }
}

static void HandleSendSignalError(int _errorStatus)
{
    switch(_errorStatus)
    {
        case EPERM:
            perror("The calling process does not have permission to send the signal");
            break;
        case ESRCH:
            perror("The target process do not exist");
            break;
        case EINVAL:
            assert(!"An invalid signal was specified");
            break;
        default:
            assert(!"An undocumented error was occurred");
            break;
    }
}