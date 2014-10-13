#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__

#pragma comment( linker, "/entry:\"mainCRTStartup\"" ) 

#define DATA_DIR "data/"


//dirent32.h
#ifndef _WIN32
#include <dirent.h>
#else

#include <windows.h>
#endif


//glm .h


#ifndef M_PI
#define M_PI 3.14159265f
#endif

#define GLM_NONE     (0)            /* render with only vertices */
#define GLM_FLAT     (1 << 0)       /* render with facet normals */
#define GLM_SMOOTH   (1 << 1)       /* render with vertex normals */
#define GLM_TEXTURE  (1 << 2)       /* render with texture coords */
#define GLM_COLOR    (1 << 3)       /* render with colors */
#define GLM_MATERIAL (1 << 4)       /* render with materials */


/* GLMmaterial: Structure that defines a material in a model.
 */
typedef struct _GLMmaterial
{
    char* name;                   /* name of material */
    GLfloat diffuse[4];           /* diffuse component */
    GLfloat ambient[4];           /* ambient component */
    GLfloat specular[4];          /* specular component */
    GLfloat emmissive[4];         /* emmissive component */
    GLfloat shininess;            /* specular exponent */
} GLMmaterial;

/* GLMtriangle: Structure that defines a triangle in a model.
 */
typedef struct _GLMtriangle {
    GLuint vindices[3];           /* array of triangle vertex indices */
    GLuint nindices[3];           /* array of triangle normal indices */
    GLuint tindices[3];           /* array of triangle texcoord indices*/
    GLuint findex;                /* index of triangle facet normal */
} GLMtriangle;

/* GLMgroup: Structure that defines a group in a model.
 */
typedef struct _GLMgroup {
    char*             name;           /* name of this group */
    GLuint            numtriangles;   /* number of triangles in this group */
    GLuint*           triangles;      /* array of triangle indices */
    GLuint            material;       /* index to material for group */
    struct _GLMgroup* next;           /* pointer to next group in model */
} GLMgroup;

/* GLMmodel: Structure that defines a model.
 */
typedef struct _GLMmodel {
    char*    pathname;            /* path to this model */
    char*    mtllibname;          /* name of the material library */
    
    GLuint   numvertices;         /* number of vertices in model */
    GLfloat* vertices;            /* array of vertices  */
    
    GLuint   numnormals;          /* number of normals in model */
    GLfloat* normals;             /* array of normals */
    
    GLuint   numtexcoords;        /* number of texcoords in model */
    GLfloat* texcoords;           /* array of texture coordinates */
    
    GLuint   numfacetnorms;       /* number of facetnorms in model */
    GLfloat* facetnorms;          /* array of facetnorms */
    
    GLuint       numtriangles;    /* number of triangles in model */
    GLMtriangle* triangles;       /* array of triangles */
    
    GLuint       nummaterials;    /* number of materials in model */
    GLMmaterial* materials;       /* array of materials */
    
    GLuint       numgroups;       /* number of groups in model */
    GLMgroup*    groups;          /* linked list of groups */
    
    GLfloat position[3];          /* position of the model */
    
} GLMmodel;

char*      model_file = NULL;       /* name of the obect file */
GLuint     model_list = 0;          /* display list for object */
GLMmodel*  model;                   /* glm model data structure */
GLfloat    smoothing_angle = 90.0;  /* smoothing angle */
GLboolean  facet_normal = GL_TRUE; /* draw with facet normal? */
GLboolean  performance = GL_FALSE;  /* performance counter on? */
//Lighting on?
GLboolean  illuminate = GL_FALSE;
GLuint     material_mode = 0;       /* 0=none, 1=color, 2=material */
GLint      entries = 0;             /* entries in model menu */
//Zoom variable
GLfloat    zoom;
//Variable that rpresents the light source
GLfloat light_ambient[] = {0.2, 0.2, 0.2, 0.1};
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat theta1 = 0.0;
GLfloat light_position[] = {0.0, 0.0, 0.0, 1.0};
//Camera. We end up rotating this to rotate the cow(camera stays put)
GLdouble   pan_x = 0.0;
GLdouble   pan_y = 0.0;
GLdouble   pan_z = 0.0;

static GLint      mouse_state;
static GLint      mouse_button;

#if defined(_WIN32)
#include <sys/timeb.h>
#define CLK_TCK 1000
#else
#define CLK_TCK 1000
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/times.h>
#endif

//gltb.c

#define GLTB_TIME_EPSILON  10


static GLuint    gltb_lasttime;
static GLfloat   gltb_lastposition[3];

static GLfloat   gltb_angle = 0.0;
static GLfloat   gltb_axis[3];
static GLfloat   gltb_transform[4][4];

static GLuint    gltb_width;
static GLuint    gltb_height;

static GLint     gltb_button = -1;
static GLboolean gltb_tracking = GL_FALSE;
static GLboolean gltb_animate = GL_TRUE;


static void
_gltbPointToVector(int x, int y, int width, int height, float v[3])
{
    float d, a;
    
    /* project x, y onto a hemi-sphere centered within width, height. */
    v[0] = (2.0 * x - width) / width;
    v[1] = (height - 2.0 * y) / height;
    d = sqrt(v[0] * v[0] + v[1] * v[1]);
    v[2] = cos((3.14159265 / 2.0) * ((d < 1.0) ? d : 1.0));
    a = 1.0 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] *= a;
    v[1] *= a;
    v[2] *= a;
}

static void
_gltbAnimate(void)
{
    glutPostRedisplay();
}

void
_gltbStartMotion(int x, int y, int button, int time)
{
    assert(gltb_button != -1);
    
    gltb_tracking = GL_TRUE;
    gltb_lasttime = time;
    _gltbPointToVector(x, y, gltb_width, gltb_height, gltb_lastposition);
}

void
_gltbStopMotion(int button, unsigned time)
{
    assert(gltb_button != -1);
    
    gltb_tracking = GL_FALSE;
    
    if (time - gltb_lasttime < GLTB_TIME_EPSILON && gltb_animate) {
        glutIdleFunc(_gltbAnimate);
    } else {
        gltb_angle = 0;
        if (gltb_animate)
            glutIdleFunc(0);
    }
}

void
gltbInit(GLuint button)
{
    gltb_button = button;
    gltb_angle = 0.0;
    
    /* put the identity in the trackball transform */
    glPushMatrix();
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)gltb_transform);
    glPopMatrix();
}

void
gltbMatrix(void)
{
    assert(gltb_button != -1);
    
    glPushMatrix();
    glLoadIdentity();
    glRotatef(gltb_angle, gltb_axis[0], gltb_axis[1], gltb_axis[2]);
    glMultMatrixf((GLfloat*)gltb_transform);
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)gltb_transform);
    glPopMatrix();
    
    glMultMatrixf((GLfloat*)gltb_transform);
}

void
gltbReshape(int width, int height)
{
    assert(gltb_button != -1);
    
    gltb_width  = width;
    gltb_height = height;
}

