
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <wchar.h>
#include <rtapi.h>

#include "motor_control.h"

void motor_control(double *pulse_width)
// control motors
// pulse_width[8] - array that holds the control pulses (0 to 2.4 ms)
{
	int i;
	static int init = 0;
	static PUCHAR psdata;
	static HANDLE mhandle;
	CHAR name[] = "smem";
	double *pw;

	if( init == 0 ) {
		mhandle = RtOpenSharedMemory(
		PAGE_READWRITE, // read-write access
		FALSE, // ignored parameter
		name, // name of the shared memory
		(VOID **)&psdata // pointer to a virtual address of the shared memory
		);

		if (mhandle == NULL) printf("\nRtOpenSharedMemory failed.");
	
		init = 1;
	}

	pw = (double *)psdata;
	for(i=0;i<8;i++) pw[i] = pulse_width[i];

}
