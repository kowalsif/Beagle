/*
    Minimal version for teaching.  Mark A. Yoder 26-July-2011
    i2cget.c - A user-space program to read an I2C register.
    Copyright (C) 2005-2010  Jean Delvare <khali@linux-fr.org>

    Based on i2cset.c:
    Copyright (C) 2001-2003  Frodo Looijaard <frodol@dds.nl>, and
                             Mark D. Studebaker <mdsxyz123@yahoo.com>
    Copyright (C) 2004-2005  Jean Delvare <khali@linux-fr.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include "i2c-dev.h"
#include <fcntl.h>

#include "gpio-utils.h"

#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */

int main(int argc, char *argv[])
{
	char *end;
	int i2cbus, size, daddress;
	int file[2];
	int res[2];
	int address[2];
	int alarum[2];
	int temp;
	char filename[20];
	

	/* handle (optional) flags first */
	if(argc < 8) {
		fprintf(stderr, "Usage:  %s <i2c-bus> <i2c-address> <i2c-address> <gpio_pin> <gpio_pin> <temperature> <dir>\n", argv[0]);
		exit(1);
	}

	i2cbus = atoi(argv[1]);
	address[0]  = atoi(argv[2]);
	address[1] = atoi(argv[3]);
	alarum[0] = atoi(argv[4]);
	alarum[1] = atoi(argv[5]);
	temp = atoi(argv[6]);
	int dir = atoi(argv[7]);
	daddress = 0;
	size = I2C_SMBUS_BYTE;
	
	printf("test0");
//	int temp0;
//	snprintf(temp0, sizeof(temp0), "%s %s %s %s %s", i2cbus, dir, address[0], address[1], temp);
	//system(temp0);
	printf("test1");

	system( "i2cset -y 1 72 3 27");
	//system("./tempSet.sh" i2cbus dir address[0] address[1] temp);
	system( "i2cset -y 1 73 3 27");
	//system("i2cset -y %d %d %d %d", i2cbus, address[1], 3, temp); //set high temperatures
	
	struct pollfd fdset[2];
	int nfds = 2;
	int gpio_fd[2];
	
	gpio_export(alarum[0]);
	gpio_export(alarum[1]);
	gpio_set_dir(alarum[0], "in");
	gpio_set_dir(alarum[1], "in");
	gpio_set_edge(alarum[0], "rising");
	gpio_set_edge(alarum[1], "rising"); //prepare to read the alarums when they trigger
	
	gpio_fd[0] = gpio_fd_open(alarum[0], O_RDONLY);
	gpio_fd[1] = gpio_fd_open(alarum[1], O_RDONLY); //setup

	sprintf(filename, "/dev/i2c-%d", i2cbus);
	file[0] = open(filename, O_RDWR);
	file[1] = open(filename, O_RDWR);
	if (file<0) {
		if (errno == ENOENT) {
			fprintf(stderr, "Error: Could not open file "
				"/dev/i2c-%d: %s\n", i2cbus, strerror(ENOENT));
		} else {
			fprintf(stderr, "Error: Could not open file "
				"`%s': %s\n", filename, strerror(errno));
			if (errno == EACCES)
				fprintf(stderr, "Run as root?\n");
		}
		exit(1);
	}

	if (ioctl(file[0], I2C_SLAVE, address[0]) < 0) { //setup 0
		fprintf(stderr,
			"Error: Could not set address to 0x%02x: %s\n",
			address[0], strerror(errno));
		return -errno;
	}
	
	if (ioctl(file[1], I2C_SLAVE, address[1]) < 0) {
		fprintf(stderr,
			"Error: Could not set address to 0x%02x: %s\n",
			address[1], strerror(errno));
		return -errno;
	}

	res[0] = i2c_smbus_read_byte_data(file[0], daddress);
	//close(file[0]);
	res[1] = i2c_smbus_read_byte_data(file[1], daddress);
	//close(file[1]);
	printf("%d, %d \n", res[0], res[1]);
	if (res < 0) {
		fprintf(stderr, "Error: Read failed, res=%d\n", res[0]);
		exit(2);
	}
	
	int timeout = POLL_TIMEOUT;
	int tempout, t0;
	int rc, len;
	int buf;
	

	
	while(1){
		memset((void*)fdset, 0, sizeof(fdset));
		
		fdset[0].fd = gpio_fd[0];
		fdset[0].events = POLLPRI;
		
		fdset[1].fd = gpio_fd[1];
		fdset[1].events = POLLPRI;
		
		rc = poll(fdset, nfds, timeout);      

		if (rc < 0) {
			printf("\npoll() failed!\n");
			return -1;
		}
		
		if (fdset[0].revents & POLLPRI) {
			lseek(fdset[0].fd, 0, SEEK_SET);// Read from the start of the file
			len = read(fdset[0].fd, buf, MAX_BUF);
			
			res[0] = i2c_smbus_read_byte_data(file[1], daddress);
			//sscanf(res[0], "%x", &t0);
			//tempout = strtol(res[0], NULL, 16);
			//tempout = t0*9/5+32; //convert to fahrenheit from celcius
			printf("address %d is at %d *F\n", address[0], res[0]*9/5+32);
			fflush(stdout);
		}
		
		if (fdset[1].revents & POLLPRI) {
			lseek(fdset[1].fd, 0, SEEK_SET);// Read from the start of the file
			len = read(fdset[1].fd, buf, MAX_BUF);
			res[1] = i2c_smbus_read_byte_data(file[0], daddress);
			//sscanf(res[0], "%x", &t0);
			//tempout = strtol(res[1], NULL, 16);
			//tempout = t0*9/5+32; //convert to fahrenheit from celcius
			printf("address %d is at %d *F\n", address[1], res[1]*9/5+32);
			fflush(stdout);
		}
		
		sleep(1);
	
	}
	
	
	//printf("0x%02x (%d)\n", res[0], res[0]);
	//printf("0x%02x (%d)\n", res[1], res[1]);
	
	close(file[0]);
	close(file[1]);

	exit(0);
}
