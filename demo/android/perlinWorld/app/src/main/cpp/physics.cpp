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
            sPhysics & physics = manager->getSystem<sPhysics>();
            sCollision & collisions = manager->getSystem<sCollision>();
            physics.step(manager.get(), &collisions, world.get());
        }
    }
}