#include <jni.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <math.h>
#include <algorithm>
#include <memory>

#include <jGL/orthoCam.h>
#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>

#include <jLog/jLog.h>

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

using Hop::Object::Component::cTransform;
using Hop::Object::Component::cPhysics;
using Hop::Object::Component::cRenderable;
using Hop::Object::Component::cCollideable;
using Hop::Object::EntityComponentSystem;
using Hop::Object::Id;

using jGL::OrthoCam;
using Hop::System::Rendering::sRender;

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

using jLog::INFO;
using jLog::WARN;

static std::unique_ptr<Boundary<double>> boundary = nullptr;
static std::unique_ptr<PerlinSource> perlin = nullptr;
static std::unique_ptr<AbstractWorld> world = nullptr;

static std::unique_ptr<EntityComponentSystem> manager = nullptr;

static std::unique_ptr<jLog::Log> hopLog = nullptr;

static std::unique_ptr<CollisionDetector> detector = nullptr;
static std::unique_ptr<CollisionResolver> resolver = nullptr;

static std::unique_ptr<sRender> renderer = nullptr;
static std::unique_ptr<OrthoCam> camera = nullptr;

static std::shared_ptr<jGL::jGLInstance> jgl = nullptr;

std::string jstring2string(JNIEnv *env, jstring jStr) {
    
    if (!jStr) {return "";}

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

            perlin = std::make_unique<Hop::World::PerlinSource>(2,0.07,5.0,5.0,256);
            perlin->setThreshold(0.2);
            perlin->setSize(64*3+1);

            camera = std::make_unique<jGL::OrthoCam>(resX, resY);

            boundary = std::make_unique<Hop::World::InfiniteBoundary<double>>();

            jgl = std::make_shared<jGL::GL::OpenGLInstance>(glm::ivec2(resX, resY), 0);

            jgl->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
            jgl->setMSAA(0);
            jgl->setClear(glm::vec4(1.0f,1.0f,1.0f,1.0f));

            world = std::make_unique<Hop::World::MarchingWorld>(
                    2,
                    camera.get(),
                    64,
                    1,
                    perlin.get(),
                    boundary.get()
            );

            hopLog = std::make_unique<jLog::Log>();

            manager = std::make_unique<EntityComponentSystem>();

            sRender & rendering = manager->getSystem<sRender>();
            rendering.setDrawMeshes(true);

            // setup physics system
            sPhysics & physics = manager->getSystem<sPhysics>();
            physics.setTimeStep(1.0/900.0);
            physics.setGravity(9.81, 0.0, -1.0);

            sCollision & collisions = manager->getSystem<sCollision>();

            auto det = std::make_unique<Hop::System::Physics::CellList>(world.get());

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