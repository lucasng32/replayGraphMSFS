let isPlaying = true;

const socket = new WebSocket('ws://localhost:1234');

socket.onopen = function () {
    console.log('WebSocket connection established');
};

function handleSliderInput() {
    const sliderValue = parseFloat(document.getElementById('timeSlider').value);
    const newTimestamp = sliderValue / 100 * totalDuration;
    socket.send(JSON.stringify({ command: 'seek', timestamp: newTimestamp }));
}

function handlePlayPause() {
    isPlaying = !isPlaying;
    const playPauseIcon = document.getElementById("playPauseIcon");
    playPauseIcon.classList.toggle('fa-play');
    playPauseIcon.classList.toggle('fa-pause');
    const command = isPlaying ? 'play' : 'pause';
    socket.send(JSON.stringify({ command }));
}

document.getElementById('playPauseButton').addEventListener('click', handlePlayPause);

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

socket.onmessage = function (event) {
    const parsedData = JSON.parse(event.data);
    const allData = parsedData.data;
    const aircraftModel = parsedData.aircraftModel;
    const currentTimestamp = parsedData.timestamp;
    const dataMode = parsedData.dataMode;
    const toReload = parsedData.toReload;

    updatePlanePosition(allData, dataMode, currentTimestamp);
    updateDrawData(allData, dataMode, currentTimestamp);

    if (dataMode) { //live
        allData.forEach(item => {
            const timestamp = item.timestamp / 1000;
            Object.keys(graphs).forEach(key => {
                graphs[key].x.push(timestamp);
                graphs[key].y.push(item[key]);
            })
            flightPathCoordinates.push([item.latitude, item.longitude]);
        });
    } else { //replay
        try {
            const currentTime = currentTimestamp / 1000;
            if (currentTime > 0.1) {
                Object.keys(graphs).forEach(type => {
                    if (!document.getElementById(graphs[type].id)) {
                        return;
                    }
                    Plotly.relayout(graphs[type].id, {
                        shapes: [{
                            type: 'line',
                            x0: currentTime,
                            y0: 0,
                            x1: currentTime,
                            y1: 1,
                            xref: 'x',
                            yref: 'paper',
                            line: {
                                color: 'blue',
                                width: 2,
                                dash: 'dashdot'
                            }
                        }]
                    });
                })
                const slider = document.getElementById('timeSlider');
                slider.value = (currentTime / totalDuration) * 100;
            }
        } catch (e) {
            console.error('Error updating graphs during replay:', e);
        }
    }

    if (shouldReRender || toReload) {
        //update limits only if reloading
        Object.keys(graphs).forEach(key => {
            const graph = graphs[key];
            const limits = graph.limits;

            Object.keys(limits).forEach(limitName => {
                // Check if the aircraftModel contains the limit property
                if (aircraftModel.hasOwnProperty(limitName)) {
                    limits[limitName] = aircraftModel[limitName];
                }
            });
        });
        
        //fully update graphs only if reloading and replay mode
        if (!dataMode){
            Object.keys(graphs).forEach(key => {
                graphs[key].x = allData.map(item => item.timestamp / 1000)
                graphs[key].y = allData.map(item => item[key]);
            });
            flightPathCoordinates = allData.map(item => [item.latitude, item.longitude]);
        }
        
        totalDuration = allData[allData.length - 1].timestamp / 1000;
        updateGraphs();
        updatePlanePath();
        shouldReRender = false;
    }

    socket.onclose = function () {
        console.log('WebSocket connection closed');
    };

    socket.onerror = function (error) {
        console.error('WebSocket error:', error);
    };
};
