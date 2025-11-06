#include <WiFi.h>
#include <WebServer.h>

// Configuración de WiFi
const char* ssid = "K2300";           // Cambia esto por tu SSID
const char* password = "Chocolate_2307";    // Cambia esto por tu contraseña

// Configuración de IP estática
IPAddress local_IP(192, 168, 1, 100);      // IP que quieres para el ESP32
IPAddress gateway(192, 168, 1, 1);         // IP de tu router (gateway)
IPAddress subnet(255, 255, 255, 0);        // Máscara de subred
IPAddress primaryDNS(8, 8, 8, 8);          // DNS primario (opcional)
IPAddress secondaryDNS(8, 8, 4, 4);        // DNS secundario (opcional)

// Pin para activar la señal de parada
const int STOP_SIGNAL_PIN = 2;  // Cambia al pin que necesites (2, 4, 5, etc.)
const int SIGNAL_DURATION = 3000; // Duración de la señal en milisegundos (3 segundos)

// Crear servidor web en el puerto 80
WebServer server(80);

// Manejador para activar la señal de parada
void handleActivateSignal() {
  digitalWrite(STOP_SIGNAL_PIN, HIGH);
  Serial.println("¡Señal de parada activada!");
  
  delay(SIGNAL_DURATION);
  
  digitalWrite(STOP_SIGNAL_PIN, LOW);
  Serial.println("Señal de parada desactivada");
  
  server.send(200, "text/plain", "OK");
}

// Manejador para resetear la señal manualmente
void handleReset() {
  digitalWrite(STOP_SIGNAL_PIN, LOW);
  Serial.println("Señal reseteada manualmente");
  server.send(200, "text/plain", "OK");
}

