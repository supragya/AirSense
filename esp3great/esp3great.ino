#include <SoftwareSerial.h>
#include <MQ135.h>
#include <DHT.h>
SoftwareSerial softSerial(2, 3);      // RX, TX pin for Arduino

#define DHTPIN A0                     // Analog pin for DHT11
#define DHTTYPE DHT11 

MQ135 gasSensor = MQ135(A1);
DHT dht(DHTPIN, DHTTYPE);

#define SSID "ut"                     // SSID - name of wifi (hotspot)
#define PASS ""               // PASS - password required to access wifi (hotspot)
#define IP "184.106.153.149"           // ThingSpeak IP

float t;
float h;
float f;
float hi;
float air_quality;
String result;
int ledPin = 13;
int ledPin2 = 7;

int r1 = 4;
int r2 = 5;
int r3 = 6;
int r4 = 10;
int r5 = 8;
int r6 = 9;
int r7 = 12;
int r8 = 11;

void setup() 
{
   
   uint32_t baud = 115200;
   Serial.begin(baud);
   dht.begin();
   softSerial.begin(baud);
   pinMode(ledPin, OUTPUT);
   pinMode(ledPin2, OUTPUT);

   pinMode(r1, OUTPUT);
   pinMode(r2, OUTPUT);
   pinMode(r3, OUTPUT);
   pinMode(r4, OUTPUT);
   pinMode(r5, OUTPUT);
   pinMode(r6, OUTPUT);
   pinMode(r7, OUTPUT);
   pinMode(r8, OUTPUT);

   testleds();
   connectWiFi();
}

void testleds(){

  digitalWrite(r1,LOW);
  digitalWrite(r2,LOW);
  digitalWrite(r3,LOW);
  digitalWrite(r4,LOW);
  digitalWrite(r5,LOW);
  digitalWrite(r6,LOW);
  digitalWrite(r7,LOW);
  digitalWrite(r8,LOW);
  
  int delaytime = 80;
  
  digitalWrite(r1,HIGH);
  delay(delaytime);
  digitalWrite(r1,LOW);

  digitalWrite(r2,HIGH);
  delay(delaytime);
  digitalWrite(r2,LOW);

      digitalWrite(r3,HIGH);
  delay(delaytime);
  digitalWrite(r3,LOW);

    digitalWrite(r4,HIGH);
  delay(delaytime);
  digitalWrite(r4,LOW);

    digitalWrite(r5,HIGH);
  delay(delaytime);
  digitalWrite(r5,LOW);

    digitalWrite(r6,HIGH);
  delay(delaytime);
  digitalWrite(r6,LOW);

    digitalWrite(r7,HIGH);
  delay(delaytime);
  digitalWrite(r7,LOW);

    digitalWrite(r8,HIGH);
  delay(delaytime);
  digitalWrite(r8,LOW);
}

void loop() 
{
  delay(6000);
  testleds();
  Serial.println("DHT11 and MQ135 test!");
  
  air_quality = gasSensor.getPPM();
  h = dht.readHumidity();
  t = dht.readTemperature();
  f = dht.readTemperature(true);
  
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  hi = dht.computeHeatIndex(f, h);

  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");
  Serial.print("Gas Level PPM : ");
  Serial.print(air_quality);
  Serial.println(" *PPM");


  digitalWrite(ledPin2, HIGH);
  delay(300);
  digitalWrite(ledPin2, LOW);
  delay(50);

  if(h > 60){
    digitalWrite(r4, HIGH);
  }
  if(h > 90){
    digitalWrite(r3, HIGH);
  }
  if(h > 100){
    digitalWrite(r2, HIGH);
  }
  if(h > 110){
    digitalWrite(r1, HIGH);
  }

  if (air_quality<=700)
  {
    digitalWrite(r6,HIGH);
    digitalWrite(r7,HIGH);
    digitalWrite(r8,HIGH);
    result = "0";       //"Pure Air"
  }
  else if(air_quality<=1500 && air_quality>700)
  {
    digitalWrite(r7,HIGH);
    digitalWrite(r8,HIGH);
    result = "1";       //"Poor Air"
  }

  else if (air_quality>1500 )
  {
    digitalWrite(r8,HIGH);
    result = "2";       //"Danger! Move to Fresh Air"
  }
  
  
  updateTS();
}

void updateTS()
{
    String cmd = "AT+CIPSTART=\"TCP\",\"";// Setup TCP connection
    cmd += IP;
    cmd += "\",80";
    sendDebug(cmd);
    delay(6000);

    String url = "GET /update?key=V8ICHOQB51BYJ8F4&field1="+String(t)+"&field2="+String(h)+"&field3="+String(hi)+"&field4="+String(f)+"&field5="+String(air_quality)+"&field6="+result+"\r\n\r\n\r\n\r\n\r\n\r\n";
    String stringLength="AT+CIPSEND=";
    stringLength +=String(url.length());

    Serial.println(stringLength);
    softSerial.println(stringLength);

   if( softSerial.find( ">" ) )
    {
      
     Serial.print(">");
     softSerial.print(url);
     Serial.print(url);
     delay(24000);
     
    }
    else
    {
     Serial.println("AT+CIPCLOSE Executing : ");
     sendDebug( "AT+CIPCLOSE" );    //close TCP connection
    }

    
}

void sendDebug(String cmd)
{
      Serial.print("SEND: ");
      softSerial.println(cmd);
      Serial.println(cmd);
}

boolean connectWiFi()
{
      softSerial.println("AT+CWMODE=1");      //Single mode of communication
      delay(6000);
      
      String cmd="AT+CWJAP=\"";               // Join accespoint (AP) with given SSID and PASS to be able to send data on cloud
      cmd+=SSID;
      cmd+="\",\"";
      cmd+=PASS;
      cmd+="\"";
      sendDebug(cmd);
      delay(6000);
      
      if(softSerial.find("OK"))
      {
       Serial.println("RECEIVED: OK");
       return true;
      }
      else
      {
       Serial.println("RECEIVED: Error");
       return false;
      }
}
