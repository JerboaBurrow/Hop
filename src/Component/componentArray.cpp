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
            double tau = 0.0;
            for (unsigned i = 0; i < workerData.size(); i++)
            {
                fx += workerData[i][j].fx;
                fy += workerData[i][j].fy;
                tau += workerData[i][j].tau;
                workerData[i][j].fx = 0.0;
                workerData[i][j].fy = 0.0;
                workerData[i][j].tau = 0.0;
            }

            if (t == REDUCTION_TYPE::EQUALS_SUM)
            {
                componentData[j].fx = fx;
                componentData[j].fy = fy;
                componentData[j].tau = tau;
            }
            else if (t == REDUCTION_TYPE::SUM_EQUALS_SUM)
            {
                componentData[j].fx += fx;
                componentData[j].fy += fy;
                componentData[j].tau += tau;
            }

        }
    }

    template <>
    void ComponentArray<cCollideable>::reduce(REDUCTION_TYPE t){/*void*/}
}