#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define WIDTH 700
#define HEIGHT 700

int Window,
    Width,
    Height;
unsigned int frames;
/* coob below */
/*GLfloat Vertices[] = {
    /* Position /        /* Texture Coords /
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};*/
/* pyramud */
GLfloat Vertices[] = {
    -0.5, -0.5, -0.5,  0.0, 0.0,
     0.0,  0.5,  0.0,  0.5, 1.0,
     0.5, -0.5, -0.5,  1.0, 0.0,

    -0.5, -0.5, -0.5,  1.0, 0.0,
    -0.5, -0.5,  0.5,  0.0, 0.0,
     0.0,  0.5,  0.0,  0.5, 1.0,

    -0.5, -0.5,  0.5,  1.0, 0.0,
     0.5, -0.5,  0.5,  0.0, 0.0,
     0.0,  0.5,  0.0,  0.5, 1.0,

     0.5, -0.5, -0.5,  0.0, 0.0,
     0.5, -0.5,  0.5,  1.0, 0.0,
     0.0,  0.5,  0.0,  0.5, 1.0,

    -0.5, -0.5,  0.5,  0.0, 0.0,
     0.5, -0.5,  0.5,  1.0, 0.0,
     0.5, -0.5, -0.5,  1.0, 1.0,
    -0.5, -0.5,  0.5,  0.0, 0.0,
    -0.5, -0.5, -0.5,  0.0, 1.0,
     0.5, -0.5, -0.5,  1.0, 1.0
};
GLfloat Projection_mat[] = {
      1,   0,   0,   0,
      0,   1,   0,   0,
      0,   0,   1,   0,
      0,   0,  .58,   1
};
float Cam_rot = 0;
char Freeze_time,
     Cam_dir;
GLuint VBO,
       VAO,
       Vtx_shader_obj,
       Frag_shader_obj,
       Shader_prgm,
       Texture1_obj,
       Texture2_obj;

uint32_t ptime_us;

const GLchar *Vtx_shader =
"#version 330 core\n"
"\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec2 in_tex_coord;\n"
"\n"
"out vec4 color;\n"
"out vec2 tex_coord;\n"
"\n"
"uniform float time;\n"
"uniform mat4 projection_mat;\n"
"uniform mat4 cam;\n"
"\n"
"void main() {\n"
"    float a = cos(time);\n"
"    float b = sin(time);\n"
"    gl_Position = projection_mat * cam * mat4(vec4(1.0, 0.0, 0.0, 0.0), vec4(0.0, a, b, 0.0), vec4(0.0, -b, a, 0.0), vec4(0.0, 0.0, 0, 1.0)) * vec4(position, 1.0f);\n"
"    tex_coord = vec2(in_tex_coord.x, 1 - in_tex_coord.y);\n"
"}";
const GLchar *Frag_shader =
"#version 330 core\n"
"\n"
"in vec2 tex_coord;\n"
"\n"
"out vec4 ex_color;\n"
"\n"
"uniform sampler2D texture1;\n"
"uniform sampler2D texture2;\n"
"uniform bool invert;\n"
"\n"
"void main() {\n"
"    ex_color = mix(texture(texture1, tex_coord), texture(texture2, tex_coord), 0.5);\n"
"    if (invert) ex_color = 1 - ex_color;\n"
"}";

#define LEFT -1;
#define RIGHT 1;
#define IDLE 0;


int main(int argc, char **argv);
void init(int argc, char **argv);
void win_resize(int w, int h);
void win_render();
void compile_shaders();
void link_shaders();
void VAO_setup();
void mouse_func(int button, int state);
void timer_func();
void texture_init(const char *filename, const char *filename2);
void special_func(int key);
void specialup_func(int key);
void keyboard_func(unsigned char key);
void keyboardup_func(unsigned char key);
void handle_sig(int sig);
void cleanup();
unsigned char *load_image(const char *filename, unsigned int *width, unsigned int *height);

