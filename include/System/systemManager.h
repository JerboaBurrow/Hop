#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <Component/componentManager.h>
#include <System/system.h>

class SystemManager {
public:

    template<typename T>
    void registerSystem(){
        const char * handle = typeid(T).name();
        if (isRegistered(handle)){

        }

        signatures[handle] = Signature();
        std::shared_ptr<System> s = std::make_shared<T>();
        systems[handle] = s;
    }

    template <class T>
    void setSignature(Signature signature){
        const char * handle = typeid(T).name();
        if (!isRegistered(handle)){
            return;
        }
        signatures[handle] = signature;
    }

    template <class T>
    T & getSystem(){
        const char * handle = typeid(T).name();
        if (!isRegistered(handle)){

        }

        return *(std::static_pointer_cast<T>(systems[handle]).get());
    }

    void objectFreed(Id i);

    void objectSignatureChanged(Id i, Signature s);

    
private:

    bool isRegistered(const char * s){return systems.find(s) != systems.end();}

    std::unordered_map<const char*, Signature> signatures;
	std::unordered_map<const char*, std::shared_ptr<System>> systems;
};

#endif /* SYSTEMMANAGER_H */
