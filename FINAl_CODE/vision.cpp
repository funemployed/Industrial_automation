
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include "image_transfer.h"
#include "vision.h"
#include "main_functions.h"

int copy(image *a, image *b)
// copy an image
// a or b can be either RGB or greyscale image types
// this function can be used to convert between different image types
// a - input
// b - output (copy)
{
	i4byte *pa_RGB, *pb_RGB, i, j, size;
	ibyte *pa_GREY, *pb_GREY, R, G, B;

	// check for size compatibility
	if( a->height != b->height || a->width != b->width ) {
		printf("\nerror in copy: sizes of a, b are not the same!");
		return 1;
	}

	// initialize pointers
	pa_RGB = (i4byte *)a->pdata;
	pb_RGB = (i4byte *)b->pdata;
	pa_GREY = a->pdata;
	pb_GREY = b->pdata;
	
	// number of pixels
	size = (i4byte)a->width * a->height;

	if( a->type==RGB_IMAGE && b->type==RGB_IMAGE ) {
		for(i=0;i<size;i++) pb_RGB[i] = pa_RGB[i];

	} else if (a->type==GREY_IMAGE && b->type==GREY_IMAGE) {
		for(i=0;i<size;i++) pb_GREY[i] = pa_GREY[i];

	} else if (a->type==RGB_IMAGE && b->type==GREY_IMAGE) {

		for(i=0,j=0;i<size;i++,j+=4) {
			B = pa_GREY[j];
			G = pa_GREY[j+1];
			R = pa_GREY[j+2];
			pb_GREY[i] = (ibyte)(0.299*R + 0.587*G + 0.114*B);
		}	

	} else if (a->type==GREY_IMAGE && b->type==RGB_IMAGE) {

		for(i=0,j=0;i<size;i++,j+=4) {
			pb_GREY[j]   = pa_GREY[i];
			pb_GREY[j+1] = pa_GREY[i];
			pb_GREY[j+2] = pa_GREY[i];
		}

	} else {
		printf("\nerror in copy: invalid types");
		return 1;
	}

	return 0;
}

int invert(image *a, image *b)
// invert an image
// a - input greyscale image
// b - output greyscale image
{
	i4byte size, i;
	ibyte *pa, *pb;

	// initialize pointers
	pa = a->pdata;
	pb = b->pdata;

	// check for compatibility of a, b
	if( a->height != b->height || a->width != b->width ) {
		printf("\nerror in invert: sizes of a, b are not the same!");
		return 1;
	}

	if ( a->type != GREY_IMAGE || b->type != GREY_IMAGE ) {
		printf("\nerror in invert: input types are not valid!");
		return 1;
	}

	// number of pixels
	size = (i4byte)a->width * a->height;

	// invert the image
	for(i=0;i<size;i++) pb[i] = 255 - pa[i];

	return 0;
}


int scale(image *a, image *b)
// scale the image intensity so that the pixels
// have the full range of intensity values (0-255)
// a, b - RGB or greyscale images
// a - input
// b - output
{
	i4byte size,i;
	ibyte *pa, *pb, min, max;

	// initialize pointers
	pa = a->pdata;
	pb = b->pdata;

	// check for compatibility of a, b
	if( a->height != b->height || a->width != b->width ) {
		printf("\nerror in scale: sizes of a, b are not the same!");
		return 1;
	}
	if( a->type != b->type ) {
		printf("\nerror in scale: types of a, b are not the same!");
		return 1;
	}

	min = 255;
	max = 0;

	if( a->type == RGB_IMAGE ) {
		size = (i4byte)a->width * a->height * 4; // number of bytes
		// find min and max pixel values
		for(i=0;i<size;i+=4) {
			if( pa[i] < min ) min = pa[i]; // B
			if( pa[i] > max ) max = pa[i]; // B
			if( pa[i+1] < min ) min = pa[i+1]; // G
			if( pa[i+1] > max ) max = pa[i+1]; // G
			if( pa[i+2] < min ) min = pa[i+2]; // R
			if( pa[i+2] > max ) max = pa[i+2]; // R
		}
		// scale the intensity
		for(i=0;i<size;i+=4) {
			pb[i]   = (ibyte)( 255.0*(pa[i]   - min)/(max - min) );
			pb[i+1] = (ibyte)( 255.0*(pa[i+1] - min)/(max - min) );
			pb[i+2] = (ibyte)( 255.0*(pa[i+2] - min)/(max - min) );
		}
	} else if( a->type == GREY_IMAGE ) {
		size = (i4byte)a->width * a->height; // number of bytes
		// find min and max pixel values
		for(i=0;i<size;i++) {
			if( pa[i] < min ) min = pa[i];
			if( pa[i] > max ) max = pa[i];
		}
		// scale the intensity
		for(i=0;i<size;i++) {
			pb[i] = (ibyte)( 255.0*(pa[i] - min)/(max - min) );
		}
	} else {
		printf("\nerror in scale: type is not valid!");
		return 1;
	}

//	printf("\nmin = %d, max = %d",min,max);
	
	return 0;
}



