#ifndef COMPONENT_H
#define COMPONENT_H

#include <typeinfo>
#include <exception>
#include <bitset>

#include <Component/componentArray.h>

namespace Hop::Object::Component
{

    class ComponentNotRegistered: public std::exception 
    {

    public:

        ComponentNotRegistered(std::string msg)
        : msg(msg)
        {}

    private:

        virtual const char * what() const throw()
        {
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
        void registerComponent(uint32_t m){
            const char * handle = typeid(T).name();

            if (componentRegistered(handle)){
                return;
            }

            if (nextComponentIndex >= MAX_COMPONENTS){
                return;
            }

            registeredComponents[handle] = nextComponentIndex;
            nextComponentIndex++;
            componentData[handle] = std::make_shared<ComponentArray<T>>(m);
        }

        template <class T>
        void addComponent(Id i, T component){
            const char * handle = typeid(T).name();

            if (!componentRegistered(handle)){
                return;
            }

            getComponentArray<T>()->insert(i,component);
        }

        template <class T>
        void removeComponent(Id i){
            const char * handle = typeid(T).name();

            if (!componentRegistered(handle)){
                return;
            }

            getComponentArray<T>()->remove(i);
        }

        template <class T>
        inline T & getComponent(const Id & i){
            // const char * handle = typeid(T).name();

            // if (!componentRegistered(handle)){
            //     throw ComponentNotRegistered(" Attempt to getComponent<"+i.idStr+")");
            // }

            return getComponentArray<T>()->get(i);
        }

        void objectFreed(Id i){
            for (auto const& pair : componentData){
                pair.second.get()->objectFreed(i);
            }
        }

        template <class T>
        uint32_t getComponentId(){
            const char * handle = typeid(T).name();
            if (!componentRegistered(handle)){
                throw ComponentNotRegistered(" Attempt to getComponent<"+std::string(handle)+">()");
            }
            return registeredComponents[handle];
        }

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

}

#endif /* COMPONENT_H */
