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
    registerComponent<cRenderable>();
    registerSystem<sRender>();

    uint32_t cId = componentManager.getComponentId<cRenderable>();
    Signature sRenderSig = Signature();
    sRenderSig.set(
        cId,
        true
    );
}