//Runs on Linux gcc rotation.c -o rotation -lglut -lGL -lGLU -lGLEW

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

static double x1;
static double x2;
static double x3;
static double x4;
static double speed = 0.1;
static double rem = 0.0;

enum special_key {
    left_arrow_key  = 100,
    up_arrow_key    = 101,
    right_arrow_key = 102,
    down_arrow_key  = 103
};

void init(void);
void display(void);
void reshape(GLsizei width, GLsizei height);
void animateScene(void);
void key_press(unsigned char key, int x, int y);
void special_key_press(int key, int x, int y);
void menu(int id);

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(argv[0]);
	init();
	glutCreateMenu(menu);
	glutAddMenuEntry("Quit", 1);
	glutAddMenuEntry("Pause", 2);
	glutAddMenuEntry("Resume", 3);
	glutAddMenuEntry("Increase", 4);
	glutAddMenuEntry("Decrease", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(animateScene);
	glutKeyboardFunc(key_press);
	glutSpecialFunc(special_key_press);
	glutMainLoop();
	return 0;
}

void init(){
	glClearColor(0.1, 0.1, 0.1, 0.0);
	glEnable(GL_DEPTH_TEST);
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
		//The sun, even though star don't rotate or evolve....who cares right
		glRotatef(x1, 0.0, 1.0, 0.0);
		glColor3f(1.0, 1.0, 0.0);
		glutWireSphere(.75, 30, 25);
	glPopMatrix();

	glPushMatrix();
		glRotatef(x1, 1.0, 0.0, 0.0);
		glColor3f(0.0, 0.0, 1.0);
		glutWireTorus(0.1, 1.15, 1, 30);
	glPopMatrix();
	
	glPushMatrix();
		glRotatef(x1, 0.0, 0.0, 1.0);
		glColor3f(0.0, 0.0, 1.0);
		glutWireTorus(1.3, 0.1, 30, 1);
	glPopMatrix();

	glPushMatrix();
		//planet
		glRotatef(x3, 1.0, 1.0, 1.0);
		glTranslatef(0.5, 0.75, -1.5);
		glColor3f(0.0, 1.0, 0.7);
		glutWireSphere(0.1, 15, 10);
	
		glPushMatrix();
			//Moon of planet
			glRotatef(x2, 1.0, 1.0, 1.0);
			glTranslatef(0.5, 0.0, 0.0);
			glColor3f(1.0, 1.0, 1.0);
			glutWireSphere(0.033, 10, 10);
		glPopMatrix();
	
		glPushMatrix();
			//Moon of planet
			glRotatef(x2, 1.0, 1.0, 1.0);
			glTranslatef(-0.5, 0.0, 0.0);
			glColor3f(1.0, 1.0, 1.0);
			glutWireSphere(0.033, 10, 10);
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		//planet
		glRotatef(x3, -1.0, -1.0, 1.0);
		glTranslatef(-1.0, -1.5, -2.5);
		glColor3f(0.7, 0.0, 0.7);
		glutWireSphere(0.1, 15, 10);
	
		glPushMatrix();
			//Moon of planet
			glRotatef(x2, 1.0, 1.0, 1.0);
			glTranslatef(0.5, 0.0, 0.0);
			glColor3f(1.0, 1.0, 1.0);
			glutWireSphere(0.033, 10, 10);
		glPopMatrix();
	
		glPushMatrix();
			//Moon of planet
			glRotatef(x2, 1.0, 1.0, 1.0);
			glTranslatef(-0.5, 0.0, 0.0);
			glColor3f(1.0, 1.0, 1.0);
			glutWireSphere(0.033, 10, 10);
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		// far slow planet1
		glRotatef(x4, -1.0, 1.0, -1.0);
		glTranslatef(-5.0, -5.0, -5.0);
		glColor3f(1.0, 0.0, 0.0);
		glutWireSphere(0.15, 10.0, 10.0);
	glPopMatrix();

	glPushMatrix();
		//far slow planet2
		glRotatef(x4, -1.0, -1.0, -1.0);
		glTranslatef(-4.5, 4.5, -4.5);
		glColor3f(1.0, 0.0, 0.0);
		glutWireSphere(0.15, 10.0, 10.0);
	glPopMatrix();

	glPushMatrix();
		//far slow planet3
		glRotatef(x4, 1.0, -1.0, -1.0);
		glTranslatef(4.0, 4.0, -4.0);
		glColor3f(1.0, 0.0, 0.0);
		glutWireSphere(0.15, 10.0, 10.0);
	glPopMatrix();

	glPushMatrix();
		//far slow planet4
		glRotatef(x4, 1.0, 1.0, -1.0);
		glTranslatef(3.5, -3.5, -3.5);
		glColor3f(1.0, 0.0, 0.0);
		glutWireSphere(0.15, 10.0, 10.0);
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height){
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(80.0, (GLfloat)width/(GLfloat)height, 1.0, 30.0);

	gluLookAt(0.0, 2.5, 5.0, 0.0, -0.5, -1.0, 0.0, 1.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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

	//Animating the scene by increment of variable degrees per second
	x1+=speed;
	x2+=(5*speed);
	x3+=(0.5*speed);
	x4+=(0.1*speed);
	last_idle_time = time_now;

	//redraw
	glutPostRedisplay();
}

void menu(int id){
	switch(id){
		case 1:
			exit(0);
			break;
		case 2:
			if(speed!=0){
				rem = speed;
				speed = 0;
			}
			break;
		case 3:
			if(rem!=0){
				speed = rem;
				rem = 0;
			}
			break;
		case 4:
			if(speed < .5){
				speed+=0.025;
			}
			break;
		case 5:
			if(speed > 0.025){
				speed-=0.025;
			}	
			break;
	}
	glutPostRedisplay();
}

void key_press(unsigned char key, int x, int y){
	switch(key){
		case 'q':
		case 'Q':
			exit(1);
			break;
		case 'p':
		case 'P':
			if(speed!=0){
				rem = speed;
				speed = 0;
			}
			break;
		case 'r':
		case 'R':
			if(rem!=0){
				speed = rem;
				rem = 0;
			}
			break;
		case 'f':
		case 'F':
			if(speed < .5){
				speed+=0.025;
			}
			break;
		case 'g':
		case 'G':
			if(speed > 0.025){
				speed-=0.025;
			}
			break;
		case 'a':
		case 'A':
			break;
		case 's':
		case 'S':
			break;
		case 'd':
		case 'D':
			break;
		case 'w':
		case 'W':
			break;
	}
}

void special_key_press( int key, int x, int y ) {
    switch( key ) {
        case right_arrow_key: // transtale right
        	
            break;
        case left_arrow_key: //translate left

            break;
        case up_arrow_key: //translate up

            break;
        case down_arrow_key: //translate down

            break;
    }
}