#include <stdio.h>
#include <vector>

#include <GL/gl.h>
#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>

#include "linmath.h"

GLFWwindow *g_pWindow;
int g_iWidth, g_iHeight;

static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
    {
        {-0.6f, -0.4f, 1.f, 0.f, 0.f},
        {0.6f, -0.4f, 0.f, 1.f, 0.f},
        {0.f, 0.6f, 0.f, 0.f, 1.f}};
static const char *vertex_shader_text =
    "uniform mat4 MVP;\n"
    "attribute vec3 vCol;\n"
    "attribute vec2 vPos;\n"
    "varying vec3 color;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
    "    color = vCol;\n"
    "}\n";
static const char *fragment_shader_text =
    //"#extension GL_ARB_shader_clock : enable"
    "varying vec3 color;\n"
    "void main()\n"
    "{\n"
    // "    uvec2 t = clock2x32ARB\n"
    // "    gl_FragColor = vec4(t.x, t.y, 0.0, 1.0);\n"
    "    gl_FragColor = vec4(color, 1.0);\n"
    "}\n";

void error_callback(int error, const char *description)
{
    printf("Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int Initialization()
{
    if (!glfwInit())
    {
        printf("ERROR: Initialization failed");
        return 1;
    }
    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    g_pWindow = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    if (!g_pWindow)
    {
        glfwTerminate();
        printf("ERROR: Window or context creation failed");
        return 1;
    }

    glfwSetKeyCallback(g_pWindow, key_callback);
    glfwMakeContextCurrent(g_pWindow);

    printf("Done: initialization %i\n", g_pWindow);

    // glfwGetFramebufferSize(g_pWindow, &g_iWidth, &g_iHeight);
    // glViewport(0, 0, g_iWidth, g_iHeight);
}

int Run()
{

    // double time;
    // NOTE: OpenGL error checks have been omitted for brevity
    if (glfwExtensionSupported 	("GL_ARB_shader_clock"))
    {
        printf("SUPPORT \n");
    }
    else
        printf("NOOO \n");
    GLuint vertex_buffer;
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;
    GLuint mvp_location, vpos_location, vcol_location;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    // GLint isCompiled = 0;
    // glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
    // if (isCompiled == GL_FALSE)
    // {
    //     GLint maxLength = 0;
    //     glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

    //     // The maxLength includes the NULL character
    //     std::vector<GLchar> errorLog(maxLength);
    //     glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &errorLog[0]);

    //     printf("COMPILE ERROR V: %s\n", errorLog.at(0));

    //     // Provide the infolog in whatever manor you deem best.
    //     // Exit with failure.
    //     glDeleteShader(vertex_shader); // Don't leak the shader.
    //     return 1;
    // }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    // glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &isCompiled);
    // if (isCompiled == GL_FALSE)
    // {
    //     GLint maxLength = 0;
    //     glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

    //     // The maxLength includes the NULL character
    //     std::vector<GLchar> errorLog(maxLength);
    //     glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &errorLog[0]);
    //     printf("COMPILE ERROR F: %s\n", errorLog.at(0));
        
    //     // Provide the infolog in whatever manor you deem best.
    //     // Exit with failure.
    //     glDeleteShader(fragment_shader); // Don't leak the shader.
    //     return 1;
    // }
    program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
    glEnableVertexAttribArray(vpos_location);

    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 5, (void *)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 5, (void *)(sizeof(float) * 2));

    GLboolean toClose = GL_FALSE;
    while (!toClose)
    {
        toClose = glfwWindowShouldClose(g_pWindow);
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;

        glfwGetFramebufferSize(g_pWindow, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT);
        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float)glfwGetTime());
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(g_pWindow);
        glfwPollEvents();
    }
    return 0;
}

int Finalize()
{
    glfwDestroyWindow(g_pWindow);

    glfwTerminate();
    return 0;
}

void InitData()
{
}

int main(int argv, char **args)
{
    Initialization();
    Run();
    Finalize();

    return 0;
}