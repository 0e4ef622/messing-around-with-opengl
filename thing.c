#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <pthread.h>

#define WIDTH 700
#define HEIGHT 700

int Window,
    Width,
    Height;
unsigned int frames;
GLfloat Vertices[] = {
    /* Position */         /* Colors */         /* Texture Coords */
    -0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
    -0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f,
     0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f
     /*0.0f,  0.0f, 0.0f,    0.5f, 0.5f, 0.5f*/
};
GLuint Indices[] = {
    0, 1, 2,
    1, 2, 3
    /*
    0, 4, 1,
    0, 4, 2,
    2, 4, 3
    */
};
GLfloat Projection_mat[] = {
      1,   0,   0,   0,
      0,   1,   0,   0,
      0,   0,   1,   0,
      0,   0,  .58,   1
};
GLuint VBO,
       VAO,
       EBO,
       Vtx_shader_obj,
       Frag_shader_obj,
       Shader_prgm,
       Texture1_obj,
       Texture2_obj;

const GLchar *Vtx_shader =
"#version 330 core\n"
"\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 in_color;\n"
"layout (location = 2) in vec2 in_tex_coord;\n"
"\n"
"out vec4 color;\n"
"out vec2 tex_coord;\n"
"\n"
"uniform float time;\n"
"uniform mat4 projection_mat;\n"
"\n"
"void main() {\n"
"    float a = cos(1.57079632679);\n" /* pi/2 */
"    float b = sin(1.57079632679);\n"
"    float c = cos(time);\n"
"    float d = sin(time);\n"
"    gl_Position = projection_mat * mat4(vec4(1.0, 0.0, 0.0, 0.0), vec4(0.0, a, b, 0.0), vec4(0.0, -b, a, 0.0), vec4(c*0.6, d*0.6, 0.5, 1.0)) * vec4(position.xyz, 1.0f);\n"
"    color = vec4(in_color.xyz, 1.0f);\n"
"    tex_coord = vec2(in_tex_coord.x, 1 - in_tex_coord.y);\n"
"}";
const GLchar *Frag_shader =
"#version 330 core\n"
"\n"
"in vec4 color;\n"
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
void timer();
unsigned char *load_image(const char *filename, unsigned int *width, unsigned int *height);

int main(int argc, char **argv) {

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

    pthread_t timer_thread;
    pthread_create(&timer_thread, NULL, (void*(*)(void*)) timer, NULL); /* an ugly cast to silence some errors ¯\_(ツ)_/¯ */
    glutMainLoop();

    return 0;
}

void init(int argc, char **argv) {
    glutInit(&argc, argv);

    glutInitContextVersion(3, 3); /* use Opengl 3.3 */
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutInitWindowSize(WIDTH, HEIGHT);

    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

    Window = glutCreateWindow("hai");

    if (Window < 1) {
        fprintf(stderr, "error creating window\n");
        exit(1);
    }

    glutReshapeFunc(win_resize);
    glutDisplayFunc(win_render);
    glutMouseFunc((void(*)(int,int,int,int)) mouse_func);
    glutTimerFunc(0, timer_func, 0);


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
}

void win_resize(int w, int h) {
    Width = w;
    Height = h;
    glViewport(0, 0, w, h);
}

GLfloat teh_time = 0;
float inc = .03;
#define PI_2 (2.0f*3.141592653589793f)
void win_render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture1_obj);
    glUniform1i(glGetUniformLocation(Shader_prgm, "texture1"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Texture2_obj);
    glUniform1i(glGetUniformLocation(Shader_prgm, "texture2"), 1);

    glUniform1f(glGetUniformLocation(Shader_prgm, "time"), teh_time);

    glBindVertexArray(VAO);
     glDrawElements(GL_TRIANGLES, sizeof(Indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
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
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) 0);
     glEnableVertexAttribArray(0);

     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (sizeof(GLfloat) * 3));
     glEnableVertexAttribArray(1);

     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (sizeof(GLfloat) * 6));
     glEnableVertexAttribArray(2);

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

void timer() {
    for (;;) {
        teh_time += .003;
        if (teh_time > PI_2) teh_time -= PI_2;
        usleep(1000);
    }
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