void
gltbMouse(int button, int state, int x, int y)
{
    assert(gltb_button != -1);
    
    if (state == GLUT_DOWN && button == gltb_button)
        _gltbStartMotion(x, y, button, glutGet(GLUT_ELAPSED_TIME));
    else if (state == GLUT_UP && button == gltb_button)
        _gltbStopMotion(button, glutGet(GLUT_ELAPSED_TIME));
}

void
gltbMotion(int x, int y)
{
    GLfloat current_position[3], dx, dy, dz;
    
    assert(gltb_button != -1);
    
    if (gltb_tracking == GL_FALSE)
        return;
    
    _gltbPointToVector(x, y, gltb_width, gltb_height, current_position);
    
    /* calculate the angle to rotate by (directly proportional to the
     length of the mouse movement) */
    dx = current_position[0] - gltb_lastposition[0];
    dy = current_position[1] - gltb_lastposition[1];
    dz = current_position[2] - gltb_lastposition[2];
    gltb_angle = 90.0 * sqrt(dx * dx + dy * dy + dz * dz);
    
    /* calculate the axis of rotation (cross product) */
    gltb_axis[0] = gltb_lastposition[1] * current_position[2] -
    gltb_lastposition[2] * current_position[1];
    gltb_axis[1] = gltb_lastposition[2] * current_position[0] -
    gltb_lastposition[0] * current_position[2];
    gltb_axis[2] = gltb_lastposition[0] * current_position[1] -
    gltb_lastposition[1] * current_position[0];
    
    /* XXX - constrain motion */
    gltb_axis[2] = 0;
    
    /* reset for next time */
    gltb_lasttime = glutGet(GLUT_ELAPSED_TIME);
    gltb_lastposition[0] = current_position[0];
    gltb_lastposition[1] = current_position[1];
    gltb_lastposition[2] = current_position[2];
    
    /* remember to draw new position */
    glutPostRedisplay();
}

//glm.c

/*
 glm.c
 Nate Robins, 1997, 2000
 nate@pobox.com, http://www.pobox.com/~nate
 
 Wavefront OBJ model file format reader/writer/manipulator.
 
 Includes routines for generating smooth normals with
 preservation of edges, welding redundant vertices & texture
 coordinate generation (spheremap and planar projections) + more.
 
 */


#include <string.h>
#include <assert.h>


#define T(x) (model->triangles[(x)])
GLuint     normalLines = 0;

/* _GLMnode: general purpose node */
typedef struct _GLMnode {
    GLuint         index;
    GLboolean      averaged;
    struct _GLMnode* next;
} GLMnode;

/* glmAbs: returns the absolute value of a float */
static GLfloat
glmAbs(GLfloat f)
{
    if (f < 0)
        return -f;
    return f;
}

/* glmDot: compute the dot product of two vectors
 *
 * u - array of 3 GLfloats (GLfloat u[3])
 * v - array of 3 GLfloats (GLfloat v[3])
 */
static GLfloat
glmDot(GLfloat* u, GLfloat* v)
{
    assert(u); assert(v);
    
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

/* glmCross: compute the cross product of two vectors
 *
 * u - array of 3 GLfloats (GLfloat u[3])
 * v - array of 3 GLfloats (GLfloat v[3])
 * n - array of 3 GLfloats (GLfloat n[3]) to return the cross product in
 */
static GLvoid
glmCross(GLfloat* u, GLfloat* v, GLfloat* n)
{
    assert(u); assert(v); assert(n);
    
    n[0] = u[1]*v[2] - u[2]*v[1];
    n[1] = u[2]*v[0] - u[0]*v[2];
    n[2] = u[0]*v[1] - u[1]*v[0];
}

/* glmNormalize: normalize a vector
 *
 * v - array of 3 GLfloats (GLfloat v[3]) to be normalized
 */
static GLvoid
glmNormalize(GLfloat* v)
{
    GLfloat l;
    
    assert(v);
    
    l = (GLfloat)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] /= l;
    v[1] /= l;
    v[2] /= l;
}

/* glmFindGroup: Find a group in the model */
GLMgroup*
glmFindGroup(GLMmodel* model, char* name)
{
    GLMgroup* group;
    
    assert(model);
    
    group = model->groups;
    while(group) {
        if (!strcmp(name, group->name))
            break;
        group = group->next;
    }
    
    return group;
}

/* glmAddGroup: Add a group to the model */
GLMgroup*
glmAddGroup(GLMmodel* model, char* name)
{
    GLMgroup* group;
    
    group = glmFindGroup(model, name);
    if (!group) {
        group = (GLMgroup*)malloc(sizeof(GLMgroup));
        group->name = strdup(name);
        group->material = 0;
        group->numtriangles = 0;
        group->triangles = NULL;
        group->next = model->groups;
        model->groups = group;
        model->numgroups++;
    }
    
    return group;
}

/* glmFindGroup: Find a material in the model */
GLuint
glmFindMaterial(GLMmodel* model, char* name)
{
    GLuint i;
    
    /* XXX doing a linear search on a string key'd list is pretty lame,
     but it works and is fast enough for now. */
    for (i = 0; i < model->nummaterials; i++) {
        if (!strcmp(model->materials[i].name, name))
            goto found;
    }
    
    /* didn't find the name, so print a warning and return the default
     material (0). */
    printf("glmFindMaterial():  can't find material \"%s\".\n", name);
    i = 0;
    
found:
    return i;
}


/* glmDirName: return the directory given a path
 *
 * path - filesystem path
 *
 * NOTE: the return value should be free'd.
 */
static char*
glmDirName(char* path)
{
    char* dir;
    char* s;
    
    dir = strdup(path);
    
    s = strrchr(dir, '/');
    if (s)
        s[1] = '\0';
    else
        dir[0] = '\0';
    
    return dir;
}


/* glmReadMTL: read a wavefront material library file
 *
 * model - properly initialized GLMmodel structure
 * name  - name of the material library
 */
