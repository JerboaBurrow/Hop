#ifndef OBJECT_H
#define OBJECT_H

#include <Object/id.h>
#include <Object/physicsState.h>

struct Rectangle{
    Rectangle(
        float x, float y, float w, float h
    )
    :x(x),y(y),w(w),h(h) {}

    const float x;
    const float y;
    const float w;
    const float h;
};

class Object {
public:
    Object()
    {
        
    }
    Id id;

private:

    Rectangle boundingRectangle;
    PhysicsState state;

};

#endif /* OBJECT_H */
