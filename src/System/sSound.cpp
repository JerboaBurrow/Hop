#include <System/sSound.h>

namespace Hop::System::Sound
{
    void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
        ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
        if (pDecoder == NULL) {
            return;
        }

        /* Reading PCM frames will loop based on what we specified when called ma_data_source_set_looping(). */
        ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

        (void)pInput;
    }
}