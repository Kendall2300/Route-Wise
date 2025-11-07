#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "K2300PT";
const char* password = "Chocolate-2307";

WebServer server(80);

const int pin1 = 1; // GPIO1
const int pin2 = 2; // GPIO2

const char* htmlForm = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Selecciona una parada</title>
</head>
<body>
  <h2>Selecciona tu parada:</h2>
  <form action="/select" method="POST">
    <select name="parada">
      <option value="Parada1">Parada 1 - Centro</option>
      <option value="Parada2">Parada 2 - Terminal</option>
    </select>
    <br><br>
    <input type="submit" value="Enviar">
  </form>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlForm);
}

void handleDev() {
  server.send(200, "text/html", "<h1>Debug</h1>");
}

void handleSelect() {
  if (server.hasArg("parada")) {
    String parada = server.arg("parada");

    if (parada == "Parada1") {
      digitalWrite(pin1, HIGH); // Enciende LED 1
      digitalWrite(pin2, LOW);  // Apaga LED 2
    } else if (parada == "Parada2") {
      digitalWrite(pin1, LOW);  // Apaga LED 1
      digitalWrite(pin2, HIGH); // Enciende LED 2
    }

    server.send(200, "text/html", "<h2>Parada seleccionada: " + parada + "</h2><a href='/'>Volver</a>");
  } else {
    server.send(400, "text/html", "<h2>Error: No se seleccionó ninguna parada.</h2>");
  }
}

void setup() {
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);

  // Al arrancar, ambos apagados
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado a WiFi. IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/dev", handleDev);
  server.on("/select", HTTP_POST, handleSelect);
  server.begin();
  Serial.println("Servidor web iniciado!");
}

void loop() {
  server.handleClient();
}
