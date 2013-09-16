//gcc gpio-utils.h gpio-utils.c etch-sketch.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>	// Defines signal-handling functions (i.e. trap Ctrl-C)
#include "gpio-utils.h"

#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64

int keepgoing = 1;	// Set to 0 when ctrl-c is pressed

void signal_handler(int sig);
// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig)
{
	printf( "Ctrl-C pressed, cleaning up and exiting..\n" );
	keepgoing = 0;
}

void draw(int **grid, int width, int height);

void draw(int **grid, int width, int height){
	system("clear");
	int i = 0; 
	while(i < width){
	   int j = 0; 
	   while(j < height){
	      printf("%d ", grid[i][j]);
	      j++;
	   }
	   printf("\n");
	   fflush(stdout);
	   i++;
	}
	fflush(stdout);
	i++;
}

int main(int argc, char **argv, char **envp)
{
	
	int length, m;
	if (argc < 4) {
		printf("Usage: Grid width, Grid Height, Operating mode (0 = key, 1 = buttons)\n\n");
		exit(-1);
	}
	
	m = atoi(argv[3]);
	printf("m: %d \n", m);
	signal(SIGINT, signal_handler);
	
	if (m == 0){
	   length = 1;
	   printf("use wasd to control, pressing enter after each letter");
	}
	else if (m == 1){
	   length = 5;
	}
	else {
	   exit(-1);
	}
	struct pollfd fdset[length];
	
	int i, j;
	int **grid;
	int width = atoi(argv[2]);
	int height = atoi(argv[1]);
	char buf[MAX_BUF];
	int timeout, rc;
	int x, y;
	int gpio_fd[length];
	
	x = 0;
	y = 0; //start in corner, wasd controls for key, 30 31 48 49 for buttons
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
        grid[0][0] = 8; //mark starting position

	if (m == 1){
	   unsigned int but[5];
	   but[0] = 50; //0->1 p14
	   but[1] = 60; //1->0 p12
	   but[2] = 3; //1->0 p3
	   but[3] = 2; //0->1 p2
	   but[4] = //0 -> 1 p42
	   
	   i = 0; 
	   while(i < length){
	      gpio_export(but[i]);
	      gpio_set_dir(but[i], "in");
	      gpio_set_edge(but[i], "rising");
	      gpio_fd[i] = gpio_fd_open(but[i], O_RDONLY);
	      i++;
	   }
	   
	}


	int nfds = length;
	int len;
	timeout = POLL_TIMEOUT;
	
	
   	system("clear");
	i = 0; 
	while(i < width){
	   j = 0; 
	   while(j < height){
	      printf("%d ", grid[i][j]);
	      j++;
	   }
	   printf("\n");
	   fflush(stdout);
	   i++;
	}
	fflush(stdout);


	char input[1];
	while (keepgoing) {
		memset((void*)fdset, 0, sizeof(fdset));
	

				    
		if (m == 0){
		   fdset[0].fd = STDIN_FILENO;
		   fdset[0].events = POLLIN;
		   
		   rc = poll(fdset, nfds, timeout);
		   if (rc < 0) {
	              printf("\npoll() failed!\n");
		      return -1;
		   }
      
		   grid[x][y] = 1;
		   scanf("%s", &input);
		   if (strcmp("d", input) == 0 && y < height){
		      y++;
		      grid[x][y] = 8;
		   }
		   if (strcmp("w", input) == 0 && x > 0){
		      x--;
		      grid[x][y] = 8;
		   }
		   if (strcmp("a", input) == 0 && y > 0){
		      y--;
		      grid[x][y] = 8;
		   }
		   if (strcmp("s", input) == 0 && x < width){
		      x++;
		      grid[x][y] = 8;
		   }
		      
		   //printf("\033[%d;%d]", width, height);
		   system("clear");
		   i = 0; 
		   while(i < width){
		      j = 0; 
		      while(j < height){
		         printf("%d ", grid[i][j]);
		         j++;
		      }
		      printf("\n");
		      i++;
		   }
		   fflush(stdout);
		   
		   
		}
		
		else if (m == 1){
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
		   
		   
		   if (fdset[0].revents && POLLPRI && x < width) {
		      lseek(fdset[0].fd, 0, SEEK_SET);  // Read from the start of
		      len = read(fdset[0].fd, buf, MAX_BUF);
		      grid[x][y] = 1;
		      x++;
		      grid[x][y] = 8;
		      draw(grid, width, height);
		   }
		   if (fdset[1].revents && POLLPRI && x > 0) {
		      lseek(fdset[1].fd, 0, SEEK_SET);  // Read from the start of
		      len = read(fdset[1].fd, buf, MAX_BUF);
		      grid[x][y] = 1;
		      x--;
		      grid[x][y] = 8;
		      draw(grid, width, height);
		   }
		   if (fdset[2].revents && POLLPRI && y < height) {
		      lseek(fdset[2].fd, 0, SEEK_SET);  // Read from the start of
		      len = read(fdset[2].fd, buf, MAX_BUF);
		      grid[x][y] = 1;
		      y++;
		      grid[x][y] = 8;
		      draw(grid, width, height);
		   }
		   if (fdset[3].revents && POLLPRI && y > 0) {
		      lseek(fdset[3].fd, 0, SEEK_SET);  // Read from the start of
		      len = read(fdset[3].fd, buf, MAX_BUF);
		      grid[x][y] = 1;
		      y--;
		      grid[x][y] = 8;
		      draw(grid, width, height);
		   }
		   if (fdset[4].revents && POLLPRI) {
		      lseek(fdset[4].fd, 0, SEEK_SET);  // Read from the start of
		      len = read(fdset[4].fd, buf, MAX_BUF);
		      x  = 0;
		      y = 0;
		      i = 0; 
		      while(i < width){
		         j = 0; 
		         while(j < height){
		            grid[i][j] = 0;
		            j++;
		         }
		         i++;
		      }
		      grid[0][0] = 8;
		      draw(grid, width, height);
		   }
		      
		}
		   
 	     }

    i = 0; 	
    while(i < length){
       gpio_fd_close(gpio_fd[i]);
       i++;
    }
    
    return 0;
}
		