static GLvoid
glmReadMTL(GLMmodel* model, char* name)
{
    FILE* file;
    char* dir;
    char* filename;
    char    buf[128];
    GLuint nummaterials, i;
    
    dir = glmDirName(model->pathname);
    filename = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(name) + 1));
    strcpy(filename, dir);
    strcat(filename, name);
    free(dir);
    
    file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "glmReadMTL() failed: can't open material file \"%s\".\n",
                filename);
        exit(1);
    }
    free(filename);
    
    /* count the number of materials in the file */
    nummaterials = 1;
    while(fscanf(file, "%s", buf) != EOF) {
        switch(buf[0]) {
            case '#':               /* comment */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
            case 'n':               /* newmtl */
                fgets(buf, sizeof(buf), file);
                nummaterials++;
                sscanf(buf, "%s %s", buf, buf);
                break;
            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
        }
    }
    
    rewind(file);
    
    model->materials = (GLMmaterial*)malloc(sizeof(GLMmaterial) * nummaterials);
    model->nummaterials = nummaterials;
    
    /* set the default material */
    for (i = 0; i < nummaterials; i++) {
        model->materials[i].name = NULL;
        model->materials[i].shininess = 65.0;
        model->materials[i].diffuse[0] = 0.8;
        model->materials[i].diffuse[1] = 0.8;
        model->materials[i].diffuse[2] = 0.8;
        model->materials[i].diffuse[3] = 1.0;
        model->materials[i].ambient[0] = 0.2;
        model->materials[i].ambient[1] = 0.2;
        model->materials[i].ambient[2] = 0.2;
        model->materials[i].ambient[3] = 1.0;
        model->materials[i].specular[0] = 0.0;
        model->materials[i].specular[1] = 0.0;
        model->materials[i].specular[2] = 0.0;
        model->materials[i].specular[3] = 1.0;
    }
    model->materials[0].name = strdup("default");
    
    /* now, read in the data */
    nummaterials = 0;
    while(fscanf(file, "%s", buf) != EOF) {
        switch(buf[0]) {
            case '#':               /* comment */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
            case 'n':               /* newmtl */
                fgets(buf, sizeof(buf), file);
                sscanf(buf, "%s %s", buf, buf);
                nummaterials++;
                model->materials[nummaterials].name = strdup(buf);
                break;
            case 'N':
                fscanf(file, "%f", &model->materials[nummaterials].shininess);
                /* wavefront shininess is from [0, 1000], so scale for OpenGL */
                model->materials[nummaterials].shininess /= 1000.0;
                model->materials[nummaterials].shininess *= 128.0;
                break;
            case 'K':
                switch(buf[1]) {
                    case 'd':
                        fscanf(file, "%f %f %f",
                               &model->materials[nummaterials].diffuse[0],
                               &model->materials[nummaterials].diffuse[1],
                               &model->materials[nummaterials].diffuse[2]);
                        break;
                    case 's':
                        fscanf(file, "%f %f %f",
                               &model->materials[nummaterials].specular[0],
                               &model->materials[nummaterials].specular[1],
                               &model->materials[nummaterials].specular[2]);
                        break;
                    case 'a':
                        fscanf(file, "%f %f %f",
                               &model->materials[nummaterials].ambient[0],
                               &model->materials[nummaterials].ambient[1],
                               &model->materials[nummaterials].ambient[2]);
                        break;
                    default:
                        /* eat up rest of line */
                        fgets(buf, sizeof(buf), file);
                        break;
                }
                break;
            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
        }
    }
    
    fclose(file);
}


/* glmFirstPass: first pass at a Wavefront OBJ file that gets all the
 * statistics of the model (such as #vertices, #normals, etc)
 *
 * model - properly initialized GLMmodel structure
 * file  - (fopen'd) file descriptor
 */
static GLvoid
glmFirstPass(GLMmodel* model, FILE* file)
{
    GLuint  numvertices;        /* number of vertices in model */
    GLuint  numnormals;         /* number of normals in model */
    GLuint  numtexcoords;       /* number of texcoords in model */
    GLuint  numtriangles;       /* number of triangles in model */
    GLMgroup* group;            /* current group */
    unsigned    v, n, t;
    char        buf[128];
    
    /* make a default group */
    group = glmAddGroup(model, "default");
    
    numvertices = numnormals = numtexcoords = numtriangles = 0;
    while(fscanf(file, "%s", buf) != EOF) {
        switch(buf[0]) {
            case '#':               /* comment */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
            case 'v':               /* v, vn, vt */
                switch(buf[1]) {
                    case '\0':          /* vertex */
                        /* eat up rest of line */
                        fgets(buf, sizeof(buf), file);
                        numvertices++;
                        break;
                    case 'n':           /* normal */
                        /* eat up rest of line */
                        fgets(buf, sizeof(buf), file);
                        numnormals++;
                        break;
                    case 't':           /* texcoord */
                        /* eat up rest of line */
                        fgets(buf, sizeof(buf), file);
                        numtexcoords++;
                        break;
                    default:
                        printf("glmFirstPass(): Unknown token \"%s\".\n", buf);
                        exit(1);
                        break;
                }
                break;
            case 'm':
                fgets(buf, sizeof(buf), file);
                sscanf(buf, "%s %s", buf, buf);
                model->mtllibname = strdup(buf);
                glmReadMTL(model, buf);
                break;
            case 'u':
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
            case 'g':               /* group */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
                sscanf(buf, "%s", buf);
#else
                buf[strlen(buf)-1] = '\0';  /* nuke '\n' */
#endif
                group = glmAddGroup(model, buf);
                break;
            case 'f':               /* face */
                v = n = t = 0;
                fscanf(file, "%s", buf);
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if (strstr(buf, "//")) {
                    /* v//n */
                    sscanf(buf, "%d//%d", &v, &n);
                    fscanf(file, "%d//%d", &v, &n);
                    fscanf(file, "%d//%d", &v, &n);
                    numtriangles++;
                    group->numtriangles++;
                    while(fscanf(file, "%d//%d", &v, &n) > 0) {
                        numtriangles++;
                        group->numtriangles++;
                    }
                } else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
                    /* v/t/n */
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    numtriangles++;
                    group->numtriangles++;
                    while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
                        numtriangles++;
                        group->numtriangles++;
                    }
                } else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
                    /* v/t */
                    fscanf(file, "%d/%d", &v, &t);
                    fscanf(file, "%d/%d", &v, &t);
                    numtriangles++;
                    group->numtriangles++;
                    while(fscanf(file, "%d/%d", &v, &t) > 0) {
                        numtriangles++;
                        group->numtriangles++;
                    }
                } else {
                    /* v */
                    fscanf(file, "%d", &v);
                    fscanf(file, "%d", &v);
                    numtriangles++;
                    group->numtriangles++;
                    while(fscanf(file, "%d", &v) > 0) {
                        numtriangles++;
                        group->numtriangles++;
                    }
                }
                break;
                
            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
        }
    }
    
    /* set the stats in the model structure */
    model->numvertices  = numvertices;
    model->numnormals   = numnormals;
    model->numtexcoords = numtexcoords;
    model->numtriangles = numtriangles;
    
    /* allocate memory for the triangles in each group */
    group = model->groups;
    while(group) {
        group->triangles = (GLuint*)malloc(sizeof(GLuint) * group->numtriangles);
        group->numtriangles = 0;
        group = group->next;
    }
}

/* glmSecondPass: second pass at a Wavefront OBJ file that gets all
 * the data.
 *
 * model - properly initialized GLMmodel structure
 * file  - (fopen'd) file descriptor
 */
