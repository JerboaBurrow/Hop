#include <System/systemManager.h>

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
