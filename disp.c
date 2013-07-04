// File by Ian Kowalski, edit and modify as necassary for your own needs. displays Hi, ised as proof of display method
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> //for ctrl-c

#define MAX_BUF 64
#define WIDTH 7
#define HEIGHT 5
//input should range from 300 to 1200, but no garuntees. Still, thats the assumption used later
#define DELTA = 900;
#define DELTAW = 128 // 900/WIDTH 
#define DELTAH = 180 // 900/HEIGHT
#define MIN = 300

void signal_handler(int sig) //borrowed code
{
	int i;
	printf( "Ctrl-C pressed, cleaning up and exiting..\n" );
	keepgoing = 0;
	
}

int main(int argc, char* argv){ //expects no arguments

		signal(SIGINT, signal_handler);

	int grid[WIDTH][HEIGHT] = 0;
	//store pin choices
	int gpio[12];
	int gpiofd[12]; 
	//pins go from 1 to 12, ensure correct wring for output
	gpio[0] = 6; //p8_3 -> 12
	gpio[1] = 7; //p8_4 -> 11
	gpio[2] = 2; //p8_5 -> 2
	gpio[3] = 3; //p8_6 -> 9
	gpio[4] = 13; //p8_11 -> 4
	gpio[5] = 12; //p8_12 -> 5
	gpio[6] = 26; //p8_14 -> 6
	gpio[7] = 15; //p8_15 -> 1
	gpio[8] = 14; //p8_16 -> 3
	gpio[9] = 27; //p8_17 -> 10
	gpio[10] = 1; //p8_18 -> 7
	gpio[11] = 31; //p8_20 -> 8

	//initialize and set to outputs
	for(i = 0; i < 12; i++){
		gpio_export(gpio[i]); //export		
		gpio_set_dir(gpio[i], "out");
		gpio_fd[i] = gpio_fd_open(gpio[i], O_RDONLY);
	}
	
	//manually enter grid pts for display
	grid[0][0] = 1;
	grid[1][1] = 1;
	grid[2][2] = 1;
	grid[3][3] = 1;
	grid[4][4] = 1;
	grid[5][3] = 1;
	grid[6][2] = 1;
	
	
	while(1){
		display(grid, locs);
	}
}

int display (int grid[WIDTH][HEIGHT], int locs[12]){ //use digital outs
	for(i = 0; i < WIDTH; i++){
		//enable column
		gpio_set_value (locs[i], 1) //turn on column (note
		for(j = 0; j < HEIGHT; j++){
			//enable row
			gpio_set_value(locs[WIDTH+j], grid[i][j]);
		}
		usleep(50); //should allow for the screen to be visible reasonably well
		for(j = 0; j < HEIGHT; j++){
			gpio_set_value(locs[WIDTH+j], 0);  //disble rows
		}			
	    //disable column
		gpio_set_value (locs[i], 0)
	}
}

