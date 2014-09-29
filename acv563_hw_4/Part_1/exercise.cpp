#define GLEW_STATIC

//#include <GLUT/glut.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

//#define GLSL(src) "#version 150\n" #src

// Shader sources
const GLchar* vertexSource =
    "#version 150 core\n"
    "in vec2 position;"
    "in vec3 color"
    "out vec3 Color"
    "void main() {"
    "	Color = color"
    "   gl_Position = vec4(position.x, -position.y, 0.0, 1.0);"
    "}";

const GLchar* fragmentSource =
    "#version 150 core\n"
    "in vec3 Color"
    "out vec4 outColor;"
    //"uniform vec3 triangleColor;"
    "void main() {"
    "   outColor = vec4(Color, 1.0);"
    "}";

float vertices[] = {
	 0.0f,  0.5f, 1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, 0.0f, 0.0f, 1.0f
};

GLint uniColor;

void display(void);

int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(argv[0]);
	glewInit();

	GLuint vao; //Vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo; //Vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//loading the v_shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

	//loading the f_shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));

	//uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
	
	glutDisplayFunc(display);
	glutMainLoop();

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	return 0;
}

void display(){
	//glUniform3f(uniColor, 1.0f, 0.0f, 0.0f);

	glClearColor(0.3, 0.3, 0.3, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glFlush();
	glutSwapBuffers();
}