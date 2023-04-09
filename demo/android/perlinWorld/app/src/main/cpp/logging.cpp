extern "C"
{
    void Java_app_jerboa_glskeleton_Hop_printLog
    (
        JNIEnv *env,
        jobject /* this */
    )
    {
        if (hop != nullptr)
        {
            hop->outputLogAndroid();
        }
    }
}