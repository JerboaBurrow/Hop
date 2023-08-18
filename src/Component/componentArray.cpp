#include <Component/componentArray.h>

namespace Hop::Object::Component
{
    template <>
    void ComponentArray<cPhysics>::reduce(REDUCTION_TYPE t)
    {
        for (unsigned j = 0; j < nextIndex; j++)
        {
            double fx = 0.0;
            double fy = 0.0;
            for (unsigned i = 0; i < workerData.size(); i++)
            {
                fx += workerData[i][j].fx;
                fy += workerData[i][j].fy;
                workerData[i][j].fx = 0.0;
                workerData[i][j].fy = 0.0;
            }

            if (t == REDUCTION_TYPE::EQUALS_SUM)
            {
                componentData[j].fx = fx;
                componentData[j].fy = fy;
            }
            else if (t == REDUCTION_TYPE::SUM_EQUALS_SUM)
            {
                componentData[j].fx += fx;
                componentData[j].fy += fy;
            }

        }
    }

    template <>
    void ComponentArray<cCollideable>::reduce(REDUCTION_TYPE t){/*void*/}
}