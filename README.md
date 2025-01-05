TwitchPlugin: Node.js native add on module to load OBS and stream to Twitch. From OBSTwitchStreamer/TwitchPlugin folder:
		
	  npm install node-addon-api
			
	  node-gyp configure
			
	  node-gyp build

OBSTwitchStreamer: loads TwitchPlugin; allows user to input a stream key, initialize, start, and stop streaming. 
The 'prepare' script copies the node module from its build directory into 'bin' for the Electron app to load it. 
From OBSTwitchStreamer/TwitchPlugin folder:
		
	  npm install electron
   
	  npm run prepare
   
	  npm start
