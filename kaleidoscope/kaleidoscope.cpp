#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <glut.h>
#include <FreeImage.h>

// This program is a kaleidoscope painter program using OpenGL and GLUT to draw mirrored points on a canvas.

float radius = 5.0f;
float delay = 1.0f;
float r = 0.0f, g = 0.0f, b = 0.0f;
int lastX = -1, lastY = -1;
bool automatic = false;
int c_width = 800, c_height = 800;
static int last_x = c_width/2;
static int last_y = c_width/2;

typedef struct {int r, g, b;} pixel;

// draws a point at (x, y) with radius r
void pen(int x, int y, float radius) {
	glPointSize(radius);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

// draws a point and its 7 mirrored points
void draw(int x, int y) {
	pen(x, y, radius);
	pen(y, x, radius);
	pen(c_width - x, y, radius);
	pen(y, c_height - x, radius);
	pen(c_width - y, x, radius);
	pen(x, c_height - y, radius);
	pen(c_width - x, c_height - y, radius);
	pen(c_width - y, c_height - x, radius);
}

// chooses random color
void rand_color() {
	// new color each time
	r = (float)rand() / (float)RAND_MAX;
	g = (float)rand() / (float)RAND_MAX;
	b = (float)rand() / (float)RAND_MAX;
	glColor3f(r, g, b);
}

// called when mouse is moved
void motion(int x, int y) {
	draw(x, c_height - y);
}

// formats the prints with the table edges
void padprint(const char* str) {
	int len = strlen(str);
	int pad = 70 - len;
	printf("| ");
	printf("%s", str);
	for (int i = 0; i < pad; i++)
		printf(" ");
	printf("|\n");
}

// auto function
// draws a random point every 2ms and deviates from the last point by a random amount
void idle() {
	if (automatic) {
		static int last_time = 0;
		int current_time = glutGet(GLUT_ELAPSED_TIME);

        if (current_time - last_time > 2) {
            last_time = current_time;

			int x_inc = 0;
			int y_inc = 0;

			// picks to affect x or y
			int rand_num = rand() % 2; 
			if (rand_num == 0) 
            	x_inc = (rand() % 20) - 10; // random number between -10 and 10
			else
            	y_inc = (rand() % 20) - 10; // random number between -10 and 10

			int x = last_x + x_inc;
            int y = last_y + y_inc;

			// draw point
            draw(last_x + x_inc, last_y + y_inc);
			last_x = x;
            last_y = y;

			// brings point back to center to loop auto mode
			if (x < 0 || x > c_width || y < 0 || y > c_height) {
				last_x = c_width/2;
				last_y = c_width/2;
			}
			glFlush();
		}
	}
}

// saves image to file wiht FreeImage - based on snippet from https://stackoverflow.com/a/43624755 and documentation (PDF) https://freeimage.sourceforge.io/documentation.html 
void save_image() {
	BYTE *pixels = (BYTE*)malloc(c_width * c_height * 3);

	glReadPixels(0, 0, c_width, c_height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);

	// -8 to fix the offset
	FIBITMAP *image = FreeImage_ConvertFromRawBits(pixels, c_width-8, c_height-8, 3 * c_width, 24, 0xf00, 0x0f0, 0x00f, false);

	if (FreeImage_Save(FIF_PNG, image, "kaleidoscope.png", 0))
	    padprint("Image saved!");
	else 
		padprint("Image failed to save!");

	// free mem
	FreeImage_Unload(image);
	free(pixels);
}

// prints controls to terminal
void showcmds() {
	printf("|-----------------------------------------------------------------------|\n");
	printf("| H: Help                 KALEIDOSCOPE PAINTER            ESC / Q: Quit |\n");
	printf("|------------------------------ CONTROLS ------------------------------ |\n");
	printf("| R: Random colour | E: Random and freeze | F: Freeze colour | C: Clear |\n");
	printf("|  S: Size-- | W: Size++ | M: Auto | P: Save | A: Speed-- | D: Speed++  |\n");
	printf("|-----------------------------------------------------------------------|\n");
}

// handles keyboard events
void keyboard(unsigned char key, int x, int y) {
	switch (tolower(key)) {
	case 'w': // increase size
		radius += 1.0f;
		break;
	case 's': // decrease size
		if (radius > 1.0f)
			radius -= 1.0f;
		break;
	case 'a': // decrease speed
		if (delay > 1.0f)
			delay -= 1.0f;
		break;
	case 'd': // increase speed
		delay += 1.0f;
		break;
	case 'f': // freeze color
		delay = 0.0f;
		padprint("- Frozen color");
		break;
	case 'r': // random color
		delay = 1.0f;
		break;
	case 'e': // random color and freeze
		delay = 0.0f;
		padprint("- Frozen on random color");
		rand_color();
		break;
	case 'm': // toggles auto draw
		last_x = c_width/2;
		last_y = c_width/2;
		automatic = !automatic;
		padprint(automatic ? "- Auto draw enabled" : "- Auto draw disabled");
		break;
	case 'c': // clear screen
		glClear(GL_COLOR_BUFFER_BIT);
		glFlush();
		break;
	case 'p': // save image
		save_image();
		break;
	case 'h': // show controls
		showcmds();
		break;
	case 'q': // quit
		exit(0);
		break;
	case 27: // quit 
		exit(0);
		break;
	}
}

void update(int value) {
	// cycles through colors with sine waves to get a rainbow effect
	if (delay > 0.0f) {
		r = 0.5f + 0.5f * sin(delay * 0.001f * glutGet(GLUT_ELAPSED_TIME));
		g = 0.5f + 0.5f * sin(delay * 0.001f * glutGet(GLUT_ELAPSED_TIME) + 2);
		b = 0.5f + 0.5f * sin(delay * 0.001f * glutGet(GLUT_ELAPSED_TIME) + 4);
	}
	glColor3f(r, g, b);
	printf("| R: %f, G: %f, B: %f, SPD: %02i, RADIUS: %02i, FROZEN: %i |\r", r, g, b, (int)delay, (int)radius, (int)(delay == 0.0f));
	glutTimerFunc(5, update, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(c_width, c_height);
	glutInitDisplayMode(GLUT_RGB);
	glutCreateWindow("Kaleidoscope Painter");
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glFlush(); // make screen start black

	showcmds();

	glutMotionFunc(motion);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(100, update, 0);

	glEnable(GL_BLEND); // attempt to smooth out points 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, c_width, 0.0, c_height);
	glutMainLoop();
	return 0;
}