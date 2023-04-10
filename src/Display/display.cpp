#include <Display/display.h>
#include <logo.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Hop
{

    void defaultKeyEventCallback
    (
        GLFWwindow * window,
        int key,
        int scancode,
        int action,
        int mods
    )
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    Display::Display
    (
        unsigned x, 
        unsigned y, 
        const char * title,
        GLFWkeyfun keyCallback
    )
    : x(x), y(y), title(title)
    {
        if ( !glfwInit() ) { exit(EXIT_FAILURE); }
        glfwWindow = NULL;
        open();
        if ( !isOpen() ) { exit(EXIT_FAILURE); }
        setAsFocus();
        glfwSetKeyCallback(glfwWindow, keyCallback);
        
        logo = new GLFWimage;

        logo->pixels = stbi_load_from_memory
        (
            &LOGO[0], 
            sizeof(LOGO), 
            &logo->width, 
            &logo->height, 
            0, 
            4
        );

        glfwSetWindowIcon(glfwWindow,1,logo);
    }

    Display::Display
    (
        unsigned x, 
        unsigned y, 
        const char * title
    )
    : Display(x,y,title,defaultKeyEventCallback)
    {}
}