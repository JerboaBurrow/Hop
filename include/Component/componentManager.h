#ifndef COMPONENT_H
#define COMPONENT_H

#include <typeinfo>
#include <exception>
#include <bitset>

#include <Component/componentArray.h>

class ComponentNotRegistered: public std::exception {
public:
    ComponentNotRegistered(std::string msg)
    : msg(msg)
    {}
private:
    virtual const char * what() const throw(){
        return msg.c_str();
    }
    std::string msg;
};


const uint32_t MAX_COMPONENTS = 64;

typedef std::bitset<MAX_COMPONENTS> Signature;

class ComponentManager {
public:
    ComponentManager()
    : nextComponentIndex(0)
    {}

    template <class T>
    void registerComponent();

    // don't think this is a good idea
    //  rug-pulls objects
    // template <class T>
    // void deregisterComponent();

    template <class T>
    void addComponent(Id i, T component);

    template <class T>
    void removeComponent(Id i);

    template <class T>
    T & getComponent(Id i);

    void objectFreed(Id i);

    template <class T>
    uint32_t getComponentId();

private:

    bool componentRegistered(const char * h){return registeredComponents.find(h)!=registeredComponents.end();}

    uint32_t nextComponentIndex;
    std::unordered_map<const char *,uint32_t> registeredComponents;
    std::unordered_map<const char*, std::shared_ptr<AbstractComponentArray>> componentData;


    template<typename T>
	std::shared_ptr<ComponentArray<T>> getComponentArray()
	{
		const char * handle = typeid(T).name();

        if (!componentRegistered(handle)){
            throw ComponentNotRegistered("");
        }

		return std::static_pointer_cast<ComponentArray<T>>(componentData[handle]);
	}
};

#endif /* COMPONENT_H */
