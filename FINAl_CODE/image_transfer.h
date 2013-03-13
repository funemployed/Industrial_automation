/* 

Basic image transfer functions.

These functions use the Vision SDK to transfer images
between different parts of the computer.  They include

1. Transfer of an image from a video source into memory

2. Transfer of an image in memory to a window created by image_view.exe

3. Loading and saving images from memory into files

These functions have the advantage of being easier to
use than the Vision SDK.

*/

// maximum image size (in bytes)
#define MAX_IMAGE 1230000

// image types
#define RGB_IMAGE 1
#define GREY_IMAGE 2
#define LABEL_IMAGE 3

// define a variable type that holds a single byte of data
typedef unsigned char ibyte;
typedef unsigned short int i2byte;
typedef unsigned long int i4byte;

// define a structure that stores an image
typedef struct {
	int type;   // image type
	i2byte height; // image height
	i2byte width;  // image width
	ibyte *pdata; // pointer to image data
	i2byte nlabels;  // number of labels (used for LABEL_IMAGE type)
} image;

// allocate image memory
int allocate_image(image *a);

// free image memory
int free_image(image *a);

// initialize Vision SDK library
int init_vissdk();

// acquire image from a video source
int acquire_image(image *a);

// view the image in the window created by view_image.exe
int view_image(image *a);

// save the image into a bitmap (*.bmp) file
int save_image(char *file_name, image *a);

// load the image from a bitmap (*.bmp) file
int load_image(char *file_name, image *a);
