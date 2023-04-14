#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <System/system.h>
#include <exception>
#include <bitset>
#include <algorithm>
#include <unordered_map>

namespace Hop::System
{

    const uint32_t MAX_COMPONENTS = 64;
    typedef std::bitset<MAX_COMPONENTS> Signature;

    class SystemNotRegistered: public std::exception 
    {

    public:

        SystemNotRegistered(std::string msg)
        : msg(msg)
        {}

    private:

        virtual const char * what() const throw()
        {
            return msg.c_str();
        }

        std::string msg;

    };


    class SystemManager 
    {

    public:

        SystemManager(){}

        template<typename T>
        void registerSystem()
        {
            const char * handle = typeid(T).name();
            if (isRegistered(handle))
            {

            }

            signatures[handle] = Signature();
            std::shared_ptr<System> s = std::make_shared<T>();
            systems[handle] = s;
        }

        template <class T>
        void setSignature(Signature signature)
        {
            const char * handle = typeid(T).name();
            if (!isRegistered(handle))
            {
                throw SystemNotRegistered("setSignature");
                return;
            }
            signatures[handle] = signature;
        }

        template <class T>
        T & getSystem()
        {
            const char * handle = typeid(T).name();
            if (!isRegistered(handle))
            {
                throw SystemNotRegistered("getSystem");
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

}

#endif /* SYSTEMMANAGER_H */
