#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <Component/componentManager.h>
#include <System/system.h>

class SystemManager {
public:

    template<class T>
	std::shared_ptr<T> registerSystem();

    template<class T>
	void setSignature(Signature signature);

    void objectFreed(Id i);

    void objectSignatureChanged(Id i, Signature s);

private:

    bool isRegistered(const char * s){return systems.find(s) != systems.end();}

    std::unordered_map<const char*, Signature> signatures;
	std::unordered_map<const char*, std::shared_ptr<System>> systems;
};

#endif /* SYSTEMMANAGER_H */
