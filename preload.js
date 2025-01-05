//set some fields when loaded
window.addEventListener('DOMContentLoaded', () => {
    const replaceText = (selector, text) => {
        const element = document.getElementById(selector)
        if (element) element.innerText = text
    }

    for (const dependency of ['chrome', 'node', 'electron']) {
        replaceText(`${dependency}-version`, process.versions[dependency])
    }
})

//send messages to main.js on button click
const { contextBridge, ipcRenderer } = require('electron')
contextBridge.exposeInMainWorld("electron", {
    send: (channel, payload) => ipcRenderer.send(channel, payload)
})

//receive message to update status
ipcRenderer.on('statusUpdate', (_event, value) => {
    const str = value.toString();
    document.getElementById("statusText").size = value.toString().length,
    document.getElementById("statusText").value = value
})