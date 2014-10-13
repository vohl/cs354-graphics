/* ======================================================================
 * draw3.c - Draw a filled polygon. Introduce use of multiple viewports.
 * ======================================================================
 */

#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/* function prototypes */
void init	(void);
void display	(void);
void reshape	(int, int);

/* user-defined abstract datatype */
typedef float	vector2f[2];

/* global variables: window dimensions and vertex list */
int		WinW, WinH;
vector2f	p[] = {{0.25, 0.25}, {0.75, 0.25}, {0.55, 0.75}, {0.45, 0.75}};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * main: Initialize window info, state variables, and callback functions.
 * Enter main loop and process events.
 */
int main(int argc, char** argv) {
	glutInit( &argc, argv );
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); /* double-buffer; RGB    */
	glutInitWindowSize(500, 500);		/* initialize window size     */
	glutInitWindowPosition(100, 100);	/* initialize window position */
	glutCreateWindow(argv[0]);		/* set titlebar name; create  */
	init();					/* initialize state variables */
	glutDisplayFunc(display);		/* display callback function  */
	glutReshapeFunc(reshape);		/* reshape callback function  */
	glutMainLoop();				/* enter infinite event loop  */
	return 0;				/* ANSI C requires int main() */
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * init: Initialize state variables.
 */
void init(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0);	/* set background color */
	glColor3f   (1.0, 1.0, 1.0);		/* set foreground color */

	/* initialize viewing values; setup unit view cube */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * display: Display callback function.
 */
void display(void) {
	int	i, w2, h2;

	/* init w2 and h2 with dimensions of a window quadrant */
	w2 = WinW >> 1;
	h2 = WinH >> 1;

	glClear(GL_COLOR_BUFFER_BIT);		/* clear FB with backgd color */

	/* draw points */
	glViewport(0, 0, w2, h2);		/* draw into lower left quad  */
	glBegin(GL_POINTS);
		for(i=0; i<4; i++) glVertex2fv(p[i]);
	glEnd();

	/* flip next two drawings (upside down) */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 1.0, 0.0, -1.0, 1.0);

	/* draw lines */
	glViewport(w2, 0, w2, h2);		/* draw into lower right quad */
	glBegin(GL_LINES);
		for(i=0; i<4; i++) glVertex2fv(p[i]);
	glEnd();

	/* draw line strip */
	glViewport(w2, h2, w2, h2);		/* draw into upper right quad */
	glBegin(GL_LINE_STRIP);
		for(i=0; i<4; i++) glVertex2fv(p[i]);
	glEnd();

	/* restore orientation of next drawing */
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

	/* draw line loop */
	glViewport(0, h2, w2, h2);		/* draw into upper left  quad */
	glBegin(GL_LINE_LOOP);
		for(i=0; i<4; i++) glVertex2fv(p[i]);
	glEnd();

   	glFlush();				/* draw to (off-screen) buffer*/
	glutSwapBuffers();			/* swap buffer to see drawing */
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * reshape: Reshape callback function.
 */
void reshape(int w, int h) {
	/* init global variables with window dimensions; for use in display() */
	WinW = w;
	WinH = h;

	/* initialize viewing values; setup unit view cube */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}