static GLvoid
glmSecondPass(GLMmodel* model, FILE* file)
{
    GLuint  numvertices;        /* number of vertices in model */
    GLuint  numnormals;         /* number of normals in model */
    GLuint  numtexcoords;       /* number of texcoords in model */
    GLuint  numtriangles;       /* number of triangles in model */
    GLfloat*    vertices;           /* array of vertices  */
    GLfloat*    normals;            /* array of normals */
    GLfloat*    texcoords;          /* array of texture coordinates */
    GLMgroup* group;            /* current group pointer */
    GLuint  material;           /* current material */
    GLuint  v, n, t;
    char        buf[128];
    
    /* set the pointer shortcuts */
    vertices       = model->vertices;
    normals    = model->normals;
    texcoords    = model->texcoords;
    group      = model->groups;
    
    /* on the second pass through the file, read all the data into the
     allocated arrays */
    numvertices = numnormals = numtexcoords = 1;
    numtriangles = 0;
    material = 0;
    while(fscanf(file, "%s", buf) != EOF) {
        switch(buf[0]) {
            case '#':               /* comment */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
            case 'v':               /* v, vn, vt */
                switch(buf[1]) {
                    case '\0':          /* vertex */
                        fscanf(file, "%f %f %f",
                               &vertices[3 * numvertices + 0],
                               &vertices[3 * numvertices + 1],
                               &vertices[3 * numvertices + 2]);
                        numvertices++;
                        break;
                    case 'n':           /* normal */
                        fscanf(file, "%f %f %f",
                               &normals[3 * numnormals + 0],
                               &normals[3 * numnormals + 1],
                               &normals[3 * numnormals + 2]);
                        numnormals++;
                        break;
                    case 't':           /* texcoord */
                        fscanf(file, "%f %f",
                               &texcoords[2 * numtexcoords + 0],
                               &texcoords[2 * numtexcoords + 1]);
                        numtexcoords++;
                        break;
                }
                break;
            case 'u':
                fgets(buf, sizeof(buf), file);
                sscanf(buf, "%s %s", buf, buf);
                group->material = material = glmFindMaterial(model, buf);
                break;
            case 'g':               /* group */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
                sscanf(buf, "%s", buf);
#else
                buf[strlen(buf)-1] = '\0';  /* nuke '\n' */
#endif
                group = glmFindGroup(model, buf);
                group->material = material;
                break;
            case 'f':               /* face */
                v = n = t = 0;
                fscanf(file, "%s", buf);
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if (strstr(buf, "//")) {
                    /* v//n */
                    sscanf(buf, "%d//%d", &v, &n);
                    T(numtriangles).vindices[0] = v;
                    T(numtriangles).nindices[0] = n;
                    fscanf(file, "%d//%d", &v, &n);
                    T(numtriangles).vindices[1] = v;
                    T(numtriangles).nindices[1] = n;
                    fscanf(file, "%d//%d", &v, &n);
                    T(numtriangles).vindices[2] = v;
                    T(numtriangles).nindices[2] = n;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%d//%d", &v, &n) > 0) {
                        T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                        T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                        T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
                        T(numtriangles).vindices[2] = v;
                        T(numtriangles).nindices[2] = n;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                } else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
                    /* v/t/n */
                    T(numtriangles).vindices[0] = v;
                    T(numtriangles).tindices[0] = t;
                    T(numtriangles).nindices[0] = n;
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    T(numtriangles).vindices[1] = v;
                    T(numtriangles).tindices[1] = t;
                    T(numtriangles).nindices[1] = n;
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    T(numtriangles).vindices[2] = v;
                    T(numtriangles).tindices[2] = t;
                    T(numtriangles).nindices[2] = n;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
                        T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                        T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
                        T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                        T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
                        T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
                        T(numtriangles).vindices[2] = v;
                        T(numtriangles).tindices[2] = t;
                        T(numtriangles).nindices[2] = n;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                } else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
                    /* v/t */
                    T(numtriangles).vindices[0] = v;
                    T(numtriangles).tindices[0] = t;
                    fscanf(file, "%d/%d", &v, &t);
                    T(numtriangles).vindices[1] = v;
                    T(numtriangles).tindices[1] = t;
                    fscanf(file, "%d/%d", &v, &t);
                    T(numtriangles).vindices[2] = v;
                    T(numtriangles).tindices[2] = t;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%d/%d", &v, &t) > 0) {
                        T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                        T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                        T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
                        T(numtriangles).vindices[2] = v;
                        T(numtriangles).tindices[2] = t;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                } else {
                    /* v */
                    sscanf(buf, "%d", &v);
                    T(numtriangles).vindices[0] = v;
                    fscanf(file, "%d", &v);
                    T(numtriangles).vindices[1] = v;
                    fscanf(file, "%d", &v);
                    T(numtriangles).vindices[2] = v;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%d", &v) > 0) {
                        T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                        T(numtriangles).vindices[2] = v;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                }
                break;
                
            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
        }
    }
    
#if 0
    /* announce the memory requirements */
    printf(" Memory: %d bytes\n",
           numvertices  * 3*sizeof(GLfloat) +
           numnormals   * 3*sizeof(GLfloat) * (numnormals ? 1 : 0) +
           numtexcoords * 3*sizeof(GLfloat) * (numtexcoords ? 1 : 0) +
           numtriangles * sizeof(GLMtriangle));
#endif
}

/* glmDimensions: Calculates the dimensions (width, height, depth) of
 * a model.
 *
 * model   - initialized GLMmodel structure
 * dimensions - array of 3 GLfloats (GLfloat dimensions[3])
 */
GLvoid
glmDimensions(GLMmodel* model, GLfloat* dimensions)
{
    GLuint i;
    GLfloat maxx, minx, maxy, miny, maxz, minz;
    
    assert(model);
    assert(model->vertices);
    assert(dimensions);
    
    /* get the max/mins */
    maxx = minx = model->vertices[3 + 0];
    maxy = miny = model->vertices[3 + 1];
    maxz = minz = model->vertices[3 + 2];
    for (i = 1; i <= model->numvertices; i++) {
        if (maxx < model->vertices[3 * i + 0])
            maxx = model->vertices[3 * i + 0];
        if (minx > model->vertices[3 * i + 0])
            minx = model->vertices[3 * i + 0];
        
        if (maxy < model->vertices[3 * i + 1])
            maxy = model->vertices[3 * i + 1];
        if (miny > model->vertices[3 * i + 1])
            miny = model->vertices[3 * i + 1];
        
        if (maxz < model->vertices[3 * i + 2])
            maxz = model->vertices[3 * i + 2];
        if (minz > model->vertices[3 * i + 2])
            minz = model->vertices[3 * i + 2];
    }
    
    /* calculate model width, height, and depth */
    dimensions[0] = glmAbs(maxx) + glmAbs(minx);
    dimensions[1] = glmAbs(maxy) + glmAbs(miny);
    dimensions[2] = glmAbs(maxz) + glmAbs(minz);
}

/* glmFacetNormals: Generates facet normals for a model (by taking the
 * cross product of the two vectors derived from the sides of each
 * triangle).  Assumes a counter-clockwise winding.
 *
 * model - initialized GLMmodel structure
 */