int convolution(image *a, image *b, int *k, double s)
// perform a 3 X 3 convolution filter with the following kernel
// k1 k2 k3
// k4 k5 k6
// k7 k8 k9
{
	i4byte size,i,j;
	ibyte *pa,*pa1,*pa2,*pa3,*pa4,*pa5,*pa6,*pa7,*pa8,*pa9,*pb;
	i2byte width,height;
	int x;

	// check for compatibility of a, b
	if( a->height != b->height || a->width != b->width ) {
		printf("\nerror in convolution: sizes of a, b are not the same!");
		return 1;
	}

	if ( a->type != GREY_IMAGE || b->type != GREY_IMAGE ) {
		printf("\nerror in convolution: input types are not valid!");
		return 1;
	}

	width  = a->width;
	height = a->height;

	// initialize pointers
	pa = a->pdata + width + 1;
	pb = b->pdata + width + 1;

	// set neighbourhood pointers
	// make sure they don't point outside of the images at the boundaries
	// when you use them
	// pa1 pa2 pa3
	// pa4 pa5 pa6
	// pa7 pa8 pa9

	pa1 = pa - width - 1;
	pa2 = pa - width;
	pa3 = pa - width + 1;
	pa4 = pa - 1;
	pa5 = pa;
	pa6 = pa + 1;
	pa7 = pa + width - 1;
	pa8 = pa + width;
	pa9 = pa + width + 1;

	// number of pixels to process
	size = (i4byte)a->width * a->height - 2*width - 2;

	// perform 3x3 convolution filter
	for(i=0;i<size;i++) {

		// set the center pixel equal to the weighted sum of the pixels
		x = k[1]*(*pa1) + k[2]*(*pa2) + k[3]*(*pa3) +
            k[4]*(*pa4) + k[5]*(*pa5) + k[6]*(*pa6) +
            k[7]*(*pa7) + k[8]*(*pa8) + k[9]*(*pa9);

		x = (int)(s*x); // apply the scale factor

		// check for out of range values
		if(x < 0)   x = 0;
		if(x > 255) x = 255;

		*pb = (ibyte)x;

		// increment pointers
		pa1++; pa2++; pa3++; pa4++; pa5++;
		pa6++; pa7++; pa8++; pa9++; pb++;

	}

	// initialize pointers
	pa = a->pdata;
	pb = b->pdata;

	// number of pixels
	size = (i4byte)a->width * a->height;

	// process the image borders
	for(i=0;i<width;i++) {
		pb[i] = pb[i+width]; // top
		pb[size-i-1] = pb[size-i-1-width]; // bottom
	}
	for(i=0,j=0;i<height;i++,j+=width) {
		pb[j] = pb[j+1]; // left
		pb[size-j-1] = pb[size-j-2]; // right
	}
	pb[0] = pb[width+1]; // top left corner
	pb[width-1] = pb[width-1+width-1]; // top right corner
	pb[size-width] = pb[size-width-width+1]; // bottom left corner
	pb[size-1] = pb[size-1-width-1]; // bottom right corner

	return 0;
}


