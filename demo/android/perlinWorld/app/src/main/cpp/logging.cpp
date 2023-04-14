extern "C"
{
    void Java_app_jerboa_glskeleton_Hop_printLog
    (
        JNIEnv *env,
        jobject /* this */
    )
    {
        if (hopLog != nullptr)
        {
            hopLog->androidLog();
        }
    }
}