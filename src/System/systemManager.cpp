#include <System/systemManager.h>
#include <iostream>

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

        if (system->threadJobs.size()==0){
            for (int j = 0; j < threads; j++){
                system->threadJobs.push_back(std::vector<Id>());
            }
        }
        Signature ss = signatures[handle];
        if ((es & ss) == ss){
            size_t n = system->objects.size();
            system->threadJobs[n%threads].push_back(i);
            system->objects.insert(i);
        }
        else{
            system->objects.erase(i);

            for (int j = 0; j < threads; j++){
                auto itr = std::find(system->threadJobs[j].begin(),system->threadJobs[j].end(),i);
                if (itr != system->threadJobs[j].end())
                    system->threadJobs[j].erase(itr);
            }
        }
    }
}
