// Flight path coordinates (start with an empty array)
var flightPathCoordinates = [];

// Store the current polyline (null initially)
var flightPathPolyline = null;

// Function to update the plane's position and refresh the polyline
function updatePlanePath() {
    // If the polyline already exists, remove it from the map
    if (flightPathPolyline) {
        map.removeLayer(flightPathPolyline);
    }
    // Create a new polyline with the updated coordinates
    flightPathPolyline = L.polyline(flightPathCoordinates, { color: 'blue' }).addTo(map);
    // Optionally, adjust the map view to fit the new path
    map.fitBounds(flightPathPolyline.getBounds());
}

function updatePlanePosition(data, dataMode, curr_timestamp) {

    let dataSlice;

    if (dataMode){
        dataSlice = data[0];
    }
    else{
        for (var i = 0; i < data.length; i++){
            if (curr_timestamp < data[i].timestamp){
                dataSlice = data[i];
                break;
            }
        }
    }
    var pos = L.latLng(dataSlice.latitude, dataSlice.longitude);
    console.log(pos);
    planeMarker.setLatLng(pos);
    planeMarker.setRotationAngle(dataSlice.heading);

    if (followPlane === true) {
        map.panTo(pos);
    }
}

