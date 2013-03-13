


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include "image_transfer.h"
#include "vision.h"
#include "main_functions.h"
#include "planning.h"
#include "motor_control.h"
#include <iostream>
#include <ctime>
#include <windows.h>
#include "timer.h"

object extern gold,avoid,robotback,robotfront,destiny, old_object, old_destination, old_avoid;
image extern a,b,rgb,rgb2,rgb_path,label;
double extern cali;
object extern Ident[255];
int extern TOTAL_OBJECTS;

int action_plan()
{

		P(1);

	return 0;

}
/// bezier( guide point 1,guide point 2,guide point 3)
int bezier(double *x, double *y, double *xg, double *yg, double *xt, double *yt)//,double *theta)
{ 
	const int DT = 100; // <-------------------------- TAKE CARE OF THIS!
	int i;
	double j;
	double dt;

	dt=1.0/double(DT);
	i=0;
	for (j=0;j<1;j+=dt)
	{
		xt[i] = pow(1.0-j,4.0)*x[0]+4.0*j*pow(1.0-j,3)*xg[0]+6.0*j*j*pow(1.0-j,2.0)*xg[1]
			+4.0*pow(j,3.0)*(1.0-j)*xg[2]+pow(j,4.0)*x[1];
		yt[i] = pow(1.0-j,4.0)*y[0]+4.0*j*pow(1.0-j,3)*yg[0]+6.0*j*j*pow(1.0-j,2.0)*yg[1]
			+4.0*pow(j,3.0)*(1.0-j)*yg[2]+pow(j,4.0)*y[1];
		

		i++;
	}




return 0;
}



int line(image *label,int nlabel)
{
	double X[3],Y[3];
	double Xd,Yd,Xd2,Yd2,theta,R,R2;
	int xL[1000],yL[1000];
	int x,y,r;
	i2byte *pl;
	i4byte size,height,width,i,j;
	int  fail,fail2,exit;


	height=(i4byte)label->height;
	width=(i4byte)label->width;
	size = height*width;

	pl = (i2byte *)label->pdata;
	X[0]=robotfront.center[0];
	Y[0]=robotfront.center[1];
	X[1]=old_object.center[0];
	Y[1]=old_object.center[1];
	X[2]=robotback.center[0];
	Y[2]=robotback.center[1];
	Xd=X[1]-X[0];
	Yd=Y[1]-Y[0];
	R=sqrt(Xd*Xd+Yd*Yd);
	theta=atan2((Y[1]-Y[0]),(X[1]-X[0]));
	if (abs(theta-robotfront.orient)*PI/180>45) fail=10;
	//const int RR=(int)R;

	for(r=0;r<=(int)R;r++)
	{
	i=(int)(r*cos(theta)+X[0]);
	j=(int)(r*sin(theta)+Y[0]);
	if (pl[i*width+j] == nlabel) { fail2= 1; break;}	
	draw_point(&a,i, j, 120);
	copy(&a,&rgb);
	
	}
	view_image(&rgb);
	exit=fail+fail2;
	return exit;

}



