//-------------------------------------------------------------------
// Created By: Arvo
// Modified By: Alejandro Montoya
//
// This is a simple OpenGL program that demonstrates
//   1) How to use 4x4 matrices to transform vertices
//   2) How to use double buffering
//   3) How to use "alpha" to create semi-transparent shapes
//   4) How to use "special" keys, such as the arrow keys.
//
// The program draws simple coordinate axes to indicate the coordinate
// origin, then draws a semi-transparent triangle on top.  The
// triangle can be translated and rotated using the arrow keys and
// the PageUp and PageDown keys, respectively.  The Home key resets
// the transformation to the identity.
//
// The "Insert" key changes how the rotation transformation is
// applied.  When the program starts, rotation matrices are multiplied
// on the right of the current modeling matrix, which means they are
// applied first, becore the translation.  This makes the triangle
// rotate about its center, no matter where it is translated to.
//
// When the "Insert" key is pressed, the rotation matrices are
// multiplied on the left of the current modeling matrix, which means
// they are applied last, afer the translation.  This makes the
// triangle rotate about the origin.
//
// Pressing the "Insert" key multiple times toggles between the
// two behaviors: multiplying on the left and on the right.
//-------------------------------------------------------------------

// Include all the matrix and vector classes that have been provided
// This also loads GL.h and GLUT.h .
#include "Helpers.h"

using std::cout;
using std::endl;

// Initialize several global variables.  The matrix "M" is the modeling
// matrix we apply to the triangle.  The flag "multiply_on_right" determines
// how we apply rotation matrices.
Mat4x4 M( Mat4x4::Identity() );


// A simple function for drawing a fixed triangle with a border.
// By default, the triangle is opaque.  If an alpha less than one
// is specified, the triangle will be semi-transparent.
void draw_square( double alpha = 1.0 )
{
    // Set the color that will be used to fill the triangle.
    glColor4f( 0.55, 0.55, 0.55, alpha );
    glBegin( GL_POLYGON );
        glVertex2f( 1, 1 );
        glVertex2f( -1, 1 );
        glVertex2f( -1, -1 );
        glVertex2f( 1, -1 );
    glEnd();
    
    // Set the color and width of the border around the triangle.
    glLineWidth( 5 ); // Make the triangle border fat.
    glColor4f( 1.0, 1.0, 1.0, alpha );
    glBegin( GL_LINE_LOOP );
    glVertex2f( 1, 1 );
    glVertex2f( -1, 1 );
    glVertex2f( -1, -1 );
    glVertex2f( 1, -1 );
    glEnd();
}

// This function is called by OpenGL whenever the display
// must be updated.  It manipulates OpenGL's "modeling"
// matrix in order to draw the untransformed coordinate axes
// followed by the transformed triangle.
void display( void )
{
    // clear frame buffer.
    glClear( GL_COLOR_BUFFER_BIT );
    
    // Indicate that we will be modifying the modeling matrix, not
    // the viewing matrix.
    glMatrixMode( GL_MODELVIEW );
    
    // Initially, set the modeling matrix to the identity, so that
    // we can draw the coordinate axes.
    glLoadIdentity();
    glColor3f( 1, 0, 0 );
    glLineWidth( 3 );
    glBegin( GL_LINES );
        glVertex2f(  0,  1 );
        glVertex2f(  0, -1 );
        glVertex2f(  1,  0 );
        glVertex2f( -1,  0 );
    glEnd();
    
    // Now load the global modeling matrix M and draw the triangle.
    // Everything drawn after the "Load" command will be transformed
    // by the given matrix.
    glLoadMatrix( M );
    draw_square( 0.70 );
    
    // Flush the commands sent to OpenGL and "swap" the buffers, so that
    // what was just drawn in the "back" buffer becomes visible.  This is
    // part of "double buffering".
    glFlush();
    glutSwapBuffers();
}

static const double delta = 0.1;
// Process key-press events.
void key_press( unsigned char key, int x, int y )
{
    
    
    Mat4x4 R, S, T1, T2;
    switch( key )
    {
        case 'r':
            // When the matrix is reset to the identity, indicate to OpenGL
            // that the image should be re-drawn.
            M = Mat4x4::Identity();
            glutPostRedisplay();
            break;
        case 'm': // Print Matrix M
            cout << "M:\n" << M << endl;
            break;
        case 'a': // rotate delta
            break;
        case 's': //rotate -delta
            break;
        case 'y': //scale 0.9
            break;
        case 't': // scale 1.1
            break;
        case 'q': //exit
        case 'Q':
            exit(0);
    }
}

// Process "special" key-press events.  This includes the arrow keys, the
// "home" key, and the "PageUp" and "PageDown" keys.  Notice that the first
// argument is an "int" rather than an "unsighed char".
void special_key_press( int key, int x, int y )
{
    Mat4x4 T;
    switch( key )
    {
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

int main( int argc, char** argv )
{
    // Do some basic initialization and setup.  Here is
    // where we set the initial size of the graphics window,
    // its placement on the screen, the background color, etc.
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );
    glutInitWindowSize( 500, 500 );
    glutInitWindowPosition( 100, 100 );
    glutCreateWindow( "Demo: Moving triangle" );
    glClearColor( 0.20, 0.30, 0.50, 0.0 ); // clear window.
    
    // Initialize the projection to be orthographic such that
    // the coordinates of the points range from -2 to 2 in all
    // three coordinates.  The XY coordinates will correspond to
    // the screen.
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -3.0, 3.0, -3.0, 3.0, -3.0, 3.0 );
    
    // Set the various callback functions, then let OpenGL take over.
    glutDisplayFunc( display );
    glutKeyboardFunc( key_press );
    glutSpecialFunc( special_key_press );
    
    
    // Tell OpenGL that we wish to use alpha blending in order to make
    // some objects semi-transparent.  Also ask OpenGL to "smooth" lines
    // so that they do not look jaggy.
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_BLEND );
    glEnable( GL_LINE_SMOOTH );
    
    // Print out instructions...
    cout << "The arrow keys translate the triangle.\n"
    << "The 'a' and 's' keys rotate the triangle.\n"
    << "The 'r' key re-centers the triangle.\n"
    << "The 'm' key prints the current modeling matrix.\n"
    << "The 't' and 'y' keys scalate the triangle.\n"
    << "The Q key exits the program.\n"
    << endl;
    
    // Give OpenGL control, which is never returned to this program.
    glutMainLoop();
    return 0;
}
