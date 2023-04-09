extern "C"
{
    void Java_app_jerboa_glskeleton_Hop_spawnBall
    (
        JNIEnv *env,
        jobject, /* this */
        jfloat x,
        jfloat y,
        jfloat s,
        jfloat r,
        jfloat g,
        jfloat b,
        jfloat a
    )
    {
        if (hop != nullptr) {

            Hop::Object::Id pid;

            s > 1.0 ? s = 1.0 : s = s;
            s < 0.01 ? s = 0.01 : s = s;

            double radius = s * hop->getCollisionPrimitiveMaxSize();

            //hop->log<Hop::Logging::INFO>("s, radius = "+std::to_string(s) + ", " + std::to_string(radius));

            pid = hop->createObject();

            hop->addComponent<Hop::Object::Component::cTransform>(
                    pid,
                    Hop::Object::Component::cTransform(
                            x, y, 0.0, radius
                    )
            );

            hop->addComponent<Hop::Object::Component::cRenderable>(
                    pid,
                    Hop::Object::Component::cRenderable(
                            "circleObjectShader",
                            r, g, b, a
                    )
            );

            //hop->log<Hop::Logging::INFO>("colour = "+std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b));

            hop->addComponent<Hop::Object::Component::cPhysics>(
                    pid,
                    Hop::Object::Component::cPhysics(
                            x, y, 0.0
                    )
            );

            hop->addComponent<Hop::Object::Component::cCollideable>(
                    pid,
                    Hop::Object::Component::cCollideable(
                            std::vector<Hop::System::Physics::CollisionVertex>
                            {
                                    Hop::System::Physics::CollisionVertex(0.0, 0.0, 1.0)
                            },
                            x, y, 0.0, radius
                    )
            );
        }
    }
}