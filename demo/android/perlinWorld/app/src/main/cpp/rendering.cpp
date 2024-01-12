extern "C"
{

    void Java_app_jerboa_glskeleton_Hop_beginFrame(JNIEnv * env, jobject /* this */)
    {
        jgl->beginFrame();
    }

    void Java_app_jerboa_glskeleton_Hop_endFrame(JNIEnv * env, jobject /* this */)
    {
        jgl->endFrame();
    }

    void Java_app_jerboa_glskeleton_Hop_render(
            JNIEnv *env,
            jobject /* this */,
            jboolean refresh
    )
    {
        if (manager == nullptr || jgl == nullptr || world == nullptr){
            return;
        }

        sRender & rendering = manager->getSystem<sRender>();

        rendering.setProjection(camera->getVP());
        rendering.draw(jgl, manager.get(), world.get());

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
        if (jgl == nullptr){
            return;
        }

        jgl->text
                (
                        jstring2string(env,text).c_str(),
                        glm::vec2(x, y),
                        scale,
                        glm::vec4(r,g,b,a)
                );
    }
}