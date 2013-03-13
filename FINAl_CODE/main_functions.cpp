
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include "image_transfer.h"
#include "vision.h"
#include "main_functions.h"
#include "motor_control.h"
const int IMAGE_WIDTH=640;
const int IMAGE_HEIGHT=480;

image extern a,b,rgb;
image extern a0,rgb1,rgb_path; 
image extern label;
int	extern tvalue,nlabels;
object extern objects[255];
object extern Ident[255];
object extern robotback,robotfront;
object extern gold;
object extern avoid;
object extern destiny;
image extern rgb0,diff;
int TOTAL_OBJECTS;
double extern calibrate,cali;
//double extern Identify[255][7];

int allocate_memory()
{
	a.type = GREY_IMAGE;
	a.width = IMAGE_WIDTH;
	a.height = IMAGE_HEIGHT;

	a0.type = GREY_IMAGE;
	a0.width = IMAGE_WIDTH;
	a0.height = IMAGE_HEIGHT;

	b.type = GREY_IMAGE;
	b.width = IMAGE_WIDTH;
	b.height = IMAGE_HEIGHT;

	rgb.type = RGB_IMAGE;
	rgb.width = IMAGE_WIDTH;
	rgb.height = IMAGE_HEIGHT;

	rgb1.type = RGB_IMAGE;
	rgb1.width = IMAGE_WIDTH;
	rgb1.height = IMAGE_HEIGHT;

	rgb_path.type = RGB_IMAGE;
	rgb_path.width = IMAGE_WIDTH;
	rgb_path.height = IMAGE_HEIGHT;

	rgb0.type = RGB_IMAGE;
	rgb0.width = IMAGE_WIDTH;
	rgb0.height = IMAGE_HEIGHT;

	diff.type = RGB_IMAGE;
	diff.width = IMAGE_WIDTH;
	diff.height = IMAGE_HEIGHT;

	label.type = LABEL_IMAGE;
	label.width = IMAGE_WIDTH;
	label.height = IMAGE_HEIGHT;

	allocate_image(&a);
	allocate_image(&a0);
	allocate_image(&b);
	allocate_image(&rgb);
	allocate_image(&rgb1);
	allocate_image(&rgb0);
	allocate_image(&label);
	allocate_image(&diff);
	allocate_image(&rgb_path);

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
int image_processing()
{	
	//motor_control(pulse_width);
	copy(&rgb,&rgb1);
	copy(&rgb,&a);
	// backup
	difference(&rgb,&rgb0,&diff);
	copy(&diff,&a);
	copy(&a,&rgb);
	save_image("adiff.bmp",&rgb);
	save_image("rgb1.bmp",&rgb1);
//	view_image(&rgb);
//	printf("\ndifference");
//	getch();
//*/

	scale(&a,&b);
	copy(&b,&a);
	copy(&a,&rgb);


	lowpass_filter(&a,&b);
	copy(&b,&a);
	copy(&a,&rgb);


	threshold(&a,&b,tvalue);
	copy(&b,&a);
	copy(&a,&rgb);

	erode(&a,&b);
	copy(&b,&a);
	copy(&a,&rgb);

	dialate(&a,&b);
	copy(&b,&a);
	dialate(&a,&b);
	copy(&b,&a);
	copy(&a,&rgb);



	return 0;

}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//JONATHAN PARISER APRIL 2012
///////////////////////////////////////////////////
int identify_objects()
{
	int i,j,k,colour,obst=254,r_theta[360];
	double xc,yc;
	double max_area;
	double count;
	label_image(&a,&label,&nlabels);
//	printf("\nnumber of labeled objects = %d",nlabels);
	max_area=0.000000;
	////////// clearing the values
		//motor_control(pulse_width);
/*		for(i=0;i<254;i++)
		{
			Ident[i].area=objects[i].area=0;
			Ident[i].center[0]=objects[i].center[0]=0;
			Ident[i].center[1]=objects[i].center[1]=0;
			Ident[i].colour=objects[i].colour=0;
			Ident[i].orient=objects[i].orient=0;
			Ident[i].name=objects[i].name=0;
			Ident[i].r_min=objects[i].r_min=0;
			Ident[i].shape=objects[i].shape=0;
			Ident[i].sratio=objects[i].sratio=0;
		}
		*/
		for(i=0;i<360;i++) r_theta[i]=0;
///////////////////////////////
	j=0; // set Ident array to the zeroeth
	if(nlabels>1)
	{
	for (i=1;i<=nlabels;i++)
	{
		centroid2(&a,&label,i,&xc,&yc,&count);
		objects[i].area=count;
		objects[i].center[0] = xc;
		objects[i].center[1] = yc;
		objects[i].NOO=i;
		objects[i].colour =find_the_colour(&label,&rgb1,(int)xc,(int)yc);
		find_shape_orientation(&label,i,&objects[i],r_theta);
		//printf("\norient=%f",objects[i].orient);
		objects[i].orient=second_order_area(&a,&label,i,xc,yc);
		// other shape identifier
		shaper(objects[i].orient,&objects[i],r_theta);
		//printf("\norient=%f",objects[i].orient*180.0/PI);
			// criteria for important objects
			//what is its area

		if(objects[i].area<50 || objects[i].area>4000) 
		{
			objects[i].name=6; 
			//printf("\nUNKNOWN  AREA=%f",objects[i].area);
		}
		//if it is a known shape
		else if(objects[i].shape==4)	
		{
			objects[i].name=6; 
			//printf("\n UNKNOWN SHAPE=%d",objects[i].shape);
		}
		else if(objects[i].colour==5) 
		{
		//	objects[i].name=6; 
		//	printf("\n UNKNOWN COLOUR=%d",objects[i].colour);
		}
		else if(objects[i].r_min<2)	
		{ 
			objects[i].name=6;
			//printf("\n UNKNOWN R_MIN=%f",objects[i].r_min);
		}
		else objects[i].name=0;
		if(objects[i].name==6)
		{
		//	draw_point(&a,objects[i].center[0],objects[i].center[1],200);
		//	copy(&a,&rgb);
		}
		if(objects[i].name!=6)
		{
			Ident[j].area=objects[i].area;
			Ident[j].center[0]=objects[i].center[0];
			Ident[j].center[1]=objects[i].center[1];
			Ident[j].colour=objects[i].colour;
			Ident[j].orient=objects[i].orient;

		////////////////////////////////
			Ident[j].R2s=objects[i].R2s;
			Ident[j].R4s=objects[i].R4s;
			Ident[j].Rs=objects[i].Rs;
			///////////

		//	Ident[j].orient_point[0]=objects[i].orient_point[0];
		//	Ident[j].orient_point[1]=objects[i].orient_point[1];
			Ident[j].r_min=objects[i].r_min;
			Ident[j].shape=objects[i].shape;
			Ident[j].sratio=objects[i].sratio;
			Ident[j].NOO=objects[i].NOO;
			//printf("\n OBJECT,NOO=%d,area=%f,colour=%d",Ident[j].NOO,Ident[j].area,Ident[j].colour);
				if(Ident[j].area>max_area)
					{
					max_area=Ident[j].area;
					obst=j;
					//printf("\n the biggest area is =%f",max_area);
					}

			j++;
		}

	}
	
	TOTAL_OBJECTS=j;
 Ident[obst].name=2; avoid=Ident[obst];
	calibrate=4.0*PI/Ident[obst].area;
	cali=sqrt(calibrate);
	//printf("\n cali=%f",calibrate);
	//printf("\n area is (%f,%f)",sqrt(calibrate)*480/12,sqrt(calibrate)*640/12);
	///printf("\n--ADJUSTED AREA--WITH CALI=%f",calibrate);
	int obj=254,dest=254;
	double min_area=1000,min_area_2=1000;
	
	for (i=0;i<TOTAL_OBJECTS;i++)
	{
		Ident[i].area=calibrate*Ident[i].area;
		if(Ident[i].colour<4) // wont take blue object
		{
			if(0.7<Ident[i].area && Ident[i].area<min_area)
			{
			min_area=Ident[i].area;
			obj=i;
			}
		}
		if(Ident[i].colour==4 && Ident[i].shape==1)
		{
			if(Ident[i].area<min_area_2 &&Ident[i].area>0.5 && 1.8>Ident[i].area)
			{
			min_area_2=Ident[i].area;
			dest=i;
			}
		}
	}
	if(obj!=254) { Ident[obj].name=OBJECT; gold=Ident[obj]; }
	if(dest!=254) { Ident[dest].name=5; destiny=Ident[dest]; }


double XXX,YYY,RRR;
XXX=gold.center[0]-avoid.center[0];
YYY=gold.center[1]-avoid.center[1];
RRR=sqrt(XXX*XXX+YYY*YYY);

//printf("\n calibrate*R=%f",RRR*calibrate);
//printf("\n calibrate* sqrt of R=%f",RRR*sqrt(calibrate));
//printf("\n size of field is (%f,%f)",480*sqrt(calibrate),640*sqrt(calibrate));



//
//*/
//printf("\n robot_search");
for(i=0;i<3;i++) robot_search2();

draw_line(a,robotback.center[0],robotfront.center[0],robotback.center[1],robotfront.center[1],100);
	
/*	for (i=0;i<TOTAL_OBJECTS;i++)
	{
		if(Ident[i].name!=4 && Ident[i].name!=3) draw_line_theta(a,Ident[i].center[0],Ident[i].center[1],Ident[i].orient,10,10);
	}
	*/
	}

	return 0;
}
///////////////////////////////////////////////////

int find_shape_orientation(image *label,int label_no,object *objectsS,int *r_value)
{
	int t,r,i,j,ic,jc,min,max;
	int min_angle;
	double sratio;
	i4byte height,width;
	i2byte *pl;
	FILE *fp;

	pl=(i2byte *)label->pdata;
 	height=(i4byte)label->height;
	width=(i4byte)label->width;

	ic=int(objectsS->center[0]);
	jc=int(objectsS->center[1]);

	min=100;
	for(t=0;t<360;t++)
	{
		for(r=0;r<100;r++)
		{
			i=(int)(double(r)*cos(double(t)*PI/180.0));
			j=(int)(double(r)*sin(double(t)*PI/180.0));
			if((ic+i)<0) i=0-ic;
			if((ic+i)>479) i=479-ic;
			if((jc+j)<0) j=0-jc;
			if((jc+j)>639) j=639-jc;
			if(pl[(ic+i)*width+(j+jc)]!=pl[ic*width+jc]) break;
			r_value[t]=r;
		}

		if(min>r_value[t])
		{
			min_angle=t;
			min=r_value[t];
		}	
	}

	t=min_angle+90;
	for(r=0;r<100;r++)
	{	
		i=(int)(double(r)*cos(double(t)*PI/180.0));
		j=(int)(double(r)*sin(double(t)*PI/180.0));
		if(pl[(ic+i)*width+(j+jc)]!=pl[ic*width+jc]) break;
	}
	//min=Find_min(r_value);
//	printf("\nthe Minimum distance to an edge is=%d",min);
//	printf("\nthe Minimum angle to an edge is=%d",min_angle);
	max=Find_max(r_value);
	
	sratio=double(max)/double(min);
	objectsS->r_min=Find_min(r_value);
	objectsS->sratio=sratio;
	
//	printf("\nthe shape ratio is=%f",sratio);

	objectsS->orient_point[0]=ic+i;
	objectsS->orient_point[1]=jc+j;
/*	objectsS->orient=min_angle+90;
	fp = fopen("r_shape.csv","w");
	for(j=0;j<360;j++) {
		if(j != 0) fprintf(fp,"\n");
		fprintf(fp,"%d , %d",j,r_value[j]);
		}
	fclose(fp);
*/
	return 0;
}

int robot_search(int TOTAL_OBJECTS)
{
	int i,j,k,store_j=254,store_i=254;
	double x,y,r,old_r=1000000,step;
	object robot[255];
	k=0;
	store_j=0;
	store_i=0;
	for (i=0;i<(TOTAL_OBJECTS);i++)
	{

		printf("\n will this pass area=%f,colour=%d",Ident[i].area,Ident[i].colour);
		if(9>Ident[i].area && Ident[i].area>4 && Ident[i].colour==4)
		{
			

			for (j=(i+1);j<(TOTAL_OBJECTS);j++)
			{
			printf("\n will this pass area=%f,colour=%d",Ident[j].area,Ident[j].colour);	
			if(9>Ident[j].area && Ident[j].area>4 && Ident[j].colour==4)
				{
					printf("\n passed i area=%f,colour=%d",Ident[i].area,Ident[i].colour);
					printf("\n passed j area=%f,colour=%d",Ident[j].area,Ident[j].colour);
			//	printf("\n pass 2=%d",j);
			//	printf("\n i=%d,j=%d",i,j);
				x=abs(Ident[j].center[0]-Ident[i].center[0]);
				y=abs(Ident[j].center[1]-Ident[i].center[1]);
				r=sqrt(x*x+y*y);
				//printf("\n r=%f,",r);
				if(r<old_r )
					{
					old_r=r;
					store_j=j;
					store_i=i;
				//	printf("\n store_j=%d,",store_j);
					
					}
				}
				//

			}
		}
		//

	}
 
if(store_i!=254 && store_j!=254)
{
//	printf("\n i got inside");

					if(Ident[store_j].area>Ident[store_i].area)
					{
					Ident[store_i].name=ROBOT_FRONT;
					Ident[store_j].name=ROBOT_BACK;
					robotfront=Ident[store_i];
					robotback=Ident[store_j];
					x=robotfront.center[0]-robotback.center[0];
					y=robotfront.center[1]-robotback.center[1];
					robotfront.orient = atan2(y,x);
					robotback.orient = atan2(y,x);
					step=sqrt(x*x+y*y);
					//printf("\nstep=%f",step);
					Ident[store_i].step=robotfront.step = step;
					Ident[store_j].step=robotback.step = step;
					Ident[store_i].orient=robotback.orient;
					Ident[store_j].orient=robotback.orient;
					}
					else
					{
					Ident[store_i].name=ROBOT_BACK;
					Ident[store_j].name=ROBOT_FRONT;
					robotfront=Ident[store_j];
					robotback=Ident[store_i];
					x=robotfront.center[0]-robotback.center[0];
					y=robotfront.center[1]-robotback.center[1];
					step=sqrt(x*x+y*y);
					//printf("step=%f",step);
					Ident[store_i].step=robotfront.step = step;
					Ident[store_j].step=robotback.step = step;
					robotfront.orient = atan2(y,x);
					robotback.orient = atan2(y,x);
					Ident[store_i].orient=robotback.orient;
					Ident[store_j].orient=robotback.orient;
					}
					draw_point(&a,Ident[store_i].center[0],Ident[store_i].center[1],20);
					draw_point(&a,Ident[store_j].center[0],Ident[store_j].center[1],50);
					copy(&a,&rgb);

}
	
	return 0;
}

int free_memory()
{
	free_image(&a);
	free_image(&a0);
	free_image(&b);
	free_image(&rgb);
	free_image(&rgb0);
	free_image(&rgb1);
	free_image(&label);
	free_image(&diff);
	free_image(&rgb_path);
return 0;
}

shaper(double theta,object *ObjectsSS, int *rvalue)
{


	double shift_theta2,shift_theta4;
	double R2s,R4s,Rs,Rt,R2,R4;
	//printf("\n theta=%f",theta);
	shift_theta2=theta+double(PI/2.0);
//	printf("\n theta2=%f",shift_theta2);
	shift_theta4=theta+double(PI/4.0);
//	printf("\n theta4=%f",shift_theta4);
	Rt=rvalue[(int)(theta*180.0/PI)];
	R2=rvalue[(int)(shift_theta2*180.0/PI)];
	R4=rvalue[(int)(shift_theta4*180.0/PI)];
	R2s=1.0*R2/Rt;
	R4s=1.0*R4/Rt;
	Rs=1.0*R4s/R2s;
	ObjectsSS->R2s=R2s;
	ObjectsSS->R4s=R4s;
	ObjectsSS->Rs=Rs ;
	if(.8<R4s<1.1) ObjectsSS->shape=1;
	if(R4s>1.1)ObjectsSS->shape=2;
	if(0.8>Rs)ObjectsSS->shape=3;
//	printf("\n R2s=%f",R2s);
	//printf("\n R4s=%f",R4s);
	//printf("\n Rs=%f",Rs);
	//if close to 1 circle
	
	// if big difference rectangle


	// less difference closer to square
	//getch();
	//*/
	return 0;
}



int robot_search2()
{
	int i,j,i_back=254,i_front=254;
	double x,y,r,old_r=1000000;
	for (i=0;i<nlabels;i++)
	{
	//	printf("\n will this pass area=%f,colour=%d",objects[i].area,objects[i].colour);
		if(objects[i].area>=700 && objects[i].shape==1 && objects[i].name==0 && objects[i].area<=1300)
		{
			for (j=(i+1);j<=nlabels;j++)
			{
			//	printf("\n will this pass area=%f,colour=%d",objects[j].area,objects[j].colour);
				if(objects[j].area>=700 && objects[j].shape==1 && objects[j].name==0 && objects[i].area<=1300)
				{
				//	printf("\n j passed area=%f,colour=%d",objects[j].area,objects[j].colour);
				//	printf("\n i passed area=%f,colour=%d",objects[i].area,objects[i].colour);
					x=objects[j].center[0]-objects[i].center[0];
					y=objects[j].center[1]-objects[i].center[1];
					r=sqrt(x*x+y*y);
					if(r<old_r)
					{
						old_r=r;
						if(objects[j].area>objects[i].area)
						{
							i_front=i;
							i_back=j;
						}
						else
						{
							i_front=j;
							i_back=i;
						}
					}
				}
			}
		}
	}

//	printf("\nTEST1 : i_front = %d , i_back = %d",i_front,i_back);
	if(i_back!=254 && i_front!=254)
	{
//	printf("\nTEST1 : i_front = %d , i_back = %d",i_front,i_back);
	objects[i_front].name=ROBOT_FRONT;
	objects[i_back].name=ROBOT_BACK;
	robotfront=objects[i_front];
	robotback=objects[i_back];
	robotfront.NOO=i_front;
	robotback.NOO=i_back;
//	printf("\nrobot_front.label_num = %d , robot_back.label_num = %d",robot_front.label_num,robot_back.label_num);

	x=robotfront.center[0]-robotback.center[0];
	y=robotfront.center[1]-robotback.center[1];
	robotfront.orient = robotback.orient = atan2(y,x);
//	printf("\nx = %lf , y = %lf , robot.orient = %lf degrees , robot_back.orient = %lf degrees",x,y,robot_front.orient*180.0/PI,robot_back.orient*180.0/PI);
	}
	return 0;
}