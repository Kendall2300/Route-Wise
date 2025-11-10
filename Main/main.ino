#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "K2300PT";
const char* password = "Chocolate-2307";

WebServer server(80);

const int pin1 = 1;
const int pin2 = 2;
unsigned long simulaStart = 0;
bool simulando = false;

// Selector centrado, sin confirmación LED visual
const char* htmlForm = R"rawliteral(
<!DOCTYPE html>
<html lang='es'>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>Selector de Parada</title>
  <style>
    body { margin: 0; padding: 0; font-family: sans-serif; background: #fafafa; }
    .center-box {
      min-height: 100vh;
      display: flex; flex-direction: column; justify-content: center; align-items: center;
    }
    form select, form input[type=submit] { font-size: 1.3em; padding: 0.5em; }
    h2 { margin-bottom: 1em; }
  </style>
</head>
<body>
  <div class="center-box">
    <h2>Selecciona tu parada:</h2>
    <form action="/select" method="POST">
      <select name="parada">
        <option value="Parada1">Parada 1 - Centro</option>
        <option value="Parada2">Parada 2 - Terminal</option>
      </select>
      <br><br>
      <input type="submit" value="Confirmar">
    </form>
  </div>
</body>
</html>
)rawliteral";

const char* htmlDev = R"rawliteral(
<!DOCTYPE html>
<html lang='es'>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>Debug</title>
  <style>
    body { display: flex; justify-content: center; align-items: center; height: 100vh; }
    .devbox { text-align: center; }
    button { font-size: 1.2em; padding: 12px 30px; }
  </style>
</head>
<body>
  <div class='devbox'>
    <h1>Debug</h1>
    <form action='/simular' method='POST'>
      <button type='submit'>Simular parada</button>
    </form>
  </div>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlForm);
}

void handleDev() {
  server.send(200, "text/html", htmlDev);
}

void handleSimular() {
  digitalWrite(pin1, HIGH);    // Enciende el LED 1
  simulaStart = millis();
  simulando = true;
  server.send(200, "text/html",
    "<!DOCTYPE html><html><head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<title>Simulación</title>"
    "<script>setTimeout(function(){window.location.href='/dev';}, 2000);</script>"
    "<style>body{display:flex;justify-content:center;align-items:center;height:100vh;}h2{font-size:1.8em}</style>"
    "</head><body><h2>Simulación activa:<br>LED 1 encendido por 1 minuto</h2>"
    "<p>Redireccionando...</p></body></html>");
}

void handleSelect() {
  if (server.hasArg("parada")) {
    String parada = server.arg("parada");
    if (parada == "Parada1") {
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
      delay(1000);
      digitalWrite(pin1, LOW);
    } else if (parada == "Parada2") {
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
      delay(1000);
      digitalWrite(pin2, LOW);
    }
    server.send(200, "text/html", htmlForm); // Solo regresa el formulario, sin mensaje LED
  } else {
    server.send(400, "text/html", "<h2>Error: No se seleccionó ninguna parada.</h2>");
  }
}

void setup() {
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  digitalWrite(pin1, LOW); digitalWrite(pin2, LOW);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  server.on("/", handleRoot);
  server.on("/dev", handleDev);
  server.on("/select", HTTP_POST, handleSelect);
  server.on("/simular", HTTP_POST, handleSimular);
  server.begin();
}

void loop() {
  server.handleClient();
  // Desactiva LED 1 después de 1 minuto si simulando
  if (simulando && (millis() - simulaStart > 60000)) {
    digitalWrite(pin1, LOW);
    simulando = false;
  }
}
