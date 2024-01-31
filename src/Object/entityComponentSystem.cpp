#include <Object/entityComponentSystem.h>
namespace Hop::Object
{

    Id EntityComponentSystem::createObject()
    {
        std::shared_ptr<Object> o = std::make_shared<Object>();
        objects[o->id] = o;
        idToSignature[o->id] = Signature();
        //handleToId[Hop::Object::to_string(o->id)] = o->id;

        return o->id;
    }

    Id EntityComponentSystem::createObject(std::string handle)
    {
        std::shared_ptr<Object> o = std::make_shared<Object>();

        objects[o->id] = o;
        idToSignature[o->id] = Signature();

        handleToId[handle] = o->id;

        return o->id;
    }

    void EntityComponentSystem::remove(Id id)
    {
        for (auto & component : componentData)
        {
            component.second->remove(id);
        }
        idToSignature[id] = Signature(0);
        systemManager.objectSignatureChanged(id,idToSignature[id]);
        idToSignature.erase(id);
        objects.erase(id);

        for (auto handle : handleToId)
        {
            if (handle.second == id)
            {
                handleToId.erase(handle.first);
                break;
            }
        }

    }

    void EntityComponentSystem::remove(std::string handle){}

    // do nothing callback
    void identityCallback(Id & i, Id & j){return;}

    void EntityComponentSystem::initialiseBaseECS()
    {

        registerComponent<cTransform>();
        registerComponent<cRenderable>();
        registerComponent<cPhysics>();
        registerComponent<cCollideable>();
        //registerComponent<cPolygon>();
        registerComponent<cSound>();

        registerSystem<sRender>();
        registerSystem<sPhysics>();
        registerSystem<sCollision>();
        registerSystem<sSound>();

        uint32_t tId = getComponentId<cTransform>();
        uint32_t rId = getComponentId<cRenderable>();
        uint32_t pId = getComponentId<cPhysics>();
        uint32_t cId = getComponentId<cCollideable>();
        //uint32_t polyId = getComponentId<cPolygon>();
        uint32_t soundId = getComponentId<cSound>();

        Signature sRenderSig = Signature();

        sRenderSig.set
        (
            rId,
            true
        );

        sRenderSig.set
        (
            tId,
            true
        );

        systemManager.setSignature<sRender>(sRenderSig);

        Signature sPhysicsSig = Signature();

        sPhysicsSig.set
        (
            pId,
            true
        );
        sPhysicsSig.set
        (
            tId,
            true
        );

        systemManager.setSignature<sPhysics>(sPhysicsSig);
        
        Signature sCollisionSig = Signature();

        sCollisionSig.set
        (
            pId,
            true
        );
        sCollisionSig.set
        (
            cId,
            true
        );

        systemManager.setSignature<sCollision>(sCollisionSig);

        Signature sSoundSig = Signature();

        sSoundSig.set
        (
            soundId,
            true
        );

        systemManager.setSignature<sSound>(sSoundSig);
        
    }
}

// Lua bindings

#include <Object/LuaBindings/lua_objectIO.cpp>
#include <Object/LuaBindings/lua_transformIO.cpp>
#include <Object/LuaBindings/lua_meshIO.cpp>
#include <Object/LuaBindings/lua_renderableIO.cpp>