int main(int argc, char **argv) {

    /* clean up if killed */
    struct sigaction sa;
    sa.sa_handler = &handle_sig;
    sa.sa_flags = SA_RESTART;
    sigfillset(&sa.sa_mask);
    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);


    init(argc, argv);

    compile_shaders();
    link_shaders();

    glDeleteShader(Vtx_shader_obj);
    glDeleteShader(Frag_shader_obj);

    VAO_setup();
    if (argc > 1) {
        if (argc > 2) {
            texture_init(argv[1], argv[2]);
        } else {
            texture_init(argv[1], "texture2.ppm");
        }
    } else {
        texture_init("texture.ppm", "texture2.ppm");
    }

    glUseProgram(Shader_prgm);

    glUniformMatrix4fv(glGetUniformLocation(Shader_prgm, "projection_mat"), 1, GL_TRUE, Projection_mat);

    struct timeval time;
    gettimeofday(&time, NULL);
    ptime_us = time.tv_sec * 1000000 + time.tv_usec;

    glutMainLoop();

    cleanup();

    return 0;
}

void init(int argc, char **argv) {
    glutInit(&argc, argv);

    glutInitContextVersion(3, 3); /* use Opengl 3.3 */
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutInitWindowSize(WIDTH, HEIGHT);

    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

    Window = glutCreateWindow("hai");

    if (Window < 1) {
        fprintf(stderr, "error creating window\n");
        exit(1);
    }

    glutReshapeFunc(win_resize);
    glutDisplayFunc(win_render);
    glutMouseFunc((void(*)(int,int,int,int)) mouse_func);
    glutTimerFunc(0, timer_func, 0);
    glutSpecialFunc((void(*)(int,int,int))special_func);
    glutSpecialUpFunc((void(*)(int,int,int))specialup_func);
    glutKeyboardFunc((void(*)(unsigned char,int,int))keyboard_func);
    glutKeyboardUpFunc((void(*)(unsigned char,int,int))keyboardup_func);


    glewExperimental = GL_TRUE; /* don't segfault when i call glGenVertexArrays() */
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "error initializing glew: %s\n", glewGetErrorString(err));
        exit(2);
    }
    printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));
    printf("OpenGL %s\n", glGetString(GL_VERSION));
    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.8666f, 0.9333f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void win_resize(int w, int h) {
    Width = w;
    Height = h;
    glViewport(0, 0, w, h);
}

GLfloat teh_time = 0;
float inc = .03;
#define PI_2 6.28318530718f
void win_render() {

    struct timeval time;
    gettimeofday(&time, NULL);
    uint32_t time_us = time.tv_sec * 1000000 + time.tv_usec;
    float dtime_us = time_us - ptime_us;
    ptime_us = time_us;

    if (!Freeze_time) {
        teh_time += dtime_us / 1000000;
        while (teh_time > PI_2) teh_time -= PI_2;
    }

    if (Cam_dir) Cam_rot += dtime_us * Cam_dir / 400000;

    float a = cosf(Cam_rot);
    float b = sinf(Cam_rot);
    float cam_mat[] = {
        a, 0, b, 0,
        0, 1, 0, 0,
       -b, 0, a, .5,
        0, 0, 0, 1
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture1_obj);
    glUniform1i(glGetUniformLocation(Shader_prgm, "texture1"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Texture2_obj);
    glUniform1i(glGetUniformLocation(Shader_prgm, "texture2"), 1);

    glUniform1f(glGetUniformLocation(Shader_prgm, "time"), teh_time);
    glUniformMatrix4fv(glGetUniformLocation(Shader_prgm, "cam"), 1, GL_TRUE, cam_mat);

    glBindVertexArray(VAO);
     glDrawArrays(GL_TRIANGLES, 0, sizeof(Vertices) / sizeof(GLuint) / 3);
    glBindVertexArray(0);

    frames++;
    glutSwapBuffers();
    glutPostRedisplay();
}

void compile_shaders() {
    Vtx_shader_obj = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(Vtx_shader_obj, 1, &Vtx_shader, NULL);
    glCompileShader(Vtx_shader_obj);

    GLint compile_status;
    glGetShaderiv(Vtx_shader_obj, GL_COMPILE_STATUS, &compile_status);
    GLchar info[512];
    if (compile_status != GL_TRUE) {
        glGetShaderInfoLog(Vtx_shader_obj, 512, NULL, info);
        fprintf(stderr, "error compiling vertex shader:\n%s\n", info);
    }

    Frag_shader_obj = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(Frag_shader_obj, 1, &Frag_shader, NULL);
    glCompileShader(Frag_shader_obj);

    glGetShaderiv(Frag_shader_obj, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        glGetShaderInfoLog(Frag_shader_obj, 512, NULL, info);
        fprintf(stderr, "error compiling fragment shader:\n%s\n", info);
    }
}

void link_shaders() {
    Shader_prgm = glCreateProgram();

    glAttachShader(Shader_prgm, Vtx_shader_obj);
    glAttachShader(Shader_prgm, Frag_shader_obj);

    glLinkProgram(Shader_prgm);

    GLchar info[512];
    GLint link_status;
    glGetProgramiv(Shader_prgm, GL_LINK_STATUS, &link_status);

    if (link_status != GL_TRUE) {
        glGetProgramInfoLog(Shader_prgm, 512, NULL, info);
        fprintf(stderr, "error linking shaders:\n%s\n", info);
    }
}

void VAO_setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*) 0);
     glEnableVertexAttribArray(0);

     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*) (sizeof(GLfloat) * 3));
     glEnableVertexAttribArray(1);

     glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void mouse_func(int button, int state) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            glUniform1i(glGetUniformLocation(Shader_prgm, "invert"), 1);
        } else {
            glUniform1i(glGetUniformLocation(Shader_prgm, "invert"), 0);
        }
    }
}

