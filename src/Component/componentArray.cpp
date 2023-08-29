#include <Component/componentArray.h>

namespace Hop::Object::Component
{
    template <>
    void ComponentArray<cPhysics>::reduce(unsigned worker, REDUCTION_TYPE t)
    {

        if (t == REDUCTION_TYPE::SUM_EQUALS)
        {
            unsigned start = std::floor(float(nextIndex)/float(workerData.size()))*worker;
            unsigned idx = 0;

            for (unsigned j = 0; j < nextIndex; j++)
            {   
                idx = (j+start) % nextIndex;
                componentData[idx].fx += workerData[worker][idx].fx;
                componentData[idx].fy += workerData[worker][idx].fy;
                componentData[idx].tau += workerData[worker][idx].tau;
            }
        }
    }

    template <>
    void ComponentArray<cCollideable>::reduce(unsigned worker, REDUCTION_TYPE t){/*void*/}
}