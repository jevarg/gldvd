#include <iostream>
#include <thread>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "shaders.h"
#include "texture.h"

// GL stuff
GLuint gVAO;
GLuint gVBO;
GLuint gTexture;
GLuint gShaderProgram;

// Config
constexpr uint32_t maxFPS = 144;
constexpr uint32_t targetFrameTime = 1000 / maxFPS;
constexpr float speed = 1.0f;
constexpr float size = 0.3f;

float gDir[] = {0.5, 0.8};
GLfloat gVertices[] = {
    // pos             // uv
    -1.0f, 1.0f, 0.0f, 0, 0,
    -(1 - size), 1.0f, 0.0f, 1, 0,
    -1.0f, (1 - size), 0.0f, 0, 1,
    -1.0f, (1 - size), 0.0f, 0, 1,
    -(1 - size), 1.0f, 0.0f, 1, 0,
    -(1 - size), (1 - size), 0.0f, 1, 1,
};

void initShaders() {
    int32_t compiled;

    // Vertex shader
    const uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        char errorLog[256];
        glGetShaderInfoLog(vertexShader, 256, reinterpret_cast<int *>(256), errorLog);
        std::cerr << errorLog << std::endl;
        throw std::runtime_error("Could not compile vertex shader!");
    }

    // Fragment shader
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        char errorLog[256];
        glGetShaderInfoLog(fragmentShader, 256, reinterpret_cast<int *>(256), errorLog);
        std::cerr << errorLog << std::endl;
        throw std::runtime_error("Could not compile fragment shader!");
    }

    gShaderProgram = glCreateProgram();
    glBindAttribLocation(gShaderProgram, 0, "aPos");
    glBindAttribLocation(gShaderProgram, 1, "aTexCoord");

    glAttachShader(gShaderProgram, vertexShader);
    glAttachShader(gShaderProgram, fragmentShader);

    glLinkProgram(gShaderProgram);
    glUseProgram(gShaderProgram);

    std::cout << "Shaders compiled!" << std::endl;
}

void initGL() {
    // VAO
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    // VBO
    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(GLfloat), gVertices, GL_STATIC_DRAW);

    constexpr auto stride = 5 * sizeof(GLfloat);

    // Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, nullptr);
    glEnableVertexAttribArray(0);

    // UVs
    glVertexAttribPointer(1, 2, GL_FLOAT, false, stride, reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture
    glGenTextures(1, &gTexture);
    glBindTexture(GL_TEXTURE_2D, gTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, DVD_WIDTH, DVD_HEIGHT, 0,  GL_RED, GL_UNSIGNED_BYTE, dvdTexture);

    constexpr int32_t swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_RED};
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    initShaders();
}

void update(const float delta) {
    // return;
    for (int v = 0; v < 6; ++v) {
        gVertices[v * 5] += gDir[0] * speed * delta;
        gVertices[v * 5 + 1] += gDir[1] * speed * delta;
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, 30 * sizeof(float), gVertices);

    if (gVertices[0] <= -1) {
        gDir[0] = 0.7;
    } else if (gVertices[5] >= 1) {
        gDir[0] = -1;
    }

    if (gVertices[1] >= 1) {
        gDir[1] = -0.9;
    } else if (gVertices[11] <= -1) {
        gDir[1] = 1.2;
    }
}

void draw() {
    const GLint loc = glGetUniformLocation(gShaderProgram, "time");
    glUniform1i(loc, glutGet(GLUT_ELAPSED_TIME));

    // clear
    glClear(GL_COLOR_BUFFER_BIT);

    // draw
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glutSwapBuffers();
    glutPostRedisplay();
}

int frames = 0;
int lastFPSShowTime = 0;
int lastFrameTime = 0;
int lastFrameStart = 0;

void frame() {
    const int startFrameTime = glutGet(GLUT_ELAPSED_TIME);
    const float delta = static_cast<float>(startFrameTime - lastFrameStart) / 1000.0f;
    lastFrameStart = startFrameTime;

    update(delta);
    draw();

    const int now = glutGet(GLUT_ELAPSED_TIME);
    if (now - lastFPSShowTime >= 1000) {
        std::cout << "FPS: " << frames << std::endl;
        lastFPSShowTime = now;
        frames = 0;
    }

    lastFrameTime = now - startFrameTime;
    if (lastFrameTime < targetFrameTime) {
        std::this_thread::sleep_for(std::chrono::milliseconds(targetFrameTime - lastFrameTime));
    }

    frames++;
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("close whisper");

    const GLenum status = glewInit();
    if (status != GLEW_OK) {
        std::cerr << "Unable to initialize GLEW: error " << status << std::endl;
        return 1;
    }

    initGL();

    glutDisplayFunc(frame);
    glutMainLoop();

    return 0;
}
