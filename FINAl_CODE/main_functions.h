
#define RED 1
#define GREEN 2
#define BLUE 3
#define BLACK 4
#define WHITE 5

#define CIRCLE 1
#define SQUARE 2
#define RECTANGLE 3
#define UNKNOWN 4

#define OBJECT 1
#define OBSTACLE 2
#define ROBOT_BACK 3
#define ROBOT_FRONT 4
#define DESTINATION 5
#define NOTHING 6

typedef struct {
	double center[2];
	double area;
	int colour;
	int shape;
	double r_min;
	double r_max;
	int orient_point[2];
	double orient;
	int name;
	int NOO;
	double sratio;
	double step;
	double R2s;
	double R4s;
	double Rs;
} object;




int allocate_memory();

int image_processing();

int identify_objects();

int find_shape_orientation(image *label,int label_no,object *objectsS,int *r_value);
int shaper(double theta,object *ObjectsSS, int *r_value);
int robot_search();
int robot_search2();
int free_memory();
int free_objects();