int lowpass_filter(image *a, image *b)
{
	int k[10];
	double s;

	// define convolution kernel
	// k1 k2 k3
	// k4 k5 k6
	// k7 k8 k9

	k[1] = 1;
	k[2] = 1;
	k[3] = 1;
	k[4] = 1;
	k[5] = 1;
	k[6] = 1;
	k[7] = 1;
	k[8] = 1;
	k[9] = 1;

	s = 1.0/9;

	convolution(a,b,k,s);

	return 0;
}


int highpass_filter(image *a, image *b)
{
	int k[10];
	double s;

	// define convolution kernel
	// k1 k2 k3
	// k4 k5 k6
	// k7 k8 k9

	k[1] = -1;
	k[2] = -1;
	k[3] = -1;
	k[4] = -1;
	k[5] = 9;
	k[6] = -1;
	k[7] = -1;
	k[8] = -1;
	k[9] = -1;

	s = 1.0;

	convolution(a,b,k,s);

	return 0;
}


int gaussian_filter(image *a, image *b)
{
	int k[10];
	double s;

	// define convolution kernel
	// k1 k2 k3
	// k4 k5 k6
	// k7 k8 k9

	k[1] = 1;
	k[2] = 2;
	k[3] = 1;
	k[4] = 2;
	k[5] = 4;
	k[6] = 2;
	k[7] = 1;
	k[8] = 2;
	k[9] = 1;

	s = 1.0/16;

	convolution(a,b,k,s);

	return 0;
}


int threshold(image *a, image *b, int tvalue)
// binary threshold operation
// a - greyscale image
// b - binary image
// tvalue - threshold value
{
	i4byte size, i;
	ibyte *pa, *pb;

	// initialize pointers
	pa = a->pdata;
	pb = b->pdata;

	// check for compatibility of a, b
	if( a->height != b->height || a->width != b->width ) {
		printf("\nerror in threshold: sizes of a, b are not the same!");
		return 1;
	}

	if ( a->type != GREY_IMAGE || b->type != GREY_IMAGE ) {
		printf("\nerror in threshold: input types are not valid!");
		return 1;
	}

	// number of bytes
	size = (i4byte)a->width * a->height;

	// threshold operation
	for(i=0;i<size;i++) {
		if( pa[i] < tvalue ) pb[i] = 0;
		else pb[i] = 255;
	}

	return 0;
}

/*int ignore_check(image *a, image *orig)
{
	int i,size;
	ibyte *pa,*porig;

	size = (i4byte)a->width * a->height;

	pa = a->pdata;
	porig = orig->pdata;

	for(i=0;i<size;i++){
		if(*porig<180 && *(porig+1)<180 && *(porig+2)>220){
			*pa = 0;
		}
		if(*porig<50 && *(porig+1)>230 && *(porig+2)<50){
			*pa = 0;
		}
		pa++;
		porig+=4;
	}
	
	return 0;
}
*/
int histogram(image *a, double *hist, int nhist, double *hmin, double *hmax)
{
	int j;
	i4byte i,size;
	ibyte *pa,min,max;
	double dn,x1,x2,r,dmin,dmax;

	// initialize pointer
	pa = a->pdata;

	// number of pixels
	size = (i4byte)a->width * a->height;

	// find min/max
	max = 0; min = 255;
	for(i=0;i<size;i++) {
		if(*pa > max) max = *pa;
		if(*pa < min) min = *pa;
		pa++;
	}
	
	printf("\nmax = %d, min = %d",(int)max,(int)min);

	dmin = (double)min - 1.0e-6;
	dmax = (double)max + 1.0e-6;

	// return parameters
	*hmin = (double)min;
	*hmax = (double)max;

	dn = (dmax - dmin)/nhist;

	// initialize bins
	for(j=0;j<nhist;j++) hist[j] = 0.0;

	// initialize pointer
	pa = a->pdata;

	for(i=0;i<size;i++) {

		for(j=0;j<nhist;j++) {
			x1 = dmin + j*dn;
			x2 = x1 + dn;
			r = (double)(*pa);
			if( r >= x1 && r < x2 ) {
				hist[j]++;
				break;
			}
		}
		pa++;

	}
	
	return 0;
}


