#include <Component/componentManager.h>

template <class T>
void ComponentManager::registerComponent(){
    const char * handle = typeid(T).name();

    if (componentRegistered(handle)){
        return;
    }

    if (nextComponentIndex >= MAX_COMPONENTS){
        return;
    }

    registeredComponents[handle] = nextComponentIndex;
    componentData[handle] = std::make_shared<ComponentArray<T>>();
}

template <class T>
void ComponentManager::addComponent(Id i, T component){
    const char * handle = typeid(T).name();

    if (!componentRegistered(handle)){
        return;
    }

    getComponentArray<T>()->insert(i,component);
}

template <class T>
void ComponentManager::removeComponent(Id i){
    const char * handle = typeid(T).name();

    if (!componentRegistered(handle)){
        return;
    }

     getComponentArray<T>()->remove(i);
}

template <class T>
T & ComponentManager::getComponent(Id i){
    const char * handle = typeid(T).name();

    if (!componentRegistered(handle)){
        throw ComponentNotRegistered(" Attempt to getComponent<"+std::string(handle)+">("+std::string(i)+")");
    }

    return getComponentArray<T>()->get(i);
}

void ComponentManager::objectFreed(Id i){
    for (auto const& pair : componentData){
        pair.second.get()->objectFreed(i);
    }
}

template <class T>
uint32_t ComponentManager::getComponentId(){
    const char * handle = typeid(T).name();
    if (!componentRegistered(handle)){
        throw ComponentNotRegistered(" Attempt to getComponent<"+std::string(handle)+">()");
    }
    return registeredComponents[handle];
}