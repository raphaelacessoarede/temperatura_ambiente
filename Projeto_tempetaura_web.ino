#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <ESP8266WiFi.h>

// Autor: RAPHAEL
// Servidor WEB ESP826
// FUNCAO: monitorar temperatura do ambiente e acionamento autonomo em horário pré-definido;

char rede [] = "usuario"; //usuario wifi
char dados[] = "senha"; //senha wifi

WiFiUDP udp;
NTPClient ntp(udp, "1.br.pool.ntp.org", -10800, 60000);//Cria um objeto "NTP" com as configurações.utilizada no Brasil
String hora;

WiFiServer server(80);

const byte pinoRele = D0;

#define DHTPIN D1
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(pinoRele, OUTPUT);
  digitalWrite(pinoRele, LOW);
  delay(10);

  conectaWiFi(rede, dados);

  ntp.begin();          // Inicia o protocolo ntp
  ntp.forceUpdate();    // Atualização

  server.begin();
  Serial.println("Serviço WEB iniciado.");
}

void autoIrrigacao() {

  //QUANDO SE USAR RELÉ ALTERAR statusLED HIGH PARA LOW
  // Então ficaria, HIGH = DESATIVA E LOW = ATIVA

Serial.println(hora);

  if ((hora > "9:00:00") && (hora < "9:00:10")) { // definido aqui 2 min de acionamento.
    digitalWrite(pinoRele, HIGH);
  } else if ((hora > "21:00:00") && (hora < "21:00:10")) {
    digitalWrite(pinoRele, HIGH);
  } else {
    digitalWrite(pinoRele, LOW);
  }
}

void conectaWiFi(char REDE[], char DADOS[]) {

  Serial.print("Conectando a rede: ");
  Serial.println(REDE);

  WiFi.begin(REDE, DADOS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("conectando...");
  }

  Serial.println("");
  Serial.println("WiFi Conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  hora = ntp.getFormattedTime();

  autoIrrigacao();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  float solo;
  solo = analogRead(A0);

  WiFiClient client = server.available();
  // if(!client){
  //   return;
  //   }

  //    while(!client.available()){
  //     delay(1);
  //}

  if (client) {
    while (client.connected()) {
      {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("");
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<title>ESP8266</title>");
        client.println("<meta http-equiv='refresh' content='5'>");
        client.println("<meta http-equiv='Content-Type' content='text/html' charset='utf-8'>");
        client.println("<h2>Servidor WEB ESP8266</h2>");
        client.println("Horário: ");
        client.println(hora);
        client.println("<br>");
        client.println("<br>");
        client.println("Umidade: ");
        client.println(h);
        client.println("<br>");
        client.println("Temperatura: ");
        client.println(t);
        client.println("<br>");
        client.println("Umidade do solo:");
        client.println(solo);
        client.println("<br>");
        client.println("</html>");
        client.stop();
      }
    }
  }
}
