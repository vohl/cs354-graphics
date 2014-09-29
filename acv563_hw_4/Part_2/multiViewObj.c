#include <math.h>
#include <stdlib.h>

#ifdef __APPLE__  // include Mac OS X verions of headers
	#include <OpenGL/OpenGL.h>
	#include <GLUT/glut.h>
#else // non-Mac OS X operating systems
	#include <GL/glew.h>
	#include <GL/freeglut.h>
	#include <GL/freeglut_ext.h>
#endif 

#ifdef _WIN32
	#include <windows.h>
#else
	#include <sys/time.h>
#endif

#ifdef _WIN32
	static DWORD last_idle_time;
#else
	static struct timeval last_idle_time;
#endif

static double x;
int WinW, WinH;

void init(void);
void display(void);
void reshape(GLsizei width, GLsizei height);
void animateScene(void);

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(animateScene);
	glutMainLoop();
	return 0;
}

void init(){
	glClearColor(0.3, 0.3, 0.3, 0.0);
	glEnable(GL_DEPTH_TEST);
}

void display(){
	int w2, h2;

	w2 = WinW >> 1;
	h2 = WinH >> 1;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, w2, h2);
	glPushMatrix();
		glRotatef(x, 1.0, 0.0, 0.0);
		glColor3f(1.0, 1.0, 1.0);
		glutWireTorus(0.4, 0.7, 30, 20);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, WinW / WinH, 1.0, 30.0);
	gluLookAt(5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(w2, 0, w2, h2);
	glPushMatrix();
		glRotatef(x, 1.0, 0.0, 0.0);
		glColor3f(1.0, 1.0, 1.0);
		glutWireTorus(0.4, 0.7, 30, 20);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, WinW / WinH, 1.0, 30.0);
	gluLookAt(0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(w2, h2, w2, h2);
	glPushMatrix();
		glRotatef(x, 1.0, 0.0, 0.0);
		glColor3f(1.0, 1.0, 1.0);
		glutWireTorus(0.4, 0.7, 30, 20);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, WinW / WinH, 1.0, 30.0);
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, h2, w2, h2);
	glPushMatrix();
		glRotatef(x, 1.0, 0.0, 0.0);
		glColor3f(1.0, 1.0, 1.0);
		glutWireTorus(0.4, 0.7, 30, 20);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, WinW / WinH, 1.0, 30.0);
	gluLookAt(5.0, 0.0, 5.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glFlush();
	glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height){
	WinW = width;
	WinH = height;
}


void animateScene(){
	float dt;

	#ifdef _WIN32
		DWORD time_now;
    	time_now = GetTickCount();
    	dt = (float) (time_now - last_idle_time) / 1000.0;
	#else
		struct timeval time_now;
		gettimeofday(&time_now, NULL);
		dt = (float)(time_now.tv_sec - last_idle_time.tv_sec) + 1.0e-6*(time_now.tv_sec - last_idle_time.tv_sec);
	#endif

	x+=0.1;
	last_idle_time = time_now;

	glutPostRedisplay();
}