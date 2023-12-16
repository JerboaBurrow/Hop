#ifndef SSOUND_H
#define SSOUND_H

#include <miniaudio/miniaudio.h>

#include <iostream>

#include <Object/entityComponentSystem.h>

namespace Hop::Object
{
    class EntityComponentSystem;
}

namespace Hop::System::Sound
{

    void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
   
    class sSound : public System
    {
    
    public:

        sSound()
        : loopingFile("")
        {
            ma_result result = ma_engine_init(NULL, &engine);
        }

        ~sSound()
        {
            ma_engine_uninit(&engine);
            ma_device_uninit(&device);
            ma_decoder_uninit(&decoder);
        }

        void setLoopSound(std::string filename)
        {
            if (loopingFile == "")
            {
                loopingFile = filename;
                ma_result result = ma_decoder_init_file(filename.c_str(), NULL, &decoder);

                ma_data_source_set_looping(&decoder, MA_TRUE);

                config = ma_device_config_init(ma_device_type_playback);
                
                config.playback.format = decoder.outputFormat;
                config.playback.channels = decoder.outputChannels;
                config.sampleRate = decoder.outputSampleRate;
                config.dataCallback = data_callback;
                config.pUserData = &decoder;

                ma_device_init(NULL, &config, &device);

                ma_device_start(&device);
                looping = true;
            }
        }

        void stopLoopSound()
        {
            if (looping)
            {
                ma_device_stop(&device);
            }
        }

    private:

        ma_engine engine;
        ma_decoder decoder;
        ma_device_config config;
        ma_device device;

        std::string loopingFile;
        bool looping = false;

    };

}
#endif /* SSOUND_H */
