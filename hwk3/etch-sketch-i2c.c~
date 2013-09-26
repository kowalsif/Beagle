//gcc etch-sketch-i2c.c i2cbusses.c i2cbusses.h gpio-utils.c gpio-utils.h


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>	// Defines signal-handling functions (i.e. trap Ctrl-C)
#include "gpio-utils.h"
#include "i2c-dev.h"
#include "i2cbusses.h"

#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64

#define BICOLOR	1

int keepgoing = 1;	// Set to 0 when ctrl-c is pressed

void signal_handler(int sig);
// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig)
{
	printf( "Ctrl-C pressed, cleaning up and exiting..\n" );
	keepgoing = 0;
}

static void help(void) __attribute__ ((noreturn));

static void help(void) {
	fprintf(stderr, "Usage: matrixLEDi2c (hardwired to bus 3, address 0x70)\n");
	exit(1);
}

static int check_funcs(int file) {
	unsigned long funcs;

	/* check adapter functionality */
	if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
		fprintf(stderr, "Error: Could not get the adapter "
			"functionality matrix: %s\n", strerror(errno));
		return -1;
	}

	if (!(funcs & I2C_FUNC_SMBUS_WRITE_BYTE)) {
		fprintf(stderr, MISSING_FUNC_FMT, "SMBus send byte");
		return -1;
	}
	return 0;
}

// Writes block of data to the display
static int write_block(int file, __u16 *data) {
	int res;
#ifdef BICOLOR
	res = i2c_smbus_write_i2c_block_data(file, 0x00, 16, 
		(__u8 *)data);
	return res;
#else
/*
 * For some reason the single color display is rotated one column, 
 * so pre-unrotate the data.
 */
	int i;
	__u16 block[I2C_SMBUS_BLOCK_MAX];
//	printf("rotating\n");
	for(i=0; i<8; i++) {
		block[i] = (data[i]&0xfe) >> 1 | 
			   (data[i]&0x01) << 7;
	}
	res = i2c_smbus_write_i2c_block_data(file, 0x00, 16, 
		(__u8 *)block);
	return res;
#endif
}

int pow(int raise);

int pow(int raise){
	int v = 1;
	while (raise > 0){
	   v = v*2;
	   raise--;
	}
	return v;
}

void draw(int** grid, int file, int delta);

void draw(int** grid, int file, int delta){
	
	int base = 2;
	int i = 0;
	int x = 0;
	int block[8];
	while (x < 8){
	   i = 0;
	   block[x] = 0; //clear for new inputs
	   printf("%d\n", grid[x]);
	   while (i < 8){
	      block[x] = block[x] + grid[x][i]*pow(i); //get grid for sending set up
	      //printf("block %d ", block[x]);
	      printf("%d ", grid[x][i]);
	      i++;
	   }
	   printf("\n");
	   x++;
	}
	
	if (delta == 1){ //set to orange
	   x = 0;
	   while (x < 8){
	      block[x] = block[x] + block[x]*256;
	      x++;
	   }
	}
	else if (delta > 1){
	   x = 0;
	   while (x < 8){
	      block[x] = block[x]*256;
	      x++;
	   }
	
	}
	
	printf("\n %d \n", 2^0);
	printf("%d \n \n", block[0]);
	
	char z[8];
	int a;
	__u16 blocko[8] = {block[0], block[1], block[2], block[3], block[4], block[5], block[6], block[7]};
	write_block(file, blocko);
	i = 0;
	
}