GLvoid
glmFacetNormals(GLMmodel* model)
{
    GLuint  i;
    GLfloat u[3];
    GLfloat v[3];
    
    assert(model);
    assert(model->vertices);
    
    /* clobber any old facetnormals */
    if (model->facetnorms)
        free(model->facetnorms);
    
    /* allocate memory for the new facet normals */
    model->numfacetnorms = model->numtriangles;
    model->facetnorms = (GLfloat*)malloc(sizeof(GLfloat) *
                                         3 * (model->numfacetnorms + 1));
    
    for (i = 0; i < model->numtriangles; i++) {
        model->triangles[i].findex = i+1;
        
        u[0] = model->vertices[3 * T(i).vindices[1] + 0] -
        model->vertices[3 * T(i).vindices[0] + 0];
        u[1] = model->vertices[3 * T(i).vindices[1] + 1] -
        model->vertices[3 * T(i).vindices[0] + 1];
        u[2] = model->vertices[3 * T(i).vindices[1] + 2] -
        model->vertices[3 * T(i).vindices[0] + 2];
        
        v[0] = model->vertices[3 * T(i).vindices[2] + 0] -
        model->vertices[3 * T(i).vindices[0] + 0];
        v[1] = model->vertices[3 * T(i).vindices[2] + 1] -
        model->vertices[3 * T(i).vindices[0] + 1];
        v[2] = model->vertices[3 * T(i).vindices[2] + 2] -
        model->vertices[3 * T(i).vindices[0] + 2];
        
        glmCross(u, v, &model->facetnorms[3 * (i+1)]);
        glmNormalize(&model->facetnorms[3 * (i+1)]);
    }
}

/* glmVertexNormals: Generates smooth vertex normals for a model.
 * First builds a list of all the triangles each vertex is in.   Then
 * loops through each vertex in the the list averaging all the facet
 * normals of the triangles each vertex is in.   Finally, sets the
 * normal index in the triangle for the vertex to the generated smooth
 * normal.   If the dot product of a facet normal and the facet normal
 * associated with the first triangle in the list of triangles the
 * current vertex is in is greater than the cosine of the angle
 * parameter to the function, that facet normal is not added into the
 * average normal calculation and the corresponding vertex is given
 * the facet normal.  This tends to preserve hard edges.  The angle to
 * use depends on the model, but 90 degrees is usually a good start.
 *
 * model - initialized GLMmodel structure
 * angle - maximum angle (in degrees) to smooth across
 */
GLvoid
glmVertexNormals(GLMmodel* model, GLfloat angle)
{
    GLMnode*    node;
    GLMnode*    tail;
    GLMnode** members;
    GLfloat*    normals;
    GLuint  numnormals;
    GLfloat average[3];
    GLfloat dot, cos_angle;
    GLuint  i, avg;
    
    assert(model);
    assert(model->facetnorms);
    
    /* calculate the cosine of the angle (in degrees) */
    cos_angle = cos(angle * M_PI / 180.0);
    
    /* nuke any previous normals */
    if (model->normals)
        free(model->normals);
    
    /* allocate space for new normals */
    model->numnormals = model->numtriangles * 3; /* 3 normals per triangle */
    model->normals = (GLfloat*)malloc(sizeof(GLfloat)* 3* (model->numnormals+1));
    
    /* allocate a structure that will hold a linked list of triangle
     indices for each vertex */
    members = (GLMnode**)malloc(sizeof(GLMnode*) * (model->numvertices + 1));
    for (i = 1; i <= model->numvertices; i++)
        members[i] = NULL;
    
    /* for every triangle, create a node for each vertex in it */
    for (i = 0; i < model->numtriangles; i++) {
        node = (GLMnode*)malloc(sizeof(GLMnode));
        node->index = i;
        node->next  = members[T(i).vindices[0]];
        members[T(i).vindices[0]] = node;
        
        node = (GLMnode*)malloc(sizeof(GLMnode));
        node->index = i;
        node->next  = members[T(i).vindices[1]];
        members[T(i).vindices[1]] = node;
        
        node = (GLMnode*)malloc(sizeof(GLMnode));
        node->index = i;
        node->next  = members[T(i).vindices[2]];
        members[T(i).vindices[2]] = node;
    }
    
    /* calculate the average normal for each vertex */
    numnormals = 1;
    for (i = 1; i <= model->numvertices; i++) {
        /* calculate an average normal for this vertex by averaging the
         facet normal of every triangle this vertex is in */
        node = members[i];
        if (!node)
            fprintf(stderr, "glmVertexNormals(): vertex w/o a triangle\n");
        average[0] = 0.0; average[1] = 0.0; average[2] = 0.0;
        avg = 0;
        while (node) {
            /* only average if the dot product of the angle between the two
             facet normals is greater than the cosine of the threshold
             angle -- or, said another way, the angle between the two
             facet normals is less than (or equal to) the threshold angle */
            dot = glmDot(&model->facetnorms[3 * T(node->index).findex],
                         &model->facetnorms[3 * T(members[i]->index).findex]);
            if (dot > cos_angle) {
                node->averaged = GL_TRUE;
                average[0] += model->facetnorms[3 * T(node->index).findex + 0];
                average[1] += model->facetnorms[3 * T(node->index).findex + 1];
                average[2] += model->facetnorms[3 * T(node->index).findex + 2];
                avg = 1;            /* we averaged at least one normal! */
            } else {
                node->averaged = GL_FALSE;
            }
            node = node->next;
        }
        
        if (avg) {
            /* normalize the averaged normal */
            glmNormalize(average);
            
            /* add the normal to the vertex normals list */
            model->normals[3 * numnormals + 0] = average[0];
            model->normals[3 * numnormals + 1] = average[1];
            model->normals[3 * numnormals + 2] = average[2];
            avg = numnormals;
            numnormals++;
        }
        
        /* set the normal of this vertex in each triangle it is in */
        node = members[i];
        while (node) {
            if (node->averaged) {
                /* if this node was averaged, use the average normal */
                if (T(node->index).vindices[0] == i)
                    T(node->index).nindices[0] = avg;
                else if (T(node->index).vindices[1] == i)
                    T(node->index).nindices[1] = avg;
                else if (T(node->index).vindices[2] == i)
                    T(node->index).nindices[2] = avg;
            } else {
                /* if this node wasn't averaged, use the facet normal */
                model->normals[3 * numnormals + 0] =
                model->facetnorms[3 * T(node->index).findex + 0];
                model->normals[3 * numnormals + 1] =
                model->facetnorms[3 * T(node->index).findex + 1];
                model->normals[3 * numnormals + 2] =
                model->facetnorms[3 * T(node->index).findex + 2];
                if (T(node->index).vindices[0] == i)
                    T(node->index).nindices[0] = numnormals;
                else if (T(node->index).vindices[1] == i)
                    T(node->index).nindices[1] = numnormals;
                else if (T(node->index).vindices[2] == i)
                    T(node->index).nindices[2] = numnormals;
                numnormals++;
            }
            node = node->next;
        }
    }
    
    model->numnormals = numnormals - 1;
    
    /* free the member information */
    for (i = 1; i <= model->numvertices; i++) {
        node = members[i];
        while (node) {
            tail = node;
            node = node->next;
            free(tail);
        }
    }
    free(members);
    
    /* pack the normals array (we previously allocated the maximum
     number of normals that could possibly be created (numtriangles *
     3), so get rid of some of them (usually alot unless none of the
     facet normals were averaged)) */
    normals = model->normals;
    model->normals = (GLfloat*)malloc(sizeof(GLfloat)* 3* (model->numnormals+1));
    for (i = 1; i <= model->numnormals; i++) {
        model->normals[3 * i + 0] = normals[3 * i + 0];
        model->normals[3 * i + 1] = normals[3 * i + 1];
        model->normals[3 * i + 2] = normals[3 * i + 2];
    }
    free(normals);
}

