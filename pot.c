#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //usleep so the display can be bloody read a little
#include <signal.h> //for ctrl-c

//
#define DELTA = 900;
#define DELTAW = 128 // 900/WIDTH 
#define DELTAH = 180 // 900/HEIGHT

int run = 1;	// Set to 0 when ctrl-c is pressed

void signal_handler(int sig) //borrowed code
{
	int i;
	printf( "Ctrl-C pressed, cleaning up and exiting..\n" );
	keepgoing = 0;
	
}

int main(int argc, char* argv){ //expects no arguments

	char PT1[] = "AIN4", PT2[] = "AIN6";
	int PT1_val, PT2_val;
	
	signal(SIGINT, signal_handler);
	
	while(run == 1){ //run until halted
		PT1_val = analogIn(PT1); //ensure correct input (seemed to hold values)
		PT1_val = analogIn(PT1);
		PT2_val = analogIn(PT2); //ensure correct input
		PT2_val = analogIn(PT2);
		printf("Value one is: %d, Value two is: %d", PT1_val, PT2_val);
		usleep(50000); //sleep for 50,0000us, so 0.5s 
	}
	
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
