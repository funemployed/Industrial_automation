
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

// include this for basic image transfer functions
#include "image_transfer.h"

int allocate_image(image *a)
// allocate image memory
{
	size_t size;

	// calculate the image size (in bytes)
	// 4 bytes per pixel for RGB (RGBA)
	if( a->type == RGB_IMAGE ) { // 4 bytes/pixel (B-G-R-A)
		size = (size_t)a->width * a->height * 4 + 8;
	} else if( a->type == GREY_IMAGE ) {
		// 1 byte/pixel (0-255)
		size = (size_t)a->width * a->height;
	} else if( a->type == LABEL_IMAGE ) { // 2 bytes/pixel (0-65535)
		size = (size_t)a->width * a->height * 2;
	} else {
		printf("\nerror in allocate_image - unknown image type");
		return 1;
	}

	// allocate size bytes of dynamic memory
	a->pdata = (ibyte *)malloc(size*sizeof(ibyte));

	if(a->pdata == NULL) {
		printf("\nmemory allocation error in allocate_image");
		return 1;
	}

	return 0;
}


int free_image(image *a)
// free image memory
{
	if(a->pdata == NULL) {
		printf("\nerror in free_image - memory already freed");
		return 1;
	}

	// free image memory so other functions can use it
	free(a->pdata);

	// set the pointer to NULL so we know the memory has been freed
	a->pdata = NULL; 
	
	return 0;
}

