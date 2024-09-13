const bankCanvas = document.getElementById('bankCanvas');
const pitchCanvas = document.getElementById('pitchCanvas');

const imgPlane = new Image();
imgPlane.src = 'plane.svg';

const imgArrow = new Image();
imgArrow.src = 'arrow.png';

const imgPlaneSide = new Image();
imgPlaneSide.src = 'planeSide.svg';

let prevData = null;
let nextData = null;
let dataMode = null;
let currTime = null;

function animate() {
    if (!nextData) {
        requestAnimationFrame(animate);
        return; // Wait until we have data
    }

    // Calculate interpolated data based on time
    const interpolatedData = interpolateData(prevData, nextData, currTime);
    // Update the canvas with the interpolated data
    updateBankArc(interpolatedData);
    updatePitchAOA(interpolatedData);

    // Advance to the next frame
    requestAnimationFrame(animate);
}

// Function to draw the arc based on the end angle in radians
function drawBankArc(canvas, endAngle, counterclockwise=false) {
    const ctx = canvas.getContext('2d');
    ctx.font = '20px Arial';
    const centerX = canvas.width / 2;
    const centerY = canvas.height / 2;
    const radius = 150;
    const startAngle = 0;
    const radiusText = 30;

    // Clear the previous drawing
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // Draw the arc
    ctx.beginPath();
    ctx.arc(centerX, centerY, radius, startAngle, endAngle, counterclockwise);
    ctx.strokeStyle = 'blue';
    ctx.lineWidth = 5;
    ctx.stroke();

    // Draw the plane image rotated
    drawImage(ctx, imgPlane, centerX, centerY, 0.7, endAngle, 34);

    addTextArc(ctx, `${Math.round(endAngle / Math.PI * 180)}°`, startAngle, endAngle, radius, radiusText, centerX, centerY);

    drawLineArc(ctx, 0, radius+20, centerX, centerY);
    
}

function drawVelocityVectors(canvas, dataSlice){
    const ctx = canvas.getContext('2d');
    ctx.font = '20px Arial';
    const centerX = canvas.width / 2;
    const centerY = canvas.height / 2;
    const velocityWorldY = dataSlice.velocityWorldY;
    const velocityWorldX = dataSlice.velocityWorldX;
    const velocityWorldZ = dataSlice.velocityWorldZ;

    const headingRadians = dataSlice.heading * Math.PI / 180;

    const VxLocal = velocityWorldX * Math.cos(headingRadians) - velocityWorldZ * Math.sin(headingRadians);
    if (Math.abs(VxLocal) > 0.1){
        if (VxLocal > 0){
            drawImage(ctx, imgArrow, centerX+130, centerY, 0.05, 0);
            ctx.fillText(`${Math.round(Math.abs(VxLocal))} ft/s`, centerX+130, centerY+20);
        }
        else{
            drawImage(ctx, imgArrow, centerX-130, centerY, 0.05, Math.PI);
            ctx.fillText(`${Math.round(Math.abs(VxLocal))} ft/s`, centerX-130, centerY+20);
        }
    }

    if (Math.abs(velocityWorldY) > 0.1){
        if (velocityWorldY < 0){
            drawImage(ctx, imgArrow, centerX, centerY+130, 0.05, Math.PI/2);
            ctx.fillText(`${Math.round(Math.abs(velocityWorldY))} ft/s`, centerX+10, centerY+130);
        }
        else{
            drawImage(ctx, imgArrow, centerX, centerY-130, 0.05, Math.PI*3/2);
            ctx.fillText(`${Math.round(Math.abs(velocityWorldY))} ft/s`, centerX+10, centerY-130);
        }
    }

}

// Update the canvas and display the current radian value
function updateBankArc(dataSlice) {
    degree = dataSlice.bank;
    const radians = parseFloat((degree/180)*Math.PI);

    if (radians < 0){
        drawBankArc(bankCanvas, radians, true);
    }
    else {
        drawBankArc(bankCanvas, radians);
    }

    drawVelocityVectors(bankCanvas, dataSlice);
}

function updateDrawData(data, dataMode, currSimTimestamp){
    dataMode = dataMode;
    currTime = currSimTimestamp;
    if (dataMode){
        nextData = data[0];
    }
    else{
        for (var i = 0; i < data.length; i++){
            if (currSimTimestamp < data[i].timestamp){
                prevData = data[i-1];
                nextData = data[i];
                break;
            }
        }
    }
}


function drawPitchAOA(canvas, aoa, pitch){

    const ctx = canvas.getContext('2d');
    ctx.font = '20px Arial';
    const centerX = canvas.width / 2;
    const centerY = canvas.height / 2;
    const radius = 150;
    const radiusText = 60;

    ctx.clearRect(0, 0, canvas.width, canvas.height);

    let pitchCounterclockwise = true;

    if (pitch < 0){
        pitchCounterclockwise = false;
    }

    //draw pitch arc
    ctx.beginPath();
    ctx.arc(centerX, centerY, radius, Math.PI, -pitch+Math.PI, pitchCounterclockwise);
    ctx.strokeStyle = 'blue';
    ctx.lineWidth = 5;
    ctx.stroke();

    let aoaCounterclockwise = true;

    if (aoa < 0){
        aoaCounterclockwise = false
    }           
    //aoa arc
    ctx.beginPath();
    ctx.arc(centerX, centerY, radius + 10, Math.PI-pitch, -pitch+Math.PI-aoa, aoaCounterclockwise);
    ctx.strokeStyle = 'red';
    ctx.lineWidth = 5;
    ctx.stroke();

    // Draw the plane image rotated
    drawImage(ctx, imgPlaneSide, centerX, centerY, 0.15, -pitch, 100);

    addTextArc(ctx, `${Math.round(aoa / Math.PI * 180)}°`, Math.PI-pitch, -pitch+Math.PI-aoa, radius, radiusText + 10, centerX, centerY, 'red');
    addTextArc(ctx, `${Math.round(-pitch / Math.PI * 180)}°`, Math.PI, -pitch+Math.PI, radius, radiusText, centerX, centerY, 'blue');

    drawLineArc(ctx, Math.PI, radius+20, centerX, centerY);
    drawLineArc(ctx, Math.PI-pitch, radius+20, centerX, centerY);
    drawLineArc(ctx, -pitch+Math.PI-aoa, radius+20, centerX, centerY);

}

function updatePitchAOA(dataSlice) {
    degreePitch = dataSlice.pitch;
    const radiansPitch = parseFloat((degreePitch/180)*Math.PI);
    drawPitchAOA(pitchCanvas, dataSlice.incidenceAlpha, radiansPitch);
}


// Make sure the image is fully loaded before drawing
imgPlane.onload = function() {
    drawBankArc(bankCanvas, 1);
    drawPitchAOA(pitchCanvas, 0.5, 1);
};

requestAnimationFrame(animate);