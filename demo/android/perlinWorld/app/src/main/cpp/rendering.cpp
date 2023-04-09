extern "C"
{
    void Java_app_jerboa_glskeleton_Hop_render(
            JNIEnv *env,
            jobject /* this */,
            jboolean refresh
    )
    {
        if (hop == nullptr){
            return;
        }

        hop->render(refresh);

        //hop->log<Hop::Logging::INFO>("refresh = "+std::to_string(refresh));
    }

    void Java_app_jerboa_glskeleton_Hop_renderText
            (
                    JNIEnv *env,
                    jobject /* this */,
                    jstring text,
                    jfloat x,
                    jfloat y,
                    jfloat scale,
                    jfloat r,
                    jfloat g,
                    jfloat b,
                    jfloat a,
                    jboolean centred
            )
    {
        if (hop == nullptr){
            return;
        }

        hop->renderText
                (
                        jstring2string(env,text),
                        x,
                        y,
                        scale,
                        glm::vec3(r,g,b),
                        a,
                        centred
                );
    }
}