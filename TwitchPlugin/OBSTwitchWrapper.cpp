//class def
#include "OBSTwitchWrapper.h"
#include <iostream>

using namespace std;

OBSTwitchWrapper::OBSTwitchWrapper()
{
    mInitialized = false;
    mStreaming = false;
    mStatusOutput = "Waiting...";
    mStreamKey = "";

    mOBSService = NULL;
    mOBSServiceData = NULL;

    mOBSVideoEncoder = NULL;
    mOBSAudioEncoder = NULL;

    mOBSOutputData = NULL;
    mOBSOutput = NULL;

}//end constructor

bool OBSTwitchWrapper::Initialize(string pKey, string pPath)
{
    //variables
    bool success = true;
    string path = "";

    //initialize status
    mStatusOutput = "";

    //if already initialized, shut everything down and start over
    if (mInitialized)
    {
        Disconnect();
    }

    //first set stream key
    mStreamKey = pKey;

    if (!pPath.empty())
    {
        SetModulePath(pPath);
    }

    //OBS initialization
    if (obs_startup(mCurLocale.c_str(), NULL, NULL))
    { 
        //first load the modules for audio and video 
        if (LoadOBSModules())
        {//if succesfully loaded continue initialization
            if (SetAV())
            {//continue if set audio and video
                if (!CreateEncoders())
                {
                    success = false;
                }
                if (!CreateOutput())
                {
                    success = false;
                }
                if (!CreateService())
                {
                    success = false;
                }

                if (success)
                {//set all objects appropriately
                    obs_encoder_set_video(mOBSVideoEncoder, obs_get_video());
                    obs_encoder_set_audio(mOBSAudioEncoder, obs_get_audio());
                    obs_output_set_video_encoder(mOBSOutput, mOBSVideoEncoder);
                    obs_output_set_audio_encoder(mOBSOutput, mOBSAudioEncoder, 0);
                    obs_output_set_service(mOBSOutput, mOBSService);

                    //set status
                    mStatusOutput = "Successful initialization; ready to stream.";
                    mInitialized = true;
                }
                else
                {//some error occurred
                    mStatusOutput += "Error initializing OBS.";
                }
            }//end if set audio/video
        }//end if loaded modules
    }//end if obs startup

    return success;
}//end Initialize


//set up audio and video contexts
bool OBSTwitchWrapper::SetAV()
{
    bool success = true;
    obs_video_info ovi;
    obs_audio_info oai;

    //set the audio and video contexts
    ovi.graphics_module = "libobs-opengl";
    ovi.fps_num = 30;
    ovi.fps_den = 1;
    ovi.base_width = 640;
    ovi.base_height = 360;
    ovi.output_width = 640;
    ovi.output_height = 360;
    ovi.output_format = VIDEO_FORMAT_I420;
    ovi.gpu_conversion = true;
    ovi.colorspace = VIDEO_CS_DEFAULT;
    ovi.range = VIDEO_RANGE_DEFAULT;
    ovi.scale_type = OBS_SCALE_BILINEAR;

    oai.samples_per_sec = 32000;
    oai.speakers = SPEAKERS_STEREO;

    if ((obs_reset_video(&ovi) != OBS_VIDEO_SUCCESS) || !obs_reset_audio(&oai))
    {
        success = false;
        mStatusOutput = "Failed to initialize audio and video contexts.\n";
    }

    return success;
}//end SetAV

bool OBSTwitchWrapper::LoadOBSModules()
{
    bool success = true;
    int retVal = 0;
    obs_module_t* openingModule;
    string modPath = "";    

    //data path for effects  
    modPath = mModulePath + "data/libobs/";
    obs_add_data_path(modPath.c_str());

    //open and initialize obs-outputs
    modPath = mModulePath + "obs-outputs.dll";
    retVal = obs_open_module(&openingModule, modPath.c_str(), "");
    if (retVal != MODULE_SUCCESS)
    {
        success = false;
        mStatusOutput +=  "Failed to open obs-outputs.dll.\n";
    }
    else
    {
        obs_init_module(openingModule);
    }

    //open and initialize obs-ffmpeg
    modPath = mModulePath + "obs-ffmpeg.dll";
    retVal = obs_open_module(&openingModule, modPath.c_str(), "");
    if (retVal != MODULE_SUCCESS)
    {
        success = false;
        mStatusOutput += "Failed to open obs-ffmpeg.dll.\n";
    }
    else
    {
        obs_init_module(openingModule);
    }

    //open and initialize obs-x264
    modPath = mModulePath + "obs-x264.dll";
    retVal = obs_open_module(&openingModule, modPath.c_str(), "");
    if (retVal != MODULE_SUCCESS)
    {
        success = false;
        mStatusOutput += "Failed to open obs-x264.dll.\n";
    }
    else
    {
        obs_init_module(openingModule);
    }

    //open and initialize rtmp-services
    modPath = mModulePath + "rtmp-services.dll";
    retVal = obs_open_module(&openingModule, modPath.c_str(), "");
    if (retVal != MODULE_SUCCESS)
    {
        success = false;
        mStatusOutput += "Failed to open rtmp-services.dll.\n";
    }
    else
    {
        obs_init_module(openingModule);
    }

    //just for info
    obs_log_loaded_modules();

    return success;
}//end LoadOBSModules

