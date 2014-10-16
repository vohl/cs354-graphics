#define GLEW_STATIC

#include <math.h>
#include <stdlib.h>

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#include <sys/time.h>

#define GLSL(src) "#version 130\n" #src

#define X 0
#define Y 1
#define Z 2

static struct timeval last_idle_time;

int axis = 3;

double y=0;
double x=0;
double z=0;

const GLchar* vertexSource = GLSL(
    varying vec3 normal;
    void main()
    {
      gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
     
      normal = gl_NormalMatrix * gl_Normal;
     
    }
);

const GLchar* fragmentSource = GLSL(
    varying vec3 normal;
 
    vec4 light0 ()
    {
      vec4 color;
     
      vec3 lightDir = vec3(gl_LightSource[0].position); 
     
      vec4 ambient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
     
      vec4 diffuse = gl_LightSource[0].diffuse * max(dot(normal,lightDir),0.0) * gl_FrontMaterial.diffuse;
     
      color = ambient + diffuse;
     
      return color;
    }
     
    vec4 light1 ()
    {
      vec4 color;
     
      vec3 lightDir = vec3(gl_LightSource[1].position); 
     
      vec4 ambient = gl_LightSource[1].ambient * gl_FrontMaterial.ambient;
     
      vec4 diffuse = gl_LightSource[1].diffuse * max(dot(normal,lightDir),0.0) * gl_FrontMaterial.diffuse;
     
      color = ambient + diffuse;
     
      return color;
    }
     
    vec4 light2 ()
    {
      vec4 color;
     
      vec3 lightDir = vec3(gl_LightSource[2].position); 
     
      vec4 ambient = gl_LightSource[2].ambient * gl_FrontMaterial.ambient;
     
      vec4 diffuse = gl_LightSource[2].diffuse * max(dot(normal,lightDir),0.0) * gl_FrontMaterial.diffuse;
     
      color = ambient + diffuse;
     
      return color;
    }
     
    vec4 light3 ()
    {
      vec4 color;
     
      vec3 lightDir = vec3(gl_LightSource[3].position); 
     
      vec4 ambient = gl_LightSource[3].ambient * gl_FrontMaterial.ambient;
     
      vec4 diffuse = gl_LightSource[3].diffuse * max(dot(normal,lightDir),0.0) * gl_FrontMaterial.diffuse;
     
      color = ambient + diffuse;
     
      return color;
    } 
     
    void main()
    { 
      vec4 light;
     
      if(vec3(gl_LightSource[0].position) != vec3(0.0, 0.0, 0.0))
        light += light0();
      if(vec3(gl_LightSource[1].position) != vec3(0.0, 0.0, 0.0))
        light += light1();
      if(vec3(gl_LightSource[2].position) != vec3(0.0, 0.0, 0.0))
        light += light2();
      if(vec3(gl_LightSource[3].position) != vec3(0.0, 0.0, 0.0))
        light += light3();
     
      gl_FragColor = light;
    }
);

void display(void);
void AnimateScene(void);
void keyInput(unsigned char key, int x, int y);

int main(int argc, char* argv[]){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Multiple Lights/Rotated Obj");

    glewInit();

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    glutDisplayFunc(display);
    glutIdleFunc(AnimateScene);
    glutKeyboardFunc(keyInput);
    glutMainLoop();

    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, &vbo);

    glDeleteVertexArrays(1, &vao);

    return 0;
}

void display(){
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
        glRotatef(x, 1, 0, 0);
        glRotatef(y, 0, 1, 0);
        glRotatef(z, 0, 0, 1);
        glColor3f(1.0, 0.0, 0.0);
        glutSolidCube(0.5);
    glPopMatrix();


    glFlush();
    glutSwapBuffers();   

}

void AnimateScene(void) {
    float dt;

    // Figure out time elapsed since last call to idle function
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    dt = (float)(time_now.tv_sec  - last_idle_time.tv_sec) +
    1.0e-6*(time_now.tv_usec - last_idle_time.tv_usec); 
    // Animate the scene by Increment 2 degrees each call
    if(axis == X) x+=2;
    if(axis == Y) y+=2;
    if(axis == Z) z+=2;
    // Save time_now for next time
    last_idle_time = time_now;
    
    // Force redraw
    glutPostRedisplay();
}

void keyInput(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'x':
            axis = X;
            break;
        case 'y':
            axis = Y;
            break;
        case 'z':
            axis = Z;
            break;
        case 27:
        case 'q':
        case 'Q':
            exit(0);
            break;
        default:
            break;
    }
}