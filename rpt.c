// File by Ian Kowalski, edit and modify as necassary for your own needs. 
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_BUF 64
#define WIDTH 7
#define HEIGHT 5
//input should range from 300 to 1200, but no garuntees. Still, thats the assumption used later
#define DELTA = 900;
#define DELTAW = 128 // 900/WIDTH 
#define DELTAH = 180 // 900/HEIGHT
#define MIN = 300

int main(int argc, char* argv){ //expects no arguments

	char PT1[] = "AIN4", PT2[] = "AIN6";
	int PT1_val, PT2_val;
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

	while(1){ //run forever
		PT1_val = analogIn(PT1); //ensure correct input (seemed to hold values for one)
		PT1_val = analogIn(PT1);
		PT2_val = analogIn(PT2); //ensure correct input
		PT2_val = analogIn(PT2);
		conversion(PT1_val, PT2_val, &grid);
		display(grid, locs);
	}
	
	//close up
	for (i = 0; i < 12; i++){
		gpio_fd_close(gpio_fd[i]);
	}
	return 0; //normal exit
}


int analogIn(char *ain) {
	FILE *fp;
	char ainPath[MAX_BUF];
	char ainVal[MAX_BUF];
	
//change paths as needed
    	snprintf(ainPath, sizeof ainPath, "/sys/devices/ocp.2/helper.14/%s", ain);

	if((fp = fopen(ainPath, "r")) == NULL){
		printf("Can't open this pin, %s\n", ain);
	return 1;
	}
	printf("value read is: %d", ainVal);
	fgets(ainVal, MAX_BUF, fp);

	fclose(fp);
	return atoi(ainVal); //str->int		
}

void conversion(int val0, int val1, int* grid){
	//val0 is for width, val1 is for height, grid is matrix
	val0 = val0-MIN;
	val1 = val1-MIN;
	grid[val0%DELTAW][val2%DELTAH] = 1; //turn the spot on
}



int display (int grid[WIDTH][HEIGHT], int locs[12]){ //use digital outs
	for(i = 0; i < WIDTH; i++){
		//enable column
		gpio_set_value (locs[i], 1) //turn on column (note
		for(j = 0; j < HEIGHT; j++){
			//enable row
			gpio_set_value(locs[WIDTH+j], grid[i][j]);
		}
		for(j = 0; j < HEIGHT; j++){
			gpio_set_value(locs[WIDTH+j], 0);  //disble rows
		}			
	//disable column
		gpio_set_value (locs[i], 0)
	}
}

