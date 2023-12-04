extern "C"
{
    void Java_app_jerboa_glskeleton_Hop_stepPhysics
    (
            JNIEnv *env,
            jobject /* this */,
            jfloat delta = 0.0,
            jboolean detectCollisions = true
    )
    {
        if (manager != nullptr)
        {
            physics->step(manager, collisions, world);
        }
    }
}