/* glmReadOBJ: Reads a model description from a Wavefront .OBJ file.
 * Returns a pointer to the created object which should be free'd with
 * glmDelete().
 *
 * filename - name of the file containing the Wavefront .OBJ format data.
 */
GLMmodel*
glmReadOBJ(char* filename)
{
    GLMmodel* model;
    FILE*   file;
    
    /* open the file */
    file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "glmReadOBJ() failed: can't open data file \"%s\".\n",
                filename);
        exit(1);
    }
    
    /* allocate a new model */
    model = (GLMmodel*)malloc(sizeof(GLMmodel));
    model->pathname    = strdup(filename);
    model->mtllibname    = NULL;
    model->numvertices   = 0;
    model->vertices    = NULL;
    model->numnormals    = 0;
    model->normals     = NULL;
    model->numtexcoords  = 0;
    model->texcoords       = NULL;
    model->numfacetnorms = 0;
    model->facetnorms    = NULL;
    model->numtriangles  = 0;
    model->triangles       = NULL;
    model->nummaterials  = 0;
    model->materials       = NULL;
    model->numgroups       = 0;
    model->groups      = NULL;
    model->position[0]   = 0.0;
    model->position[1]   = 0.0;
    model->position[2]   = 0.0;
    
    /* make a first pass through the file to get a count of the number
     of vertices, normals, texcoords & triangles */
    glmFirstPass(model, file);
    
    /* allocate memory */
    model->vertices = (GLfloat*)malloc(sizeof(GLfloat) *
                                       3 * (model->numvertices + 1));
    model->triangles = (GLMtriangle*)malloc(sizeof(GLMtriangle) *
                                            model->numtriangles);
    if (model->numnormals) {
        model->normals = (GLfloat*)malloc(sizeof(GLfloat) *
                                          3 * (model->numnormals + 1));
    }
    if (model->numtexcoords) {
        model->texcoords = (GLfloat*)malloc(sizeof(GLfloat) *
                                            2 * (model->numtexcoords + 1));
    }
    
    /* rewind to beginning of file and read in the data this pass */
    rewind(file);
    
    glmSecondPass(model, file);
    
    /* close the file */
    fclose(file);
    
    return model;
}


/* glmDraw: Renders the model to the current OpenGL context using the
 * mode specified.
 *
 * model - initialized GLMmodel structure
 * mode  - a bitwise OR of values describing what is to be rendered.
 *             GLM_NONE     -  render with only vertices
 *             GLM_FLAT     -  render with facet normals
 *             GLM_SMOOTH   -  render with vertex normals
 *             GLM_TEXTURE  -  render with texture coords
 *             GLM_COLOR    -  render with colors (color material)
 *             GLM_MATERIAL -  render with materials
 *             GLM_COLOR and GLM_MATERIAL should not both be specified.
 *             GLM_FLAT and GLM_SMOOTH should not both be specified.
 */
GLvoid
glmDraw(GLMmodel* model, GLuint mode)
{
    static GLuint i;
    static GLMgroup* group;
    static GLMtriangle* triangle;
    static GLMmaterial* material;
    
    assert(model);
    assert(model->vertices);
    
    /* do a bit of warning */
    if (mode & GLM_FLAT && !model->facetnorms) {
        printf("glmDraw() warning: flat render mode requested "
               "with no facet normals defined.\n");
        mode &= ~GLM_FLAT;
    }
    if (mode & GLM_SMOOTH && !model->normals) {
        printf("glmDraw() warning: smooth render mode requested "
               "with no normals defined.\n");
        mode &= ~GLM_SMOOTH;
    }
    if (mode & GLM_TEXTURE && !model->texcoords) {
        printf("glmDraw() warning: texture render mode requested "
               "with no texture coordinates defined.\n");
        mode &= ~GLM_TEXTURE;
    }
    if (mode & GLM_FLAT && mode & GLM_SMOOTH) {
        printf("glmDraw() warning: flat render mode requested "
               "and smooth render mode requested (using smooth).\n");
        mode &= ~GLM_FLAT;
    }
    if (mode & GLM_COLOR && !model->materials) {
        printf("glmDraw() warning: color render mode requested "
               "with no materials defined.\n");
        mode &= ~GLM_COLOR;
    }
    if (mode & GLM_MATERIAL && !model->materials) {
        printf("glmDraw() warning: material render mode requested "
               "with no materials defined.\n");
        mode &= ~GLM_MATERIAL;
    }
    if (mode & GLM_COLOR && mode & GLM_MATERIAL) {
        printf("glmDraw() warning: color and material render mode requested "
               "using only material mode.\n");
        mode &= ~GLM_COLOR;
    }
    if (mode & GLM_COLOR)
        glEnable(GL_COLOR_MATERIAL);
    else if (mode & GLM_MATERIAL)
        glDisable(GL_COLOR_MATERIAL);
    
    /* perhaps this loop should be unrolled into material, color, flat,
     smooth, etc. loops?  since most cpu's have good branch prediction
     schemes (and these branches will always go one way), probably
     wouldn't gain too much?  */
    
    group = model->groups;
    while (group) {
        if (mode & GLM_MATERIAL) {
            material = &model->materials[group->material];
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
        }
        
        if (mode & GLM_COLOR) {
            glColor3fv(material->diffuse);
        }
        
        glBegin(GL_TRIANGLES);
        for (i = 0; i < group->numtriangles; i++) {
            triangle = &T(group->triangles[i]);
            
            if (mode & GLM_FLAT)
                glNormal3fv(&model->facetnorms[3 * triangle->findex]);
            
            if (mode & GLM_SMOOTH)
                glNormal3fv(&model->normals[3 * triangle->nindices[0]]);
            if (mode & GLM_TEXTURE)
                glTexCoord2fv(&model->texcoords[2 * triangle->tindices[0]]);
            glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
            
            if (mode & GLM_SMOOTH)
                glNormal3fv(&model->normals[3 * triangle->nindices[1]]);
            if (mode & GLM_TEXTURE)
                glTexCoord2fv(&model->texcoords[2 * triangle->tindices[1]]);
            glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);
            
            if (mode & GLM_SMOOTH)
                glNormal3fv(&model->normals[3 * triangle->nindices[2]]);
            if (mode & GLM_TEXTURE)
                glTexCoord2fv(&model->texcoords[2 * triangle->tindices[2]]);
            glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
            
        }
        glEnd();
        
        if(normalLines == 1){
            glBegin(GL_LINES);
            for (i = 0; i < group->numtriangles; i++) {
                GLfloat glf[3];
                glf[0] = model->vertices[3 * T(i).vindices[0] + 0];
                glf[1] = model->vertices[3 * T(i).vindices[0] + 1];
                glf[2] = model->vertices[3 * T(i).vindices[0] + 2];
                
                GLfloat glf1[3];
                glf1[0] = glf[0] - (glf[0] - model->vertices[3 * T(i).vindices[1] + 0]) / 2;
                glf1[1] = glf[1] - (glf[1] - model->vertices[3 * T(i).vindices[1] + 1]) / 2;
                glf1[2] = glf[2] - (glf[2] - model->vertices[3 * T(i).vindices[1] + 2]) / 2;
                
                GLfloat glf2[3];
                glf2[0] = glf[0] - (glf[0] - model->vertices[3 * T(i).vindices[2] + 0]) / 2;
                glf2[1] = glf[1] - (glf[1] - model->vertices[3 * T(i).vindices[2] + 1]) / 2;
                glf2[2] = glf[2] - (glf[2] - model->vertices[3 * T(i).vindices[2] + 2]) / 2;
                
                glf[0] = glf1[0] - (glf1[0] - glf2[0]) / 2;
                glf[1] = glf1[1] - (glf1[1] - glf2[1]) / 2;
                glf[2] = glf1[2] - (glf1[2] - glf2[2]) / 2;
                
                int f = (i+1)*3;
                
                glVertex3fv(&glf[0]);
                glVertex3f(glf[0] + model->facetnorms[f]*.1, glf[1] + model->facetnorms[f+1]*.1, glf[2] + model->facetnorms[f+2]*.1);
            }
            glEnd();
        }
        group = group->next;
    }
}

