#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <World/world.h>
#include <Object/object.h>

#include <Component/componentManager.h>
#include <System/systemManager.h>

#include <Component/cTransform.h>
#include <Component/cRenderable.h>
#include <Component/cPhysics.h>
#include <Component/cCollideable.h>

#include <System/sRender.h>
#include <System/sPhysics.h>

#include <unordered_map>
#include <string>

#include <log.h>

class ObjectManager;
class sRender;
class sPhysics;


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

const uint32_t MAX_OBJECTS = 1000000;

class ObjectManager {
public:

    ObjectManager(
        void (*callback)(std::string,std::string) = &identityCallback
    )
    : collisionCallback(callback)
    {
        initialiseBaseECS();
    }

    Id createObject();
    Id createObject(std::string handle);

    void remove(Id id);
    void remove(std::string handle);

    std::shared_ptr<Object> getObject(Id id);
    std::shared_ptr<Object> getObject(std::string name);

    CollisionCallback collisionCallback;

    // component interface
    template<class T>
    void registerComponent(){
        componentManager.registerComponent<T>(MAX_OBJECTS);
    }

    template <class T>
    void addComponent(Id i, T component){
        componentManager.addComponent<T>(i,component);
        idToSignature[i].set(
            componentManager.getComponentId<T>(),
            true
        );
        systemManager.objectSignatureChanged(i,idToSignature[i]);
    }

    template <class T>
    void removeComponent(Id i){
        componentManager.removeComponent<T>(i);
        idToSignature[i].set(
            componentManager.getComponentId<T>(),
            false
        );
        systemManager.objectSignatureChanged(i,idToSignature[i]);
    }

    template <class T>
    T & getComponent(Id i){
        return componentManager.getComponent<T>(i);
    }

    // system interface
    template<class T>
	void registerSystem(){systemManager.registerSystem<T>();}

    template<class T>
	void setSystemSignature(Signature signature){systemManager.setSignature<T>(signature);}

    Id idFromHandle(std::string handle){return handleToId[handle];}

    template <class T>
    T & getSystem(){return systemManager.getSystem<T>();}

    void postToLog(LogType msg){
        msg >> log; 
    }

    Log & getLog(){return log;}

private:

    std::unordered_map<std::string,Id> handleToId;
    std::unordered_map<Id,Signature> idToSignature;
    std::unordered_map<Id,std::shared_ptr<Object>> objects;

    SystemManager systemManager;
    ComponentManager componentManager;

    void initialiseBaseECS();

    Log log;
};


#endif /* OBJECTMANAGER_H */
