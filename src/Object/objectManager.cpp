#include <Object/objectManager.h>

Object & ObjectManager::getObject(std::string name){
    return objects[name];
}

// do nothing callback
void identityCallback(std::string a, std::string b){return;}