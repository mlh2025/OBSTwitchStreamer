const { app, BrowserWindow, ipcMain } = require('electron')
const path = require('node:path')

// OBS module
const twitchPlugin = require('./bin/TwitchPlugin');
module.exports = twitchPlugin;

//set the current directory in the plugin
twitchPlugin.SetWorkingDir(__dirname);

let win;
const createWindow = () => {
    const win = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            contextIsolation: true,
            nodeIntegration: true,
            preload: path.join(__dirname, 'preload.js')
        }
    })

    win.openDevTools()

    win.loadFile('index.html')

    //"Initialize with Stream Key" button click handler
    ipcMain.on("setBtnClick", (event, data) => {
        const updateStr = twitchPlugin.InitializeOBS(data, "")
        console.log(updateStr)
        win.webContents.send('statusUpdate', updateStr)
    })

    //"Start Streaming" button handler
    ipcMain.on("startBtnClick", (event, data) => {
        const updateStr = twitchPlugin.StartStreaming()
        console.log(updateStr)
        win.webContents.send('statusUpdate', updateStr)
    })

    //"Stop Streaming" button handler
    ipcMain.on("stopBtnClick", (event, data) => {
        const updateStr = twitchPlugin.StopStreaming()
        console.log(updateStr)
        win.webContents.send('statusUpdate', updateStr)
    })

}

app.whenReady().then(() => {
    createWindow()
})

app.on('window-all-closed', () => {
    console.log(twitchPlugin.Shutdown())
    if (process.platform !== 'darwin') app.quit()
})

