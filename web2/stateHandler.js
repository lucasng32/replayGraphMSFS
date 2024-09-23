// use redux in the future

let state = {
    connectState: null,
    flightData: [],
    flightPathCoordinates: [],
    flightPath: null,
    graphs: null,
    followPlane: false,
    currentAccordion: null,
    currTime: null,
    logbook: null,
}

const Events = {
    Logbook: 0,
    LoadFlight: 1,
    ReplayTimestampChanged: 2,
    ReplayStart: 3,
    ReplayStop: 4,
    ReplayPause: 5,
    RecordingStart: 6,
    RecordingTimestampChanged: 7,
    RecordingPause: 8,
    RecordingStop: 9,
}

/*
Received Events: 
Logbook -> update logbook
ReplayTimestampChanged -> update currTime
ReplayStart -> update flight data, currTime
ReplayStop -> update Flag
ReplayPause -> 
RecordingStart
RecordingTimestampChanged -> update flight data
RecordingPause -> update 
RecordingStop -> update 

State:
Connected
Disconnected
Replay
Recording
*/

ws.bind(Events['Logbook'], (data) => {

})

