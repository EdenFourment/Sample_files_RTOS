// ---------------------------------------------------------------------
// ioctl-gpio-list.c
//
//   This is a small example of ioctl() accesses to the new GPIO
//   user-space API under Linux to list all the GPIO controllers
//   and their GPIO lines.
//
// Copyright 2018 Christophe BLAESS (christophe@blaess.fr) 
// License GPL v2.
// ---------------------------------------------------------------------

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/gpio.h>
#include <sys/ioctl.h>


static int   filter_chips_files  (const struct dirent *entry);
static void  display_chip_info   (const char *filename);
static void  display_line_info   (int fd, int offset);



int main(int argc, char *argv[])
{
	int i;
	int chips_count;
	struct dirent **chips_files;


	chips_count = scandir("/dev", &chips_files, filter_chips_files, alphasort);
	if (chips_count <= 0) {
		fprintf(stderr, "%s: unable to find any GPIO chip.\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < chips_count; i++)
		display_chip_info(chips_files[i]->d_name);

	free(chips_files);
}



static int filter_chips_files(const struct dirent *entry)
{
	return strncmp(entry->d_name, "gpiochip", 7) == 0;
}


static void display_chip_info(const char *filename)
{
	int i;
	int fd;
	struct gpiochip_info info;

	if (chdir("/dev") != 0) {
		perror("/dev");
		return;
	}

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror(filename);
		return;
	}

	if (ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &info) < 0) {
		perror(filename);
		close(fd);
		return;
	}

	fprintf(stdout, "file = /dev/%s, name = %s, label = %s, lines = %d\n", filename, info.name, info.label, info.lines);

	for (i = 0; i < info.lines; i ++)
		display_line_info(fd, i);

	fprintf(stdout, "\n");
	close(fd);
}


static void display_line_info(int fd, int offset)
{
	struct gpioline_info info;

	memset(&info, 0, sizeof(struct gpioline_info));
	info.line_offset = offset;

	if (ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &info) < 0)
		return;

	fprintf(stdout, "    offset = %d, name = %s, consumer = %s, flags = %s %s %s %s %s\n", info.line_offset, info.name, info.consumer,
		info.flags & GPIOLINE_FLAG_IS_OUT ? "OUT" : "IN ",
		info.flags & GPIOLINE_FLAG_ACTIVE_LOW ? "ACTIVE_LOW " : "ACTIVE_HIGH",
		info.flags & GPIOLINE_FLAG_OPEN_DRAIN ? "OPEN_DRAIN" : "",
		info.flags & GPIOLINE_FLAG_OPEN_SOURCE ? "OPEN_SOURCE" : "",
		info.flags & GPIOLINE_FLAG_KERNEL ? "KERNEL" : "");
}
