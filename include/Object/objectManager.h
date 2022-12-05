#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <World/world.h>
#include <Object/object.h>
#include <Object/id.h>
#include <Object/objectRenderer.h>
#include <Object/collisionDetector.h>
#include <Object/collisionResolver.h>

#include <unordered_map>
#include <string>

/*
    Stores an unordered map of objects that can be added to
     and removed from

    Objects are associated with a string identity, which can be
     the string form of Object/id.h or a user provided handle
     subject to a uniqueness check

    Object dynamics is step by step(delta), and drawing
     is dispatched with draw(debug)

    Callback is calle on collisions, can be user specified with
     user logic, e.g:
     "if collision between player and power up call player.collectPowerUp() and powerUp.delete()"
*/

// define CollisionCallback as this func ptr
typedef void (*CollisionCallback)(std::string,std::string);
void identityCallback(std::string a, std::string b);

class ObjectManager {
public:

    ObjectManager(
        void (*callback)(std::string,std::string) = &identityCallback
    )
    : collisionCallback(callback)
    {}

    void add(std::shared_ptr<Object> o);
    void add(std::shared_ptr<Object> o, std::string handle);

    void remove(Id id);
    void remove(std::string handle);

    void step(double delta, World * world);
    void draw(bool debug);

    std::unordered_map<std::string,std::shared_ptr<Object>> * getObjects(){return &objects;}
    std::shared_ptr<Object> getObject(std::string name);

    CollisionCallback collisionCallback;

private:

    friend class ObjectRenderer;

    std::unordered_map<std::string,std::shared_ptr<Object>> objects;

    ObjectRenderer * renderer;
    CollisionDetector * detector;
    CollisionResolver * resolver;
};

#endif /* OBJECTMANAGER_H */
