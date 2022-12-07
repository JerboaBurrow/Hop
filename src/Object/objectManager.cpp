#include <Object/objectManager.h>

void ObjectManager::createObject(){
    std::shared_ptr<Object> o = std::make_shared<Object>();

    objects[o->id] = o;
    idToSignature[o->id] = Signature();
}

void ObjectManager::createObject(std::string handle){}

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

template <class T>
void ObjectManager::addComponent(Id i, T component){
    componentManager->addComponent<T>(i,component);
    idToSignature[i].set(
        componentManager->getComponentId<T>(),
        true
    );
    systemManager->objectSignatureChanged(i,idToSignature[i]);
}

template <class T>
void ObjectManager::removeComponent(Id i){
    componentManager->removeComponent<T>(i);
    idToSignature[i].set(
        componentManager->getComponentId<T>(),
        false
    );
    systemManager->objectSignatureChanged(i,idToSignature[i]);
}