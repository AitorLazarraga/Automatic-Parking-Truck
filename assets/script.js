// Función principal para actualizar los sensores
function updateSensors() {
    fetch('/data')
        .then(response => response.json())
        .then(data => {
            const infoGrid = document.getElementById('infoGrid');
            infoGrid.innerHTML = '';
            
            // Actualizar cada sensor
            for (const [sensorId, info] of Object.entries(data)) {
                const sensorElement = document.querySelector(`[data-sensor="${sensorId}"]`);
                const valueElement = sensorElement?.querySelector('.sensor-value');
                
                if (valueElement) {
                    if (info.ultimo !== null) {
                        const distance = parseFloat(info.ultimo);
                        valueElement.textContent = distance.toFixed(1) + ' cm';
                        
                        // Cambiar color según distancia
                        sensorElement.classList.remove('active', 'inactive', 'warning');
                        if (distance <= 50) {
                            sensorElement.classList.add('warning');
                        } else {
                            sensorElement.classList.add('active');
                        }
                        
                        // Añadir a panel de información
                        const infoItem = document.createElement('div');
                        infoItem.className = 'info-item';
                        infoItem.innerHTML = `
                            <strong>Sensor ${sensorId}</strong>
                            <span>${distance.toFixed(1)} cm (${info.total_medidas} medidas)</span>
                        `;
                        infoGrid.appendChild(infoItem);
                    } else {
                        valueElement.textContent = '---';
                        sensorElement.classList.remove('active', 'warning');
                        sensorElement.classList.add('inactive');
                    }
                }
            }
        })
        .catch(error => console.error('Error al obtener datos:', error));
}

function sendMotor(command) {

    fetch('/motor', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ cmd: command })
    })
    .then(response => response.json())
    .then(data => {
        console.log("Respuesta motor:", data);
    })
    .catch(error => console.error("Error enviando comando:", error));
}

async function sendAngle() {

    const angle = document.getElementById("angleInput").value;
    const statusText = document.getElementById("angleStatus");

    if (angle === "" || isNaN(angle)) {
        statusText.innerText = "Please enter a valid number.";
        return;
    }

    try {
        const response = await fetch("/motor", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                cmd: "G" + angle
            })
        });

        const result = await response.json();

        statusText.innerText = "Command sent successfully!";
        console.log(result);

    } catch (error) {
        statusText.innerText = "Error sending command.";
        console.error(error);
    }
}
// Actualizar inmediatamente al cargar la página
updateSensors();

// Auto-refresh cada 1 segundo
setInterval(updateSensors, 1000);