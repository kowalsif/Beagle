I currently have set up all the files from the homework, below is some information on them:

readTemp.sh: a simple shell script that takes an i2c bus and two addresses, 
and the reads the values there and converts them to fahrenheit before printing

tempSet.sh: a shell script which takes an i2c bus, register (determines if high or low temperature threshold), two addresses, and a temperature. Does nothing else. 

myi2cget.c: a c program (compile instrucitons below) That prints the temperature when the alarm pin goes off. It takes an i2c bus, two addresses, two gpio pins, a temperature, and a register. Unfortunatly, the
code in it for setting thresholds is hardwired to i2c addresses 0x48 and 0x49, high threshold of 27 degrees celcious. 

compile with: gcc myi2cget.c gpio-utils.c gpio-utils.h i2c-dev.h

etch-sketch-i2c.c: This program set up and etch-e-sketch in c using an 8x8 
led grid. It takes no arguments when called, and assumes that there are two temperature sensors hooked up at 0x48 and 0x49. Gpio pins 50, 60, 3, and 2 are used to control the buttons which move on the grid. Sadly, the current spot on the grid is not marked. The temperature sensors values are saved at the start, and when one of them increases in value it causes the color to change to either red or orange (if it cools down to the initial temperature it will return to green) based on how much it heats up. The other sensor causes a kind of blinking effect as the temperature increases. 

compile with: gcc etch-sketch-i2c.c i2cbusses.c i2cbusses.h gpio-utils.c gpio-utils.h

