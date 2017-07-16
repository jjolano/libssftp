#include <sys/time.h>

#include "compat.h"
#include "selectpoll.h"

int poll(struct pollfd* fds, nfds_t nfds, int timeout)
{
	struct timeval timeout;
	struct fd_set readset, writeset;

	FD_ZERO(&readset);
	FD_ZERO(&writeset);
	
	int i;
	for(i = 0; i < nfds; i++)
	{
		if(fds[i].events & POLLIN)
		{
			FD_SET(fds[i].fd, &readset);
		}

		if(fds[i].events & POLLOUT)
		{
			FD_SET(fds[i].fd, &writeset);
		}
	}

	timeout.tv_sec = 0;
	timeout.tv_usec = timeout * 1000;

	int s = select(nfds, &readset, &writeset, NULL, &timeout);

	for(i = 0; i < nfds; i++)
	{
		if(fds[i].fd <= 0)
		{
			fds[i].revents |= POLLNVAL;
		}

		if(fds[i].events & POLLIN)
		{
			if(FD_ISSET(fds[i].fd), &readset)
			{
				fds[i].revents |= POLLIN;
			}
		}

		if(fds[i].events & POLLOUT)
		{
			if(FD_ISSET(fds[i].fd), &writeset)
			{
				fds[i].revents |= POLLOUT;
			}
		}
	}

	return s;
}
