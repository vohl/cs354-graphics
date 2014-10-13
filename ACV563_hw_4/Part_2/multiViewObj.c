#include <math.h>
#include <stdlib.h>
#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__

#include <sys/time.h>

static struct timeval last_idle_time;

double x;

void init(void);
void display(void);
void animateScene(void);

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutIdleFunc(animateScene);
	glutMainLoop();
	return 0;
}

void init(){
	glClearColor(0.3, 0.3, 0.3, 0.0);
	glEnable(GL_DEPTH_TEST);
}

void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 266, 266);
	glPushMatrix();
		glRotatef(x, 1.0, 0.0, 0.0);
		glColor3f(1.0, 1.0, 1.0);
		glutWireTorus(0.4, 0.7, 30, 20);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, 1.0, 1.0, 30.0);
	gluLookAt(5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(266, 0, 266, 266);
	glPushMatrix();
		glRotatef(x, 1.0, 0.0, 0.0);
		glColor3f(1.0, 1.0, 1.0);
		glutWireTorus(0.4, 0.7, 30, 20);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, 1.0, 1.0, 30.0);
	gluLookAt(0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(266, 266, 266, 266);
	glPushMatrix();
		glRotatef(x, 1.0, 0.0, 0.0);
		glColor3f(1.0, 1.0, 1.0);
		glutWireTorus(0.4, 0.7, 30, 20);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, 1.0, 1.0, 30.0);
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(0, 266, 266, 266);
	glPushMatrix();
		glRotatef(x, 1.0, 0.0, 0.0);
		glColor3f(1.0, 1.0, 1.0);
		glutWireTorus(0.4, 0.7, 30, 20);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, 1.0, 1.0, 30.0);
	gluLookAt(5.0, 0.0, 5.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glFlush();
	glutSwapBuffers();
}

void animateScene(){
	float dt;

    // Figure out time elapsed since last call to idle function
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    dt = (float)(time_now.tv_sec  - last_idle_time.tv_sec) + 1.0e-6*(time_now.tv_usec - last_idle_time.tv_usec);
    // Animate the scene by Increment 2 degrees each call
    x+=2;
    // Save time_now for next time
    last_idle_time = time_now;
    // Force redraw
    glutPostRedisplay();
}