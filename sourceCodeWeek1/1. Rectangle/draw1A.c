/* ======================================================================
 * draw.c - Draw a filled polygon.
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

/* function prototypes */
void init	(void);
void display	(void);

static GLfloat vertices[8] = {
    0.75, 0.75, 0.75, 0.25, 0.25, 0.25, 0.25, 0.75
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * main:
 * Initialize window info, state variables, and callback functions.
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
	glMatrixMode(GL_PROJECTION); /* Specify which matrix is the current matrix */
	glLoadIdentity(); /* Replace the current matrix with the identity matrix*/
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0); /* Multiply the current matrix with an orthographic matrix*/
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * display: Display callback function.
 */
void display(void) {
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window
    
    glFlush();
    glutSwapBuffers();			/* swap buffer to see drawing */
}
