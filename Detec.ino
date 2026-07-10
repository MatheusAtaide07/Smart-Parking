// Make sure to install mobzit firebase-esp32-client
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
const char* ssid     = "Wi-Fi_Network_Name";
const char* password = "Wi-Fi_Password";

// Firebase
#define API_KEY      "Firebase_API_Key"
#define DATABASE_URL "Firebase_URL"
FirebaseData   fbdo;
FirebaseAuth   auth;
FirebaseConfig config;
bool signupOK = false;

// Pins Setup
const int trigPin1 = 33;
const int echoPin1 = 32;
const int trigPin2 = 27;
const int echoPin2 = 26;

const int th = // alter it to determine the how close the car must be to count it as occupied

long  duration1, duration2;
float distance1, distance2;
int   statusSp1 = 0;
int   statusSp2 = 0;
unsigned long lastread = 0;
const unsigned long intervalRead = 1000;

long readSensor(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH, 30000);
}

void sendtoFirebase() {

  FirebaseJson json1;
  json1.set("Status", statusSp1);
  json1.set("Group", "Disabled or common");

  if (Firebase.RTDB.setJSON(&fbdo, "/Floor/Parkingspot1", &json1)) {
    Serial.println("Spot 1 → Firebase OK");
  } else {
    Serial.print("Spot 1 erro: ");
    Serial.println(fbdo.errorReason());
  }

  FirebaseJson json2;
  json2.set("Status", statusSp2);
  json2.set("Group", "Disabled or common");

  if (Firebase.RTDB.setJSON(&fbdo, "/G1/Parkingspot2", &json2)) {
    Serial.println("Spot 1 → Firebase OK");
  } else {
    Serial.print("Spot 1 erro: ");
    Serial.println(fbdo.errorReason());
  }
}

void setup() {
  Serial.begin(115200);

  // sensors pin's
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 30) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WiFi. Restarting...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("\nWiFi conected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Firebase — no autentication
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if(Firebase.signUp(&config, &auth, "", "")){
    signupOK = true;
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  while (!Firebase.ready()) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nFirebase ready!");
}

void loop() {
  unsigned long now = millis();

  if (now - lastread >= intervalRead) {
    lastread = now;

    statusSp1 = 0;
    statusSp2 = 0;

    // Sequencial read to deny interference between sensors
    duration1 = readSensor(trigPin1, echoPin1);
    delay(30);
    duration2 = readSensor(trigPin2, echoPin2);
    // (999 = no echo / timeout)
    distance1 = (duration1 > 0) ? duration1 * 0.0343 / 2 : 999;
    distance2 = (duration2 > 0) ? duration2 * 0.0343 / 2 : 999;
    if (distance1 > th) {statusSp1 = 1};
    if (distance2 > th) {statusSp2 = 1};

    sendtoFirebase();
  }
}