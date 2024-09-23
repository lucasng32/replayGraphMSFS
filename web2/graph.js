function renderGraphs(card) {
    arr_cards[card].forEach(type => {
        const container = document.getElementById(type + 'Container');

        if (!container) {
            return;
        }
        container.innerHTML = ''; // Clear existing state.graphs

        const graphDiv = document.createElement('div');
        graphDiv.id = state.graphs[type].id;
        container.appendChild(graphDiv);

        const layout = {
            title: {text: state.graphs[type].title, font: {size: 30}},
            xaxis: { title: 'Time (s)' },
            yaxis: { title: type.charAt(0).toUpperCase() + type.slice(1) },
            annotations: [],
            autosize: true,
            height: container.offsetHeight,
        };

        const data = [{
            x: state.graphs[type].x,
            y: state.graphs[type].y,
            mode: 'lines',
            line: { color: 'blue' }
        }];

        const limits = state.graphs[type].limits;
        Object.keys(limits).forEach(limitName => {
            const limitValue = limits[limitName];
            data.push({
                x: [Math.min(...state.graphs[type].x), Math.max(...state.graphs[type].x)],
                y: [limitValue, limitValue],
                mode: 'lines',
                line: { color: 'red', dash: 'dashdot' },
                name: limitName
            });

            layout.annotations.push({
                x: Math.max(...state.graphs[type].x),
                y: limitValue,
                text: limitName,
                showarrow: true,
                arrowhead: 2,
                ax: 0,
                ay: -40
            });
        });

        Plotly.newPlot(state.graphs[type].id, data, layout, { responsive: true });
    });
    console.log('render');
}

function updateGraphs() {
    Object.keys(state.graphs).forEach(type => {

        if (!document.getElementById(state.graphs[type].id)) {
            return;
        }
        const update = {
            x: [state.graphs[type].x],
            y: [state.graphs[type].y]
        };

        Plotly.restyle(state.graphs[type].id, update, [0]);
    });
}

function updateGraphTime(){
    const currentTime = state.currTime / 1000;
    if (currentTime > 0.1) {
        Object.keys(graphs).forEach(type => {
            if (!document.getElementById(graphs[type].id)) {
                return;
            }
            Plotly.relayout(graphs[type].id, {
                shapes: [{
                    type: 'line',
                    x0: currentTime,
                    y0: 0,
                    x1: currentTime,
                    y1: 1,
                    xref: 'x',
                    yref: 'paper',
                    line: {
                        color: 'blue',
                        width: 2,
                        dash: 'dashdot'
                    }
                }]
            });
        })
        const slider = document.getElementById('timeSlider');
        slider.value = (currentTime / state.totalDuration) * 100;
    }
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

