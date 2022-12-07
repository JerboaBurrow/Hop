#include <System/systemManager.h>

template<typename T>
std::shared_ptr<T> SystemManager::registerSystem(){
    const char * handle = typeid(T).name();
    if (isRegistered(handle)){

    }

    signatures[handle] = Signature();
    std::shared_ptr<System> s = std::make_shared<T>();
    systems[handle] = s;
    return s;
}

template <class T>
void SystemManager::setSignature(Signature signature){
    const char * handle = typeid(T).name();
    if (!isRegistered(handle)){
        return;
    }
    signatures[handle] = signature;
}

void SystemManager::objectFreed(Id i){
    for (auto const& pair : systems){
        auto const& system = pair.second;
        system->objects.erase(i);
    }
}

void SystemManager::objectSignatureChanged(Id i, Signature es){
    for (auto const& pair : systems){
        const char * handle = pair.first;
        std::shared_ptr<System> system = pair.second;
        Signature ss = signatures[handle];

        if ((es & ss) == ss){
            system->objects.insert(i);
        }
        else{
            system->objects.erase(i);
        }
    }
}
