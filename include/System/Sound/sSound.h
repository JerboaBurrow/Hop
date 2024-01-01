#ifndef SSOUND_H
#define SSOUND_H

#define MA_NO_VORBIS 
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>
#include <System/Sound/vorbis.h>

#include <jLog/jLog.h>
#include <sstream>

#include <Object/entityComponentSystem.h>

namespace Hop::Object
{
    class EntityComponentSystem;
}

namespace Hop::System::Sound
{

    void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
   
    std::string to_string(ma_result code);

    enum class DECODER {MA, VORBIS, NONE};

    class sSound : public System
    {
    
    public:

        sSound()
        : loopingFile("")
        {
            ma_result result = ma_engine_init(NULL, &engine);
            if (result != MA_SUCCESS)
            {
                throw std::runtime_error("Could not initialise sound system, "+to_string(result));
            }
        }

        ~sSound()
        {
            ma_engine_uninit(&engine);
            if (deviceInUse) { ma_device_uninit(&device); }
            if (decoderInUse != DECODER::NONE) { ma_decoder_uninit(&decoder); }
        }

        ma_result decode(std::string file)
        {
            ma_result result = ma_decoder_init_file(file.c_str(), NULL, &decoder);

            if (result != MA_SUCCESS)
            {
                vorbisConfig = ma_decoder_config_init_default();
                vorbisConfig.pCustomBackendUserData = NULL; 
                vorbisConfig.ppCustomBackendVTables = pCustomBackendVTables;
                vorbisConfig.customBackendCount     = sizeof(pCustomBackendVTables) / sizeof(pCustomBackendVTables[0]);

                result = ma_decoder_init_file(file.c_str(), &vorbisConfig, &vorbisDecoder);

                if (result == MA_SUCCESS)
                {
                    decoderInUse = DECODER::VORBIS;
                }
            }
            else
            {
                decoderInUse = DECODER::MA;
            }

            return result;
        }

        void setLoopSound(std::string filename)
        {
            if (loopingFile == "")
            {
                loopingFile = filename;
                
                ma_result result = decode(filename);

                if (result != MA_SUCCESS)
                {
                    jLog::Log log;
                    std::stringstream ss;
                    ss << "Could not decode audio file: " << filename << ", got error: " << to_string(result);
                    jLog::ERROR(ss.str()) >> log;
                }

                if (decoderInUse == DECODER::MA)
                {
                    ma_data_source_set_looping(&decoder, MA_TRUE);

                    config = ma_device_config_init(ma_device_type_playback);
                    
                    config.playback.format = decoder.outputFormat;
                    config.playback.channels = decoder.outputChannels;
                    config.sampleRate = decoder.outputSampleRate;
                    config.dataCallback = data_callback;
                    config.pUserData = &decoder;
                }
                else
                {
                    ma_data_source_set_looping(&vorbisDecoder, MA_TRUE);

                    config = ma_device_config_init(ma_device_type_playback);
                    
                    config.playback.format = vorbisDecoder.outputFormat;
                    config.playback.channels = vorbisDecoder.outputChannels;
                    config.sampleRate = vorbisDecoder.outputSampleRate;
                    config.dataCallback = data_callback;
                    config.pUserData = &vorbisDecoder;
                }

                ma_device_init(NULL, &config, &device);
                deviceInUse = true;

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

        ma_decoder vorbisDecoder;
        ma_decoder_config vorbisConfig;

        ma_device device;
        ma_device_config config;

        std::string loopingFile;
        bool looping = false;

        DECODER decoderInUse = DECODER::NONE;
        bool deviceInUse = false;

        ma_decoding_backend_vtable* pCustomBackendVTables[1] =
        {
            &g_ma_decoding_backend_vtable_libvorbis
        };


    };

}
#endif /* SSOUND_H */
