// Function to update the plane's position and refresh the polyline
function updatePlanePath() {
    // If the polyline already exists, remove it from the map
    if (state.flightPathPolyline) {
        map.removeLayer(state.flightPathPolyline);
    }
    // Create a new polyline with the updated coordinates
    state.flightPathPolyline = L.polyline(state.flightPathCoordinates, { color: 'blue' }).addTo(map);

}

function updateReplayPlanePosition() {

    let dataSlice;

    
    for (var i = 0; i < state.flightData.length; i++){
        if (state.currTime < state.flightData[i].timestamp){
            dataSlice = state.flightData[i];
            break;
        }
    }
    
    var pos = L.latLng(dataSlice.latitude, dataSlice.longitude);

    planeMarker.setLatLng(pos);
    planeMarker.setRotationAngle(dataSlice.heading);

    if (followPlane === true) {
        map.panTo(pos);
    }
}

