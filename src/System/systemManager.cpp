#include <System/systemManager.h>
#include <iostream>


namespace Hop::System
{

    void SystemManager::objectFreed(Id i)
    {
        for (auto const& pair : systems)
        {
            auto const& system = pair.second;
            system->objects.erase(i);
        }
    }

    void SystemManager::objectSignatureChanged(Id i, Signature es)
    {
        for (auto const& pair : systems)
        {
            const char * handle = pair.first;
            std::shared_ptr<System> system = pair.second;

            if (system->threadJobs.size()==0)
            {
                for (int j = 0; j < threads; j++)
                {
                    system->threadJobs.push_back(std::set<Id>());
                }
            }

            Signature ss = signatures[handle];

            if ((es & ss) == ss)
            {
                size_t n = system->objects.size();
                if (threads != 0)
                {
                    bool isAccounted = false;
                    for (unsigned j = 0; j < system->threadJobs.size(); j++){
                        if (system->threadJobs[j].find(i) != system->threadJobs[j].end()){
                            isAccounted = true;
                            break;
                        }
                    }
                    if (!isAccounted){
                        system->threadJobs[n%threads].insert(i);
                    }
                }
                system->objects.insert(i);
            }
            else
            {
                system->objects.erase(i);

                for (int j = 0; j < threads; j++)
                {
                    auto itr = std::find(system->threadJobs[j].begin(),system->threadJobs[j].end(),i);
                    if (itr != system->threadJobs[j].end())
                    {
                        system->threadJobs[j].erase(itr);
                    }
                }
            }
        }
    }
}