#ifndef ENTITYCOMPONENTSYSTEM_H
#define ENTITYCOMPONENTSYSTEM_H

#include <constants.h>

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

    /**
     * @brief Stores objects (identity) dynamic components and systems.
     * @remark An object is associated with a unique Id.
     * @remark Components can be dynamically added, removed, and modified.
     */
    class EntityComponentSystem
    {

    public:

        /**
         * @brief Construct a new EntityComponentSystem
         *
         * @remark Constructs an default ECS with all Hop components and systems.
         */
        EntityComponentSystem()
        : nextComponentIndex(0)
        {
            initialiseBaseECS();
            objects.clear();
        }

        Id createObject();

        /**
         * @brief Create a Object returning its id and associating a user defined string name.
         *
         * @param handle user defined alias.
         * @return Id the objects unique id.
         */
        Id createObject(std::string handle);

        void remove(Id id);

        /**
         * @brief Remove an object by the users alias.
         *
         * @param handle user defined alias.
         */
        void remove(std::string handle);

        bool handleExists(std::string handle) const { return handleToId.find(handle) != handleToId.cend(); }

        Id & idFromHandle(std::string handle)
        {
            return handleToId[handle];
        }

        const std::unordered_map<Id,std::shared_ptr<Object>> & getObjects() { return objects; }

        // component interface

        /**
         * @brief Register a new component.
         *
         * @tparam T the component class to register.
         */
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

        /**
         * @brief Add a component to an object.
         *
         * @tparam T the registered component.
         * @param i the object Id.
         * @param component the component value.
         */
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

        /**
         * @brief Remove a component from an object.
         *
         * @tparam T the registered component class.
         * @param i the object Id to remove from.
         */
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

        /**
         * @brief Get a Component for an object.
         *
         * @tparam T the component class.
         * @param i the object Id to get the component T from.
         * @return T& the component values for the object Id;
         */
        template <class T>
        inline T & getComponent(const Id & i)
        {
            const char * handle = typeid(T).name();
            return (std::static_pointer_cast<ComponentArray<T>>(componentData[handle]))->get(i);
        }

        // system interface

        /**
         * @brief Register a class T as a new system.
         *
         * @tparam T the class to register.
         */
        template<class T>
        void registerSystem(){systemManager.registerSystem<T>();}

        /**
         * @brief Set the Signature for the system T
         *
         * @tparam T the registered system class.
         * @param signature the new Signature.
         */
        template<class T>
        void setSystemSignature(Signature signature){systemManager.setSignature<T>(signature);}

        /**
         * @brief Get the System T
         *
         * @tparam T the registered system T to get..
         * @return T& the system.
         */
        template <class T>
        T & getSystem(){return systemManager.getSystem<T>();}

        /**
         * @brief Check if the object has the given component.
         *
         * @tparam T the registered component class.
         * @param i the object Id to check.
         * @return true i has the component.
         * @return false i does not have the component.
         */
        template<class T>
        bool hasComponent(const Id & i){return idToSignature[i][getComponentId<T>()];}

        /**
         * @brief Get the full array of component T, as a copy;
         *
         * @tparam T the registered component
         * @return ComponentArray<T> all values of component T.
         */
        template <class T>
        ComponentArray<T> getComponentArrayCopy()
        {
            const char * handle = typeid(T).name();
            return *(std::static_pointer_cast<ComponentArray<T>>(componentData[handle]));
        }

        /**
         * @brief Get the full array of component T, as a copy;
         *
         * @tparam T the registered component
         * @return ComponentArray<T> & all values of component T.
         */
        template <class T>
        ComponentArray<T> & getComponentArray()
        {
            const char * handle = typeid(T).name();

            return *(std::static_pointer_cast<ComponentArray<T>>(componentData[handle]));

        }

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

        int lua_setTextureRegion(lua_State * lua);
        int lua_getTextureRegion(lua_State * lua);

        int lua_setTexturePath(lua_State * lua);
        int lua_getTexturePath(lua_State * lua);


    private:

        std::unordered_map<std::string,Id> handleToId;
        std::unordered_map<Id,Signature> idToSignature;
        std::unordered_map<Id,std::shared_ptr<Object>> objects;

        SystemManager systemManager;

        void initialiseBaseECS();

        bool componentRegistered(const char * h){return registeredComponents.find(h)!=registeredComponents.end();}

        uint32_t nextComponentIndex;

        std::unordered_map<const char *, uint32_t> registeredComponents;

        std::unordered_map<const char *, std::shared_ptr<AbstractComponentArray>> componentData;

        template <class T>
        uint32_t getComponentId()
        {
            const char * handle = typeid(T).name();
            return registeredComponents[handle];
        }
    };
}


#endif /* ENTITYCOMPONENTSYSTEM_H */
