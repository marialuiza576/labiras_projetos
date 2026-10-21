#include <ThingerESP32.h>
#include <DHT.h>

#define USER_ID  "luiza"         
#define DEVICE_ID "esp_monitoramento"    
#define DEVICE_CREDENTIAL "biblioteca"
#define SSID   "Luiza"
#define SSID_PASSWORD "malu9947"

ThingerESP32 thing(USER_ID, DEVICE_ID, DEVICE_CREDENTIAL);

#define DHTPIN  15   
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define LEDg  5        
#define LEDy  18      
#define LEDr  19       
#define buzzer 23     
#define KY  34

float temperatura = 0.0;
int som = 0;
float decibeis = 0.0;

unsigned long tempoAnterior = 0;
const long intervalo = 2000;


float calcularDecibeis(int leitura) {
  if (leitura <= 500)       return 30.0;  
  else if (leitura <= 2000) return 50.0;  
  else                      return 75.0;  
}

void setup() {
  Serial.begin(115200);
  pinMode(LEDg, OUTPUT);
  pinMode(LEDy, OUTPUT);
  pinMode(LEDr, OUTPUT);
  pinMode(buzzer, OUTPUT);
  dht.begin();
  thing.add_wifi(SSID, SSID_PASSWORD);

  thing["dados_sensor"] >> [](pson& out){
    out["temperatura"] = temperatura;
    out["ruido"] = som;
    out["decibeis"] = decibeis;
  };
}

void loop() {
 digitalWrite(LEDy,HIGH)
 digitalWrite(LEDg,HIGH)
 digitalWrite(LEDr,HIGH)

  thing.handle();
  unsigned long tempoAtual = millis();
  
  if (tempoAtual - tempoAnterior >= intervalo) {
    tempoAnterior = tempoAtual;
    
    som = analogRead(KY);
    decibeis = calcularDecibeis(som);

    float leituraTemp = dht.readTemperature();
    
    if (isnan(leituraTemp)) {
      Serial.println("Falha ao ler o sensor DHT11");
    } else {
      temperatura = leituraTemp;
    }
    thing.write_bucket("dados_biblioteca", "dados_sensor");

//mensagem caso db>=75
    if (decibeis>=75 ){
        thing.call_endpoint("alerta_ruido");
        Serial.println("Alerta: Ruído está na faixa crítica! Endpoint acionado.");
            }
  
    Serial.print(F("Temperatura: "));
    Serial.print(temperatura, 1);
    Serial.print(F("°C | Ruído : "));
    Serial.print(som);
    Serial.print(F(" | Decibéis: "));
    Serial.print(decibeis, 0);
    Serial.println(F(" dB"));

    if (temperatura >= 18 && temperatura < 26) {
      digitalWrite(LEDg, LOW);
      digitalWrite(LEDy, HIGH); 
      digitalWrite(LEDr, LOW);
      noTone(buzzer);  
    } else if (temperatura >= 20 && temperatura <= 24) {  
      digitalWrite(LEDg, HIGH); 
      digitalWrite(LEDy, LOW);
      digitalWrite(LEDr, LOW); 
      noTone(buzzer);
    } else {
      digitalWrite(LEDg, LOW);
      digitalWrite(LEDy, LOW);
      digitalWrite(LEDr, HIGH);  
      tone(buzzer, 440); 
    }
  }
}
