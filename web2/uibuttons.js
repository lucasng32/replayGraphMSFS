function handleSliderInput() {
    const sliderValue = parseFloat(document.getElementById('timeSlider').value);
    const newTimestamp = sliderValue / 100 * totalDuration;
    ws.send(ClientEvent.ReplaySeek, { timestamp: newTimestamp });
}

function sendPlayPause() {
    const playPauseIcon = document.getElementById("playPauseIcon");
    playPauseIcon.classList.toggle('fa-play');
    playPauseIcon.classList.toggle('fa-pause');
    switch(state.connectState){
        case connectState.recordingPause:
            ws.send(ClientEvent.RecordingPlay, null);
            break;
        case connectState.recording:
            ws.send(ClientEvent.RecordingPause, null);
            break;
        case connectState.replay:
            ws.send(ClientEvent.ReplayPause, null);
            break;
        case connectState.replayPause:
            ws.send(ClientEvent.ReplayPlay, null);
            break;
    }
}

function handleButtonState(){
    console.log(state.connectState);
    const playPauseIcon = document.getElementById("playPauseIcon");
    const recordIcon = document.getElementById("recordIcon");
    switch(state.connectState){
        case connectState.recordingPause:
            if (playPauseIcon.classList.contains('fa-play')) {
                playPauseIcon.classList.toggle('fa-play');
                playPauseIcon.classList.toggle('fa-pause');
            }
            if (recordIcon.classList.contains('fa-circle')) {
                recordIcon.classList.toggle('fa-circle');
                recordIcon.classList.toggle('fa-square');
            }
            break;
        case connectState.recording:
            if (playPauseIcon.classList.contains('fa-pause')) {
                playPauseIcon.classList.toggle('fa-play');
                playPauseIcon.classList.toggle('fa-pause');
            }
            if (recordIcon.classList.contains('fa-circle')) {
                recordIcon.classList.toggle('fa-circle');
                recordIcon.classList.toggle('fa-square');
            }
            break;
        case connectState.replay:
            if (playPauseIcon.classList.contains('fa-pause')) {
                playPauseIcon.classList.toggle('fa-play');
                playPauseIcon.classList.toggle('fa-pause');
            }
            if (recordIcon.classList.contains('fa-circle')) {
                recordIcon.classList.toggle('fa-circle');
                recordIcon.classList.toggle('fa-square');
            }
            break;
        case connectState.replayPause:
            if (playPauseIcon.classList.contains('fa-play')) {
                playPauseIcon.classList.toggle('fa-play');
                playPauseIcon.classList.toggle('fa-pause');
            }
            if (recordIcon.classList.contains('fa-circle')) {
                recordIcon.classList.toggle('fa-circle');
                recordIcon.classList.toggle('fa-square');
            }
            break;
        case connectState.idle:
            if (recordIcon.classList.contains('fa-square')) {
                recordIcon.classList.toggle('fa-circle');
                recordIcon.classList.toggle('fa-square');
            }
            break;
    }
}

document.getElementById('playPauseButton').addEventListener('click', sendPlayPause);

document.getElementById('timeSlider').addEventListener('input', handleSliderInput);

function toggleFollowPlane() {
    followPlane = !followPlane;
    if (followPlane === true) {
        $("#followMode").text("Moving map enabled")
        $("#followModeButton").removeClass("btn-outline-danger").addClass("btn-primary")
    }
    if (followPlane === false) {
        $("#followMode").text("Moving map disabled")
        $("#followModeButton").removeClass("btn-primary").addClass("btn-outline-danger")
    }
}