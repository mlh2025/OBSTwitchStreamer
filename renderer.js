//set button click handlers

//set key/init button
const setButton = document.getElementById("setBtn")
setButton.onclick = function () {    
    //disable so now we can only start and stop
    setButton.disabled = true,
    window.electron.send("setBtnClick", document.getElementById("streamKey").value)
}

//start button
const startButton = document.getElementById("startBtn")
startButton.onclick = function () {

    window.electron.send("startBtnClick", "")
}

//stop button
const stopButton = document.getElementById("stopBtn")
stopButton.onclick = function () {

    window.electron.send("stopBtnClick", "")
}


