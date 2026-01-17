#ifdef __unix__
#include <GLFW/glfw3.h>

void init_platform() {
    glfwInit();
}

void terminate_platform() {
    glfwTerminate();
}
#endif