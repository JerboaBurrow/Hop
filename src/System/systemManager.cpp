#include <System/systemManager.h>
#include <iostream>


namespace Hop::System
{
    void SystemManager::objectSignatureChanged(Id i, Signature es)
    {
        for (auto const& pair : systems)
        {
            const char * handle = pair.first;
            std::shared_ptr<System> system = pair.second;

            Signature ss = signatures[handle];

            if ((es & ss) == ss)
            {
                system->queueAdd(i);
            }
            else
            {
                system->queueRemove(i);
            }
        }
    }
}