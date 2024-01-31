#ifndef ENTITYCOMPONENTSYSTEM_H
#define ENTITYCOMPONENTSYSTEM_H

#include <World/world.h>
#include <Object/object.h>

#include <System/systemManager.h>

#include <Component/cTransform.h>
#include <Component/cRenderable.h>
#include <Component/cPhysics.h>
#include <Component/cCollideable.h>
#include <Component/cPolygon.h>
#include <Component/cSound.h>

#include <System/Rendering/sRender.h>
#include <System/Physics/sPhysics.h>
#include <System/Physics/sCollision.h>
#include <System/Sound/sSound.h>

#include <unordered_map>
#include <map>
#include <string>
#include <iterator>

#include <typeinfo>
#include <exception>

#include <Component/componentArray.h>

#include <Console/lua.h>

#include <chrono>
using namespace std::chrono;

namespace Hop 
{
    class Console;
}

namespace Hop::System::Rendering
{
    class sRender;
}

namespace Hop::System::Sound
{
    class sSound;
}

namespace Hop::System::Physics
{
    class sPhysics;
    class sCollision;
}

namespace Hop::Object
{

    using namespace Hop::Object::Component;
    
    using namespace Hop::System;
    using Hop::System::Physics::sPhysics;
    using Hop::System::Physics::sCollision;
    using Hop::System::Rendering::sRender;
    using Hop::System::Sound::sSound;
    using Hop::Console;


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

    class EntityComponentSystem 
    {

    public:

        EntityComponentSystem(
            void (*callback)(Id & i, Id & j) = &identityCallback
        )
        : collisionCallback(callback), 
        nextComponentIndex(0)
        {
            initialiseBaseECS();
            objects.clear();
        }

        Id createObject();
        Id createObject(std::string handle);

        void remove(Id id);
        void remove(std::string handle);

        bool handleExists(std::string handle) const { return handleToId.find(handle) != handleToId.cend(); }

        Id & idFromHandle(std::string handle)
        {
            return handleToId[handle];
        }

        const std::unordered_map<Id,std::shared_ptr<Object>> & getObjects() { return objects; }

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

            getComponentArray<T>().insert(i,component);
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

            getComponentArray<T>().remove(i);
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

            // if (!componentRegistered(handle)){
            //     throw ComponentNotRegistered(" Attempt to getComponent<"+i.idStr+")");
            // }
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
            return registeredComponents[handle];
        }

        // system interface
        template<class T>
        void registerSystem(){systemManager.registerSystem<T>();}

        template<class T>
        void setSystemSignature(Signature signature){systemManager.setSignature<T>(signature);}

        template <class T>
        T & getSystem(){return systemManager.getSystem<T>();}

        template<class T>
        bool hasComponent(const Id & i){return idToSignature[i][getComponentId<T>()];}

        template <class T>
        ComponentArray<T> getComponentArrayCopy()
        {
            const char * handle = typeid(T).name();

            return *(std::static_pointer_cast<ComponentArray<T>>(componentData[handle]));

        }

        template <class T>
        ComponentArray<T> & getComponentArray()
        {
            const char * handle = typeid(T).name();

            return *(std::static_pointer_cast<ComponentArray<T>>(componentData[handle]));

        }

        template <class T>
        void updateMainComponents();

        // Lua bindings

        int lua_loadObject(lua_State * lua);
        int lua_deleteObject(lua_State * lua);
        
        int lua_getTransform(lua_State * lua);
        int lua_setTransform(lua_State * lua);

        int lua_removeFromMeshByTag(lua_State * lua);
        int lua_meshBoundingBox(lua_State * lua);
        int lua_meshBoundingBoxByTag(lua_State * lua);

        int lua_getColour(lua_State * lua);
        int lua_setColour(lua_State * lua);


    private:

        std::unordered_map<std::string,Id> handleToId;
        std::unordered_map<Id,Signature> idToSignature;
        std::unordered_map<Id,std::shared_ptr<Object>> objects;

        SystemManager systemManager;

        void initialiseBaseECS();

        // components

        bool componentRegistered(const char * h){return registeredComponents.find(h)!=registeredComponents.end();}

        uint32_t nextComponentIndex;

        std::unordered_map<const char *, uint32_t> registeredComponents;

        std::unordered_map<const char *, std::shared_ptr<AbstractComponentArray>> componentData;

    };
}


#endif /* ENTITYCOMPONENTSYSTEM_H */
