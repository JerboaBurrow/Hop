#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <World/world.h>
#include <Object/object.h>
#include <Object/collisionDetector.h>
#include <Object/collisionResolver.h>

#include <Component/componentManager.h>
#include <System/systemManager.h>

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

typedef std::unordered_map<
            Id,
            std::pair<
                std::shared_ptr<Object>,
                Signature
            >
        > ObjectMap;

class ObjectManager {
public:

    ObjectManager(
        CollisionDetector * d,
        CollisionResolver * res,
        void (*callback)(std::string,std::string) = &identityCallback
    )
    : collisionCallback(callback), detector(d), resolver(res)
    {}

    void add(std::shared_ptr<Object> o);
    void add(std::shared_ptr<Object> o, std::string handle);

    void remove(Id id);
    void remove(std::string handle);

    ObjectMap * getObjects(){return &objects;}
    std::shared_ptr<Object> getObject(Id id);
    std::shared_ptr<Object> getObject(std::string name);

    CollisionCallback collisionCallback;

    // component interface
    template<class T>
    void registerComponent(){
        componentManager->registerComponent<T>();
    }

    template <class T>
    void addComponent(Id i, T component);

    template <class T>
    void removeComponent(Id i);

    // system interface
    template<class T>
	std::shared_ptr<T> registerSystem(){systemManager->registerSystem<T>();}

    template<class T>
	void setSystemSignature(Signature signature){systemManager->setSignature<T>(signature);}

private:

    ObjectMap objects;
    std::unordered_map<std::string,Id> handleToId;

    CollisionDetector * detector;
    CollisionResolver * resolver;

    std::unique_ptr<SystemManager> systemManager;
    std::unique_ptr<ComponentManager> componentManager;
};

#endif /* OBJECTMANAGER_H */
