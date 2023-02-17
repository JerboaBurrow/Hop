#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <World/world.h>
#include <Object/object.h>

#include <System/systemManager.h>

#include <Component/cTransform.h>
#include <Component/cRenderable.h>
#include <Component/cPhysics.h>
#include <Component/cCollideable.h>

#include <System/sRender.h>
#include <System/sPhysics.h>
#include <System/sCollision.h>

#include <unordered_map>
#include <string>

#include <typeinfo>
#include <exception>

#include <Component/componentArray.h>

#include <Thread/threadPool.h>

#include <log.h>

class ComponentNotRegistered: public std::exception 
{

public:

    ComponentNotRegistered(std::string msg)
    : msg(msg)
    {}

private:

    virtual const char * what() const throw()
    {
        return msg.c_str();
    }
    std::string msg;
};

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
typedef void (*CollisionCallback)(Id & i, Id & j);
void identityCallback(Id & i, Id & j);

const uint32_t MAX_OBJECTS = 100000;

class ObjectManager 
{

public:

    ObjectManager(
        size_t threads,
        void (*callback)(Id & i, Id & j) = &identityCallback
    )
    : collisionCallback(callback), 
    nextComponentIndex(0), 
    workers(ThreadPool(threads)),
    systemManager(SystemManager(threads)),
    threads(threads)
    {
        initialiseBaseECS();
        INFO("using "+std::to_string(threads)+" threads") >> log;
    }

    Id createObject();
    Id createObject(std::string handle);

    void remove(Id id);
    void remove(std::string handle);

    Id & idFromHandle(std::string handle)
    {
        if (handleToId.find(handle) == handleToId.end())
        {
            WARN("Tried to access id with non-existent handle "+handle)>>log;
        }
        return handleToId[handle];
    }


    CollisionCallback collisionCallback;

    // component interface

    template <class T>
    void registerComponent()
    {
        const char * handle = typeid(T).name();

        if (componentRegistered(handle))
        {
            return;
        }

        if (nextComponentIndex >= MAX_COMPONENTS)
        {
            return;
        }

        registeredComponents[handle] = nextComponentIndex;
        nextComponentIndex++;
        componentData[handle] = std::make_shared<ComponentArray<T>>(MAX_OBJECTS);
    }

    template <class T>
    void addComponent(Id i, T component)
    {
        const char * handle = typeid(T).name();

        if (!componentRegistered(handle))
        {
            return;
        }

        getComponentArray<T>()->insert(i,component);
        idToSignature[i].set(
            getComponentId<T>(),
            true
        );
        systemManager.objectSignatureChanged(i,idToSignature[i]);
    }

    template <class T>
    void removeComponent(Id i)
    {
        const char * handle = typeid(T).name();

        if (!componentRegistered(handle))
        {
            return;
        }

        getComponentArray<T>()->remove(i);
        idToSignature[i].set(
            getComponentId<T>(),
            false
        );
        systemManager.objectSignatureChanged(i,idToSignature[i]);
    }

    template <class T>
    inline T & getComponent(const Id & i)
    {
        const char * handle = typeid(T).name();

        if (!componentRegistered(handle)){
            throw ComponentNotRegistered(" Attempt to getComponent<"+i.idStr+")");
        }
        const char * handle = typeid(T).name();
        return (std::static_pointer_cast<ComponentArray<T>>(componentData[handle]))->get(i);
    }

    void objectFreed(Id i)
    {
        for (auto const& pair : componentData)
        {
            pair.second.get()->objectFreed(i);
        }
    }

    template <class T>
    uint32_t getComponentId()
    {
        const char * handle = typeid(T).name();
        if (!componentRegistered(handle))
        {
            throw ComponentNotRegistered(" Attempt to getComponent<"+std::string(handle)+">()");
        }
        return registeredComponents[handle];
    }

    // system interface
    template<class T>
	void registerSystem(){systemManager.registerSystem<T>();}

    template<class T>
	void setSystemSignature(Signature signature){systemManager.setSignature<T>(signature);}

    template <class T>
    T & getSystem(){return systemManager.getSystem<T>();}

    Log & getLog(){return log;}

    void postJob(
        const std::function<void(void)> & job
    )
    {
        workers.queueJob(job);
    }

    void waitForJobs()
    {
        workers.wait();
    }

    bool isThreaded()
    {
        return workers.size()>0;
    }

    void releaseThread()
    {
        INFO("releaseing a thread (nThreads): "+std::to_string(workers.size()))>>log;
        workers.joinThread();
        INFO("join called (nThreads): "+std::to_string(workers.size()))>>log;
    }

    void addThread()
    {
        INFO("adding a thread (nThreads): "+std::to_string(workers.size()))>>log;
        workers.createThread();
        INFO("create called (nThreads): "+std::to_string(workers.size()))>>log;
    }

    template<class T>
    bool hasComponent(const Id & i){return idToSignature[i][getComponentId<T>()];}

private:

    std::unordered_map<std::string,Id> handleToId;
    std::unordered_map<Id,Signature> idToSignature;
    std::unordered_map<Id,std::shared_ptr<Object>> objects;

    SystemManager systemManager;

    ThreadPool workers;
    const size_t threads;

    void initialiseBaseECS();

    Log log;

    // components

    bool componentRegistered(const char * h){return registeredComponents.find(h)!=registeredComponents.end();}

    uint32_t nextComponentIndex;
    std::unordered_map<const char *,uint32_t> registeredComponents;
    std::unordered_map<const char*, std::shared_ptr<AbstractComponentArray>> componentData;


    template<typename T>
	std::shared_ptr<ComponentArray<T>> getComponentArray()
	{
        const char * handle = typeid(T).name();
		return std::static_pointer_cast<ComponentArray<T>>(componentData[handle]);
	}
};


#endif /* OBJECTMANAGER_H */
