let isPlaying = true;

function handlePlayPause() {
    isPlaying = !isPlaying;
    document.getElementById('playPauseButton').textContent = isPlaying ? 'Pause' : 'Play';
    const command = isPlaying ? 'play' : 'pause';
    socket.send(JSON.stringify({ command }));
}

function handleSliderInput() {
    const sliderValue = parseFloat(document.getElementById('timeSlider').value);
    const newTimestamp = sliderValue / 100 * totalDuration;
    socket.send(JSON.stringify({ command: 'seek', timestamp: newTimestamp }));
}

document.getElementById('playPauseButton').addEventListener('click', handlePlayPause);

document.getElementById('timeSlider').addEventListener('input', handleSliderInput);

document.getElementById('settingsButton').addEventListener('click', function () {
    const toolbar = document.getElementById('toolbar');
    toolbar.classList.toggle('hidden');
    const settingsButton = document.getElementById('settingsButton');
    settingsButton.classList.toggle('move');
    const playPauseButton = document.getElementById('playPauseButton');
    playPauseButton.classList.toggle('move');
});

const socket = new WebSocket('ws://localhost:1234');

socket.onopen = function () {
    console.log('WebSocket connection established');
};

socket.onmessage = function (event) {
    const parsedData = JSON.parse(event.data);
    const allData = parsedData.data;
    const aircraftModel = parsedData.aircraftModel;
    const currentTimestamp = parsedData.timestamp;
    const dataMode = parsedData.dataMode;
    const toReload = parsedData.toReload;

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
                document.querySelectorAll('.graph-container').forEach(container => {
                    Plotly.relayout(container.querySelector('div').id, {
                        shapes: [{
                            type: 'line',
                            x0: currentTime,
                            y0: 0,
                            x1: currentTime,
                            y1: 1,
                            xref: 'x',
                            yref: 'paper',
                            line: {
                                color: 'red',
                                width: 2,
                                dash: 'dashdot'
                            }
                        }]
                    });
                });
                const slider = document.getElementById('timeSlider');
                slider.value = (currentTime / totalDuration) * 100;
            }
        } catch (e) {
            console.error('Error updating graphs during replay:', e);
        }
    }

    if (shouldReRender || toReload) {
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

        if (!dataMode){
            Object.keys(graphs).forEach(key => {
                graphs[key].x = allData.map(item => item.timestamp / 1000)
                graphs[key].y = allData.map(item => item[key]);
            });
            flightPathCoordinates = allData.map(item => [item.latitude, item.longitude]);
        }
        
        totalDuration = allData[allData.length - 1].timestamp / 1000;
        renderGraphs();
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
