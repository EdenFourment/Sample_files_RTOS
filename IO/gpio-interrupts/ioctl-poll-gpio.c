// ---------------------------------------------------------------------
// ioctl-poll-gpio.c
//
//   This is a small example of ioctl() accesses to the new GPIO
//   user-space API under Linux to wait on a input GPIO line for
//   some events.
//
// Copyright 2018 Christophe BLAESS (christophe@blaess.fr) 
// License GPL v2.
// ---------------------------------------------------------------------

#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/gpio.h>
#include <sys/ioctl.h>


int main(int argc, char *argv[])
{
	int input_fd;
	int input_offset;
	struct gpioevent_request  input_event_request;
	struct gpioevent_data     input_event_data;
	struct pollfd poll_fd;
	int ret;

	if (argc != 3) {
		fprintf(stderr, "usage: %s <input-device> <input-offset>\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	if (sscanf(argv[2], "%d", &input_offset) != 1) {
		fprintf(stderr, "%s: invalid <input-offset> value.\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	input_fd = open(argv[1], O_RDONLY);
	if (input_fd < 0) {
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}

	memset(&input_event_request, 0, sizeof(struct gpioevent_request));
	input_event_request.lineoffset = input_offset;
	input_event_request.handleflags = GPIOHANDLE_REQUEST_INPUT;
//	input_event_request.eventflags  = GPIOEVENT_REQUEST_FALLING_EDGE;
//	input_event_request.eventflags  = GPIOEVENT_REQUEST_RISING_EDGE;
	input_event_request.eventflags  = GPIOEVENT_REQUEST_BOTH_EDGES;
	if (ioctl(input_fd, GPIO_GET_LINEEVENT_IOCTL, &input_event_request) < 0) {
		perror("GPIO_GET_LINEEVENT_IOCTL");
		close(input_fd);
		exit(EXIT_FAILURE);
	}

	for (;;) {
		poll_fd.fd = input_event_request.fd;
		poll_fd.events = POLLIN | POLLPRI;


		ret = poll(&poll_fd, 1, 1000); // 1 second timeout		
//		ret = poll(&poll_fd, 1, -1); // infinite timeout
		if (ret < 0) {
			perror("poll");
			break;
		}
		if (ret > 0) {
			if (read(input_event_request.fd, &input_event_data, sizeof(struct gpioevent_data)) > 0) {
				fprintf(stdout, "[%llu.%09llu]", input_event_data.timestamp / 1000000000, input_event_data.timestamp % 1000000000);
				if (input_event_data.id & GPIOEVENT_EVENT_RISING_EDGE)
					fprintf(stdout, " RISING");
				if (input_event_data.id & GPIOEVENT_EVENT_FALLING_EDGE)
					fprintf(stdout, " FALLING");
			}
		}
		fprintf(stdout, "\n");
	}
	close(input_fd);
	return EXIT_FAILURE;
}