char fps_string[20];
void timer_func() {
    snprintf(fps_string, 20, "%d FPS", frames * 4);
    glutSetWindowTitle(fps_string);

    frames = 0;

    glutTimerFunc(250, timer_func, 0);
}

void texture_init(const char *filename, const char *filename2) {

    unsigned int width, height;
    unsigned char *image_data = load_image(filename, &width, &height);

    glGenTextures(1, &Texture1_obj);
    glBindTexture(GL_TEXTURE_2D, Texture1_obj);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    free(image_data);


    image_data = load_image(filename2, &width, &height);
    glGenTextures(1, &Texture2_obj);
    glBindTexture(GL_TEXTURE_2D, Texture2_obj);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    free(image_data);

    glBindTexture(GL_TEXTURE_2D, 0);

}

void special_func(int key) {
    switch (key) {

        case GLUT_KEY_LEFT:
            Cam_dir = LEFT;
            break;

        case GLUT_KEY_RIGHT:
            Cam_dir = RIGHT;
            break;

        case GLUT_KEY_UP:
            Freeze_time = 1;
            break;

        case GLUT_KEY_DOWN:
            glDisable(GL_MULTISAMPLE);
    }
}

void specialup_func(int key) {
    switch (key) {

        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            Cam_dir = IDLE;
            break;

        case GLUT_KEY_UP:
            Freeze_time = 0;
            break;

        case GLUT_KEY_DOWN:
            glEnable(GL_MULTISAMPLE);
    }
}

void keyboard_func(unsigned char key) {
    switch(key) {

        case 'h':
            Cam_dir = LEFT;
            break;

        case 'l':
            Cam_dir = RIGHT;
            break;

        case 'k':
            Freeze_time = 1;
            break;

        case 'j':
            glDisable(GL_MULTISAMPLE);
            break;
    }
}

void keyboardup_func(unsigned char key) {
    switch(key) {

        case 'h':
        case 'l':
            Cam_dir = IDLE;
            break;

        case 'k':
            Freeze_time = 0;
            break;

        case 'j':
            glEnable(GL_MULTISAMPLE);
    }
}

void handle_sig(int signal) {
    switch (signal) {
        case SIGINT:
            printf("Caught SIGINT, cleaning up...\n");
            break;
        case SIGHUP:
            printf("Caught SIGHUP, cleaning up...\n");
    }

    glutLeaveMainLoop();
}

void cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    printf("potatos are done \n"); //???
}

unsigned char *load_image(const char *filename, unsigned int *width, unsigned int *height) {

    FILE *file = fopen(filename, "r");

    if (!file) {
        fprintf(stderr, "error opening %s\n", filename);
        exit(3);
    }

    int version, max;
    int matched = fscanf(file, "P%d %d %d %d ", &version, width, height, &max);

    if (matched != 4) {

        fprintf(stderr, "is dis an legit ppm?\n");
        fclose(file);
        exit(4);

    } else if (max != 255) {

        fprintf(stderr, "make the max 255 or strange things may happen\n");

    }
    if (version == 6) {

        unsigned char *image_data = malloc((*width) * (*height) * 3);
        fread(image_data, 3, (*height) * (*width), file);
        fclose(file);

        return image_data;

    } else {

        fprintf(stderr, "sorry bro i dont support dis format ¯\\_(ツ)_/¯\n"
                        "binary ppms only plz\n");
        fclose(file);
        exit(5);

    }
}