void OBSTwitchWrapper::Disconnect()
{
    if (mInitialized)
    {
        if (mStreaming)
        {//stop streaming first
            Stop();
        }

        //stop and release everything
        if (mOBSAudioEncoder != NULL)
        {
            obs_encoder_release(mOBSAudioEncoder);
            mOBSAudioEncoder = NULL;
        }
        if (mOBSVideoEncoder != NULL)
        {
            obs_encoder_release(mOBSVideoEncoder);
            mOBSVideoEncoder = NULL;
        }
        if (mOBSService != NULL)
        {
            obs_service_release(mOBSService);
            obs_data_release(mOBSServiceData);
            mOBSService = NULL;
        }
        if (mOBSOutput != NULL)
        {
            obs_output_release(mOBSOutput);
            obs_data_release(mOBSOutputData);
            mOBSOutput = NULL;
        }
    }//end if initialized

    //shutdown 
    obs_shutdown();

    //set status
    mStatusOutput = "OBS has been shut down.";
    mInitialized = false;
}//end Disconnect

//set the module path to working dir + bin
void OBSTwitchWrapper::SetModulePath(string pPath)
{
    mModulePath = pPath;
    if (mModulePath.find_last_of('/') != mModulePath.length())
    {
        mModulePath += "/";
    }
    mModulePath += "bin/";
}//end SetModulePath


//get status
string OBSTwitchWrapper::GetStatus()
{
    return mStatusOutput;
}//end GetStatus

//creates the service to connect to Twitch
bool OBSTwitchWrapper::CreateService()
{
    mOBSServiceData = obs_data_create();
    
    if (mOBSServiceData != NULL)
    {
        obs_data_set_string(mOBSServiceData, "service", mService.c_str());//QT_TO_UTF8(ui->service->currentText()));
        obs_data_set_string(mOBSServiceData, "server", mServer.c_str());//QT_TO_UTF8(ui->server->currentData().toString()));
        obs_data_set_string(mOBSServiceData, "key", mStreamKey.c_str());//QT_TO_UTF8(ui->key->text()));
    }

    mOBSService = obs_service_create("rtmp_common", "My Twitch Service", mOBSServiceData, nullptr);

    return (mOBSService != NULL);
}//end CreateService


//RMTP output -> will receive encoders and Twitch service
bool OBSTwitchWrapper::CreateOutput()
{   
    mOBSOutputData = obs_data_create();

    if (mOBSOutputData != NULL)
    {
        obs_data_set_int(mOBSOutputData, "output_flags", OBS_OUTPUT_AV | OBS_OUTPUT_ENCODED | OBS_OUTPUT_SERVICE | OBS_OUTPUT_MULTI_TRACK_AV);
        obs_data_set_string(mOBSOutputData, "bind_ip", "default");
        obs_data_set_string(mOBSOutputData, "ip_family", "IPv4+IPv6");

        mOBSOutput = obs_output_create("rtmp_output", "RTMP Output to Twitch", mOBSOutputData, nullptr);
    }

    return (mOBSOutput != NULL);
}//end CreateOutput

bool OBSTwitchWrapper::CreateEncoders()
{
    mOBSVideoEncoder = obs_video_encoder_create("obs_x264", "x264 Encoder", NULL, NULL);
    mOBSAudioEncoder = obs_audio_encoder_create("ffmpeg_aac", "AAC Encoder", NULL, 0, NULL);

    return !((mOBSAudioEncoder == NULL) || (mOBSVideoEncoder == NULL));
}//end CreateEncoders

bool OBSTwitchWrapper::Start()
{
    if (mInitialized)
    {
        //everything is set up - start streaming
        mStreaming = obs_output_start(mOBSOutput);

        if (!mStreaming)
        {
            const char* e = obs_output_get_last_error(mOBSOutput);
            if (e == NULL)
            {
                e = "Unknown error starting streaming.";
            }
            mStatusOutput += e;
            Disconnect();
        }
        else
        {
            //set status
            mStatusOutput = "Streaming started successfully.";
        }
    }
    else
    {
        mStatusOutput = "OBS not initialized - cannot start streaming.";
    }

    return mStreaming;
}//end Start

void OBSTwitchWrapper::Stop()
{
    if (mStreaming)
    {//don't stop if not started successfully
        obs_output_stop(mOBSOutput);
        mStatusOutput = "Streaming stopped.";
    }
    else
    {
        mStatusOutput = "Streaming is not currently active.";
    }

    //set status
    mStreaming = false;
}//end Stop
