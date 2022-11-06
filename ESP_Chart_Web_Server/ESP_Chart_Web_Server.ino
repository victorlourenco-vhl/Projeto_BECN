#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
#endif

#include <Wire.h>  
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h> 

#define planta1 A0

#define motorPlanta1 D5
#define motorPlanta2 D6
#define motorPlanta3 D7


const char* ssid = "Redmi Note 8";
const char* password = "Vitinho40";

LiquidCrystal_I2C lcd(0x27, 16, 2); //FUNÇÃO DO TIPO "LiquidCrystal_I2C"

AsyncWebServer server(80);

String umidadeSolo(int pinoSensor){
  int umidadeSolo = map(analogRead(pinoSensor), 0, 1023, 0, 100); //Mapeando o valor entre 0 e 100
  umidadeSolo = 100 - umidadeSolo;
  return String(umidadeSolo);
}

void regarPlanta(String umidade, int motor){
  int umidadeSolo = umidade.toInt();
  if(umidadeSolo <  53){
    digitalWrite(motor, HIGH);
    delay(1500);
    digitalWrite(motor, LOW);   
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(motorPlanta1, OUTPUT);
  pinMode(motorPlanta2, OUTPUT);
  pinMode(motorPlanta3, OUTPUT);

  pinMode(planta1, INPUT); //Configura o Pino do Sensor como Entrada

  lcd.init();   // INICIALIZA O DISPLAY LCD
  lcd.backlight(); // HABILITA O BACKLIGHT (LUZ DE FUNDO) 
  lcd.setCursor(0, 0); //SETA A POSIÇÃO EM QUE O CURSOR INCIALIZA(LINHA 1)
  lcd.print("Iniciando...");
  delay(3000);
  lcd.clear();

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi..");
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Conect. Wifi");
    delay(1000);
    lcd.print(".");
    delay(1000);
    lcd.print(".");
    delay(1000);
    lcd.print(".");
    delay(1000);
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("Conectado");
  lcd.setCursor(0, 1); 
  lcd.print("IP ");
  lcd.print(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  //server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/plain", readBME280Temperature().c_str());
  //});
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", umidadeSolo(planta1).c_str());
  });
  //server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/plain", readBME280Pressure().c_str());
  //});

  // Start server
  server.begin();

}

void loop()
{
  regarPlanta(umidadeSolo(planta1), motorPlanta1);
}
