#include <jni.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <engine.h>

static Hop::Engine * hop = nullptr;
static Hop::World::Boundary * boundary = nullptr;
static Hop::World::PerlinSource * perlin = nullptr;
static Hop::World::MapSource * source = nullptr;

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

            if (hop == nullptr) {

                hop = new Hop::Engine
                        (
                                resX,
                                resY,
                                source,
                                boundary,
                                WorldOptions(2, 64, 0, true),
                                PhysicsOptions(1.0 / 600.0, 9.81, 0.66, true),
                                0
                        );
            }
        }

    #include <world.cpp>
    #include <rendering.cpp>
    #include <logging.cpp>
    #include <object.cpp>
    #include <physics.cpp>
}