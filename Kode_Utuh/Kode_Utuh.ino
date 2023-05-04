const int trigPin = 15;
const int trigPin1 = 5;
const int trigPin2= 19;
const int echoPin = 4;
const int echoPin1 = 18;
const int echoPin2 = 21;
const int potPin = 34;
const int relayasam = 26;
const int relaybasa = 27;
float ph;
float Value=0;
float sisaair;
float sisaAsam;
float sisaBasa;
int tinggikolam;
int tinggiAsam;
int tinggiBasa;
int luas;
int volume;
float xbasa[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float xasam[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int Yasam[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
int Ybasa[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
float larasam;
float larbasa;
int delayasam;
int delaybasa;
float sumasam;
float sumbasa;
float avga = 0;
float avgb = 0;



#define SOUND_SPEED 0.034

long duration;
long duration1;
long duration2;
float distanceCm;
float distanceInch;
float distanceCm1;
float distanceCm2;

#include <Arduino.h>
#if defined(ESP8266)
  /* ESP8266 Dependencies */
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
#elif defined(ESP32)
  /* ESP32 Dependencies */
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
#endif
#include <ESPDash.h>

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

/* Your WiFi Credentials */
const char* ssid = "home1"; // SSID
const char* password = "wasken67"; // Password

/* Start Webserver */
AsyncWebServer server(80);

/* Attach ESP-DASH to AsyncWebServer */
ESPDash dashboard(&server); 

/* 
  Dashboard Cards 
  Format - (Dashboard Instance, Card Type, Card Name, Card Symbol(optional) )
*/
Card Air(&dashboard, HUMIDITY_CARD, "Ketinggian air kolam", "cm");
Card Asam(&dashboard, HUMIDITY_CARD, "Sisa Larutan Asam", "%");
Card Basa(&dashboard, HUMIDITY_CARD, "Sisa Larutan Basa", "%");
Card pHCard(&dashboard, GENERIC_CARD, "pH Kolam");
Card Tinggi(&dashboard, SLIDER_CARD, "Tinggi Kolam", "(m)", 1, 4);
Card tAsam(&dashboard, SLIDER_CARD, "Tinggi Wadah Larutan Asam", "(cm)", 1, 30);
Card tBasa(&dashboard, SLIDER_CARD, "Tinggi Wadah Larutan Basa", "(cm)", 1, 30);
Card Luas(&dashboard, SLIDER_CARD, "Luas Kolam", "(m^2)", 1, 10);
Chart chartasam(&dashboard, BAR_CHART, "Penggunaan Larutan Asam (l)");
Chart chartbasa(&dashboard, BAR_CHART, "Penggunaan Larutan Basa (l)");
Card avgAsam(&dashboard, GENERIC_CARD, "Perkiraan Asam (l)");
Card avgBasa(&dashboard, GENERIC_CARD, "Perkiraan Basa (l)");

void setup() {
  Serial.begin(115200);

  pinMode(potPin,INPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(relayasam, OUTPUT);
  pinMode(relaybasa, OUTPUT);

  Tinggi.attachCallback([&](int t){
    Serial.println("[Tinggi] Slider Callback Triggered: "+String(t));
    tinggikolam = t*100;
    Tinggi.update(t);
    dashboard.sendUpdates();
  });

  Luas.attachCallback([&](int l){
    Serial.println("[Luas] Slider Callback Triggered: "+String(l));
    luas = l*100;
    Luas.update(l);
    dashboard.sendUpdates();
  });

  tAsam.attachCallback([&](int ta){
    Serial.println("[Asam] Slider Callback Triggered: "+String(ta));
    tinggiAsam = ta;
    tAsam.update(ta);
    dashboard.sendUpdates();
  });

  tBasa.attachCallback([&](int tb){
    Serial.println("[Basa] Slider Callback Triggered: "+String(tb));
    tinggiBasa = tb;
    tBasa.update(tb);
    dashboard.sendUpdates();
  });
  
  /* Connect WiFi */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.printf("WiFi Failed!\n");
      return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  /* Start AsyncWebServer */
  server.begin();
  dashboard.setAuthentication("Delvin", "adminpass");
  digitalWrite(relayasam, LOW);
  digitalWrite(relaybasa, LOW);
}

void loop() {
  digitalWrite (relayasam, LOW);
  digitalWrite (relaybasa, LOW);
  /* Update Card Values */
  Air.update(sisaair);
  Asam.update(sisaAsam);
  Basa.update(sisaBasa);
  pHCard.update(ph);
  if (ph>7.5){
    chartasam.updateX(Yasam, 30);
    chartasam.updateY(xasam, 30);
  }
  else if (ph<6.5){
    chartbasa.updateX(Ybasa, 30);
    chartbasa.updateY(xbasa, 30);
  }
  avgAsam.update(avga);
  avgBasa.update(avgb);
  /* Send Updates to our Dashboard (realtime) */
  dashboard.sendUpdates();

  Value= analogRead(potPin);
  Serial.print(Value);
  Serial.print(" | ");
  float voltage=Value*(3.3/4095.0);
  ph=(3.3*voltage-2.1);
  Serial.println(ph);
  delay(500);
 
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm); // jarak sensor ke permukaan kolam
  sisaair = tinggikolam - distanceCm + 50; //+50 akomodasi posisi sensor
  delay(500);


  // Clears the trigPin
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration1 = pulseIn(echoPin1, HIGH);
  
  // Calculate the distance
  distanceCm1 = duration1 * SOUND_SPEED/2;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance1 (cm): ");
  Serial.println(distanceCm1);  
  sisaAsam =  ((tinggiAsam - distanceCm1 + 5)*10)/3; // +5 akomodasi posisi sensor
  delay(500);


    // Clears the trigPin
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration2 = pulseIn(echoPin2, HIGH);
  
  // Calculate the distance
  distanceCm2 = duration2 * SOUND_SPEED/2;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance2 (cm): ");
  Serial.println(distanceCm2);  
  sisaBasa = ((tinggiBasa - distanceCm2 + 5)*10)/3; //+5 akomodasi posisi sensor
  delay(500);

  volume = luas * tinggikolam;
  
  if (ph > 7.5 && sisaAsam > 0){
    sumasam = 0;
    Serial.println("Kondisi terlalu basa");
    larasam = -((pow(10,-ph)-pow(10,-7.4))*1000*volume/1.37);
    Serial.print("Penambahan asam: ");
    Serial.println(larasam);
     Serial.print("Delay asam: ");
    delayasam = floor(larasam*36);
    Serial.print(delayasam);
    for (int i = 0; i <= 29; i++) {
      Yasam[i] = Yasam[i] + 1;    
    }
    for (int i = 0; i <= 28; i++) {
      xasam[i] = xasam[i+1];
      sumasam = sumasam + xasam[i];
    }
    xasam[29] = larasam;
    sumasam = sumasam + larasam;
    larasam = 0;
    
    digitalWrite(relayasam, HIGH);
    delay(1000);
    digitalWrite(relayasam, LOW);
    delay(100);
    }

  else if (ph < 6.5 && sisaBasa > 0){
    sumbasa = 0;
    Serial.println("Kondisi terlalu asam"); 
    larbasa = (pow(10,-ph)-pow(10,-6.6))*1000*volume/1.37;  
    Serial.print("Penambahan basa: ");
    Serial.println(larbasa);
    Serial.print("Delay basa: ");
    delaybasa = floor(larbasa*36);
    Serial.print(delaybasa);
    for (int i = 0; i <= 29; i++) {
      Ybasa[i] = Ybasa[i] + 1;
      }
    for (int i = 0; i <= 28; i++) {
      xbasa[i] = xbasa[i+1];
      sumbasa = sumbasa + xbasa[i];
      }
    xbasa[29] = larbasa;
    sumbasa = sumbasa + larbasa;
    larbasa = 0;
    digitalWrite(relaybasa, HIGH);
    delay(1000);
    digitalWrite(relaybasa, LOW);
    delay(100);
    }  
  avga = sumasam/30;
  avgb = sumbasa/30;



}
