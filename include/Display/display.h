#ifndef DISPLAY_H
#define DISPLAY_H

#include <gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


namespace Hop
{

    void defaultKeyEventCallback
    (
        GLFWwindow * window,
        int key,
        int scancode,
        int action,
        int mods
    );

    class Display
    {
        public:

            Display
            (
                unsigned x, 
                unsigned y, 
                const char * title,
                GLFWkeyfun keyCallback
            );

            Display
            (
                unsigned x, 
                unsigned y, 
                const char * title
            );

            ~Display(){ glfwTerminate(); free(logo); }

            bool isOpen(){ if (glfwWindow != NULL) { return !glfwWindow ? false : true; } return false; }

            void open(){ 
                if (glfwWindow == NULL)
                { 
                    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                    glfwWindow = glfwCreateWindow(x,y,title,NULL,NULL); glfwSwapInterval(1); 
                } 
            }

            void close(){ if (glfwWindow != NULL) { glfwDestroyWindow(glfwWindow); glfwWindow = NULL; } }

            void setAsFocus(){ if (glfwWindow != NULL) { glfwMakeContextCurrent(glfwWindow); } }

            void keyCallback
            (
                GLFWwindow * window,
                int key,
                int scancode,
                int action,
                int mods
            );

            void mousePosition(double & x, double & y){ if (glfwWindow != NULL){ glfwGetCursorPos(glfwWindow,&x,&y); } }

            void loop() { swap(); handleEvents(); if (glfwWindowShouldClose(glfwWindow)){ close(); } }

        private:

        unsigned x, y;
        const char * title;

        GLFWimage * logo;

        GLFWwindow * glfwWindow;

        void swap(){ if (glfwWindow != NULL) { glfwSwapBuffers(glfwWindow); } }

        void handleEvents(){ if (glfwWindow != NULL){ glfwPollEvents(); } }

    };
}

#endif /* DISPLAY_H */