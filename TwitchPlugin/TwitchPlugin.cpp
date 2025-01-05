//node_addon_api
#include <napi.h>
#include <iostream>

//obs studio
#include "OBSTwitchWrapper.h"

using namespace std;

//global OBS wrapper class
OBSTwitchWrapper gOBSTwitchWrapper;

static void SetWorkingDir(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    string curPath = info[0].As<Napi::String>().Utf8Value();

    //set the stream key first
    gOBSTwitchWrapper.SetModulePath(curPath);
}//end SetWorkingDir

static Napi::String InitializeOBS(const Napi::CallbackInfo& info) 
{
    Napi::Env env = info.Env();

    string streamKey = info[0].As<Napi::String>().Utf8Value();
    string curPath = info[1].As<Napi::String>().Utf8Value();

    //OBS initialization
    gOBSTwitchWrapper.Initialize(streamKey, curPath);

    return Napi::String::New(env, gOBSTwitchWrapper.GetStatus());
}//end InitializeOBS

static Napi::String StartStreaming(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    gOBSTwitchWrapper.Start();

    return Napi::String::New(env, gOBSTwitchWrapper.GetStatus());
}//end StartStreaming

static Napi::String StopStreaming(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    gOBSTwitchWrapper.Stop();

    return Napi::String::New(env, gOBSTwitchWrapper.GetStatus());
}//end StopStreaming


static Napi::String Shutdown(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    gOBSTwitchWrapper.Disconnect();

    return Napi::String::New(env, gOBSTwitchWrapper.GetStatus());
}//end Shutdown

static Napi::String GetStatus(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::string retString = gOBSTwitchWrapper.GetStatus();

    return Napi::String::New(env, retString);
}//end Shutdown



static Napi::Object Init(Napi::Env env, Napi::Object exports)
{

    exports.Set(Napi::String::New(env, "SetWorkingDir"),
        Napi::Function::New(env, SetWorkingDir));

    exports.Set(Napi::String::New(env, "InitializeOBS"),
        Napi::Function::New(env, InitializeOBS));

    exports.Set(Napi::String::New(env, "StartStreaming"),
        Napi::Function::New(env, StartStreaming));

    exports.Set(Napi::String::New(env, "StopStreaming"),
        Napi::Function::New(env, StopStreaming));

    exports.Set(Napi::String::New(env, "Shutdown"),
        Napi::Function::New(env, Shutdown));

    exports.Set(Napi::String::New(env, "GetStatus"),
        Napi::Function::New(env, GetStatus));


    return exports;
}//end Init

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
