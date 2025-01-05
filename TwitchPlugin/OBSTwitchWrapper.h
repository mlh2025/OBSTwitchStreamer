#pragma once

//strings
#include <string>

//obs studio
#include "obs.h"

using namespace::std;

class OBSTwitchWrapper 
{
public:

	//constructor
	OBSTwitchWrapper(); 

	//hack to not hard code the path here
	void SetModulePath(string pPath);

	//do all setup
	bool Initialize(string pKey, string pPath);

	//start streaming
	bool Start();

	//stop streaming
	void Stop();

	//shut down 
	void Disconnect();

	//get the status
	string GetStatus();

private:

	const string mCurLocale = "en - US";
	const string mServer = "rtmp://live.twitch.tv/app";
	const string mService = "Twitch";
	bool mAVSet;
	bool mStreaming;
	bool mInitialized;
	string mStreamKey;
	string mModulePath;
	string mStatusOutput;

	obs_data_t* mOBSServiceData;
	obs_service_t *mOBSService;

	obs_encoder_t *mOBSVideoEncoder;
	obs_encoder_t* mOBSAudioEncoder;

	obs_data_t *mOBSOutputData;
	obs_output_t* mOBSOutput;

	//create all needed elements
	bool SetAV();
	bool LoadOBSModules();
	bool CreateService();
	bool CreateOutput();
	bool CreateEncoders();
};
