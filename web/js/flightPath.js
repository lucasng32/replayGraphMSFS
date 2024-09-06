// Initialize the map
var map = L.map('map').setView([51.505, -0.09], 13); // Set the initial view

// Load map tiles
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '&copy; OpenStreetMap contributors'
}).addTo(map);

// Flight path coordinates (start with an empty array)
var flightPathCoordinates = [];

// Store the current polyline (null initially)
var flightPathPolyline = null;

// Add a marker for the current position of the plane
var planeMarker = L.marker([51.505, -0.09]).addTo(map);

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

function updatePlanePosition() {
    planeMarker.setLatLng([lat, lng]);
}