int dialate(image *a, image *b)
// four pixel neighbourhood dialation
{
	i4byte size,i,j;
	ibyte *pa,*pa1,*pa2,*pa3,*pa4,*pb,max;
	i2byte width,height;

	// check for compatibility of a, b
	if( a->height != b->height || a->width != b->width ) {
		printf("\nerror in dialate: sizes of a, b are not the same!");
		return 1;
	}

	if ( a->type != GREY_IMAGE || b->type != GREY_IMAGE ) {
		printf("\nerror in dialate: input types are not valid!");
		return 1;
	}

	width  = a->width;
	height = a->height;

	// initialize pointers
	pa = a->pdata + width;
	pb = b->pdata + width;

	// set neighbourhood pointers
	// make sure they don't point outside of the images at the boundaries
	// when you use them
	//		pa2
	//	pa3 pa	pa1
	//      pa4
	pa1 = pa + 1;
	pa2 = pa - width;
	pa3 = pa - 1;
	pa4 = pa + width;

	// number of pixels to process
	size = (i4byte)a->width * a->height - 2*width;

	// perform greyscale dialation filter
	for(i=0;i<size;i++) {
		max = 0;
		if( *pa  > max ) max = *pa;
		if( *pa1 > max ) max = *pa1;
		if( *pa2 > max ) max = *pa2;
		if( *pa3 > max ) max = *pa3;
		if( *pa4 > max ) max = *pa4;

		// set the center pixel equal to the maximum in the neighbourhood
		*pb = max;

		// increment pointers
		pa++; pa1++; pa2++; pa3++; pa4++; pb++;
	}

	// initialize pointers
	pa = a->pdata;
	pb = b->pdata;

	// number of pixels
	size = (i4byte)a->width * a->height;

	// erase the image borders
	for(i=0;i<width;i++) {
		pb[i] = 0; // top
		pb[size-i-1] = 0; // bottom
	}
	for(i=0,j=0;i<height;i++,j+=width) {
		pb[j] = 0; // left
		pb[size-j-1] = 0; // right
	}

	return 0;
}


int erode(image *a, image *b)
// four pixel neighbourhood erosion
// morphological function
{
	i4byte size,i,j;
	ibyte *pa,*pa1,*pa2,*pa3,*pa4,*pb,min;
	i2byte width,height;

	// check for compatibility of a, b
	if( a->height != b->height || a->width != b->width ) {
		printf("\nerror in erode: sizes of a, b are not the same!");
		return 1;
	}

	if ( a->type != GREY_IMAGE || b->type != GREY_IMAGE ) {
		printf("\nerror in erode: input types are not valid!");
		return 1;
	}

	width  = a->width;
	height = a->height;

	// initialize pointers
	pa = a->pdata + width;
	pb = b->pdata + width;

	// set neighbourhood pointers
	// make sure they don't point outside of the images at the boundaries
	// when you use them
	//		pa2
	//	pa3 pa	pa1
	//      pa4
	pa1 = pa + 1;
	pa2 = pa - width;
	pa3 = pa - 1;
	pa4 = pa + width;

	// number of pixels to process
	size = (i4byte)a->width * a->height - 2*width;

	// perform greyscale dialation filter
	for(i=0;i<size;i++) {
		min = 255;
		if( *pa  < min ) min = *pa;
		if( *pa1 < min ) min = *pa1;
		if( *pa2 < min ) min = *pa2;
		if( *pa3 < min ) min = *pa3;
		if( *pa4 < min ) min = *pa4;

		// set the center pixel equal to the maximum in the neighbourhood
		*pb = min;

		// increment pointers
		pa++; pa1++; pa2++; pa3++; pa4++; pb++;
	}

	// initialize pointers
	pa = a->pdata;
	pb = b->pdata;

	// number of pixels
	size = (i4byte)a->width * a->height;

	// erase the image borders
	for(i=0;i<width;i++) {
		pb[i] = 0; // top
		pb[size-i-1] = 0; // bottom
	}
	for(i=0,j=0;i<height;i++,j+=width) {
		pb[j] = 0; // left
		pb[size-j-1] = 0; // right
	}

	return 0;
}

