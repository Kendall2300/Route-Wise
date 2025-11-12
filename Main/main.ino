#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

const char* ssid = "K2300PT";
const char* password = "Chocolate-2307";

WebServer server(80);

const int pin1 = 1;
const int pin2 = 2;
unsigned long seatTime = 0;
bool seatSelected = false;
unsigned long simulaStart = 0;
bool simulando = false;

// Página principal con diseño mejorado
const char* htmlForm = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>RouteWise</title>
  <style>
    body {
      margin:0; padding:0;
      font-family:'Montserrat',Arial,sans-serif;
      background: linear-gradient(135deg,#e0f7fa 0%,#ffe0b2 100%);
      min-height:100vh; color:#333;
    }
    .logo-box { text-align:center; margin-top:30px; }
    .logo-box img {
      width:120px; height:auto; border-radius:18px; box-shadow:0 6px 24px #0002;
    }
    .app-title {
      text-align:center; font-size:2.3em; margin:10px 0 24px 0; font-weight:700; letter-spacing:2px;
    }
    .center-box {
      display:flex; flex-direction:column; justify-content:center; align-items:center; margin:0 auto; padding:32px;
      max-width:340px; background:#fff7; border-radius:24px; box-shadow:0 4px 18px #aaa2;
    }
    label { font-size:1.07em; font-weight:600; }
    select, input[type=submit], button {
      font-size:1.15em; margin:0 0 14px 0; padding:6px 14px;
      border:1.5px solid #00838f; border-radius:12px; background:#fff; color:#00796b;
      transition:all 0.2s;
    }
    select:focus, input[type=submit]:hover, button:hover { background:#b2ebf2; color:#333; border-color:#ffb74d; }
    .separador { margin:14px 0;}
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
  <div class="logo-box">
    <img src="/logo.jpg" alt="Logo RouteWise">
  </div>
  <div class="app-title">RouteWise</div>
  <div class="center-box">
    <form action="/selectruta" method="POST">
      <label for="ruta">Ruta</label>
      <select id="ruta" name="ruta" onchange="actualizarParadas()">
        <option value="Cartago-Tec">Cartago-Tec</option>
        <option value="San Jose-Tec">San Jose-Tec</option>
      </select>
      <div class="separador"></div>
      <label for="parada">Parada</label>
      <select id="parada" name="parada">
        <option value="Ruinas">Ruinas</option>
        <option value="Estacion">Estacion</option>
      </select>
      <div class="separador"></div>
      <label for="asiento">Asiento</label>
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

// Página de confirmación estética
String htmlAsiento(String ruta, String parada, String asiento) {
  return "<!DOCTYPE html><html><head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<title>Asiento</title>"
    "<style>"
    "body{background:linear-gradient(135deg,#ffe0b2 0%,#e0f7fa 100%);font-family:Montserrat,Arial,sans-serif;min-height:100vh;"
    "display:flex;flex-direction:column;justify-content:center;align-items:center;}h2{margin-bottom:1em;}"
    ".confirm-box{background:#fff7;padding:30px 26px;max-width:320px;border-radius:20px;box-shadow:0 4px 18px #aaa2;text-align:center;}"
    ".button{font-size:1em;padding:7px 20px;border-radius:10px;border:1.5px solid #00838f;background:#fff; color:#00796b;margin-top:22px;transition:.2s;} "
    ".button:hover{background:#b2ebf2;color:#333;border-color:#ffb74d;}"
    "</style>"
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
    "</head><body>"
    "<div class='logo-box'><img src='/logo.jpg' alt='Logo RouteWise'></div>"
    "<div class='confirm-box'>"
    "<h2>Ruta: <b>"+ruta+"</b></h2>"
    "<h2>Parada: <b>"+parada+"</b></h2>"
    "<h2>Asiento: <span style='font-size:2em;color:#0066a2;'>" + asiento + "</span></h2>"
    "<a href='/'><button class='button'>Volver al inicio</button></a>"
    "</div></body></html>";
}

const char* htmlDev = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>Debug</title>
  <style>
    body { display:flex;justify-content:center;align-items:center;height:100vh; background:linear-gradient(135deg,#e0f7fa 0%,#ffe0b2 100%);}
    .devbox { background:#fff7; padding:32px 22px; border-radius:20px; box-shadow:0 4px 18px #aaa2; text-align:center;}
    button { font-size:1.2em; padding:12px 30px; border-radius:12px; border:1.5px solid #00838f; background:#fff; color:#00796b; transition:all 0.2s;}
    button:hover{background:#b2ebf2;color:#333;border-color:#ffb74d;}
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
void handleLogo() {
  File file = SPIFFS.open("/logo.jpg", "r");
  if (!file) {
    server.send(404, "text/plain", "No encontrado");
    return;
  }
  server.streamFile(file, "image/jpeg");
  file.close();
}

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
    "<style>body{display:flex;justify-content:center;align-items:center;height:100vh;background:linear-gradient(135deg,#e0f7fa 0%,#ffe0b2 100%);}h2{font-size:1.8em}</style>"
    "<script>setTimeout(function(){window.location.href='/dev';},2000);</script>"
    "</head><body><div class='devbox'><h2>Los leds se apagan en 30 segundos</h2><p>Redireccionando...</p></div></body></html>");
}

void setup() {
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  digitalWrite(pin1, LOW); digitalWrite(pin2, LOW);

  Serial.begin(115200);

  // Inicializa SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("Error al montar SPIFFS");
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }

  server.on("/", handleRoot);
  server.on("/dev", handleDev);
  server.on("/logo.jpg", HTTP_GET, handleLogo); // logo
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
