#ifndef MAIN
#define MAIN

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <gl.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <time.h>
#include <random>
#include <math.h>
#include <vector>

#include <chrono>
using namespace std::chrono;

#include <logo.h>

#include <Display/display.h>
#include <orthoCam.h>

#include <Text/textRenderer.h>

#include <Object/entityComponentSystem.h>

#include <System/Physics/sPhysics.h>
#include <System/Rendering/sRender.h>
#include <System/Physics/sCollision.h>

#include <World/world.h>
#include <World/marchingWorld.h>
#include <World/tileWorld.h>

#include <Console/console.h>

#include <Util/util.h>
#include <jLog/jLog.h>

#include <Debug/collisionMeshDebug.h>

using namespace std::chrono;

const int resX = 1000;
const int resY = 1000;
const float MAX_SPEED = 1.0/60.0;

// for smoothing delta numbers
uint8_t frameId = 0;
double deltas[60];

bool debug = false;

const double deltaPhysics = 1.0/(900.0);
const unsigned subSamples = 1;

using Hop::Object::Component::cTransform;
using Hop::Object::Component::cPhysics;
using Hop::Object::Component::cRenderable;
using Hop::Object::Component::cCollideable;
using Hop::Object::EntityComponentSystem;
using Hop::Object::Id;

using Hop::System::Rendering::OrthoCam;
using Hop::System::Rendering::Type;
using Hop::System::Rendering::TextRenderer;
using Hop::System::Rendering::sRender;
using Hop::System::Rendering::Shaders;

using Hop::System::Physics::CollisionDetector;
using Hop::System::Physics::CollisionResolver;
using Hop::System::Physics::sPhysics;
using Hop::System::Physics::sCollision;

using Hop::System::Signature;

using Hop::World::MapSource;
using Hop::World::Boundary;
using Hop::World::AbstractWorld;
using Hop::World::TileWorld;

using jLog::INFO;
using jLog::WARN;

using Hop::Util::fixedLengthNumber;

#endif /* MAIN */