/* glmList: Generates and returns a display list for the model using
 * the mode specified.
 *
 * model - initialized GLMmodel structure
 * mode  - a bitwise OR of values describing what is to be rendered.
 *             GLM_NONE     -  render with only vertices
 *             GLM_FLAT     -  render with facet normals
 *             GLM_SMOOTH   -  render with vertex normals
 *             GLM_TEXTURE  -  render with texture coords
 *             GLM_COLOR    -  render with colors (color material)
 *             GLM_MATERIAL -  render with materials
 *             GLM_COLOR and GLM_MATERIAL should not both be specified.
 * GLM_FLAT and GLM_SMOOTH should not both be specified.
 */
GLuint
glmList(GLMmodel* model, GLuint mode)
{
    GLuint list;
    
    list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glmDraw(model, mode);
    glEndList();
    
    return list;
}

void
lists(void)
{
    if (model_list)
        glDeleteLists(model_list, 1);
    
    /* generate a list */
    if (material_mode == 0) { 
        if (facet_normal)
            model_list = glmList(model, GLM_FLAT);
        else
            model_list = glmList(model, GLM_SMOOTH);
    } else if (material_mode == 1) {
        if (facet_normal)
            model_list = glmList(model, GLM_FLAT | GLM_COLOR);
        else
            model_list = glmList(model, GLM_SMOOTH | GLM_COLOR);
    } else if (material_mode == 2) {
        if (facet_normal)
            model_list = glmList(model, GLM_FLAT | GLM_MATERIAL);
        else
            model_list = glmList(model, GLM_SMOOTH | GLM_MATERIAL);
    }
}