int label_image(image *a, image *b, int *nlabels)
// labels a binary image
// labels go from 1 to nlabels
// a - binary GREY_SCALE image type (pixel values must be 0 or 255)
// b - LABEL_IMAGE type (pixel values are 0 to 65535)
{ 
	long k,startpixel,endpixel;
	i2byte i,j,nlabel,m;
	ibyte *k1,*k2,*k3,*k4,*k5,*k6,*ip;
	i2byte *gp,*gp2,*L,*L2;

	i2byte *Pgrid; // label image pointer
	i2byte Nobjects;
	i2byte Hdim,Vdim;
	long Isize;

	Pgrid = (i2byte *)b->pdata;
	Hdim = a->width;
	Vdim = a->height;
	Isize = (long)Hdim*Vdim;

	L = (i2byte *)malloc( 65535*sizeof(i2byte) );
	if(L==NULL) {
		printf("\nmemory allocation error in labelgrid::Label!");
	}
	for(k=0;k<65535;k++) L[k]=(i2byte)k;

	// remove 1st layer and 1st pixel of 2nd from binary image
	ip  = a->pdata;
	for(k=0;k<=Hdim;k++) *ip++ = 0;

	// set up pointers for 2 X 3 kernel

	// k1 k2 k3
	// k4 k5 k6
  
	gp = Pgrid + Hdim;
	k5 = a->pdata + Hdim; // start on 2nd row
	k1 = k5 - Hdim - 1;
	k2 = k5 - Hdim;
	k3 = k5 - Hdim + 1;
	k4 = k5 - 1;
	k6 = k5 + 1;
	startpixel = Hdim+1;
	endpixel = (long)Hdim*(Vdim-1)-1;
	nlabel = 0;

	// label 4-connected white objects
	for(k=startpixel;k<endpixel;k++) {

		k1++; k2++;	k3++; k4++; k5++; k6++; gp++;

		if(*k5) {

			if(*k2 && *k4) {
				i = *(gp-1); j = *(gp-Hdim); *gp = i;
				if(L[i]!=L[j]) {
					if (L[i]<L[j]) {
						L[L[j]] = L[i];
						L[j] = L[i];
					} else if(L[j]<L[i]) {
						L[L[i]] = L[j];
						L[i] = L[j];
					}
      			}
			} else if(*k2 && !*k4) {
      			*gp = *(gp - Hdim);
			} else if(!*k2 && *k4) {
				*gp = *(gp - 1);
			} else {
				if(*k3 && *k6) {
					*gp = *(gp - Hdim + 1);
				} else {
					*gp = ++nlabel;
					if(nlabel>65533) break;
				}
			}
		} 
		else *gp=0;
	}

	// find redundant labels
	for(k=nlabel;k>0;k--) {
		while(L[L[k]]!=L[k]) L[k]=L[L[k]];
 	}

	L2 = (i2byte *)malloc( (nlabel+1)*sizeof(i2byte) );

	if(L2==NULL) {
		printf("\nmemory allocation error in labelgrid::Label!");
	}
	for(k=0;k<=nlabel;k++) L2[k]=0;

	m = 0;
	for(k=1;k<=nlabel;k++) {
		if( L2[L[k]]==0 ) L2[L[k]] = ++m;
	}

	Nobjects = m;

	// resolve label redundancy
 	gp = Pgrid + Hdim;
	for(k=startpixel;k<endpixel;k++) {
		gp++;
		*gp = L2[L[*gp]];
	}

	// erase borders
	gp  = Pgrid;
	gp2 = Pgrid+Hdim-1;
	for(k=0;k<Vdim;k++) {
		*gp = 0;
		*gp2 = 0;
		gp += Hdim;
		gp2 += Hdim;
	}

	gp  = Pgrid;
	gp2 = Pgrid+(long)Hdim*(Vdim-1);
	for(k=0;k<Hdim;k++) {
		*gp++ = 0;
		*gp2++ = 0;
	}

	free(L);
	free(L2);

	b->nlabels = Nobjects;	

	*nlabels = Nobjects;

//	printf("\n# of objects: %d",Nobjects);

	return 0;
}

