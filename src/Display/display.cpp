#include <Display/display.h>
#include <logo.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Hop
{

    Display::EventType glfwCodeToEvent(int action)
    {
        switch (action)
        {
            case GLFW_PRESS:
                return Display::EventType::PRESS;

            case GLFW_RELEASE:
                return Display::EventType::RELEASE;

            case GLFW_REPEAT:
                return Display::EventType::HOLD;

            default:
                return Display::EventType::NONE;
        }
    }

    void parseAction
    (
        GLFWwindow * window,
        int code,
        int action
    )
    {
        auto data = reinterpret_cast<Display::WindowData*>(glfwGetWindowUserPointer(window));

        double x, y;
        glfwGetCursorPos(window,&x,&y);

        Display::Event e(x, y, glfwCodeToEvent(action));

        if (data->events.find(code) == data->events.cend())
        {
            (*data).events[code] = {e};
        }
        else
        {
            (*data).events[code].push_back(e);
        }
    }

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

        parseAction(window, key, action);
        
    }

    void defaultMouseButtonCallback
    (
        GLFWwindow * window,
        int button,
        int action,
        int mods
    )
    {
        parseAction(window, button, action);
    }

    void defaultScrollCallback
    (
        GLFWwindow * window,
        double x,
        double y
    )
    {
        auto e = reinterpret_cast<Display::WindowData*>(glfwGetWindowUserPointer(window));
        e->scrolled = true;
        e->scrollX = x;
        e->scrollY = y;
    }

    Display::Display
    (
        unsigned x, 
        unsigned y, 
        const char * title,
        GLFWkeyfun keyCallback,
        GLFWmousebuttonfun mouseButtonCallback,
        GLFWscrollfun mouseScrollCallback
    )
    : x(x), y(y), title(title)
    {
        if ( !glfwInit() ) { exit(EXIT_FAILURE); }
        glfwWindow = NULL;
        open();
        if ( !isOpen() ) { exit(EXIT_FAILURE); }
        setAsFocus();
        glfwSetKeyCallback(glfwWindow, keyCallback);
        glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
        glfwSetScrollCallback(glfwWindow, mouseScrollCallback);
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

        data.clear();

        glfwSetWindowUserPointer(glfwWindow, &this->data);
    }

    Display::Display
    (
        unsigned x, 
        unsigned y, 
        const char * title
    )
    : Display(x,y,title,defaultKeyEventCallback, defaultMouseButtonCallback, defaultScrollCallback)
    {}
}