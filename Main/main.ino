#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "K2300PT";
const char* password = "Chocolate-2307";

WebServer server(80);

const int pin1 = 1;
const int pin2 = 2;
unsigned long seatTime = 0;
bool seatSelected = false;
unsigned long simulaStart = 0;
bool simulando = false;

const char* htmlForm = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>RouteWise</title>
  <style>
    body { margin:0; padding:0; font-family:sans-serif; background:#fafafa; }
    .app-title { text-align:center; font-size:2em; margin-top:1.2em; font-weight:bold; }
    .center-box { min-height:70vh; display:flex; flex-direction:column; justify-content:center; align-items:center; }
    label, select, input { font-size:1.2em; margin:8px 0;padding:5px }
    select { min-width:170px; }
    .separador { margin:18px 0;}
  </style>
  <script>
    function actualizarParadas() {
      var ruta = document.getElementById('ruta').value;
      var paradas = {
        "Cartago-Tec": ["Ruinas", "Estacion"],
        "San Jose-Tec": ["Morasan", "Estacion"]
      };
      var paradaSelect = document.getElementById('parada');
      paradaSelect.innerHTML = '';
      paradas[ruta].forEach(function(p){
        var opt = document.createElement('option'); opt.value=p; opt.innerHTML=p;
        paradaSelect.appendChild(opt);
      });
    }
    window.onload = actualizarParadas;
  </script>
</head>
<body>
  <div class="app-title">RouteWise</div>
  <div class="center-box">
    <form action="/selectruta" method="POST">
      <label for="ruta">Ruta:</label>
      <select id="ruta" name="ruta" onchange="actualizarParadas()">
        <option value="Cartago-Tec">Cartago-Tec</option>
        <option value="San Jose-Tec">San Jose-Tec</option>
      </select>
      <div class="separador"></div>
      <label for="parada">Parada:</label>
      <select id="parada" name="parada">
        <option value="Ruinas">Ruinas</option>
        <option value="Estacion">Estacion</option>
      </select>
      <div class="separador"></div>
      <label for="asiento">Asiento:</label>
      <select id="asiento" name="asiento">
        <option value="1">1</option>
        <option value="2">2</option>
        <option value="3">3</option>
        <option value="4">4</option>
      </select>
      <div class="separador"></div>
      <input type="submit" value="Solicitar Asiento">
    </form>
  </div>
</body>
</html>
)rawliteral";

// Confirmacion sin audio, solo notificacion/alerta
String htmlAsiento(String ruta, String parada, String asiento) {
  return "<!DOCTYPE html><html><head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<title>Asiento</title>"
    "<script>"
    "setTimeout(function(){"
      "if('Notification'in window){"
        "if(Notification.permission==='granted'){"
          "new Notification('RouteWise le informa que su parada es la siguiente');"
        "}else if(Notification.permission!=='denied'){"
          "Notification.requestPermission().then(function(permission){"
            "if(permission==='granted'){"
              "new Notification('RouteWise le informa que su parada es la siguiente');"
            "}else{alert('RouteWise le informa que su parada es la siguiente');}"
          "});"
        "}else{alert('RouteWise le informa que su parada es la siguiente');}"
      "}else{alert('RouteWise le informa que su parada es la siguiente');}"
    "},60000);"
    "</script>"
    "<style>body{display:flex;justify-content:center;align-items:center;height:100vh;flex-direction:column;font-family:sans-serif;}h2{margin-bottom:1em;}</style>"
    "</head><body>"
    "<h2>Tu ruta: <b>"+ruta+"</b></h2>"
    "<h2>Parada: <b>"+parada+"</b></h2>"
    "<h2>Asiento elegido:<br><span style='font-size:2em; color:#0066a2;'>" + asiento + "</span></h2>"
    "<a href='/'><button style='font-size:1.1em;padding:7px 20px'>Volver al inicio</button></a>"
    "</body></html>";
}

const char* htmlDev = R"rawliteral(
<!DOCTYPE html>
<html>
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

// --- Handlers ---
void handleRoot() { server.send(200, "text/html", htmlForm); }
void handleDev()  { server.send(200, "text/html", htmlDev); }

void handleSelectRuta() {
  if (server.hasArg("ruta") && server.hasArg("parada") && server.hasArg("asiento")) {
    String ruta = server.arg("ruta");
    String parada = server.arg("parada");
    String asiento = server.arg("asiento");
    seatTime = millis();
    seatSelected = true;
    server.send(200, "text/html", htmlAsiento(ruta, parada, asiento));
  } else {
    server.send(400, "text/html", "<h2>Error: Faltan datos de ruta, parada o asiento</h2>");
  }
}

void handleSimular() {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  simulaStart = millis();
  simulando = true;
  server.send(200, "text/html",
    "<!DOCTYPE html><html><head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<title>Simulacion</title>"
    "<script>setTimeout(function(){window.location.href='/dev';},2000);</script>"
    "<style>body{display:flex;justify-content:center;align-items:center;height:100vh;}h2{font-size:1.8em}</style>"
    "</head><body><h2>Los leds se apagan en 30 segundos</h2>"
    "<p>Redireccionando...</p></body></html>");
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
  server.on("/selectruta", HTTP_POST, handleSelectRuta);
  server.on("/simular", HTTP_POST, handleSimular);
  server.begin();
}

void loop() {
  server.handleClient();
  // Se enciende LED1 a 1 min de seleccionar asiento
  if (seatSelected && (millis() - seatTime > 60000)) {
    digitalWrite(pin1, HIGH);
    seatSelected = false;
  }
  // Simulacion: apaga todos los LEDs luego de 30 s
  if (simulando && (millis() - simulaStart > 30000)) {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    simulando = false;
  }
}
