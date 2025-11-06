#include <WiFi.h>
#include <WebServer.h>

<<<<<<< Updated upstream
// Configuración de WiFi
const char* ssid = "TU_WIFI";           // Cambia esto por tu SSID
const char* password = "TU_PASSWORD";    // Cambia esto por tu contraseña
=======
const char* ssid = "K2300PT";
const char* password = "Chocolate-2307";
>>>>>>> Stashed changes

WebServer server(80);

// HTML con selector de paradas
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

// Muestra el formulario en "/"
void handleRoot() {
  server.send(200, "text/html", htmlForm);
}

// Muestra "Debug" en "/dev"
void handleDev() {
  server.send(200, "text/html", "<h1>Debug</h1>");
}

// Manejador para la selección POST
void handleSelect() {
  Serial.println("Entró a handleSelect"); // Línea de prueba
  if (server.hasArg("parada")) {
    String parada = server.arg("parada");
    Serial.print("Parada seleccionada: ");
    Serial.println(parada);
    server.send(200, "text/html", "<h2>Parada seleccionada: " + parada + "</h2><a href='/'>Volver</a>");
  } else {
    server.send(400, "text/html", "<h2>Error: No se seleccionó ninguna parada.</h2>");
  }
}


void setup() {
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
  server.on("/select", HTTP_POST, handleSelect);  // Nuevo endpoint para el formulario
  server.begin();
  Serial.println("Servidor web iniciado!");
}

void loop() {
  server.handleClient();
}
