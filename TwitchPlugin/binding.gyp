{
	"targets": 
	[
		{
		"target_name": "TwitchPlugin",
		"sources" : ["TwitchPlugin.cpp", "OBSTwitchWrapper.cpp"],
		"include_dirs": ["<!@(node -p \"require('node-addon-api').include\")", "<(module_root_dir)/libobs/"],	
		"link_settings": 
			{
			"libraries": ["-lobs.lib"],
			"library_dirs": ["<(module_root_dir)/libs"]
			},
		'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
		
		}
    ]
}