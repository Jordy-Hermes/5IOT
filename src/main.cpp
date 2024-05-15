#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_AHTX0.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Hesias";
const char* password = "bienvenuechezHesias";
const char* mqtt_server = "172.20.63.156"; 
const int ledPin = 8;
WiFiClient espClient;
PubSubClient mqttClient(espClient);
AsyncWebServer server(80);

Adafruit_AHTX0 aht;

void setup_wifi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void connectToMQTT() {
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (mqttClient.connect("ESP32Client")) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin(2, 3);

    
    if (!aht.begin()) {
        Serial.println("Failed to initialize AHT sensor!");
        while (1) delay(10);
    }

      pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW); 

    setup_wifi();
    mqttClient.setServer(mqtt_server, 1883);

    


   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);  

    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 Weather Station</title>
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css">
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<style>
  body {
    font-family: 'Arial', sans-serif;
    background: linear-gradient(to right, #4facfe, #00f2fe);
    color: #fff;
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    text-align: center;
     position: relative;
  }
  @keyframes spin {
    0% { transform: rotate(0deg); }
    100% { transform: rotate(360deg); }
  }
  .fa-sun {
    font-size: 4em;
    position: absolute;
    top: 30px;
    left: 30%;
    transform: translateX(-50%);
    animation: spin 5s linear infinite;
  }
  .container {
    background-color: rgba(255, 255, 255, 0.8);
    border-radius: 15px;
    box-shadow: 0 4px 8px rgba(0,0,0,0.3);
    padding: 20px 40px;
    margin: 20px;
    transition: all 0.3s ease-in-out;
  }
  .container:hover {
    box-shadow: 0 12px 24px rgba(0,0,0,0.5);
    transform: scale(1.05);
  }
  h1 {
    margin: 0.5em 0;
    color: #333;
  }
  p {
    font-size: 1.2em;
    color: #333;
    margin: 10px 0;
  }
  .sensor-data {
    font-size: 2em;
    font-weight: bold;
    background-color: #ffffff;
    color: #333;
    border-radius: 8px;
    padding: 10px;
    display: inline-block;
    margin: 0 auto;
  }
    canvas {
  p {
    width: 100% !important;
    font-size: 1.2em;
    color: #333;
    margin: 10px 0;
  }
  .sensor-data {
    font-size: 2em;
    height: auto !important;
    font-weight: bold;
    background-color: #ffffff;
    color: #333;
    border-radius: 8px;
    padding: 10px;
    display: inline-block;
    margin: 0 auto;
  }
   canvas {
     width: 100% !important;
     height: auto !important;
     }
  
</style>
</head>
<body>
<i class="fa-solid fa-sun" style="color: #FFD43B;"></i>
<div class="container">
  <h1>Weather Station</h1>
  <p class="sensor-data" id="temperature" ></p>
  <p class="sensor-data" id="humidity" ></p>
  <canvas id="weatherChart"></canvas>
</div>
<script>
var ctx = document.getElementById('weatherChart').getContext('2d');
var weatherChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [], // Time labels
        datasets: [{
            label: 'Temperature (°C)',
            data: [2,4,13,32],
            backgroundColor: 'rgba(255, 99, 132, 0.2)',
            borderColor: 'rgba(255, 99, 132, 1)',
            borderWidth: 1,
            fill: true
        }, {
            label: 'Humidity (%)',
            data: [2,6,8,,11,30],
            backgroundColor: 'rgba(54, 162, 235, 0.2)',
            borderColor: 'rgba(54, 162, 235, 1)',
            borderWidth: 1,
            fill: true
        }]
    },
    options: {
        scales: {
            y: {
                beginAtZero: true
            }
        }
    }
});
function fetchData() {
    fetch('/data') // Assume '/data' endpoint returns JSON {temp: [value], humidity: [value], labels: [time]}
    .then(response => response.json())
    .then(data => {
      document.getElementById('temperature').innerHTML = 'Temperature: ' + data.temperature + ' C';
              document.getElementById('humidity').innerHTML = 'Humidity: ' + data.humidity + ' %';
      console.log(data)
        weatherChart.data.labels = data.labels;
        weatherChart.data.datasets[0].data = data.temp;
        weatherChart.data.datasets[1].data = data.humidity;
        weatherChart.update();
    })
    .catch(error => console.error('Error fetching data:', error));
}
fetchData(); // Initial fetch
setInterval(fetchData, 2000); // Fetch data every 5 seconds
</script>
</body>
</html>
)rawliteral";
    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
        sensors_event_t humidity, temp;
        aht.getEvent(&humidity, &temp);
        String json = "{\"temperature\": \"" + String(temp.temperature) + "\", \"humidity\": \"" + String(humidity.relative_humidity) + "\"}";
        request->send(200, "application/json", json);
    });
    request->send(200, "text/html", html);
});

 
    

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    if (!mqttClient.connected()) {
        connectToMQTT();
    }
    mqttClient.loop();

    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);

    char tempStr[8], humidStr[8];
    dtostrf(temp.temperature, 1, 2, tempStr);
    dtostrf(humidity.relative_humidity, 1, 2, humidStr);

    mqttClient.publish("jordy/temperature", tempStr);
    mqttClient.publish("jordy/humidity", humidStr);

    delay(2000);
}