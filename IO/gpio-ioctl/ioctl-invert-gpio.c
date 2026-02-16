// ---------------------------------------------------------------------
// ioctl-invert-gpio.c
//
//   This is a small example of ioctl() accesses to the new GPIO
//   user-space API under Linux to output on a GPIO line the
//   complementary value of those read on an input GPIO line.
//
// Copyright 2018 Christophe BLAESS (christophe@blaess.fr) 
// License GPL v2.
// ---------------------------------------------------------------------

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/gpio.h>
#include <sys/ioctl.h>


int main(int argc, char *argv[])
{
	int input_fd;
	int output_fd;

	int input_offset;
	int output_offset;

	struct gpiohandle_request input_request;
	struct gpiohandle_request output_request;

	struct gpiohandle_data input_values;
	struct gpiohandle_data output_values;

	if (argc !=5 ) {
		fprintf(stderr, "usage: %s <input-device> <input-offset> <output_device> <output-offset>\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	if (sscanf(argv[2], "%d", &input_offset) != 1) {
		fprintf(stderr, "%s: invalid <input-offset> value.\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (sscanf(argv[4], "%d", &output_offset) != 1) {
		fprintf(stderr, "%s: invalid <output-offset> value.\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	input_fd = open(argv[1], O_RDONLY);
	if (input_fd < 0) {
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}

	output_fd = open(argv[3], O_RDONLY);
	if (output_fd < 0) {
		perror(argv[3]);
		close(input_fd);
		exit(EXIT_FAILURE);
	}

	memset(&input_request, 0, sizeof(struct gpiohandle_request));
	input_request.lineoffsets[0] = input_offset;
	input_request.flags = GPIOHANDLE_REQUEST_INPUT;
	input_request.lines = 1;

	if (ioctl(input_fd, GPIO_GET_LINEHANDLE_IOCTL, &input_request) < 0) {
		perror(argv[1]);
		close(input_fd);
		close(output_fd);
		exit(EXIT_FAILURE);
	}

	memset(&output_request, 0, sizeof(struct gpiohandle_request));
	output_request.lineoffsets[0] = output_offset;
	output_request.flags = GPIOHANDLE_REQUEST_OUTPUT;
	output_request.lines = 1;

	if (ioctl(output_fd, GPIO_GET_LINEHANDLE_IOCTL, &output_request) < 0) {
		perror(argv[3]);
		close(input_fd);
		close(output_fd);
		exit(EXIT_FAILURE);
	}

	for (;;) {
		if (ioctl(input_request.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &input_values) < 0) {
			perror(argv[1]);
			break;
		}
		output_values.values[0] = (input_values.values[0] == 0) ? 1 : 0;

		if (ioctl(output_request.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &output_values) < 0) {
			perror(argv[3]);
			break;
		}
	}
	close(input_fd);
	close(output_request.fd);
	close(output_fd);
	return EXIT_FAILURE;
}

