// ---------------------------------------------------------------------
// wait-gpio-event.c
//
//   This is an example of GPIO access with the libGPIOd library.
//
// Copyright 2018 Christophe BLAESS (christophe@blaess.fr)
// License GPL v2.
// ---------------------------------------------------------------------

#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
	struct gpiod_chip *input_chip;
	struct gpiod_line *input_line;
	int input_offset;


	struct gpiod_line_event event;

	if (argc !=3 ) {
		fprintf(stderr, "usage: %s <input-device> <input-offset> \n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	if (sscanf(argv[2], "%d", &input_offset) != 1) {
		fprintf(stderr, "%s: invalid <input-offset> value.\n", argv[0]);
		exit(EXIT_FAILURE);
	}



	input_chip = gpiod_chip_open_lookup(argv[1]);
	if (input_chip == NULL) {
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}



	input_line = gpiod_chip_get_line(input_chip, input_offset);
	if (input_line == NULL) {
		perror(argv[2]);
		exit(EXIT_FAILURE);
	}



//	if (gpiod_line_request_rising_edge_events(input_line, argv[0]) < 0) {
//	if (gpiod_line_request_falling_edge_events(input_line, argv[0]) < 0) {
	if (gpiod_line_request_both_edges_events(input_line, argv[0]) < 0) {
		perror(argv[2]);
		exit(EXIT_FAILURE);
	}


	for (;;) {
		gpiod_line_event_wait(input_line, NULL);
		gpiod_line_event_read(input_line, &event);
/*		if (gpiod_line_event_read(input_line, &event) == 0) {
			if (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE) {
				gpiod_line_set_value(output_line, 0);
			} else {
				gpiod_line_set_value(output_line, 1);
			}
		}
*/
	if (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE) fprintf(stdout, "rising\n");
	if (event.event_type == GPIOD_LINE_EVENT_FALLING_EDGE) fprintf(stdout, "falling\n");

	}
	gpiod_line_release(input_line);


	return EXIT_FAILURE;
}

