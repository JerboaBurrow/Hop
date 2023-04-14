// World

extern "C" {

    void Java_app_jerboa_glskeleton_Hop_getWorldPosition
            (
                    JNIEnv *env,
                    jobject /* this */
            );

    void Java_app_jerboa_glskeleton_Hop_screenToWorld
            (
                    JNIEnv *env,
                    jobject /* this */,
                    jfloat x,
                    jfloat y,
                    jfloat rx,
                    jfloat ry
            ) {
        if (camera == nullptr) {
            return;
        }

        glm::vec4 w = camera->screenToWorld(x,y);
        rx = w[0];
        ry = w[1];
    }

}