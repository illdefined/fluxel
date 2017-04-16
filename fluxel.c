#include "config.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/epoll.h>

#include <systemd/sd-daemon.h>

#define MAX_EV 256

static struct epoll_event events[MAX_EV];

static int set_nonblock(int fd) {
	int r = -1;

	r = fcntl(fd, F_GETFL);
	if (r < 0) {
		fprintf(stderr, SD_ERR "Failed to get file status flags on %i: %m\n", fd);
		goto exit;
	}

	r = fcntl(fd, F_SETFL, r | O_NONBLOCK);
	if (r != 0) {
		fprintf(stderr, SD_ERR "Failed to set file status flags on %i: %m\n", fd);
		goto exit;
	}

exit:
	return r;
}

static int setup_sockets(int n) {
	int r = -1;

	int ep = epoll_create1(0);
	if (ep < 0) {
		fprintf(stderr, SD_ERR "Failed to create epoll instance: %m\n", stderr);
		goto exit;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;

	for (int i = 0; i < n; ++i) {
		int fd = SD_LISTEN_FDS_START + i;

		int s = sd_is_socket(fd, AF_UNSPEC, SOCK_STREAM, 0);
		if (s < 0) {
			fprintf(stderr, SD_ERR "sd_is_socket(): %m\n");
			goto exit;
		} else if (s != 0) {
			fprintf(stderr, SD_ERR "File descriptor %i is not a listening socket\n", fd);
			goto exit;
		}

		if (set_nonblock(fd)) {
			fprintf(stderr, SD_ERR "Could not mark descriptor %i as non‐blocking: %m\n", fd);
			goto exit;
		}

		ev.data.fd = fd;

		if (epoll_ctl(ep, EPOLL_CTL_ADD, fd, &ev)) {
			fprintf(stderr, SD_ERR "Failed to add socket %i to epoll instance: %m\n", fd);
			goto exit;
		}
	}

	r = ep;

exit:
	return r;
}

static int handle_accept(int ep, struct epoll_event *ev) {
	int r = -1;

	if (ev->events & EPOLLERR) {
		fprintf(stderr, SD_ERR "An error occured on listening socket %i\n", ev->data.fd);
		goto exit;
	}

	int c = -1;

	do {
		c = accept4(ev->data.fd, NULL, NULL, SOCK_NONBLOCK);
		if (c < 0) {
			if (errno == ECONNABORTED || errno == EINTR) {
				continue;
			} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
				/* No more connections to accept */
				r = 0;
			} else {
				fprintf(stderr, SD_ERR "Failed to accept connection: %m\n");
			}

			goto exit;
		}

		struct epoll_event cev;
		cev.events = EPOLLIN | EPOLLET;
		cev.data.fd = c;

		if (epoll_ctl(ep, EPOLL_CTL_ADD, c, &cev)) {
			fprintf(stderr, SD_ERR "Failed to add connection %i to epoll instance: %m\n", c);
			goto exit;
		}

		/* TODO: Initialise connection state */
	} while (c >= 0);

	r = -1;

exit:
	return r;
}

static int handle_read(int ep, struct epoll_event *ev) {
	int r = -1;

	if (ev->events & (EPOLLERR | EPOLLHUP)) {
		epoll_ctl(ep, EPOLL_CTL_DEL, ev->data.fd, NULL);

		if (ev->events & EPOLLERR) {
			fprintf(stderr, SD_ERR "An error occured on connection socket %i\n", ev->data.fd);
			goto exit;
		}
	}

	/* TODO: Handle read */

exit:
	return r;
}

static int event_loop(int ep, int n) {
	int r = -1;
	int nev = -1;

	do {
		nev = epoll_wait(ep, events, MAX_EV, -1);
		assert(nev != 0);

		if (nev < 0) {
			if (errno == EINTR) {
				continue;
			}

			fprintf(stderr, SD_ERR "Failure while waiting for event: %m\n");
			goto exit;
		}

		for (int i = 0; i < nev; ++i) {
			if (events[i].data.fd < SD_LISTEN_FDS_START + n) {
				if (handle_accept(ep, events + i)) {
					fputs(SD_ERR "Could not handle connection\n", stderr);
					goto exit;
				}
			} else {
				if (handle_read(ep, events + i)) {
					fputs(SD_ERR "Could not handle read\n", stderr);
					goto exit;
				}
			}
		}
	} while (nev >= 0);

exit:
	return r;
}

int main(int argc, char *argv[]) {
	int ex = EXIT_FAILURE;

	int n = sd_listen_fds(1);
	if (n < 0) {
		fprintf(stderr, SD_CRIT "sd_listen_fds(): %m\n");
		goto exit;
	} else if (n == 0) {
		fputs(SD_CRIT "No listening sockets were passed\n", stderr);
		goto exit;
	}

	int ep = setup_sockets(n);
	if (ep < 0) {
		fputs(SD_CRIT "Failed to set up listening sockets\n", stderr);
		goto exit;
	}

	sd_notify(0, "READY=1");

	event_loop(ep, n);

	ex = EXIT_SUCCESS;

exit:
	return ex;
}
