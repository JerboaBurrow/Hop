#ifndef DISPLAY_H
#define DISPLAY_H

#include <gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>
#include <map>


namespace Hop
{

    class Display
    {
    
    public:

        Display
        (
            unsigned x, 
            unsigned y, 
            const char * title,
            GLFWkeyfun keyCallback,
            GLFWmousebuttonfun mouseButtonCallback,
            GLFWscrollfun mouseScrollCallback
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

        int getKeyLastState(int key) { return glfwGetKey(glfwWindow, key); }

        void loop() 
        { 
            data.clear();
            swap(); 
            handleEvents(); 
            if (glfwWindowShouldClose(glfwWindow)){ close(); }
        }

        enum class EventType {NONE, PRESS, RELEASE, HOLD};

        struct Event
        {
            Event(double x, double y, EventType t)
            : mouseX(x), mouseY(y), type(t)
            {}

            Event()
            : mouseX(0.0), mouseY(0.0), type(Display::EventType::NONE)
            {}

            double mouseX;
            double mouseY;
            EventType type;
        };

        std::vector<Event> getEvents(int code) 
        {
            if (data.events.find(code) == data.events.cend())
            {
                return {Event()};
            }
            else
            {
                return data.events[code];
            }
        }

        Event getEvent(int code)
        {
            if (data.events.find(code) == data.events.cend())
            {
                return Event();
            }
            else
            {
                return data.events[code][0];
            }
        }

        struct WindowData 
        {
            std::map<int, std::vector<Event>> events;
            double scrollX = 0.0;
            double scrollY = 0.0;
            bool scrolled = false;

            void clear()
            {
                scrollX = 0.0;
                scrollY = 0.0;
                scrolled = false;
                events.clear();
            }
        };

    private:

        unsigned x, y;
        const char * title;

        GLFWimage * logo;

        GLFWwindow * glfwWindow;

        WindowData data;

        void swap(){ if (glfwWindow != NULL) { glfwSwapBuffers(glfwWindow); } }

        void handleEvents(){ if (glfwWindow != NULL){ glfwPollEvents(); } }

    };
}

#endif /* DISPLAY_H */