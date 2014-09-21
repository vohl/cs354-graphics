#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

int w, h;

typedef GLfloat point2f[2];

point2f list[100];

int n = 0;

void init(void);
void display(void);
void mouse_button(int button, int state, int x, int y);
void key_press(unsigned char key, int x, int y);

int main(int argc, char** argv){
	w = 512;
	h = 512;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(w, h);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutMouseFunc(mouse_button);
	glutKeyboardFunc(key_press);
	glutMainLoop();
	return 0;
}

void init(void){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT);

	glFlush();
	glutSwapBuffers();
}

void mouse_button(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		list[n][0] = (float) x / (w/2) - 1.0;
		list[n][1] = (float) (h-y) / (h/2) - 1.0;
		glBegin(GL_POINTS);
			glVertex2fv(list[n]);
		glEnd();
		++n;
		glFlush();
		glutSwapBuffers();
	}
	if(n >= 3){
		if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
			glBegin(GL_POLYGON);
				int i;
				for(i = 0; i < n; ++i){
					glVertex2fv(list[i]);
				}
			n = 0;
			glEnd();
			glFlush();
			glutSwapBuffers();
		}
	}
}

void key_press(unsigned char key, int x, int y){
	switch(key){
		case 'q':
		case 'Q':
			exit(0);
			break;
		case 'r':
		case 'R':
			n = 0;
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();
			glutSwapBuffers();
			break;
	}
}