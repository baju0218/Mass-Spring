#include <windows.h>
#include <GLFW/glfw3.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <fstream>
#include <string>
#include "massSpring.h"

MassSpring object = MassSpring();



/***************************************************************/
/************************* RENDER PART *************************/
/***************************************************************/

double azimuth = atan(1);
double elevation = asin(0.5);
double distance = 500;
Vector3d up = { 0, cos(elevation), 0 };
Vector3d target = { 0, 0, 0 };
Vector3d eye = { target[0] + distance * cos(elevation) * sin(azimuth), target[1] + distance * sin(elevation), target[2] + distance * cos(elevation) * cos(azimuth) };

void drawGrid() {
    static double size = 100;

    /* Draw grid */
    glBegin(GL_LINES);
    glColor3ub(255, 0, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(size, 0, 0);
    glColor3ub(0, 255, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, size, 0);
    glColor3ub(0, 0, 255);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, size);
    glColor3ub(0, 0, 0);
    glVertex3d(size, 0, 0);
    glVertex3d(size, size, 0);
    glVertex3d(size, size, 0);
    glVertex3d(0, size, 0);
    glVertex3d(0, size, 0);
    glVertex3d(0, size, size);
    glVertex3d(0, size, size);
    glVertex3d(0, 0, size);
    glVertex3d(0, 0, size);
    glVertex3d(size, 0, size);
    glVertex3d(size, 0, size);
    glVertex3d(size, 0, 0);
    glEnd();
}

void drawObject() {
    double* vtx = new double[3 * object.np];
    int* el = new int[2 * object.ns];
    object.particleGetVertex(vtx);
    object.particleGetElement(el);

    /* Draw object */
    glColor3ub(178, 204, 255);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_DOUBLE, 3 * sizeof(double), vtx);
    glDrawElements(GL_LINES, 2 * object.ns, GL_UNSIGNED_INT, el);
}

void render(GLFWwindow* window) {
    /* Initialize the buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glClearColor(1, 1, 1, 1);

    /* Set projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, 1, 1, 1000);

    /* Set modelview */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], target[0], target[1], target[2], up[0], up[1], up[2]);

    /* Draw grid and object */
    drawGrid();
    drawObject();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);
}



/**************************************************************/
/************************* INPUT PART *************************/
/**************************************************************/

double XPOS = 0;
double YPOS = 0;
int BUTTON = 0;
int MODE = 1;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        MODE = 1;
    else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        MODE = 2;
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    if (MODE == 1) {
        if (BUTTON == -1) {
            double eyeSpeed = 0.009825;

            azimuth -= eyeSpeed * (xpos - XPOS);
            elevation += eyeSpeed * (ypos - YPOS);

            up = { 0, cos(elevation), 0 };
            eye = { target[0] + distance * cos(elevation) * sin(azimuth), target[1] + distance * sin(elevation), target[2] + distance * cos(elevation) * cos(azimuth) };
        }
        else if (BUTTON == 1) {
            double targetSpeed = distance * 0.000835;

            Vector3d w = (eye - target).normalized();
            Vector3d u = up.cross(w).normalized();
            Vector3d v = w.cross(u);

            target -= targetSpeed * (xpos - XPOS) * u;
            target += targetSpeed * (ypos - YPOS) * v;
            eye = { target[0] + distance * cos(elevation) * sin(azimuth), target[1] + distance * sin(elevation), target[2] + distance * cos(elevation) * cos(azimuth) };
        }
    }
    else if (MODE == 2) {

    }

    XPOS = xpos;
    YPOS = ypos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        BUTTON -= 1;
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        BUTTON += 1;
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        BUTTON += 1;
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        BUTTON -= 1;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    double distanceSpeed = distance * 0.1;

    distance -= distanceSpeed * yoffset;

    eye = { target[0] + distance * cos(elevation) * sin(azimuth), target[1] + distance * sin(elevation), target[2] + distance * cos(elevation) * cos(azimuth) };
}

void dropCallback(GLFWwindow* window, int count, const char** paths) {
    object = MassSpring();

    std::ifstream fin;
    fin.open(paths[0]);

    while (!fin.eof()) {
        std::string str;
        getline(fin, str);

        if (!str.compare("*PARTICLES")) {
            fin >> object.np;
            object.p = new Particle[object.np];

            for (int i = 0; i < object.np; i++) {
                int index;
                fin >> index
                    >> object.p[i].x[0] >> object.p[i].x[1] >> object.p[i].x[2] >> object.p[i].m;

                object.p[i].v = { 0, 0, 0 };
                object.p[i].f = { 0, 0, 0 };
            }
        }
        else if (!str.compare("*SPRINGS")) {
            fin >> object.ns;
            object.s = new Spring[object.ns];

            for (int i = 0; i < object.ns; i++) {
                int index;
                fin >> index
                    >> object.s[i].a >> object.s[i].b >> object.s[i].s >> object.s[i].d;

                object.s[i].r = (object.p[object.s[i].a].x - object.p[object.s[i].b].x).norm();
            }
        }
        else if (!str.compare("*CONTROLLED PARTICLES")) {
            fin >> object.ncp;
            object.cp = new ControlledParticle[object.ncp];

            for (int i = 0; i < object.ncp; i++) {
                int index;
                fin >> index
                    >> object.cp[i].i >> object.cp[i].m[0] >> object.cp[i].m[1] >> object.cp[i].m[2];
            }
        }
        else if (!str.compare("*CONTROLLED SPRINGS")) {
            fin >> object.ncs;
            object.cs = new ControlledSpring[object.ncs];
        }
    }

    fin.close();
}



/*************************************************************/
/************************* MAIN PART *************************/
/*************************************************************/

int main() {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 640, "Mass Spring", NULL, NULL);
    if (!window) {
        glfwTerminate();

        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Set the window's input callback */
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetDropCallback(window, dropCallback);

    /* Loop until the user closes the window */
    double previousTime = glfwGetTime();
    int previousFrame = previousTime * 60;
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        int currentFrame = currentTime * 60;

        /* Simulate here */
        object.eulerStep(currentTime - previousTime);
        previousTime = currentTime;

        /* Render here */
        if (currentFrame != previousFrame) {
            render(window);
            previousFrame = currentFrame;
        }

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}