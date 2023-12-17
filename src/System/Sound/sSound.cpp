#include <System/Sound/sSound.h>

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

    /*
        This function was generated from julia by copy pasting the enum declaration
        from miniaudio.h into a file "error"
        ````Julia
            e = readlines("errors")
            e = [replace.(replace.(reverse(split(l,"=")),","=>"")," "=>"") for l in e]

            # 71-element Vector{Vector{String}}:
            # ["0", "MA_SUCCESS"]
            # ["-1", "MA_ERROR"]
            # ["-2", "MA_INVALID_ARGS"]
            # ...

            code = "std::string maErrorToString(ma_result code)\n{\n    switch (code)\n    {\n"
            for l in e
                global code*="        case $(l[1]):\n            return \"$(l[2])\";\n"
            end
            code *= "        default:\n            return \"UNKNOWN\";\n    }\n}"
            write("switch.cpp", code)
        ```
        

    */
    std::string maErrorToString(ma_result code)
    {
        switch (code)
        {
            case 0:
                return "MA_SUCCESS";
            case -1:
                return "MA_ERROR";
            case -2:
                return "MA_INVALID_ARGS";
            case -3:
                return "MA_INVALID_OPERATION";
            case -4:
                return "MA_OUT_OF_MEMORY";
            case -5:
                return "MA_OUT_OF_RANGE";
            case -6:
                return "MA_ACCESS_DENIED";
            case -7:
                return "MA_DOES_NOT_EXIST";
            case -8:
                return "MA_ALREADY_EXISTS";
            case -9:
                return "MA_TOO_MANY_OPEN_FILES";
            case -10:
                return "MA_INVALID_FILE";
            case -11:
                return "MA_TOO_BIG";
            case -12:
                return "MA_PATH_TOO_LONG";
            case -13:
                return "MA_NAME_TOO_LONG";
            case -14:
                return "MA_NOT_DIRECTORY";
            case -15:
                return "MA_IS_DIRECTORY";
            case -16:
                return "MA_DIRECTORY_NOT_EMPTY";
            case -17:
                return "MA_AT_END";
            case -18:
                return "MA_NO_SPACE";
            case -19:
                return "MA_BUSY";
            case -20:
                return "MA_IO_ERROR";
            case -21:
                return "MA_INTERRUPT";
            case -22:
                return "MA_UNAVAILABLE";
            case -23:
                return "MA_ALREADY_IN_USE";
            case -24:
                return "MA_BAD_ADDRESS";
            case -25:
                return "MA_BAD_SEEK";
            case -26:
                return "MA_BAD_PIPE";
            case -27:
                return "MA_DEADLOCK";
            case -28:
                return "MA_TOO_MANY_LINKS";
            case -29:
                return "MA_NOT_IMPLEMENTED";
            case -30:
                return "MA_NO_MESSAGE";
            case -31:
                return "MA_BAD_MESSAGE";
            case -32:
                return "MA_NO_DATA_AVAILABLE";
            case -33:
                return "MA_INVALID_DATA";
            case -34:
                return "MA_TIMEOUT";
            case -35:
                return "MA_NO_NETWORK";
            case -36:
                return "MA_NOT_UNIQUE";
            case -37:
                return "MA_NOT_SOCKET";
            case -38:
                return "MA_NO_ADDRESS";
            case -39:
                return "MA_BAD_PROTOCOL";
            case -40:
                return "MA_PROTOCOL_UNAVAILABLE";
            case -41:
                return "MA_PROTOCOL_NOT_SUPPORTED";
            case -42:
                return "MA_PROTOCOL_FAMILY_NOT_SUPPORTED";
            case -43:
                return "MA_ADDRESS_FAMILY_NOT_SUPPORTED";
            case -44:
                return "MA_SOCKET_NOT_SUPPORTED";
            case -45:
                return "MA_CONNECTION_RESET";
            case -46:
                return "MA_ALREADY_CONNECTED";
            case -47:
                return "MA_NOT_CONNECTED";
            case -48:
                return "MA_CONNECTION_REFUSED";
            case -49:
                return "MA_NO_HOST";
            case -50:
                return "MA_IN_PROGRESS";
            case -51:
                return "MA_CANCELLED";
            case -52:
                return "MA_MEMORY_ALREADY_MAPPED";
            case -100:
                return "MA_CRC_MISMATCH";
            case -200:
                return "MA_FORMAT_NOT_SUPPORTED";
            case -201:
                return "MA_DEVICE_TYPE_NOT_SUPPORTED";
            case -202:
                return "MA_SHARE_MODE_NOT_SUPPORTED";
            case -203:
                return "MA_NO_BACKEND"; 
            case -204:
                return "MA_NO_DEVICE";
            case -205:
                return "MA_API_NOT_FOUND";
            case -206:
                return "MA_INVALID_DEVICE_CONFIG";
            case -207:
                return "MA_LOOP";
            case -208:
                return "MA_BACKEND_NOT_ENABLED";
            case -300:
                return "MA_DEVICE_NOT_INITIALIZED";
            case -301:
                return "MA_DEVICE_ALREADY_INITIALIZED";
            case -302:
                return "MA_DEVICE_NOT_STARTED";
            case -303:
                return "MA_DEVICE_NOT_STOPPED";
            case -400:
                return "MA_FAILED_TO_INIT_BACKEND";
            case -401:
                return "MA_FAILED_TO_OPEN_BACKEND_DEVICE";
            case -402:
                return "MA_FAILED_TO_START_BACKEND_DEVICE";
            case -403:
                return "MA_FAILED_TO_STOP_BACKEND_DEVICE";
            default:
                return "UNKNOWN";
        }
    }
}