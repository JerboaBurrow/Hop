#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <World/world.h>
#include <Object/object.h>

#include <unordered_map>
#include <string>

#include <System/sRender.h>
#include <System/sPhysics.h>

#include <log.h>

class ObjectManager;
class sRender;
class sPhysics;

// define CollisionCallback as this func ptr
typedef void (*CollisionCallback)(std::string,std::string);
void identityCallback(std::string a, std::string b);

const uint32_t MAX_OBJECTS = 100000;

class ObjectManager {
public:

    const size_t physData=15;
    const size_t renderData=8;

    friend class sRender;
    friend class sPhysics;

    ObjectManager(
        void (*callback)(std::string,std::string) = &identityCallback
    )
    : collisionCallback(callback), lastObject(0)
    {
        objectPhysData.reserve(MAX_OBJECTS*physData);
        objectRenderData.reserve(MAX_OBJECTS*renderData);
        objectShaders.reserve(MAX_OBJECTS);
    }

    Id createObject(
        double x,
        double y,
        double s,
        std::string shader
    );

    void remove(Id id);

    CollisionCallback collisionCallback;

    void postToLog(LogType msg){
        msg >> log; 
    }

    std::vector<double> & getPhysData(){return objectPhysData;}
    size_t nObjects(){return lastObject;}
    Log & getLog(){return log;}

private:

    std::unordered_map<Id,size_t> idToIndex;

    std::vector<double> objectPhysData;
    std::vector<float> objectRenderData;
    std::vector<std::string> objectShaders;

    std::vector<std::shared_ptr<Id>> ids;

    size_t lastObject;

    Log log;
};


#endif /* OBJECTMANAGER_H */
