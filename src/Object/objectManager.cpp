#include <Object/objectManager.h>

Id ObjectManager::createObject(){
    std::shared_ptr<Object> o = std::make_shared<Object>();

    objects[o->id] = o;
    idToSignature[o->id] = Signature();

    return o->id;
}

Id ObjectManager::createObject(std::string handle){
    std::shared_ptr<Object> o = std::make_shared<Object>();

    objects[o->id] = o;
    idToSignature[o->id] = Signature();
    handleToId[handle] = o->id;

    return o->id;
}

void ObjectManager::remove(Id id){}

void ObjectManager::remove(std::string handle){}

std::shared_ptr<Object> ObjectManager::getObject(Id id){
    return objects[id];
}

std::shared_ptr<Object> ObjectManager::getObject(std::string name){
    Id id = handleToId[name];
    return objects[id];
}

// do nothing callback
void identityCallback(std::string a, std::string b){return;}

void ObjectManager::initialiseBaseECS(){

    registerComponent<cTransform>();
    registerComponent<cRenderable>();
    registerComponent<cPhysics>();

    registerSystem<sRender>();
    registerSystem<sPhysics>();

    uint32_t tId = getComponentId<cTransform>();
    uint32_t rId = getComponentId<cRenderable>();
    uint32_t pId = getComponentId<cPhysics>();

    Signature sRenderSig = Signature();

    sRenderSig.set(
        rId,
        true
    );

    sRenderSig.set(
        tId,
        true
    );

    systemManager.setSignature<sRender>(sRenderSig);

    Signature sPhysicsSig = Signature();

    sPhysicsSig.set(
        pId,
        true
    );
    sPhysicsSig.set(
        tId,
        true
    );

    std::cout << sRenderSig << ", " << sPhysicsSig << "\n";

    systemManager.setSignature<sPhysics>(sPhysicsSig);
}