int P(int path)
{
	double errorX,errorY,errorT;
	double us,ux,uy,u,Kp=1.0/90.0, Kps=1.0/0.8;
	double Ki = 0.000, Kis = 0.000;
	double errorPx,errorPy,sumX,sumY,sumT,Xg,Yg;
	double t0;
	char ch;
	double pulse_width[8],pw_neutral[8],pw_max[8],pw_min[8];
	double pw_release,pw_grab,pw_gripper_initial,pw_gripper, pw_steer;
	double shiftr,shiftt;
	object goal;
	double tolerance = 17;
	double dx,dy,thetaC,shift_x,shift_y;
	double start_time,end_time,deltat = 0.01;
	double lasterror=0;
	sumX = 0.0;
	sumY = 0.0;
	sumT = 0.0;

	pw_max[1] = 1.55; // 1.9
	pw_max[2] = 2.2;
	pw_max[3] = 2.5;

	pw_min[1] = 1.46; // 1.9
	pw_min[2] = 1.3;
	pw_min[3] = 1.41;

	pw_neutral[1] = 1.52; //power
	pw_neutral[2] =1.9;	//steering
	pw_neutral[3] = 2.6;	//gripper

	pw_release = pw_max[3];
	pw_grab = pw_min[3];

	pulse_width[1] = pw_neutral[1] ;
	pulse_width[2] = pw_neutral[2] ;
	pulse_width[3] = pw_neutral[3] ;
	motor_control(pulse_width);

	

	if (path == 1) 
	{ 
		
		goal = old_object; pw_gripper_initial = pw_release; pw_gripper = pw_grab; 
		shift_x=5.0;
		shift_y=10.0;
//		shift_y=0; // <------ CHANGE THIS !!!
		shiftr = sqrt(shift_x*shift_x+shift_y*shift_y);
		shiftt = PI/2.0+atan2(shift_y,shift_x);
		tolerance = 26.0;
	}
	else if (path == 2) 
	{
		goal = old_destination; pw_gripper_initial = pw_grab; pw_gripper = pw_release; 
		
		shift_x=-1.0;
		shift_y=5.0;
//		shift_y=0; // <------ CHANGE THIS !!!
		shiftr = sqrt(shift_x*shift_x+shift_y*shift_y);
		shiftt = PI/2.0+atan2(shift_y,shift_x);
		tolerance = 30.0;
			}
	if (path == 3)
	{
		pulse_width[3]=pw_release;
		pulse_width[1]=pw_neutral[1];
		pulse_width[2]=pw_neutral[2] ;
		motor_control(pulse_width);
		wait(0.1);
		pulse_width[1]=pw_neutral[1];
		pulse_width[2]=pw_neutral[2];
		pulse_width[3]=pw_release;
		motor_control(pulse_width);
		wait(0.1);
		pulse_width[1]=pw_neutral[1]-(pw_max[1]-pw_neutral[1]);
		pulse_width[2]=pw_neutral[2];
		motor_control(pulse_width);
		wait(4);
	}
	if (path == 4)
	{

		pulse_width[1]=pw_neutral[1]-(pw_max[1]-pw_neutral[1]);
		pulse_width[2]=pw_max[2] -0.2;
		motor_control(pulse_width);
		wait(2);
	}

	printf("\n %f,%f",old_object.center[0],old_object.center[1]);
	pulse_width[3] = pw_gripper_initial;
	motor_control(pulse_width);

	if (path == 1 || path == 2)
	{
		while(1)
		{
			/*	for(int i=0;i<TOTAL_OBJECTS;i++)
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
	
				}
//*/


			start_time = high_resolution_time();
		
			acquire_image(&rgb);
			image_processing();
			identify_objects();		
			double robotshift[2];


			robotshift[0] = robotfront.center[0] + shiftr*cos(robotfront.orient + shiftt);
			robotshift[1] = robotfront.center[1] + shiftr*sin(robotfront.orient + shiftt);
			draw_point(&a, robotshift[0],robotshift[1],70);
			draw_point(&a, old_object.center[0],old_object.center[1],150);
			copy(&a,&rgb);
			draw_point(&a, old_destination.center[0],old_destination.center[1],150);
			copy(&a,&rgb);
			draw_point(&a, old_avoid.center[0],old_avoid.center[1],150);
			copy(&a,&rgb);
			view_image(&rgb);

			errorX = goal.center[0] - robotshift[0];
			errorY = goal.center[1] - robotshift[1];
			t0 = atan2(errorY,errorX);
			sumX += errorX*deltat;
			sumY += errorY*deltat;
			sumT += errorT*deltat;

				//anti saturation term for sum variable
			if ( Ki*sumX > 1.0 ) sumX = 1.0/(Ki+1e-7);
			if ( Ki*sumX < -1.0 ) sumX = -1.0/(Ki+1e-7);
			if ( Ki*sumY > 1.0 ) sumY = 1.0/(Ki+1e-7);
			if ( Ki*sumY < -1.0 ) sumY = -1.0/(Ki+1e-7);
			if ( Ki*sumT > 1.0 ) sumT = 1.0/(Ki+1e-7);
			if ( Ki*sumT < -1.0 ) sumT = -1.0/(Ki+1e-7);

		//	printf("\n old object=%f,%f",old_object.center[0],old_object.center[1]);
			errorT = t0 - robotfront.orient;
		//	printf("\nt0 = %lf , robotfront.orient = %lf , errorT = %lf",t0,robotfront.orient,errorT);
			
			ux = -1.0 * absolute(Kp * errorX + Ki * sumX);
			uy = -1.0 * absolute(Kp * errorY + Ki * sumY);
			us = Kps * errorT + Kis * sumT;
			u = (ux+uy) / 2.0;
		//	printf("\n robotfront.center[0]=%f",robotfront.center[0]);
		//	printf("\n robotfront.center[1]=%f",robotfront.center[1]);
		//	printf("\n ux=%f",ux);
		//	printf("\n uy=%f",uy);
			printf("\n u=%f",u);
			if(u>1) u=1;
			if(u<-1) u=-1;
			if(us>1) us=1;
			if(us<-1) us=-1;
			
			if((u<0.16 && u>0)|| (u>-0.16 && u<0)) 
			{
			if(u<0) u=-.15;
			if(u>0) u=.15;
			pulse_width[1] = pw_neutral[1] -(pw_max[1] - pw_neutral[1])*u;
			pulse_width[2] = pw_neutral[2] +(pw_max[2] - pw_neutral[2])*us;
			pulse_width[3] = pw_gripper_initial ;
			motor_control(pulse_width);

			}
			else {pulse_width[1] = pw_neutral[1] -(pw_max[1] - pw_neutral[1])*u;
			pulse_width[2] = pw_neutral[2] +(pw_max[2] - pw_neutral[2])*us;
			pulse_width[3] = pw_gripper_initial ;
			motor_control(pulse_width);
			}

			printf("\n the error is= %f",sqrt(errorX*errorX + errorY*errorY));
			
			lasterror=absolute(sqrt(errorX*errorX + errorY*errorY)-lasterror);
			printf("\n last error=%f",lasterror);

			if((sqrt(errorX*errorX + errorY*errorY) < tolerance)) 
					{
							printf("\n GRABBED");
							pulse_width[1] = pw_neutral[1] ;
							pulse_width[2] = pw_neutral[2] ;
							pulse_width[3] = pw_gripper ;
							motor_control(pulse_width);
							wait(1);

							break;

				}
			
//////////////////////////////

			if(1==collision(45.0*PI/180.0, 120))		
			{
					pulse_width[1] = pw_neutral[1] ;
					pulse_width[2] = pw_neutral[2];
					motor_control(pulse_width);
					pulse_width[1] = pw_neutral[1] -0.03 ;
					if(pulse_width[2]<pw_neutral[2]) pulse_width[2] = pw_min[2]+.3;
					if(pulse_width[2]>pw_neutral[2]) pulse_width[2] = pw_max[2]-.3;

					motor_control(pulse_width);
					wait(0.1);
					pulse_width[2] = pw_neutral[2];
					pulse_width[1]= pw_neutral[2]+0.5;
					motor_control(pulse_width);
				printf("\n Collision eminant");
				while(1)
				{
					double kp2=1/0.005;
					printf("\n avoiding  Collision");
					acquire_image(&rgb);
					//	save_image("a.bmp",&rgb);
					image_processing();
					identify_objects();
					view_image(&rgb);
					//if(1==collision_check(&label,  avoid.NOO,  robotfront.center[0],  robotfront.center[1], robotfront.orient,  45, 1, 25)) 
					if(1!=collision(32.0*PI/180.0, 120)) break;

					dx=avoid.center[0]-robotfront.center[0];
					dy=avoid.center[1]-robotfront.center[1];
					thetaC=atan2(dy,dx);
					if(thetaC<robotfront.orient)  errorT=(robotfront.orient-thetaC);
					if(thetaC>robotfront.orient)  errorT=(robotfront.orient-thetaC);
					//if(absolute(errorT)<15.0*PI/180.0) errorT = sign(errorT)*15.0*PI/180.0;
					us = kp2*errorT;
					//printf("\n us=%f",us);
					//printf("\n thetaC=%f",thetaC);
					//printf("\n orient=%f",robotfront.orient);
					//printf("\n errorT=%f",errorT);
					if(us>1) us=1;
					if(us<-1) us=-1;
					if(us<0) pw_steer=pw_min[2];
					if(us>0) pw_steer=pw_max[2];
					pulse_width[1] = pw_neutral[1] +0.03 ;
					pulse_width[2] = pw_neutral[2] +absolute(pw_steer*0.3 - pw_neutral[2])*us;
					motor_control(pulse_width);


				}
			
				
			}

/////////////////////////////
			printf("\n free of obstacle");

		//	printf("\n grabber=%f,steering=%f,left=%f,right=%f",pulse_width[4],pulse_width[1],pulse_width[5],pulse_width[6]);
			
			
	/*if(0!=collision_check(&label,avoid.NOO, (int)robotfront.center[0],(int)robotfront.center[1],robotfront.orient, 90, 1, int(5.0/cali)))
			{
							printf("\n obstacle avoidance go");
							pulse_width[1] = pw_neutral[1] + (pw_max[1] - pw_neutral[1])*us;
							pulse_width[5] = pw_neutral[5] + (pw_max[1] - pw_neutral[1])*u*0.5;
							pulse_width[6] = pw_neutral[6] - (pw_max[1] - pw_neutral[1])*u*0.5;
							motor_control(pulse_width);
							wait(100);

			}*/
			end_time = high_resolution_time();

		}
	}


				//		pulse_width[1] = pw_neutral[1] ;
				//		pulse_width[2] = pw_neutral[2] ;
				//		pulse_width[3] = pw_neutral[3] ;
				//		motor_control(pulse_width);
	
	return 0;
}

int collision(int theta,int R)
{
	double dx,dy,r,thetaC;

		
		dx=old_avoid.center[0]-robotfront.center[0];
		dy=old_avoid.center[1]-robotfront.center[1];
		r=sqrt(dx*dx+dy*dy);
		thetaC=atan2(dy,dx);
		if(r<R && absolute(thetaC-robotfront.orient)<absolute(robotfront.orient-theta) )return 1;

	return 0;
	}