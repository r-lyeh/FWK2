#include "glfm.h"

#if defined __linux__ || defined _WIN32 || (defined __APPLE__ && defined TARGET_OS_OSX)

#include <stdlib.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "glfm_internal.h"

#define GL_LITE_IMPLEMENTATION
#include "gllite.h"

//#include "glfw.h"

#define TIGR_IMPLEMENTATION
#include "../../tigr/3rd_tigr.h"

#define GLFWwindow Tigr
#define glfwInit() (1)
#define glfwTerminate() (0)
#define glfwCreateWindow(width, height, title, null1, null2) tigrWindow(width, height, title, 0)
#define glfwWindowShouldClose(window) ( tigrClosed(window) ? 1 : (tigrUpdate(window), tigrBeginOpenGL(window), 0) )
#define glfwSwapBuffers(window)
#define glfwPollEvents()
#define glfwMakeContextCurrent(window) gl_lite_init()
#define glfwSetWindowUserPointer(window, display)
#define glfwGetWindowUserPointer(window) ((window)->handle)
#define glfwSetFramebufferSizeCallback(window, on_size_change)
#define glfwGetFramebufferSize(window, width, height) (*(width) = (window)->w, *(height) = (window)->h)

typedef struct {
    GLFWwindow *window;
    int width;
    int height;
    double scale;

    GLFMDisplay *display;
    GLFMRenderingAPI renderingAPI;
} GLFMPlatformData;

static GLFMPlatformData* _platform_data = 0;

static void glfm__displayChromeUpdated(GLFMDisplay *display) {}
static void glfm__sensorFuncUpdated(GLFMDisplay *display) {}


//createRendererGLES2
void glfmGetDisplayChromeInsets(const GLFMDisplay *display, double *top, double *right,
                                double *bottom, double *left) {
    *top = *right = *bottom = *left = 0;
}
GLFMInterfaceOrientation glfmGetInterfaceOrientation(const GLFMDisplay *display) { return GLFMInterfaceOrientationLandscapeLeft; }
void glfmSetSupportedInterfaceOrientation(GLFMDisplay *display, GLFMInterfaceOrientation supportedOrientations) {}
bool glfmIsMetalSupported(const GLFMDisplay *display) { return false; }
bool glfmIsSensorAvailable(const GLFMDisplay *display, GLFMSensor sensor) { return false; }
void glfmPerformHapticFeedback(GLFMDisplay *display, GLFMHapticFeedbackStyle style) {}

double glfmGetTime(void) { return tigrTime(); }

void glfmSwapBuffers(GLFMDisplay *display) {
    glfwSwapBuffers((GLFWwindow*)display->userData);
    glfwPollEvents();
}

static void on_size_change(GLFWwindow* window, int width, int height) {
    GLFMDisplay* display = (GLFMDisplay*)glfwGetWindowUserPointer(window);
    if (display->surfaceResizedFunc) {
        display->surfaceResizedFunc(display, width, height);
    }
}

void glfmGetDisplaySize(const GLFMDisplay *display, int *width, int *height) {
    glfwGetFramebufferSize((GLFWwindow*)display->userData, width, height);
}

GLFMRenderingAPI glfmGetRenderingAPI(const GLFMDisplay *display) {
    return GLFMRenderingAPIOpenGLES2;
}

int main(int argc, char** argv) {
    if (!glfwInit())
        exit(EXIT_FAILURE);

    if (!_platform_data) {
        _platform_data = calloc(1, sizeof(GLFMPlatformData));
    }
    _platform_data->window = glfwCreateWindow(375, 667, "GLFM Simulator", NULL, NULL);
    if (!_platform_data->window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    if (_platform_data->display == NULL) {
        _platform_data->display = calloc(1, sizeof(GLFMDisplay));
        _platform_data->display->platformData = _platform_data;
        glfmMain(_platform_data->display);
    }
    _platform_data->display->userData = _platform_data->window;

    glfwSetWindowUserPointer(_platform_data->window, _platform_data->display);
    glfwSetFramebufferSizeCallback(_platform_data->window, on_size_change);
    int fwidth, fheight;
    glfwGetFramebufferSize(_platform_data->window, &fwidth, &fheight);
    _platform_data->scale = (double)fwidth / 375;
    if (_platform_data->display->surfaceCreatedFunc) {
        _platform_data->display->surfaceCreatedFunc(_platform_data->display, fwidth, fheight);
    }

    glfwMakeContextCurrent(_platform_data->window);
    while (glfwWindowShouldClose(_platform_data->window) == 0) {
        if (_platform_data->display->renderFunc) {
            _platform_data->display->renderFunc(_platform_data->display);
        }
    }
}
#endif
