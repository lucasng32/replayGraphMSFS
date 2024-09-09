let isPlaying = true;

const socket = new WebSocket('ws://localhost:1234');

socket.onopen = function () {
    console.log('WebSocket connection established');
};

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
                    if (!document.getElementById(type + 'Container')){
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
                                color: 'red',
                                width: 2,
                                dash: 'dashdot'
                            }
                        }]
                    });
                })
                updatePlanePosition(allData, dataMode, currentTimestamp);
                updateArc(allData, dataMode, currentTimestamp);
                //const slider = document.getElementById('timeSlider');
                //slider.value = (currentTime / totalDuration) * 100;
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
