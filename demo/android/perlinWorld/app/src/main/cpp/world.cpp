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
        if (hop == nullptr) {
            return;
        }

        std::pair<float, float> w = hop->screenToWorld(x, y);

        rx = w.first;
        ry = w.second;
    }

}