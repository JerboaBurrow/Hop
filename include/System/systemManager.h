#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <constants.h>
#include <System/system.h>

#include <exception>
#include <bitset>
#include <algorithm>
#include <unordered_map>

namespace Hop::System
{
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

    /**
     * @brief Registers and manages systems signatures.
     *
     */
    class SystemManager
    {

    public:

        SystemManager(){}

        /**
         * @brief Register T as a system.
         *
         * @tparam T the system class.
         * @remark Gives T the catch all Signature.
         */
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

        /**
         * @brief Set the Signature for system T.
         *
         * @tparam T the registered System class.
         * @param signature object Signature.
         */
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

        /**
         * @brief Return the given system T.
         *
         * @tparam T the registered system class.
         * @return T& the system object.
         */
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

        /**
         * @brief Refresh object i's Signature.
         *
         * @param i Id of the changed object.
         * @param s the new Signature.
         */
        void objectSignatureChanged(Id i, Signature s);


    private:

        bool isRegistered(const char * s){return systems.find(s) != systems.end();}

        std::unordered_map<const char*, Signature> signatures;
        std::unordered_map<const char*, std::shared_ptr<System>> systems;

    };

}

#endif /* SYSTEMMANAGER_H */
