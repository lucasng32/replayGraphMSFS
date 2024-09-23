// use redux in the future
/*
namespace Client {
enum Event {
    LoadFlight,
    ReplaySeek,
    ReplayPlay,
    ReplayPause,
    ReplayStop,
    RecordingStart,
    RecordingPause,
    RecordingPlay,
    RecordingStop
};
}
namespace Server{
enum ServerEvent {
    Logbook,
    FlightLoaded,
    ReplayTimestampChanged,
    ReplayPlay,
    ReplayPause,
    PrevRecordingData,
    RecordingTimestampChanged,
    RecordingStart,
    RecordingPause,
    RecordingStop
};
}
*/

let graphs = {
    altitude: { x: [], y: [], id: 'altitudeGraph', limits: [], title: 'Altitude'},
    gForce: { x: [], y: [], id: 'gForceGraph', limits: {}, title: 'G-Force'},
    velocityWorldY: { x: [], y: [], id: 'rateOfClimbGraph', limits: [], title: 'Rate of Climb'},
    incidenceAlpha: { x: [], y: [], id: 'angleOfAttackGraph', limits: {'stallAlpha': 0}, title: 'Angle of Attack'},
    pitch: { x: [], y: [], id: 'pitchGraph', limits: {'staticPitch': 0}, title: 'Pitch'},
    trueAirspeed: { x: [], y: [], id: 'trueAirspeedGraph', limits: {'flapsUpStallSpeed': 0}, title: 'True Airspeed'},
    groundSpeed: { x: [], y: [], id: 'groundSpeedGraph', limits: {}, title: 'Ground Speed'},
};

const arr_cards = {
    'collapseAltitude': ['altitude', 'velocityWorldY'], 
    'collapseSpeed': ['trueAirspeed', 'groundSpeed'], 
    'collapsePitch': ['incidenceAlpha'], 
    'collapseRoll': [], 
    'collapseGForce': ['gForce'],
};

let state = {
    connectState: 0, // idle, recording, recording pause, replaying, replaying pause
    flightData: [],
    aircraftModelData: {},
    flightPathCoordinates: [],
    flightPath: null,
    graphs: graphs,
    followPlane: true,
    currentAccordion: 'collapseAltitude',
    currTime: null,
    logbook: [],
}

const connectState = {
    idle: 0,
    recording: 1,
    recordingPause: 2,
    replay: 3,
    replayPause: 4
}

const ClientEvent = {
    LoadFlight: 0, 
    ReplaySeek: 1,
    ReplayPlay: 2,
    ReplayPause: 3,
    ReplayStop: 4,
    RecordingStart: 5,
    RecordingPause: 6,
    RecordingPlay: 7,
    RecordingStop: 8,
}

const ServerEvent = {
    Logbook: 9,
    FlightLoaded: 10,
    ReplayTimestampChanged: 11,
    ReplayPlay: 12,
    ReplayPause: 13,
    PrevRecordingData: 14,
    RecordingTimestampChanged: 15,
    RecordingStart: 16,
    RecordingPause: 17,
    RecordingStop: 18,
}

ws.bind(ServerEvent['Logbook'], (data) => {
    // Populate table
    state.logbook = data.logbook;
    const tableBody = document.querySelector('#data-table tbody');
    state.logbook.forEach(item => {
        const row = document.createElement('tr');
        row.innerHTML = `
            <td>${item.flightId}</td>
            <td>${item.aircraftType}</td>
            <td>${item.creationDate}</td>
        `;
        row.setAttribute('data-id', item.flightId); // store item id in a custom data attribute
        tableBody.appendChild(row);
    });
})

ws.bind(ServerEvent['FlightLoaded'], (data) => {
    //update state
    state.connectState = connectState.replayPause;
    state.flightData = data.flightData;
    state.currTime = data.timestamp;
    state.aircraftModelData = data.aircraftModel;

    Object.keys(state.graphs).forEach(key => {
        // update graphs data
        state.graphs[key].x = state.flightData.map(item => item.timestamp / 1000)
        state.graphs[key].y = state.flightData.map(item => item[key]);
        // update graph limits
        Object.keys(state.graphs[key].limits).forEach(limitName => {
            // Check if the aircraftModel contains the limit property
            if (state.aircraftModelData.hasOwnProperty(limitName)) {
                state.graphs[key].limits[limitName] = state.aircraftModelData[limitName];
            }
        });
    });
    
    state.flightPathCoordinates = state.flightData.map(item => [item.latitude, item.longitude]);
    
    state.totalDuration = state.flightData[state.flightData.length - 1].timestamp / 1000;

    //draw flight path
    updatePlanePath();
    updateReplayPlanePosition();
    updateGraphs();
})

ws.bind(ServerEvent['ReplayTimestampChanged'], (data) => {
    state.currTime = data.timestamp;
    //redraw
    //update graph timestamp line
    updateReplayPlanePosition();
    updateGraphTime();
})

ws.bind(ServerEvent['ReplayPlay'], () => {
    state.connectState = connectState.replay;
    handleButtonState();
})

ws.bind(ServerEvent['ReplayPause'], () => {
    state.connectState = connectState.replayPause;
    handleButtonState();
})

ws.bind(ServerEvent['RecordingStart'], () => {
    state.connectState = connectState.recording;
    handleButtonState();
})

ws.bind(ServerEvent['RecordingPause'], () => {
    state.connectState = connectState.recordingPause;
    handleButtonState();
})

ws.bind(ServerEvent['RecordingStop'], () => {
    state.connectState = connectState.idle;
    handleButtonState();
})