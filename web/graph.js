const graphs = {
    altitude: { x: [], y: [], id: 'altitudeGraph', limits: [], title: 'Altitude'},
    gForce: { x: [], y: [], id: 'gForceGraph', limits: {}, title: 'G-Force'},
    velocityWorldY: { x: [], y: [], id: 'rateOfClimbGraph', limits: [], title: 'Rate of Climb'},
    incidenceAlpha: { x: [], y: [], id: 'angleOfAttackGraph', limits: {'stallAlpha': 0}, title: 'Angle of Attack'},
    pitch: { x: [], y: [], id: 'pitchGraph', limits: {'staticPitch': 0}, title: 'Pitch'},
    indicatedAirspeed: { x: [], y: [], id: 'indicatedAirspeedGraph', limits: {'flapsUpStallSpeed': 0}, title: 'Indicated Airspeed'},
    trueAirspeed: { x: [], y: [], id: 'trueAirspeedGraph', limits: {'flapsUpStallSpeed': 0}, title: 'True Airspeed'},
    groundSpeed: { x: [], y: [], id: 'groundSpeedGraph', limits: {}, title: 'Ground Speed'},
    windSpeed: { x: [], y: [], id: 'windSpeedGraph', limits: {}, title: 'Wind Speed'},
    aileronPosition: { x: [], y: [], id: 'aileronPositionGraph', limits: {}, title: 'Aileron Position'},
    bank: { x: [], y: [], id: 'bankGraph', limits: {}, title: 'Bank'},
};

arr_cards = {
    'collapseAltitude': ['altitude', 'velocityWorldY'], 
    'collapseSpeed': ['trueAirspeed', 'groundSpeed'], 
    'collapsePitch': ['incidenceAlpha'], 
    'collapseRoll': [], 
    'collapseGForce': ['gForce'],
};

let shouldReRender = true;
let totalDuration = 100;

function renderGraphs(card) {
    arr_cards[card].forEach(type => {
        const container = document.getElementById(type + 'Container');

        if (!container) {
            return;
        }
        container.innerHTML = ''; // Clear existing graphs

        const graphDiv = document.createElement('div');
        graphDiv.id = graphs[type].id;
        container.appendChild(graphDiv);

        const layout = {
            title: {text: graphs[type].title, font: {size: 30}},
            xaxis: { title: 'Time (s)' },
            yaxis: { title: type.charAt(0).toUpperCase() + type.slice(1) },
            annotations: [],
            autosize: true,
            height: container.offsetHeight,
        };

        const data = [{
            x: graphs[type].x,
            y: graphs[type].y,
            mode: 'lines',
            line: { color: 'blue' }
        }];

        const limits = graphs[type].limits;
        Object.keys(limits).forEach(limitName => {
            const limitValue = limits[limitName];
            data.push({
                x: [Math.min(...graphs[type].x), Math.max(...graphs[type].x)],
                y: [limitValue, limitValue],
                mode: 'lines',
                line: { color: 'red', dash: 'dashdot' },
                name: limitName
            });

            layout.annotations.push({
                x: Math.max(...graphs[type].x),
                y: limitValue,
                text: limitName,
                showarrow: true,
                arrowhead: 2,
                ax: 0,
                ay: -40
            });
        });

        Plotly.newPlot(graphs[type].id, data, layout, { responsive: true });
    });
    console.log('render');
}

function updateGraphs() {
    Object.keys(graphs).forEach(type => {

        if (!document.getElementById(graphs[type].id)) {
            return;
        }
        const update = {
            x: [graphs[type].x],
            y: [graphs[type].y]
        };

        Plotly.restyle(graphs[type].id, update, [0]);
    });
}

renderGraphs('collapseAltitude');

Object.keys(arr_cards).forEach(collapseElementName => {
    $('#' + collapseElementName).on('shown.bs.collapse', function () {
        renderGraphs(collapseElementName);
    })
});

$(window).resize(function(){
    renderGraphs('collapseAltitude');
  });

