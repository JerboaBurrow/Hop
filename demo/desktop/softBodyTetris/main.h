#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <time.h>
#include <random>
#include <math.h>
#include <vector>

#include <chrono>
using namespace std::chrono;

#include <logo.h>

#include <Object/entityComponentSystem.h>

#include <System/Physics/sPhysics.h>
#include <System/Rendering/sRender.h>
#include <System/Physics/sCollision.h>
#include <System/Sound/sSound.h>

#include <World/world.h>
#include <World/marchingWorld.h>
#include <World/tileWorld.h>

#include <Console/console.h>

#include <Debug/collisionMeshDebug.h>

#include <jLog/jLog.h>

#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>

const int resX = 1000;
const int resY = 1000;
const float MAX_SPEED = 1.0/60.0;

// for smoothing delta numbers
uint8_t frameId = 0;
double deltas[60];

bool debug = true;
bool paused = true;

const double deltaPhysics = 1.0/900.0;

using Hop::Object::Component::cTransform;
using Hop::Object::Component::cPhysics;
using Hop::Object::Component::cRenderable;
using Hop::Object::Component::cCollideable;
using Hop::Object::EntityComponentSystem;
using Hop::Object::Id;

using Hop::System::Rendering::sRender;

using Hop::System::Physics::CollisionDetector;
using Hop::System::Physics::CollisionResolver;
using Hop::System::Physics::sPhysics;
using Hop::System::Physics::sCollision;
using Hop::System::Sound::sSound;

using Hop::System::Signature;

using Hop::World::MapSource;
using Hop::World::Boundary;
using Hop::World::AbstractWorld;
using Hop::World::TileWorld;
using jLog::INFO;
using jLog::WARN;

std::string fixedLengthNumber(double x, unsigned length)
{
    std::string d = std::to_string(x);
    std::string dtrunc(length,' ');
    for (unsigned c = 0; c < dtrunc.length(); c++/*ayy lmao*/)
    {

        if (c >= d.length())
        {
            dtrunc[c] = '0';
        }
        else
        {
            dtrunc[c] = d[c];
        }
    }
    return dtrunc;
}

std::shared_ptr<jGL::jGLInstance> jGLInstance;

#endif /* MAIN_H */