int difference(image *rgb, image *rgb0, image *diff)
{
	ibyte *p,*p0,*diffI;
	int H,W,i;
	H=(i4byte)(rgb->width);
	W=(i4byte)(rgb->height);
	p=(ibyte *)rgb->pdata;
	p0=(ibyte *)rgb0->pdata;
	diffI=(ibyte *)diff->pdata;
	for(i=0;i<(H*W);i++)
	{
		*diffI=abs(*p0-*p);
		*(diffI+1)=abs(*(p0+1)-*(p+1));
		*(diffI+2)=abs(*(p0+2)-*(p+2));
		diffI+=4;
		p+=4;
		p0+=4;
		
	}
	
return 0;
}

int centroid(image *a, image *label, int nlabel, double *ic, double *jc)
// calculate the greyscale centroid of a labelled object
// a - GREY_IMAGE type
// label - LABEL_IMAGE type (pixel values 0-65535)
// nlabel - label number
// ic - i centroid coordinate
// jc - j centroid coordinate
{
	ibyte *pa;
	i2byte *pl;
	i4byte i,j,width,height;
	double mi,mj,m,rho;

	// check for compatibility of a, label
	if( a->height != label->height || a->width != label->width ) {
		printf("\nerror in centroid: sizes of a, label are not the same!");
		return 1;
	}

	if ( a->type != GREY_IMAGE || label->type != LABEL_IMAGE ) {
		printf("\nerror in centroid: input types are not valid!");
		return 1;
	}

	pa = a->pdata;
	pl = (i2byte *)label->pdata;

	// number of pixels
	width  = a->width;
	height = a->height;

	mi = mj = m = 0.0;

	for(i=0;i<height;i++) {
		for(j=0;j<width;j++) {
			if( *pl == nlabel ) {
				rho = pa[i*width+j];
				m  += rho;
				mi += rho*i;
				mj += rho*j;
			}
			pl++;
		}
	}

	*ic = mi/m;
	*jc = mj/m;

	return 0;
}

int centroid2(image *a, image *label, int nlabel, double *ic, double *jc, double *area)
// calculate the greyscale centroid of a labelled object
// a - GREY_IMAGE type
// label - LABEL_IMAGE type (pixel values 0-65535)
// nlabel - label number
// ic - i centroid coordinate
// jc - j centroid coordinate
{
	ibyte *pa;
	i2byte *pl;
	i4byte i,j,width,height;
	double mi,mj,m,rho, count;

	// check for compatibility of a, label
	if( a->height != label->height || a->width != label->width ) {
		printf("\nerror in centroid: sizes of a, label are not the same!");
		return 1;
	}

	if ( a->type != GREY_IMAGE || label->type != LABEL_IMAGE ) {
		printf("\nerror in centroid: input types are not valid!");
		return 1;
	}

	pa = a->pdata;
	pl = (i2byte *)label->pdata;

	// number of pixels
	width  = a->width;
	height = a->height;

	mi = mj = m = 0.0;
	count=0;
	for(i=0;i<height;i++) {
		for(j=0;j<width;j++) {
			if( *pl == nlabel ) {
				rho = pa[i*width+j];
				m  += rho;
				mi += rho*i;
				mj += rho*j;
				count++;
			
			}
			pl++;
		}
	}
	*area=count;
	*ic = mi/m;
	*jc = mj/m;

	return 0;
}

int draw_point(image *a, int ip, int jp, int value)
// draw a point at pixel location (ip,jp)
{
	ibyte *pa;
	int i,j,w=2;

	// initialize pointer
	pa  = a->pdata;

	if ( a->type != GREY_IMAGE ) {
		printf("\nerror in draw_point: input type not valid!");
		return 1;
	}

	// limit out of range values
	if( ip < w ) ip = w;
	if( ip > a->height-w-1 ) ip = a->height-w-1;
	if( jp < w ) jp = w;
	if( jp > a->width-w-1 ) jp = a->width-w-1;

	for(i=-w;i<=w;i++) {
		for(j=-w;j<=w;j++) {
			pa[a->width*(ip+i)+(jp+j)] = value;
		}
	}

	return 0;
}

int draw_line(image a,double i1, double i2, double j1, double j2, int value)
{
	double X,Y,R,theta;
	int x,y,r;
	X=i2-i1;
	Y=j2-j1;
	R=sqrt(X*X+Y*Y);
	theta=atan2((j2-j1),(i2-i1));
	for(r=0;r<=(int)R;r++)
	{
	x=(int)(r*cos(theta)+i1);
	y=(int)(r*sin(theta)+j1);
	draw_point(&a,x, y, value);
	}

return 0;
}