void
init(void)
{
    gltbInit(GLUT_LEFT_BUTTON);
    
    /* read in the model */
    model = glmReadOBJ(model_file);
    glmFacetNormals(model);
    glmVertexNormals(model, smoothing_angle);
    
    if (model->nummaterials > 0)
        material_mode = 2;
    
    /* create new display lists */
    lists();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void
menu(int item)
{
    int i = 0;
    DIR* dirp;
    char* name;
    struct dirent* direntp;
    
    if (item > 0) {
        //keyboard((unsigned char)item, 0, 0);
    } else {
        dirp = opendir(DATA_DIR);
        while ((direntp = readdir(dirp)) != NULL) {
            if (strstr(direntp->d_name, ".obj")) {
                i++;
                if (i == -item)
                    break;
            }
        }
        if (!direntp)
            return;
        name = (char*)malloc(strlen(direntp->d_name) + strlen(DATA_DIR) + 1);
        strcpy(name, DATA_DIR);
        strcat(name, direntp->d_name);
        model = glmReadOBJ(name);
        glmFacetNormals(model);
        glmVertexNormals(model, smoothing_angle);
        
        if (model->nummaterials > 0)
            material_mode = 2;
        else
            material_mode = 0;
        
        lists();
        free(name);
        
        glutPostRedisplay();
    }
}

void
motion(int x, int y)
{
    GLdouble model[4*4];
    GLdouble proj[4*4];
    GLint view[4];
    
    gltbMotion(x, y);
    
    if (mouse_state == GLUT_DOWN && mouse_button == GLUT_MIDDLE_BUTTON) {
        glGetDoublev(GL_MODELVIEW_MATRIX, model);
        glGetDoublev(GL_PROJECTION_MATRIX, proj);
        glGetIntegerv(GL_VIEWPORT, view);
        gluProject((GLdouble)x, (GLdouble)y, 0.0,
            model, proj, view,
            &pan_x, &pan_y, &pan_z);
        gluUnProject((GLdouble)x, (GLdouble)y, pan_z,
            model, proj, view,
            &pan_x, &pan_y, &pan_z);
        pan_y = -pan_y;
    }
    
    glutPostRedisplay();
}

void
mouse(int button, int state, int x, int y)
{
    GLdouble model[4*4];
    GLdouble proj[4*4];
    GLint view[4];
    
    /* fix for two-button mice -- left mouse + shift = middle mouse */
    if (button == GLUT_LEFT_BUTTON && glutGetModifiers() & GLUT_ACTIVE_SHIFT)
        button = GLUT_MIDDLE_BUTTON;
    
    gltbMouse(button, state, x, y);
    
    mouse_state = state;
    mouse_button = button;
    
    if (state == GLUT_DOWN && button == GLUT_MIDDLE_BUTTON) {
        glGetDoublev(GL_MODELVIEW_MATRIX, model);
        glGetDoublev(GL_PROJECTION_MATRIX, proj);
        glGetIntegerv(GL_VIEWPORT, view);
        gluProject((GLdouble)x, (GLdouble)y, 0.0,
            model, proj, view,
            &pan_x, &pan_y, &pan_z);
        gluUnProject((GLdouble)x, (GLdouble)y, pan_z,
            model, proj, view,
            &pan_x, &pan_y, &pan_z);
        pan_y = -pan_y;
    }
    
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    GLint params[2];
    
    switch (key) {
        case 'p':
            performance = !performance;
            break;
        case 'l':
        case 'L':
            illuminate = !illuminate;
            lists();
            break;
        case 'm':
        case 'M':
            material_mode++;
            if (material_mode > 2)
                material_mode = 0;
            printf("material_mode = %d\n", material_mode);
            lists();
            break;
        case 'w':
        case 'W':
            glGetIntegerv(GL_POLYGON_MODE, params);
            if (params[0] == GL_FILL)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;   
        case 'c':
        case 'C':
            if (glIsEnabled(GL_CULL_FACE))
                glDisable(GL_CULL_FACE);
            else
                glEnable(GL_CULL_FACE);
            break;
        case 'd':
        case 'D':
            if (glIsEnabled(GL_DEPTH_TEST))
                glDisable(GL_DEPTH_TEST);
            else
                glEnable(GL_DEPTH_TEST);
            break;
        case 'n':   
        case 'N':
            if(normalLines == 0)
                normalLines = 1;
            else
                normalLines = 0;
            break;
        case 'F':    
        case 'f':
            facet_normal = !facet_normal;
            lists();
            break;
        case 'r':   
        case 'R':
            {
                GLuint i;
                GLfloat swap;
                for (i = 1; i <= model->numvertices; i++) {
                    swap = model->vertices[3 * i + 1];
                    model->vertices[3 * i + 1] = model->vertices[3 * i + 2];
                    model->vertices[3 * i + 2] = -swap;
                }
                glmFacetNormals(model);
                lists();
                break;
            }
        case 'Z':
            zoom *= .95;
            break;
        case 'z':
            zoom *= 1.05;
            break;         
        case '<':
            theta1 -= 5.0;
            if(theta1 < 360){
                theta1 += 360;
            }
            break;
        case '>':
            theta1 += 5.0;
            if(theta1 > 360){
                theta1 -= 360;
            }
            break;   
        case '-':
            smoothing_angle -= 1.0;
            printf("Smoothing angle: %.1f\n", smoothing_angle);
            glmVertexNormals(model, smoothing_angle);
            lists();
            break;   
        case '+':
            smoothing_angle += 1.0;
            printf("Smoothing angle: %.1f\n", smoothing_angle);
            glmVertexNormals(model, smoothing_angle);
            lists();
            break;       
        case 27:
            exit(0);
            break;
    }
    
    glutPostRedisplay();
}

#define NUM_FRAMES 5
void display(void)
{
    static char s[256], t[32];
    static char* p;
    static int frames = 0;
    
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
    gluLookAt(0.00, 0.00, zoom*1.00 + 2.00,
              0.00, 0.00, 0.00,
              0.00, 1.00, 0.00);
    
    glPushMatrix();
        glTranslatef(pan_x, pan_y, 0.0);
        
        gltbMatrix();
        
        if (material_mode == 0) { 
            if (facet_normal)
                glmDraw(model, GLM_FLAT);
            else
                glmDraw(model, GLM_SMOOTH);
        } 
        else if (material_mode == 1) {
            if (facet_normal)
                glmDraw(model, GLM_FLAT | GLM_COLOR);
            else
                glmDraw(model, GLM_SMOOTH | GLM_COLOR);
        } 
        else if (material_mode == 2) {
            if (facet_normal)
                glmDraw(model, GLM_FLAT | GLM_MATERIAL);
            else
                glmDraw(model, GLM_SMOOTH | GLM_MATERIAL);
        }
    
        glDisable(GL_LIGHTING);

        //lighting
        if(illuminate == GL_FALSE){
            glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
            glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

            glPushMatrix();  // Saves the glu:LookAt() matrix on the top of the stack for later recovery
            glRotatef(theta1,1,0,0); 
            glTranslatef(0,0,20);  //Post multiplies a translation in the - z direction
            glLightfv(GL_LIGHT0, GL_POSITION, light_position);  //Positions the light
            glPopMatrix();

            glEnable(GL_LIGHT0);
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

        }
        else{
            glDisable(GL_LIGHT0);
        }

    glPopMatrix();
    glutSwapBuffers();
    glEnable(GL_LIGHTING);

}

void reshape(int width, int height)
{
    gltbReshape(width, height);
    
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)height / (GLfloat)width, 1.0, 128.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -3.0);
}

int main(int argc, char** argv)
{
    zoom = 1.0;
    struct dirent* direntp;
    DIR* dirp;
    int models;

    glutInitWindowSize(512, 512);
    glutInit(&argc, argv);
    
    while (--argc) {
        model_file = argv[argc];
    }
    if (!model_file) {
        model_file = "data/cow2.obj";
    }
    
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("HW5");
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    
    models = glutCreateMenu(menu);
    dirp = opendir(DATA_DIR);
    if (!dirp) {
        fprintf(stderr, "%s: can't open data directory.\n", argv[0]);
    } else {
        while ((direntp = readdir(dirp)) != NULL) {
            if (strstr(direntp->d_name, ".obj")) {
                entries++;
                glutAddMenuEntry(direntp->d_name, -entries);
            }
        }
        closedir(dirp);
    }
    
    printf("help\n\n");
    printf("p         -  Toggle performance indicator\n");
    printf("l         -  Toggle lighting on the object\n");
    printf("m         -  Toggle color/material/none mode\n");
    printf("w         -  Toggle wireframe/filled\n");
    printf("c         -  Toggle culling\n");
    printf("d         -  Toggle depth testing\n");
    printf("n         -  Toggle normal/orthoganal lines\n");
    printf("f         -  Toggle flat shading\n");
    printf("r         -  Rotate scene/cow 45 degrees\n");
    printf("Z/z       -  Zoom in/out\n");
    printf("</>       -  Rotate light source\n");
    printf("+/-       -  Increase/decrease smoothing angle\n");
    printf("q/escape  -  Quit\n\n");

    glutCreateMenu(menu);
        glutAddMenuEntry("Smooth", 0);
        glutAddMenuEntry("", 0);
        glutAddSubMenu("Models", models);
        glutAddMenuEntry("", 0);
        glutAddMenuEntry("[p]   Toggle frame rate on/off", 'p');
        glutAddMenuEntry("[l]   Toggle lighting", 'l');
        glutAddMenuEntry("[m]   Toggle color/material/none mode", 'm');
        glutAddMenuEntry("[w]   Toggle wireframe/filled", 'w');
        glutAddMenuEntry("[c]   Toggle culling on/off", 'c');
        glutAddMenuEntry("[d]   Toggle depth testing on/off", 'd');
        glutAddMenuEntry("[n]   Toggle normal/orthoganal lines", 'f');
        glutAddMenuEntry("[f]   Toggle flat shading", 'n');
        glutAddMenuEntry("[r]   Rotate", 'r');
        glutAddMenuEntry("[Z]   Zoom in", 'Z');
        glutAddMenuEntry("[z]   Zoom out", 'z');
        glutAddMenuEntry("[<]   Rotate light source counter-clock wise", '<');
        glutAddMenuEntry("[>]   Rotate light source clock-wise", '>');
        glutAddMenuEntry("[+]   Increase smoothing angle", '+');
        glutAddMenuEntry("[-]   Decrease smoothing angle", '-');
        glutAddMenuEntry("", 0);
        glutAddMenuEntry("[Esc] Quit", 27);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    init();
    
    glutMainLoop();
    return 0;
}