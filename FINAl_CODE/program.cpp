
// vision example showing how to dynamically track the
// centroid of an object

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include "image_transfer.h"
#include "vision.h"
#include "main_functions.h"
#include "planning.h"
//#include "robot.h"
// motor control functions
#include "motor_control.h"
#include <iostream>
#include <ctime>

image a,b,rgb,rgb_path;
image a0,rgb1;
image label;
int	tvalue = 75,nlabels; // higher threshold less objects
object objects[255];
object Ident[255];
object robotback,robotfront;

object gold, old_object;
object avoid, old_avoid;
object destiny, old_destination;
double calibrate,cali;

image rgb0,diff;
image rgb2;
int extern TOTAL_OBJECTS;
char ch;
double extern X_Plan[1000],Y_Plan[1000];
const int DT=1000;
double extern xt[DT],yt[DT];
double X_Plan[DT],Y_Plan[DT];
int extern TOTAL_OBJECTS;


int main(int argc, char* argv[])
{ 
	clock_t s1,s2;
	s1= s2=clock();
	int i;
	double pulse_width[8],OUT_T,OUT_P,j,k;
	FILE *fp;
	printf("\n LEEERRRRRRROOOOOOOOOOOOOOOOYYYYYYYYYYYYYYYYY   JJJJEEEEEEEEENNNNNNNNNNNKKKKKKKIIIIIINNNNNNSSSSSS");
	printf("\n when even the best laid plans fail");

	init_vissdk();
	printf("\npress any key to begin");
	allocate_memory();
	getch();
	s1=clock();
	printf("\npress x to get a background");
	load_image("rgb0.bmp",&rgb0);
	view_image(&rgb0);
	
	ch=getch();
// background shot
	if(ch=='x')
	{
	acquire_image(&rgb0);
	save_image("rgb0.bmp",&rgb0);
    load_image("rgb0.bmp",&rgb0);
	view_image(&rgb0);
	printf("\nready to continue?");
	acquire_image(&rgb0);
	save_image("rgb0.bmp",&rgb0);
    load_image("rgb0.bmp",&rgb0);
	view_image(&rgb0);
	printf("\n thats the background");
	getch();
	}
//*/
	// first shot to figure out the colour and shape and premodial path 
	acquire_image(&rgb);
	Sleep(100);
	load_image("rgb0.bmp",&rgb0);
	view_image(&rgb);
	save_image("a.bmp",&rgb);	
	image_processing();
	acquire_image(&rgb);
	Sleep(100);
	load_image("rgb0.bmp",&rgb0);
	view_image(&rgb);
	save_image("a.bmp",&rgb);	
	image_processing();
	view_image(&rgb);
	identify_objects();
	copy(&a,&rgb);
	save_image("aout.bmp",&rgb);
	view_image(&rgb);
	printf("\n TOTAL_OBJECTS=%d",TOTAL_OBJECTS);
	for(i=0;i<TOTAL_OBJECTS;i++)
	{
		
	
		//printf( "\n --------The OBJECT IS------");
		printf( "\n number of object=%d",Ident[i].NOO);
		printf( "\n object shape (1=CIRCLE,2=SQUARE,3=RECTANGlE) =%d",Ident[i].shape);
		printf( "\n object area (inches^2) =%f",Ident[i].area);
		printf( "\n object colour (1=RED,2=GREEN,3=BLUE) =%d",Ident[i].colour);
		printf( "\n object min r =%f",Ident[i].r_min);
		printf( "\n object center X =%f",Ident[i].center[0]);
		printf( "\n object center Y =%f",Ident[i].center[1]);
		printf( "\n object name(OBJ=1,OBS=2,RB=3,RF=4,DES=5)=%d",Ident[i].name);
		printf( "\n object sratio=%f",Ident[i].sratio);
		printf( "\n object orient=%f",Ident[i].orient);
		printf( "\n object name=%d",Ident[i].name);
		printf( "\n --------------");
		draw_point(&a,(int)Ident[i].center[0],(int)Ident[i].center[1],120);
		if(Ident[i].name==1) old_object=Ident[i];
		if(Ident[i].name==2) old_avoid=Ident[i];
		if(Ident[i].name==5) old_destination=Ident[i];
		copy(&a,&rgb);
	
	}
	view_image(&rgb);
	getch();

	for(int path=1;path<5;path++)
	{
	P(path);

	}

		acquire_image(&rgb);
			image_processing();
			identify_objects();
			printf("\n check x=%f, y=%f", old_destination.center[0]-gold.center[0],old_destination.center[1]-gold.center[1]);
			view_image(&rgb);
			getch();
	 printf("\n after plan");
	//if object is circle/square/rectangle create diffrent methods 
		save_image("aout.bmp",&rgb);
		view_image(&rgb);

 

//////////////////  to sum things up

	printf("\n TOTAL_OBJECTS=%d",TOTAL_OBJECTS-1);
	for(i=0;i<TOTAL_OBJECTS;i++)
	{
	printf( "\n --------------");
	printf( "\n number of object=%d",Ident[i].NOO);
	printf( "\n object shape =%d",Ident[i].shape);
	printf( "\n object area =%f",Ident[i].area);
	printf( "\n object colour =%d",Ident[i].colour);
	printf( "\n object min r =%f",Ident[i].r_min);
	printf( "\n object center X =%f",Ident[i].center[0]);
	printf( "\n object center Y =%f",Ident[i].center[1]);
	printf( "\n object name=%d",Ident[i].name);
	printf( "\n object sratio=%f",Ident[i].sratio);
	printf( "\n object orient=%f",Ident[i].orient*180.0/PI);
	draw_point(&a,(int)Ident[i].center[0],(int)Ident[i].center[1],Ident[i].name*10+70);
	draw_line_theta(a,Ident[i].center[0],Ident[i].center[1],Ident[i].orient,10,120);
	printf( "\n --------------");
	copy(&a,&rgb);
	}
	view_image(&rgb);
	printf("\n\ndone.\n");

	getch();

	free_memory();
 	return 0;
}












