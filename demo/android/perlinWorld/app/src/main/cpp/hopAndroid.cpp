#include <jni.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <math.h>
#include <algorithm>

#include <orthoCam.h>

#include <Text/textRenderer.h>

#include <Object/entityComponentSystem.h>

#include <System/sPhysics.h>
#include <System/sRender.h>
#include <System/sCollision.h>

#include <World/world.h>
#include <World/marchingWorld.h>
#include <World/tileWorld.h>

#include <Console/console.h>

#include <Util/util.h>

#include <log.h>

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
using Hop::System::Physics::CollisionPrimitive;

using Hop::System::Signature;

using Hop::World::MapSource;
using Hop::World::PerlinSource;
using Hop::World::Boundary;
using Hop::World::AbstractWorld;
using Hop::World::TileWorld;
using Hop::World::MarchingWorld;

using Hop::Logging::INFO;
using Hop::Logging::WARN;

static Boundary * boundary = nullptr;
static PerlinSource * perlin = nullptr;
static MapSource * source = nullptr;
static MarchingWorld * world = nullptr;

static EntityComponentSystem * manager = nullptr;

static Hop::Logging::Log * hopLog = nullptr;

static CollisionDetector * detector = nullptr;
static CollisionResolver * resolver = nullptr;

static sRender * renderer = nullptr;
static OrthoCam * camera = nullptr;
static TextRenderer * textRenderer = nullptr;
static Type * font;
static Shaders * shaderPool = nullptr;

std::string jstring2string(JNIEnv *env, jstring jStr) {
    if (!jStr)
        return "";

    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

    size_t length = (size_t) env->GetArrayLength(stringJbytes);
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

    std::string ret = std::string((char *)pBytes, length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}

extern "C"
{
    void Java_app_jerboa_glskeleton_Hop_hop(
            JNIEnv *env,
            jobject /* this */,
            jint resX,
            jint resY
        )
        {
            float posX = 0.0;
            float posY = 0.0;

            perlin = new Hop::World::PerlinSource(2,0.07,5.0,5.0,256);
            perlin->setThreshold(0.2);
            perlin->setSize(64*3+1);

            source = static_cast<Hop::World::MapSource*>(perlin);
            boundary = new Hop::World::InfiniteBoundary;

            camera = new Hop::System::Rendering::OrthoCam(resX,resY,glm::vec2(0.0,0.0));

            world = new Hop::World::MarchingWorld(
                    2,
                    camera,
                    64,
                    1,
                    source,
                    boundary
            );

            textRenderer = new Hop::System::Rendering::TextRenderer(glm::ortho(0.0,double(resX),0.0,double(resY)));
            font = new Hop::System::Rendering::Type(48);

            hopLog = new Hop::Logging::Log;

            shaderPool = new Hop::System::Rendering::Shaders;
            shaderPool->defaultShaders(*hopLog);

            manager = new EntityComponentSystem;

            sRender & rendering = manager->getSystem<sRender>();

            // setup physics system
            sPhysics & physics = manager->getSystem<sPhysics>();
            physics.setTimeStep(1.0/900.0);
            physics.setGravity(9.81);

            sCollision & collisions = manager->getSystem<sCollision>();

            auto det = std::make_unique<Hop::System::Physics::CellList>(world);

            auto res = std::make_unique<Hop::System::Physics::SpringDashpot>
                    (
                            (1.0/900.0)*10.0,
                            0.66,
                            0.0
                    );

            collisions.setDetector(std::move(det));
            collisions.setResolver(std::move(res));
        }

    #include <world.cpp>
    #include <rendering.cpp>
    #include <logging.cpp>
    #include <object.cpp>
    #include <physics.cpp>
}