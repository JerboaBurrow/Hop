#include <Object/objectManager.h>

void ObjectManager::add(std::shared_ptr<Object> o){
    
}

void ObjectManager::add(std::shared_ptr<Object> o, std::string handle){}

void ObjectManager::remove(Id id){}

void ObjectManager::remove(std::string handle){}

std::shared_ptr<Object> ObjectManager::getObject(Id id){
    return objects[id].first;
}

std::shared_ptr<Object> ObjectManager::getObject(std::string name){
    Id id = handleToId[name];
    return objects[id].first;
}

// do nothing callback
void identityCallback(std::string a, std::string b){return;}

template <class T>
void ObjectManager::addComponent(Id i, T component){
    componentManager->addComponent<T>(i,component);
    objects[i].second.set(
        componentManager->getComponentId<T>(),
        true
    );
    systemManager->objectSignatureChanged(i,objects[i].second);
}

template <class T>
void ObjectManager::removeComponent(Id i){
    componentManager->removeComponent<T>(i);
    objects[i].second.set(
        componentManager->getComponentId<T>(),
        false
    );
    systemManager->objectSignatureChanged(i,objects[i].second);
}