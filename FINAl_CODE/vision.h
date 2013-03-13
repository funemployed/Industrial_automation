
#define PI 3.1415926538

int copy(image *a, image *b);

int scale(image *a, image *b);

int invert(image *a, image *b);

int convolution(image *a, image *b, int *k, double s);

int lowpass_filter(image *a, image *b);

int highpass_filter(image *a, image *b);

int gaussian_filter(image *a, image *b);

int threshold(image *a, image *b, int tvalue);

int ignore_check(image *a, image *rgb0);

int histogram(image *a, double *hist, int nhist, double *hmin, double *hmax);

int dialate(image *a, image *b);

int erode(image *a, image *b);

int label_image(image *a, image *b, int *nlabels);

int difference(image *rgb, image *rgb0, image *diff);

int centroid(image *a, image *label, int nlabel, double *ic, double *jc);

int centroid2(image *a, image *label, int nlabel, double *ic, double *jc, double *area);

int draw_point(image *a, int ip, int jp, int value);

int draw_line(image a, double i1, double i2, double j1, double j2, int value);

int robot_search(int nlabels);

double area(image *a, image *label, int nlabel);

int find_the_colour(image *label, image *rgb1,int ic,int jc);

int Cubic_spline(int *r);
int Find_min(int *r);
int Find_max(int *r);
double second_order_area(image *a, image *label, int nlabel, double ic, double jc);
int draw_line_theta(image a,double i1, double i2, double theta,double R, int value);
int collision_check(image *label, int nlabel, int ic, int jc, double orient, double angle_diff, int option, int R);