int main(int argc, char **argv, char **envp)
{
	
	int length, m;
	if (argc < 1) {
		printf("Usage: Grid width, Grid Height)\n\n");
		exit(-1);
	}
	
	signal(SIGINT, signal_handler);
	
	length = 4; //number of buttons
	struct pollfd fdset[length];
	
	int i, j;
	int **grid;
	int width = 8;
	int height = 8;
	char buf[MAX_BUF];
	int timeout, rc;
	int x, y;
	int gpio_fd[length];
	
	int res[3], address[3], file[3];
	address[0] = 0x70;
	address[1] = 0x48;
	address[2] = 0x49;
	
	int i2cbus;
	char filename[20];
	int force = 0;
	int daddress = 0;
	
	int nfds = length;
	int len;
	timeout = POLL_TIMEOUT;
	
	unsigned int but[5];
	but[0] = 50; //0->1 p14
	but[1] = 60; //1->0 p12
	but[2] = 3; //1->0 p3
	but[3] = 2; //0->1 p2
	
	//i2c setup
	i2cbus = lookup_i2c_bus("1");
	printf("i2cbus = %d\n", i2cbus);
	if (i2cbus < 0)
		help();

	address[0] = parse_i2c_address("0x70");
	printf("address = 0x%2x\n", address);
	if (address[0] < 0)
		help();

	file[0] = open_i2c_dev(i2cbus, filename, sizeof(filename), 0);
	file[1] = open(filename, O_RDWR);
	file[2] = open(filename, O_RDWR); //temp sensors
	
	
//	printf("file = %d\n", file);
	if (file[0] < 0
	 || check_funcs(file[0])
	 || set_slave_addr(file[0], address[0], force))
		exit(1);
	
		// Check the return value on these if there is trouble
	i2c_smbus_write_byte(file[0], 0x21); // Start oscillator (p10)
	i2c_smbus_write_byte(file[0], 0x81); // Disp on, blink off (p11)
	i2c_smbus_write_byte(file[0], 0xe7); // Full brightness (page 15)
	if (ioctl(file[1], I2C_SLAVE, address[1]) < 0) { //setup 0
		fprintf(stderr,
			"Error: Could not set address to 0x%02x: %s\n",
			address[1], strerror(errno));
		return -errno;
	}
	
	if (ioctl(file[2], I2C_SLAVE, address[2]) < 0) {
		fprintf(stderr,
			"Error: Could not set address to 0x%02x: %s\n",
			address[2], strerror(errno));
		return -errno;
	}
	
	
	x = 0;
	y = 0; //start in corner, 30 31 48 49 for buttons
	grid = malloc(width * sizeof(int *));
	i = 0; 
	while (i < width) {
	   grid[i] = malloc(height * sizeof(int)); //create the 2d array
	   i++;
	}
	
	i = 0; 
	while (i < width) {
	   j = 0; 
	   while (j < height){
	      grid[i][j] = 0; //fill array with zeroes
	      j++;
	   }
	   i++;
	}
        grid[0][0] = 1; //mark starting position

	   
	i = 0; 
	while(i < length){
	   gpio_export(but[i]);
	   gpio_set_dir(but[i], "in");
	   gpio_set_edge(but[i], "rising");
	   gpio_fd[i] = gpio_fd_open(but[i], O_RDONLY);
	   i++;
	}
	
	int resb[2];
	resb[0] = i2c_smbus_read_byte_data(file[1], daddress);
	resb[1] = i2c_smbus_read_byte_data(file[2], daddress);
	   
	char input[1];
	int delta, deltal;
	deltal = 0;
	delta = 0;
	while (keepgoing) {
		memset((void*)fdset, 0, sizeof(fdset));
				    
		res[1] = i2c_smbus_read_byte_data(file[1], daddress);
		res[2] = i2c_smbus_read_byte_data(file[2], daddress); //read sensors, once base works properly use delta to determine color
		
		delta = res[2]-resb[1];
		//printf("%d \n" delta);
		if (delta != deltal){
		   draw(grid, file[0], delta);
		}
	
		fdset[0].fd = gpio_fd[0];
		fdset[0].events = POLLPRI;
		fdset[1].fd = gpio_fd[1];
		fdset[1].events = POLLPRI;
		fdset[2].fd = gpio_fd[2];
		fdset[2].events = POLLPRI;
		fdset[3].fd = gpio_fd[3];
		fdset[3].events = POLLPRI;
		   
		rc = poll(fdset, nfds, timeout);
		if (rc < 0) {
	           printf("\npoll() failed!\n");
		   return -1;
		}
		   
		   
		if (fdset[0].revents && POLLPRI && x < 7) {
		   lseek(fdset[0].fd, 0, SEEK_SET);  // Read from the start of
		   len = read(fdset[0].fd, buf, MAX_BUF);
		   grid[x][y] = 1;
		   x++;
		   grid[x][y] = 1;
		   draw(grid, file[0], delta);
		}
		if (fdset[1].revents && POLLPRI && x > 0) {
		   lseek(fdset[1].fd, 0, SEEK_SET);  // Read from the start of
		   len = read(fdset[1].fd, buf, MAX_BUF);
		   grid[x][y] = 1;
		   x--;
		   grid[x][y] = 1;
		   draw(grid, file[0], delta);
		}
		if (fdset[2].revents && POLLPRI && y < 7) {
		   lseek(fdset[2].fd, 0, SEEK_SET);  // Read from the start of
		   len = read(fdset[2].fd, buf, MAX_BUF);
		   
		   grid[x][y] = 1;
		   y++;
		   grid[x][y] = 1;
		   draw(grid, file[0], delta);
		}
		if (fdset[3].revents && POLLPRI && y > 0) {
		   lseek(fdset[3].fd, 0, SEEK_SET);  // Read from the start of
		   len = read(fdset[3].fd, buf, MAX_BUF);
		   grid[x][y] = 1;
		   y--;
		   grid[x][y] = 1;
		   draw(grid, file[0], delta);
		}
		
		int d2address;
		for(d2address = 0xef; d2address >= 0xef - 2*(res[1]-resb[0]); d2address--) {
//	    	   printf("writing: 0x%02x\n", daddress);
	    	   res[0] = i2c_smbus_write_byte(file[0], d2address);
	    	usleep(100000);	// Sleep 0.1 seconds
		} //should adjust based on temperature
		
		deltal = delta;
		usleep(100000);     
	     }
		   

    i = 0; 	
    while(i < length){
       gpio_fd_close(gpio_fd[i]);
       i++;
    }
    
    return 0;
}
		

