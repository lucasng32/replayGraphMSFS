function lerp(start, end, t) {
    return start + t * (end - start);
}

function interpolateData(prevData, nextData, currTime) {

    if (!prevData || dataMode){
        return nextData;
    }
    const totalTime = nextData.timestamp - prevData.timestamp;
    const timeElapsed = currTime - prevData.timestamp;
    const t = timeElapsed / totalTime;
    let interpolatedData = {};

    Object.keys(nextData).forEach(key => {
        if (key in prevData) {
            interpolatedData[key] = lerp(prevData[key], nextData[key], t);
        }
        else{
            interpolatedData[key] = nextData[key];
        }
    })

    return interpolatedData;
}


function addTextArc(ctx, text, start, end, radiusArc, radiusText, centerX, centerY, color = 'black') {
    const midAngle = (start + end) / 2;

    const textx = (radiusText+radiusArc) * Math.cos(midAngle) + centerX;
    const texty = (radiusText+radiusArc) * Math.sin(midAngle) + centerY;

    // Draw the text
    ctx.fillStyle = color;
    ctx.fillText(text, textx, texty);
}

function drawLineArc(ctx, angle, radius, centerX, centerY, color = 'black') {
    ctx.beginPath();
    ctx.moveTo(centerX, centerY);
    ctx.lineTo(centerX + radius * Math.cos(angle), centerY + radius * Math.sin(angle));
    ctx.strokeStyle = color;
    ctx.lineWidth = 0.5;
    ctx.stroke();
}

// Function to draw and rotate the image
function drawImage(ctx, image, x, y, scale, rotation, offsetHeight = 0){
    ctx.save(); // Save the current state
    ctx.translate(x, y); // Move to the center\
    ctx.scale(scale,scale);
    ctx.rotate(rotation); // Rotate the canvas
    ctx.drawImage(image, -image.width / 2, -(image.height / 2) - offsetHeight); // Draw the image centered
    ctx.restore(); // Restore the previous state
} 