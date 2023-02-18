
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <math.h>

// This program is an animated screensaver using OpenGL and GLUT.

float angle = 0.0f;
const int NUM_CUBES = 25;
const float PI = 3.14159265358979323846;
bool pulse = false;
bool slowmode = false;
bool rgbmode = false;

struct Color { float r, g, b; };
struct Cube { float x, y, z; };

Color color = { 1.0f, 1.0f, 1.0f };
Cube cubes[NUM_CUBES];

// randomizes the rgb values 
void change_color() {
	color.r = (float)rand() / (float)RAND_MAX;
	color.g = (float)rand() / (float)RAND_MAX;
	color.b = (float)rand() / (float)RAND_MAX;
}

// inits paramters to defaults
void init() {
	angle = 0.0f;
	color = { 0.25f, 1.0f, 0.0f };
	pulse = false;
	slowmode = false;
	rgbmode = false;

	// init random cube positions
    for (int i = 0; i < NUM_CUBES; i++) {
        cubes[i].x = rand() % 10 - 5;
        cubes[i].y = rand() % 10 - 5;
        cubes[i].z = rand() % 10 - 5;
    }
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
	printf("|-----------------------------------------------------------------------|\r");
}

// prints controls to terminal
void showcmds() {
	printf("|-----------------------------------------------------------------------|\n");
	printf("| H: Help                   CUBE SCREENSAVER              ESC / Q: Quit |\n");
	printf("|------------------------------ CONTROLS ------------------------------ |\n");
	printf("| S: Slow mode | C: Change Colour | Z: Default | P: Pulse | R: RGB Mode |\n");
	printf("|-----------------------------------------------------------------------|\n");
}

// handles keyboard events
void keyboard(unsigned char key, int x, int y) {
    switch (tolower(key)) {
    // quit with q
    case 'q':
        exit(0);
        break;
	// quit with esc
	case 27:
		exit(0);
		break;
	// toggle slowdown
	case 's':
		slowmode = !slowmode;
		padprint(slowmode ? "Slow mode ON" : "Slow mode OFF");
		break;
	// change colors
	case 'c':
		change_color();
		break;
	// reset to default
	case 'z':
		init();
		padprint("Reset to default");
		break;
	// toggle size pulsing 
	case 'p':
		pulse = !pulse;
		padprint( pulse ? "Pulse ON" : "Pulse OFF");
		break;
	// toggle rgb color cycle mode 
	case 'r':
		rgbmode = !rgbmode;
		padprint(rgbmode ? "RGB mode ON" : "RGB mode OFF");
		break;
	case 'h':
		showcmds();
	}
}

void display(void) {
	float size = 2.0f;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(600, 1, 0.01, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 7, 0, 0, 5, 0, 1, 0);

    glRotatef(-angle, 1.0, 1.0, 1.0);

    for (int i = 0; i < NUM_CUBES; i++) {
        glPushMatrix();
        glTranslatef(cubes[i].x, cubes[i].y, cubes[i].z);
		if (pulse)
			size = 2.0f + 1.0f * sin(angle + 2 * i * (2 * PI / NUM_CUBES));
		if (rgbmode) {
			color.r = 0.5f + 0.5f * sin(0.001f * glutGet(GLUT_ELAPSED_TIME));
			color.g = 0.5f + 0.5f * sin(0.001f * glutGet(GLUT_ELAPSED_TIME) + 2);
			color.b = 0.5f + 0.5f * sin(0.001f * glutGet(GLUT_ELAPSED_TIME) + 4);
		}
		glColor3f(color.r, color.g, color.b);
        glutWireCube(size);
        glPopMatrix();
    }
    glutSwapBuffers();
}


// idle function for animation
void idle(void) {
    angle += 0.005;

	if (slowmode) 
		Sleep(16);

    // rotate cubes 
    for (int i = 0; i < NUM_CUBES; i++) {
        float x = 6 * cos(angle + i * (2 * PI / NUM_CUBES));
        float z = 6 * sin(angle + i * (2 * PI / NUM_CUBES));
        cubes[i].x = x;
        cubes[i].z = z;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1920, 1080);
	glutInitWindowPosition(0, 0);
    glutCreateWindow("Cube Screensaver");
	glutFullScreen();
    glEnable(GL_DEPTH_TEST);

	// print controls
	showcmds();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

	// init defaults
	init();

    // attempt to smooth edges
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutSetCursor(GLUT_CURSOR_NONE);

    glutMainLoop();
    return 0;
}