int draw_line_theta(image a,double i1, double j1, double theta,double R, int value)
{
	int x,y,r;
	for(r=0;r<=(int)R;r++)
	{
	x=(int)(r*cos(theta)+i1);
	y=(int)(r*sin(theta)+j1);
	draw_point(&a,x, y, value);
	}

return 0;
}


double area(image *a, image *label, int nlabel)
{
	ibyte *pa;
	i2byte *pl;
	i4byte i,j,width,height,count;

	// check for compatibility of a, label
	if( a->height != label->height || a->width != label->width ) {
		printf("\nerror in area: sizes of a, label are not the same!");
		return 1;
	}

	if ( a->type != GREY_IMAGE || label->type != LABEL_IMAGE ) {
		printf("\nerror in centroid: input types are not valid!");
		return 1;
	}

	pa = a->pdata;
	pl = (i2byte *)label->pdata;

	width  = a->width;
	height = a->height;

	count=0;
	for(i=0;i<height;i++) {
		for(j=0;j<width;j++) {
			if( *pl == nlabel ) count++;
			pl++;
		}
	}
	return count/(double)(width*height)*100.0;
}

int find_the_colour(image *label, image *rgb1,int ic,int jc)
{
	i4byte size,height,width,i,no_p,R,G,B;
//	i4byte	Rmin,Gmin,Bmin,Rmax,Gmax,Bmax;
	i4byte Rf,Gf,Bf;
	i2byte *pl,label_No;
	ibyte *po;
	int output;
	height=(i4byte)rgb1->height;
	width=(i4byte)rgb1->width;
	
	size = height*width;
	pl = (i2byte *)label->pdata;
	po = rgb1->pdata;

	label_No=*(pl+ic*width+jc);

	R = G = B = 0;
	//Rmin = Gmin = Bmin = 255;
	//Rmax = Gmax = Bmax = 0;
	no_p=0;

	for(i=0;i<size;i++) {
		if(*pl == label_No) {
			B = B + (i4byte)*po;
			G = G + (i4byte)*(po+1);
			R = R + (i4byte)*(po+2);
/*			if( (i4byte)*po < Bmin ) Bmin = (i4byte)*po;
			if ( (i4byte)*po > Bmax ) Bmax = (i4byte)*po;
			if( (i4byte)*(po+1) < Gmin ) Gmin = (i4byte)*(po+1);
			if ( (i4byte)*(po+1) > Gmax ) Gmax = (i4byte)*(po+1);
			if( (i4byte)*(po+2) < Rmin ) Rmin = (i4byte)*(po+2);
			if ( (i4byte)*(po+2) > Rmax ) Rmax = (i4byte)*(po+2);
*/			no_p++;
		}
		pl++;
		po+=4;
	}

	Bf = (int)((double)B / (double)no_p);
	Gf = (int)((double)G / (double)no_p);
	Rf = (int)((double)R / (double)no_p);

/*	printf("\n--------");
	printf("\nBf=%d,",Bf);
	printf("\nGf=%d,",Gf);
	printf("\nRf=%d,",Rf);
	printf("\n--------");
///*/
	if( Bf<100 && Gf<70 && Rf<70 )
		output=4;
	else if( Bf>130 && Gf>130 && Rf>130 )
		output=5;
	else if( Rf>Gf && Rf>Bf )
		output=1;
	else if( Gf>Rf && Gf>Bf )
		output=2;
	else if( Bf>Gf && Bf>Rf )
		output=3;

	return output;
}

