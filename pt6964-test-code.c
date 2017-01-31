/**
*	pt6964-test-code
*	Copyright (C) 2017  Norbert Kiszka <norbert at linux dot pl>
*	This program is free software; you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation; either version 2 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License along
*	with this program; if not, write to the Free Software Foundation, Inc.,
*	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

// Connection to LPT port:
// DATA 1 - CLK
// DATA 2 - STB
// DATA 3 - DAT (DIO)

#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define BASEPORT 0x378

void reset(void)
{
	outb(0b00000100, BASEPORT+2);
	outb(0b00000010, BASEPORT);
}

void init(void)
{
	if(ioperm(BASEPORT, 3, 1))
	{
		perror("ioperm");
		exit(1);
	}
	
	reset();
}

void send_byte(unsigned char byte)
{
	unsigned char i = 0;
	
	outb(0b00000000, BASEPORT);
	usleep(20);
	
	for(i = 0; i < 8; i++)
	{
		if((byte >> i) & 1)
		{
			//printf("1");
			outb(0b00000100, BASEPORT);
			usleep(2);
			outb(0b00000101, BASEPORT);
			usleep(2);
			outb(0b00000100, BASEPORT);
		}
		else
		{
			//printf("0");
			outb(0b00000000, BASEPORT);
			usleep(2);
			outb(0b00000001, BASEPORT);
			usleep(2);
			outb(0b00000000, BASEPORT);
		}
	}
	usleep(10);
	outb(0b00000010, BASEPORT);
	usleep(10);
	//printf("\n");
}

void send_bytes(char *bytes, int count)
{
	unsigned i = 0, byte = 0;
	
	outb(0b00000000, BASEPORT);
	usleep(20);
	
	for(byte = 0; byte < count; byte++)
	{
		for(i = 0; i < 8; i++)
		{
			if((*(bytes+byte) >> i) & 1)
			{
				printf("1");
				outb(0b00000100, BASEPORT);
				usleep(2);
				outb(0b00000101, BASEPORT);
				usleep(2);
				outb(0b00000100, BASEPORT);
			}
			else
			{
				printf("0");
				outb(0b00000000, BASEPORT);
				usleep(2);
				outb(0b00000001, BASEPORT);
				usleep(2);
				outb(0b00000000, BASEPORT);
			}
		}
	}
	usleep(10);
	outb(0b00000010, BASEPORT);
	usleep(10);
	printf("\n");
}

int main(void)
{
	unsigned char i = 1, j = 0;
	char addr_start[] = {0b11000000, 0b00000000};
	char data[] = {0b10001010, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
	init();
	
	send_byte(0b00000011); // display mode
	send_byte(0b01000000); // data settings
	
	send_bytes(addr_start, 2);

#if 0
	// blink all leds
	while(1)
	{
		memset(data+1, 255, 14);
		send_bytes(data, 15);
		
		usleep(500000);
		
		memset(data+1, 0, 14);
		send_bytes(data, 15);
		
		usleep(500000);
	}
#else
	// increment display bytes one by one
	while(1)
	{
		data[i] = j;
		
		send_bytes(data, 15);
		
		i++;
		
		if(i >= 15)
		{
			i = 1;
			j++;
		}
		
		usleep(5000);
	}
#endif
	
	return 0;
}