// Manejador para la ruta raíz "/"
void handleRoot() {
  String html = "<!DOCTYPE html>";
  html += "<html lang='es'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Aviso Paradas Autobús</title>";
  html += "<style>";
  html += "* { margin: 0; padding: 0; box-sizing: border-box; }";
  html += "body {";
  html += "  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;";
  html += "  background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);";
  html += "  min-height: 100vh;";
  html += "  padding: 20px;";
  html += "}";
  html += ".container {";
  html += "  background: white;";
  html += "  padding: 30px;";
  html += "  border-radius: 20px;";
  html += "  box-shadow: 0 20px 60px rgba(0,0,0,0.3);";
  html += "  max-width: 600px;";
  html += "  margin: 0 auto;";
  html += "}";
  html += "h1 {";
  html += "  color: #1e3c72;";
  html += "  margin-bottom: 10px;";
  html += "  font-size: 2em;";
  html += "  text-align: center;";
  html += "}";
  html += ".subtitle {";
  html += "  text-align: center;";
  html += "  color: #666;";
  html += "  margin-bottom: 30px;";
  html += "  font-size: 0.9em;";
  html += "}";
  html += ".status-box {";
  html += "  background: #e3f2fd;";
  html += "  padding: 15px;";
  html += "  border-radius: 10px;";
  html += "  margin-bottom: 20px;";
  html += "  border-left: 4px solid #2196f3;";
  html += "}";
  html += ".status-box.success {";
  html += "  background: #e8f5e9;";
  html += "  border-left-color: #4caf50;";
  html += "}";
  html += ".status-box.warning {";
  html += "  background: #fff3e0;";
  html += "  border-left-color: #ff9800;";
  html += "}";
  html += ".status-box.alert {";
  html += "  background: #ffebee;";
  html += "  border-left-color: #f44336;";
  html += "  animation: pulse 2s infinite;";
  html += "}";
  html += "@keyframes pulse {";
  html += "  0%, 100% { transform: scale(1); }";
  html += "  50% { transform: scale(1.02); }";
  html += "}";
  html += ".stop-card {";
  html += "  background: #f5f5f5;";
  html += "  padding: 20px;";
  html += "  border-radius: 12px;";
  html += "  margin-bottom: 15px;";
  html += "  cursor: pointer;";
  html += "  transition: all 0.3s;";
  html += "  border: 2px solid transparent;";
  html += "}";
  html += ".stop-card:hover {";
  html += "  background: #e3f2fd;";
  html += "  transform: translateY(-2px);";
  html += "  box-shadow: 0 4px 12px rgba(0,0,0,0.1);";
  html += "}";
  html += ".stop-card.selected {";
  html += "  background: #e8f5e9;";
  html += "  border-color: #4caf50;";
  html += "}";
  html += ".stop-name {";
  html += "  font-weight: bold;";
  html += "  color: #333;";
  html += "  font-size: 1.2em;";
  html += "  margin-bottom: 5px;";
  html += "}";
  html += ".stop-info {";
  html += "  color: #666;";
  html += "  font-size: 0.9em;";
  html += "}";
  html += ".btn {";
  html += "  background: #2196f3;";
  html += "  color: white;";
  html += "  border: none;";
  html += "  padding: 15px 30px;";
  html += "  border-radius: 10px;";
  html += "  font-size: 1.1em;";
  html += "  cursor: pointer;";
  html += "  width: 100%;";
  html += "  margin-top: 20px;";
  html += "  transition: background 0.3s;";
  html += "}";
  html += ".btn:hover { background: #1976d2; }";
  html += ".btn:disabled {";
  html += "  background: #ccc;";
  html += "  cursor: not-allowed;";
  html += "}";
  html += ".btn.stop-btn {";
  html += "  background: #f44336;";
  html += "}";
  html += ".btn.stop-btn:hover { background: #d32f2f; }";
  html += ".btn.reset-btn {";
  html += "  background: #ff9800;";
  html += "  margin-top: 10px;";
  html += "}";
  html += ".btn.reset-btn:hover { background: #f57c00; }";
  html += ".distance {";
  html += "  font-size: 1.5em;";
  html += "  font-weight: bold;";
  html += "  color: #2196f3;";
  html += "  text-align: center;";
  html += "  margin: 10px 0;";
  html += "}";
  html += ".signal-status {";
  html += "  background: #fff3e0;";
  html += "  padding: 15px;";
  html += "  border-radius: 10px;";
  html += "  margin-top: 15px;";
  html += "  border-left: 4px solid #ff9800;";
  html += "  text-align: center;";
  html += "  font-weight: bold;";
  html += "}";
  html += ".signal-status.active {";
  html += "  background: #ffebee;";
  html += "  border-left-color: #f44336;";
  html += "  color: #f44336;";
  html += "  animation: blink 1s infinite;";
  html += "}";
  html += "@keyframes blink {";
  html += "  0%, 50%, 100% { opacity: 1; }";
  html += "  25%, 75% { opacity: 0.5; }";
  html += "}";
  html += ".hidden { display: none; }";
  html += "</style>";
  html += "</head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>🚌 Aviso Paradas Autobús</h1>";
  html += "<p class='subtitle'>Selecciona tu parada destino</p>";
  
  html += "<div id='statusBox' class='status-box hidden'></div>";
  
  html += "<div id='stopSelection'>";
  html += "<div class='stop-card' onclick='selectStop(0)'>";
  html += "<div class='stop-name'>🏛️ Parada Centro</div>";
  html += "<div class='stop-info'>Plaza Principal<br>Lat: 9.9325, Lon: -84.0795</div>";
  html += "</div>";
  
  html += "<div class='stop-card' onclick='selectStop(1)'>";
  html += "<div class='stop-name'>🏫 Parada Universidad</div>";
  html += "<div class='stop-info'>Campus Principal<br>Lat: 9.9356, Lon: -84.0778</div>";
  html += "</div>";
  
  html += "<button class='btn' id='startBtn' onclick='startTracking()' disabled>";
  html += "Iniciar Seguimiento";
  html += "</button>";
  html += "</div>";
  
  html += "<div id='trackingView' class='hidden'>";
  html += "<div class='status-box success'>";
  html += "<strong>📍 Siguiendo:</strong> <span id='selectedStopName'></span>";
  html += "</div>";
  html += "<div class='distance' id='distanceDisplay'>Calculando...</div>";
  html += "<div class='signal-status' id='signalStatus'>🔕 Señal inactiva</div>";
  html += "<button class='btn stop-btn' onclick='stopTracking()'>Detener Seguimiento</button>";
  html += "<button class='btn reset-btn' onclick='resetSignal()'>🔄 Reset Señal</button>";
  html += "</div>";
  
  html += "</div>";
  
  html += "<script>";
  html += "const stops = [";
  html += "  { name: 'Parada Centro', lat: 9.9325, lon: -84.0795 },";
  html += "  { name: 'Parada Universidad', lat: 9.9356, lon: -84.0778 }";
  html += "];";
  html += "let selectedStopIndex = -1;";
  html += "let watchId = null;";
  html += "let hasAlerted = false;";
  html += "let signalActivated = false;";
  html += "const ALERT_DISTANCE = 200;";
  
  html += "function selectStop(index) {";
  html += "  selectedStopIndex = index;";
  html += "  document.querySelectorAll('.stop-card').forEach((card, i) => {";
  html += "    card.classList.toggle('selected', i === index);";
  html += "  });";
  html += "  document.getElementById('startBtn').disabled = false;";
  html += "}";
  
  html += "function calculateDistance(lat1, lon1, lat2, lon2) {";
  html += "  const R = 6371e3;";
  html += "  const φ1 = lat1 * Math.PI / 180;";
  html += "  const φ2 = lat2 * Math.PI / 180;";
  html += "  const Δφ = (lat2 - lat1) * Math.PI / 180;";
  html += "  const Δλ = (lon2 - lon1) * Math.PI / 180;";
  html += "  const a = Math.sin(Δφ/2) * Math.sin(Δφ/2) +";
  html += "            Math.cos(φ1) * Math.cos(φ2) *";
  html += "            Math.sin(Δλ/2) * Math.sin(Δλ/2);";
  html += "  const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));";
  html += "  return R * c;";
  html += "}";
  
  html += "function showStatus(message, type = 'info') {";
  html += "  const box = document.getElementById('statusBox');";
  html += "  box.textContent = message;";
  html += "  box.className = 'status-box ' + type;";
  html += "  box.classList.remove('hidden');";
  html += "}";
  
  html += "function startTracking() {";
  html += "  if (selectedStopIndex === -1) return;";
  html += "  if (!navigator.geolocation) {";
  html += "    showStatus('❌ Tu navegador no soporta geolocalización', 'alert');";
  html += "    return;";
  html += "  }";
  html += "  document.getElementById('stopSelection').classList.add('hidden');";
  html += "  document.getElementById('trackingView').classList.remove('hidden');";
  html += "  document.getElementById('selectedStopName').textContent = stops[selectedStopIndex].name;";
  html += "  hasAlerted = false;";
  html += "  watchId = navigator.geolocation.watchPosition(updatePosition, handleError, {";
  html += "    enableHighAccuracy: true,";
  html += "    maximumAge: 5000,";
  html += "    timeout: 10000";
  html += "  });";
  html += "}";
  
  html += "function updatePosition(position) {";
  html += "  const userLat = position.coords.latitude;";
  html += "  const userLon = position.coords.longitude;";
  html += "  const stop = stops[selectedStopIndex];";
  html += "  const distance = calculateDistance(userLat, userLon, stop.lat, stop.lon);";
  html += "  const distanceText = distance < 1000 ? ";
  html += "    Math.round(distance) + ' metros' : ";
  html += "    (distance / 1000).toFixed(2) + ' km';";
  html += "  document.getElementById('distanceDisplay').textContent = distanceText;";
  html += "  if (distance <= ALERT_DISTANCE && !hasAlerted) {";
  html += "    hasAlerted = true;";
  html += "    activateStopSignal();";
  html += "    showStatus('🔔 ¡ALERTA! Estás cerca de tu parada (' + Math.round(distance) + 'm)', 'alert');";
  html += "    if ('vibrate' in navigator) navigator.vibrate([200, 100, 200]);";
  html += "    if ('Notification' in window && Notification.permission === 'granted') {";
  html += "      new Notification('¡Parada próxima!', {";
  html += "        body: 'Estás a ' + Math.round(distance) + ' metros de ' + stop.name,";
  html += "        icon: '🚌'";
  html += "      });";
  html += "    }";
  html += "  } else if (distance <= ALERT_DISTANCE) {";
  html += "    showStatus('🔔 Muy cerca de tu parada', 'alert');";
  html += "  } else if (distance <= 500) {";
  html += "    showStatus('⚠️ Te estás acercando a tu parada', 'warning');";
  html += "  } else {";
  html += "    showStatus('✓ Seguimiento activo', 'success');";
  html += "  }";
  html += "}";
  
  html += "function activateStopSignal() {";
  html += "  if (signalActivated) return;";
  html += "  signalActivated = true;";
  html += "  const statusDiv = document.getElementById('signalStatus');";
  html += "  statusDiv.textContent = '🔔 SEÑAL ACTIVADA - Timbre sonando';";
  html += "  statusDiv.classList.add('active');";
  html += "  fetch('/activate')";
  html += "    .then(response => console.log('Señal enviada al ESP32'))";
  html += "    .catch(error => console.error('Error:', error));";
  html += "  setTimeout(() => {";
  html += "    statusDiv.textContent = '✓ Señal completada';";
  html += "    statusDiv.classList.remove('active');";
  html += "  }, 3000);";
  html += "}";
  
  html += "function resetSignal() {";
  html += "  signalActivated = false;";
  html += "  hasAlerted = false;";
  html += "  const statusDiv = document.getElementById('signalStatus');";
  html += "  statusDiv.textContent = '🔕 Señal inactiva';";
  html += "  statusDiv.classList.remove('active');";
  html += "  fetch('/reset')";
  html += "    .then(response => console.log('Señal reseteada'))";
  html += "    .catch(error => console.error('Error:', error));";
  html += "}";
  
  html += "function handleError(error) {";
  html += "  let message = '❌ Error: ';";
  html += "  switch(error.code) {";
  html += "    case error.PERMISSION_DENIED:";
  html += "      message += 'Permiso de ubicación denegado';";
  html += "      break;";
  html += "    case error.POSITION_UNAVAILABLE:";
  html += "      message += 'Ubicación no disponible';";
  html += "      break;";
  html += "    case error.TIMEOUT:";
  html += "      message += 'Tiempo de espera agotado';";
  html += "      break;";
  html += "  }";
  html += "  showStatus(message, 'alert');";
  html += "}";
  
  html += "function stopTracking() {";
  html += "  if (watchId !== null) {";
  html += "    navigator.geolocation.clearWatch(watchId);";
  html += "    watchId = null;";
  html += "  }";
  html += "  resetSignal();";
  html += "  document.getElementById('trackingView').classList.add('hidden');";
  html += "  document.getElementById('stopSelection').classList.remove('hidden');";
  html += "  document.getElementById('statusBox').classList.add('hidden');";
  html += "  selectedStopIndex = -1;";
  html += "  document.querySelectorAll('.stop-card').forEach(card => {";
  html += "    card.classList.remove('selected');";
  html += "  });";
  html += "  document.getElementById('startBtn').disabled = true;";
  html += "}";
  
  html += "if ('Notification' in window && Notification.permission === 'default') {";
  html += "  Notification.requestPermission();";
  html += "}";
  html += "</script>";
  html += "</body>";
  html += "</html>";
  
  server.send(200, "text/html", html);
}

// Manejador para rutas no encontradas
void handleNotFound() {
  server.send(404, "text/plain", "404: No encontrado");
}

void setup() {
  // Iniciar comunicación serial
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\nIniciando ESP32...");
  
  // Configurar pin de señal de parada
  pinMode(STOP_SIGNAL_PIN, OUTPUT);
  digitalWrite(STOP_SIGNAL_PIN, LOW);
  Serial.println("Pin de señal configurado en: " + String(STOP_SIGNAL_PIN));
  
  // Configurar IP estática antes de conectar
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Fallo al configurar IP estática");
  }
  
  // Conectar a WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n¡Conectado a WiFi!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  
  // Configurar rutas del servidor
  server.on("/", handleRoot);
  server.on("/activate", handleActivateSignal);
  server.on("/reset", handleReset);
  server.onNotFound(handleNotFound);
  
  // Iniciar servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado");
  Serial.println("Abre tu navegador y accede a: http://" + WiFi.localIP().toString());
}

void loop() {
  // Manejar peticiones de clientes
  server.handleClient();
}