import process from 'process';

const INSTANCE = "nos3";
const URL = `http://localhost:8090/api/archive/${INSTANCE}/events`;

const events = [
    {
        type: "PRESSURE_ALERT",
        message: "Pressure threshold exceeded",
        severity: "CRITICAL",
        source: "PressureModule",
        sequenceNumber: 1,
        extra: {
            pressure: "150 PSI",
            location: "Hydraulic System"
        }
    },
    {
        type: "PRESSURE_WARNING",
        message: "Pressure nearing critical level",
        severity: "WARNING",
        source: "PressureModule",
        sequenceNumber: 2,
        extra: {
            pressure: "140 PSI",
            location: "Hydraulic System"
        }
    },
    {
        type: "PRESSURE_INFO",
        message: "Pressure system check completed",
        severity: "INFO",
        source: "PressureModule",
        sequenceNumber: 3,
        extra: {
            checkType: "Routine Inspection",
            duration: "10m"
        }
    },
    {
        type: "TEMPERATURE_ALERT",
        message: "Temperature threshold exceeded",
        severity: "CRITICAL",
        source: "TemperatureModule",
        sequenceNumber: 4,
        extra: {
            temperature: "100°C",
            location: "Engine Room"
        }
    },
    {
        type: "TEMPERATURE_WARNING",
        message: "Temperature nearing critical level",
        severity: "WARNING",
        source: "TemperatureModule",
        sequenceNumber: 5,
        extra: {
            temperature: "95°C",
            location: "Engine Room"
        }
    },
    {
        type: "TEMPERATURE_INFO",
        message: "Temperature nominal",
        severity: "INFO",
        source: "TemperatureModule",
        sequenceNumber: 6,
        extra: {
            temperature: "35°C",
            location: "Life Support"
        }
    },
    {
        type: "TEMPERATURE_INFO",
        message: "Temperature nominal",
        severity: "INFO",
        source: "TemperatureModule",
        sequenceNumber: 7,
        extra: {
            temperature: "30°C",
            location: "Life Support"
        }
    },
    {
        type: "TEMPERATURE_SEVERE",
        message: "Temperature nominal",
        severity: "SEVERE",
        source: "TemperatureModule",
        sequenceNumber: 8,
        extra: {
            temperature: "200°C",
            location: "Engine Room"
        }
    }
];

async function postEvent(event, delaySeconds) {
    const eventTime = new Date(Date.now() + delaySeconds * 1000).toISOString();
    event.time = eventTime;

    try {
        const response = await fetch(URL, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(event)
        });

        if (response.ok) {
            console.log(`Event posted successfully: ${event.type}`);
        } else {
            console.error(`Failed to post event: ${event.type}. HTTP Status: ${response.status}`);
        }
    } catch (error) {
        console.error(`Error posting event: ${event.type}.`, error);
    }
}

export async function postAllEvents() {
    for (let i = 0; i < events.length; i++) {
        await postEvent(events[i], i * 5);
    }
}

// If you still want to run it standalone
if (import.meta.url === `file://${process.argv[1]}`) {
    postAllEvents();
}