int Cubic_spline(int *r)
{
	double l[359],u[359],z[359],al[359],c[359],b[359],d[359];
	
double hi=1.0;
	l[0]=1;
	u[0]=0;
	z[0]=0;
	for (int i=1; i<=358;i++)
	{
	al[i]=(3/hi)*(r[i+1]-r[i])-(3/hi)*(r[i]-r[i-1]);
	l[i]=2*(1)-hi*u[i-1];
	u[i]=hi/l[i];
	z[i]=(al[i]-hi*z[i-1])/l[i];
	}
	l[359]=1;
	z[359]=0;
	c[359]=0;
	for (int j=358;j>=0;j--)
	{
	c[j]=z[j]-u[j]*c[j+1];
	b[j]=(r[j+1]-r[j])/hi-hi*(c[j+1]+2*c[j])/3;
	d[j]=(c[j+1]-c[j])/(3*hi);
	}
	for(j=1;j<=358;j++)
	{
	r[j]=int(r[j]+b[j]+c[j]+d[j]);
	}

//S(x)=aj+bj(x-xj)+cj(c-cj)^2+dj(x-xj)^3
	return 0;
}

int Find_max(int *r)
{ 
	int max_r,i;
	
	max_r=0;
	for (i=0;i<=359;i++)
	{
		if(max_r<r[i]) max_r=r[i];
	}

	return max_r;
}


int Find_min(int *r)
{ 
	int min_r,i;
	
	min_r=100;
	for (i=0;i<=359;i++)
	{
		if(min_r>r[i]) min_r=r[i];
	}

	return min_r;
}


double second_order_area(image *a, image *label, int nlabel, double ic, double jc)
// calculate the greyscale centroid of a labelled object
// a - GREY_IMAGE type
// label - LABEL_IMAGE type (pixel values 0-65535)
// nlabel - label number
// ic - i centroid coordinate
// jc - j centroid coordinate
{
	ibyte *pa;
	i2byte *pl;
	i4byte i,j,width,height;
	double mi,mj,m,rho, count,theta;
	double m11=0.0,m02=0.0,m20=0.0;
	// check for compatibility of a, label
	if( a->height != label->height || a->width != label->width ) {
		printf("\nerror in centroid: sizes of a, label are not the same!");
		return 1;
	}

	if ( a->type != GREY_IMAGE || label->type != LABEL_IMAGE ) {
		printf("\nerror in centroid: input types are not valid!");
		return 1;
	}

	pa = a->pdata;
	pl = (i2byte *)label->pdata;

	// number of pixels
	width  = a->width;
	height = a->height;

	mi = mj = m = 0.0;
	count=0;
	for(i=0;i<height;i++) {
		for(j=0;j<width;j++) {
			if( pl[i*width+j] == nlabel ) {
			m11=m11+(i-ic)*(j-jc);
			m02=m02+(j-jc)*(j-jc);
			m20=m20+(i-ic)*(i-ic);
			}

		}
	}
	

	theta=PI/2.0-0.5*atan2(2*m11,m02-m20);

	return theta;
}


int collision_check(image *label, int nlabel, int ic, int jc, double orient, double angle_diff, int option, int R)
{
	i2byte *pl;
	i4byte size,height,width,i,j;
	int r;
	double theta;

	height=(i4byte)label->height;
	width=(i4byte)label->width;
	size = height*width;

	pl = (i2byte *)label->pdata;

	if (option == 1) {

		for (theta=(orient-angle_diff)*180.0/PI;theta<(orient+angle_diff)*180.0/PI;theta+=5.0)
		{
			for (r=0;r<R;r++)
			{
				i=ic+int(double(r)*cos(double(theta)*PI/180.0));
				j=jc+int(double(r)*sin(double(theta)*PI/180.0));
				if (i < 0) i = 0;
				if (i > height) i = height;
				if (j < 0) j = 0;
				if (j > width) j = width;
				if (pl[i*width+j] == nlabel) return 1;				
				else if (i<1 || i >= height || j<1 || j >= width) return 2;
			}
		}
	}
	else if (option == 2) {

		for (theta=(orient+180.0-angle_diff)*180.0/PI;theta<(orient+180.0+angle_diff)*180.0/PI;theta+=5.0)
		{
			for (r=0;r<R;r++)
			{
				i=ic+int(double(r)*cos(double(theta)*PI/180.0));
				j=jc+int(double(r)*sin(double(theta)*PI/180.0));

				if (i < 0) i = 0;
				if (i > height) i = height;
				if (j < 0) j = 0;
				if (j > width) j = width;
				if (pl[i*width+j] == nlabel) return 1;
				else if (i<1 || i >= height || j<1 || j >= width) return 2;
			}
		}
	}

	return 0;
}