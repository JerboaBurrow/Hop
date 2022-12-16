#include <Object/objectManager.h>

Id ObjectManager::createObject(
    double x,
    double y,
    double s,
    std::string shader
){
    std::shared_ptr<Id> id = std::make_shared<Id>();

    for (int i = 0; i < physData; i++){
        objectPhysData.push_back(0.0);
    }

    for (int i = 0; i < renderData; i++){
        objectRenderData.push_back(0.0);
    }

    objectShaders.push_back("");

    objectPhysData[physData*lastObject] = x;
    objectPhysData[physData*lastObject+1] = x;

    objectPhysData[physData*lastObject+2] = y;
    objectPhysData[physData*lastObject+3] = y;

    objectPhysData[physData*lastObject+4] = 0.0;
    objectPhysData[physData*lastObject+5] = 0.0;

    objectPhysData[physData*lastObject+6] = s;

    objectPhysData[physData*lastObject+7] = 0.0;
    objectPhysData[physData*lastObject+8] = 0.0;
    objectPhysData[physData*lastObject+9] = 0.0;

    objectPhysData[physData*lastObject+10] = 0.01;
    objectPhysData[physData*lastObject+11] = 0.01;

    objectPhysData[physData*lastObject+12] = 0.0;
    objectPhysData[physData*lastObject+13] = 0.0;
    objectPhysData[physData*lastObject+14] = 0.0;

    objectRenderData[renderData*lastObject] = 1.0;
    objectRenderData[renderData*lastObject+1] = 0.0;
    objectRenderData[renderData*lastObject+2] = 0.0;
    objectRenderData[renderData*lastObject+3] = 1.0;

    objectRenderData[renderData*lastObject+4] = 0.0;
    objectRenderData[renderData*lastObject+5] = 0.0;
    objectRenderData[renderData*lastObject+6] = 0.0;
    objectRenderData[renderData*lastObject+7] = 0.0;

    objectShaders[lastObject] = shader;

    idToIndex[*id.get()] = lastObject;
    ids.push_back(id);
    lastObject++;
    return *id.get();
}

// do nothing callback
void identityCallback(std::string a, std::string b){return;}