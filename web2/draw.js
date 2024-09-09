const canvas = document.getElementById('bankCanvas');
const ctx = canvas.getContext('2d');

const img = new Image();
img.src = 'plane.svg'; // Make sure the image path is correct

// Function to draw the arc based on the end angle in radians
function drawArc(endAngle, counterclockwise=false) {
    const centerX = canvas.width / 2;
    const centerY = canvas.height / 2;
    const radius = 100;
    const startAngle = 0;

    // Clear the previous drawing
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // Draw the arc
    ctx.beginPath();
    ctx.arc(centerX, centerY, radius, startAngle, endAngle, counterclockwise);
    ctx.strokeStyle = 'blue';
    ctx.lineWidth = 5;
    ctx.stroke();

    // Draw the plane image rotated
    drawImage(img, centerX, centerY, 0.5, endAngle);
}

// Function to draw and rotate the image
function drawImage(image, x, y, scale, rotation){
    ctx.save(); // Save the current state
    ctx.translate(x, y); // Move to the center\
    ctx.scale(scale,scale);
    ctx.rotate(rotation); // Rotate the canvas
    ctx.drawImage(image, -image.width / 2, -(image.height / 2) - 34); // Draw the image centered
    ctx.restore(); // Restore the previous state
} 

// Update the canvas and display the current radian value
function updateArc(data, dataMode, curr_timestamp) {
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

    degree = dataSlice.bank;
    const radians = parseFloat((degree/180)*Math.PI);

    if (radians < 0){
        drawArc(2*Math.PI + radians, true);
    }
    else {
        drawArc(radians);
    }
}

// Make sure the image is fully loaded before drawing
img.onload = function() {
    updateArc(0); // Initial render after the image loads
};