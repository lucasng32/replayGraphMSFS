const graphs = {
    altitude: { x: [], y: [], id: 'altitudeGraph', limits: []},
    gForce: { x: [], y: [], id: 'gForceGraph', limits: {}},
    velocityWorldY: { x: [], y: [], id: 'rateOfClimbGraph', limits: []},
    incidenceAlpha: { x: [], y: [], id: 'angleOfAttackGraph', limits: {'stallAlpha': 0}},
    pitch: { x: [], y: [], id: 'pitchGraph', limits: {'staticPitch': 0}},
    indicatedAirspeed: { x: [], y: [], id: 'indicatedAirspeedGraph', limits: {'flapsUpStallSpeed': 0}},
    windSpeed: { x: [], y: [], id: 'windSpeedGraph', limits: {}},
    aileronPosition: { x: [], y: [], id: 'aileronPositionGraph', limits: {}}
};

const activeGraphs = new Set();
let shouldReRender = true;
let totalDuration = 100;

function renderGraphs() {
    const container = document.getElementById('graphsContainer');
    container.innerHTML = ''; // Clear existing graphs

    activeGraphs.forEach(type => {
        const div = document.createElement('div');
        div.className = `col-md-${12 / activeGraphs.size} graph-container`; // Dynamic graph resizing
        div.id = type + 'Container';

        const graphDiv = document.createElement('div');
        graphDiv.id = graphs[type].id;
        div.appendChild(graphDiv);
        container.appendChild(div);

        const layout = {
            title: type.charAt(0).toUpperCase() + type.slice(1) + ' Data',
            xaxis: { title: 'Time (s)' },
            yaxis: { title: type.charAt(0).toUpperCase() + type.slice(1) },
            annotations: [],
            autosize: true,
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

    activeGraphs.forEach(type => {
        const graphDiv = document.getElementById(type+'Container');
        Plotly.relayout(graphs[type].id, {
            width: graphDiv.offsetWidth,
            height: graphDiv.offsetHeight
        });

});
}

document.querySelectorAll('.graph-select').forEach(button => {
    button.addEventListener('click', function () {
        const type = this.id.replace('Button', '');
        if (activeGraphs.has(type)) {
            activeGraphs.delete(type);
        } else {
            activeGraphs.add(type);
        }
        shouldReRender = true;
        renderGraphs();
    });
});
