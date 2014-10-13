#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__
/* SimpleVBO.c Mon Aug 13 16:11:28 EST 2012 pknowles */

/*
 * This program draws a 2x2 quad (3x3 vertices) grid using
 * triangle strips and vertex buffer objects. Indices/elements are used to
 * reduce the amount of data required to store the geometry.
 * Requires the glut library and GL (installed with video drivers).
 *
 * To compile and run: gcc SimpleVBO.c -lGL -lglut -o simplevbo && ./simplevbo
 */


/* array of vertices - each vertex is unique and only appears once */
float vertices[] = {
    0.0, 0.0, 0.0, /* vertex 0 */
    0.0, 1.0, 0.0, /* vertex 1, etc... */
    0.0, 2.0, 0.0,
    
    1.0, 0.0, 0.0,
    1.0, 1.0, 0.0,
    1.0, 2.0, 0.0,
    
    2.0, 0.0, 0.0,
    2.0, 1.0, 0.0,
    2.0, 2.0, 0.0,
};

/* array of indices - these connect the above vertices, forming
 * primitives. Note that each vertex at the start and end of
 * each row appear twice (degenerates) so the entire grid can
 * be drawn as a single triangle strip */
unsigned int indices[] = {
    0, 0, 3, 1, 4, 2, 5, 5, /* row 0 */
    3, 3, 6, 4, 7, 5, 8, 8, /* row 1 */
};

/* gl vertex buffer object handles */
GLuint vbo_vertices = 0;
GLuint vbo_indices = 0;

void initVBOs()
{
    /* request unique buffer handles */
    glGenBuffers(1, &vbo_vertices);
    glGenBuffers(1, &vbo_indices);
}

void bufferVBOs()
{
    /* buffer vertices */
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    /* buffer indices */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    /* NOTE: sizeof only works for static arrays */
}

void drawGrid()
{
    /* Note: when buffers are bound, the pointer argument becomes an offset
     * within the buffer. In our case the data begins at the start, 0 */
    
    /* use data from a buffer by binding it before the gl*Pointer() call */
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    /* this turns on the use of glVertexPointer vertex positions for glDraw*() */
    glEnableClientState(GL_VERTEX_ARRAY);
    
    /* similarly to glVertexPointer, bind the indices buffer before glDraw*() */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
    glDrawElements(GL_TRIANGLE_STRIP, 16, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    /* it is good practice to leave client states disabled */
    glDisableClientState(GL_VERTEX_ARRAY);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    /* center the grid */
    glTranslatef(-0.5, -0.5, 0.0);
    
    /* the grid is 0->2, scale to 0->1 */
    glScalef(1.0/2.0, 1.0/2.0, 1.0/2.0);
    
    /* wireframe helps debug mesh problems */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //GL_FILL
    
    /* can update data if needed (eg. vertex animation) */
    /* bufferVBOs() */
    
    /* draw the grid */
    drawGrid();
    
    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("OpenGL");
    glutDisplayFunc(display);
    
    /* get unique vbo handles */
    initVBOs();
    
    /* buffer initial data */
    bufferVBOs();
    
    glutMainLoop();
}