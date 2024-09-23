// Highlight the selected row
const tableBody = document.querySelector('#data-table tbody');
let selectedRowId = null;
tableBody.addEventListener('click', (e) => {
    const clickedRow = e.target.closest('tr'); // Get the parent row of the clicked cell

    if (clickedRow) {
        // Check if the clicked row is already selected
        if (clickedRow.classList.contains('selected')) {
            // Deselect the row
            clickedRow.classList.remove('selected');
            selectedRowId = null;
            console.log('Row deselected');
        } else {
            // Remove the previous selection
            document.querySelectorAll('tr.selected').forEach(row => row.classList.remove('selected'));

            // Highlight the new selection
            clickedRow.classList.add('selected');
            selectedRowId = clickedRow.getAttribute('data-id'); // Get the selected row's id from the correct attribute
            console.log(`Selected row id: ${selectedRowId}`);
        }
    }
});

// Handle button click to send selected row data
document.getElementById('send-data').addEventListener('click', () => {
    if (selectedRowId != null) {
        console.log(selectedRowId);
        ws.send(ClientEvent['LoadFlight'], {flightId: selectedRowId});
    } else {
        alert('No row selected');
    }
});