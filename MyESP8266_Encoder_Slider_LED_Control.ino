//23 FUNZT  Heartbeat LED und gelbe LED werden über LDR in de Helligkeit geregelt
//22 FadeIn/Out FUNZT noch ohne LDR
//Basis20  FadeOut FUNZT mit Led Encoder Slider
//Basis10

//Basis 09 Encoder eingepflegt ohne FadeIn/Out kein Absturtz
//09 mit blink und reset FUNZT aber ohne Encoder
//Basis 06
//Slider/Led folgt Encoder und andersrum noch kein fadeIn/Out kein Absturtz
//Encoder wird im Phone korret angezeigt
//Encoder eingepflegt



// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <neotimer.h>
#include <RotaryEncoder.h>
#include <Bounce2.h>
Bounce bounce = Bounce();
#define ROTARYSTEPS 5
#define ROTARYMIN 0
#define ROTARYMAX 255
RotaryEncoder encoder(4, 0, RotaryEncoder::LatchMode::TWO03);  //(D2 + D3)
int lastPos = -1;
int sensorValue = 0;
int w = 0;
int c = 0;

// Replace with your network credentials
const char* ssid = "Your_Network";
const char* password = "Your_Password";//Don´t forget to change the static IP address


const int Light = 12;     //RED (D6)
const int LightAna = 16;  //(D0) BLUE
const int button = 13;    //(D7)
const int LdrLed = 15;    // (D8)
boolean FadeIn = false;
boolean FadeOut = false;
boolean Blink = false;
boolean Reboot = false;
boolean runLoop = true;
String sliderValue = "0";

boolean neuerWert = false;
const char* PARAM_INPUT = "value";
Neotimer HeartBeat1 = Neotimer(1000);  //timer
Neotimer HeartBeat2 = Neotimer(800);   //timer
Neotimer HeartBeat3 = Neotimer(200);   //timer
Neotimer HeartBeat4 = Neotimer(200);   //timer
Neotimer FadeIn1 = Neotimer(10);       //timer fade in time steps
Neotimer FadeOut1 = Neotimer(10);      //timer fade in time steps
Neotimer ReadSensor = Neotimer(100);   //Read LDR Sensor
Neotimer reconnect = Neotimer(1000);

int i = 0;
int j = 0;
int k = 0;
String Pos;
String inputMessage;
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="3">
  <title>ESP Entrance Lights</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 25px;}
    .slider { -webkit-appearance: none; marginz: 14px; width: 360px; height: 25px; background: #FFD65C;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .slider::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; } 
  </style>
</head>
<body>
  <h2>Entrance Lights</h2>
  <p><span id="textSliderValue">%SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="pwmSlider" min="0" max="255" value="%SLIDERVALUE%" step="1" class="slider"></p>
  <p>&nbsp;</p>
  <font color="blue">
  <p>Horsepower Electronic</p>
   </font>
  <font color="grey">
  <p>&copy; rb April 2025 &#x265E;</p>
  </font>
<script>
function updateSliderPWM(element) {
  var sliderValue = document.getElementById("pwmSlider").value;
  document.getElementById("textSliderValue").innerHTML = sliderValue;
  console.log(sliderValue);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?value="+sliderValue, true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var) {
  if (neuerWert) {
    neuerWert = false;
    inputMessage = sliderValue;
    return sliderValue;
  }
  if (var == "SLIDERVALUE") {
    return sliderValue;
  }
  return String();
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(74880);
  Serial.println("INFO: ");
  Serial.println(F(__FILE__ " " __DATE__ " " __TIME__));
  analogWrite(Light, sliderValue.toInt());
  // Neuer Zusatz für statische IP - Begin
  IPAddress ip(192, 168, 4, 2);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress gt(192, 168, 4, 254);
  WiFi.config(ip, gt, subnet);
  // Ende
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
        if (c <= 9) {
        w++;
        }
    delay(1000);
    Serial.println("Connecting to WiFi..");
    if (c = 8) {
      Serial.println("          B R E A K");
      runLoop = true;
      break;
    }
  }//end of while

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
  server.on("/slider", HTTP_GET, [](AsyncWebServerRequest* request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      sliderValue = inputMessage;
      analogWrite(Light, sliderValue.toInt());
      k = sliderValue.toInt();
    } else {
      inputMessage = "No message sent";
    }
    Serial.print("inputMessage = ");
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
  bounce.attach(button, INPUT_PULLUP);
  bounce.interval(5);  // interval in ms
  pinMode(Light, OUTPUT);
  pinMode(LightAna, OUTPUT);
  pinMode(LdrLed, OUTPUT);
  HeartBeat1.reset();
  HeartBeat1.start();
  ReadSensor.reset();
  ReadSensor.start();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Blink = true;
    runLoop = true;
    reconnect.reset();
    reconnect.start();
  } else {
    Blink = false;
    runLoop = true;
    //Reboot = true;
    digitalWrite(LightAna, LOW);  // turn LED OFF
    /*
    if (reconnect.done()) {
      if (w <= 9) {
        w++;
        //delay(500);
        Serial.print("count w = ");
        Serial.println(w);
        runLoop = false;
        //w++
        if (w >= 8) {
          Serial.print("           w = ");
          Serial.println(w);
          w = 0;
          runLoop = true;
        }
      }  // end of if w
      Serial.println("reconnect done");
      reconnect.reset();
      reconnect.start();
    }//reconnect.done
*/
  }  //end of else
  if (Reboot) {
    ESP.restart();
    Serial.println("Reboot");
    Reboot = false;
  }
  if (runLoop) {
    if (ReadSensor.done()) {  //Importand delay, without too much stress and breakdown
      sensorValue = analogRead(A0);
      sensorValue = map(sensorValue, 480, 1023, 255, 0);
      ReadSensor.reset();
      ReadSensor.start();
    }

    if (k >= 2) {                        //wenn letzter positiver Wert im Speicher
      analogWrite(LdrLed, sensorValue);  //mache LED an
    }
    bounce.update();
    encoder.tick();
    int newPos = encoder.getPosition() * ROTARYSTEPS;
    if (newPos < ROTARYMIN) {
      encoder.setPosition(ROTARYMIN / ROTARYSTEPS);
      newPos = ROTARYMIN;
    } else if (newPos > ROTARYMAX) {
      encoder.setPosition(ROTARYMAX / ROTARYSTEPS);
      newPos = ROTARYMAX;
    }  // if

    if (lastPos != newPos) {
      lastPos = newPos;
      sliderValue = newPos;
      inputMessage = sliderValue;
      analogWrite(Light, sliderValue.toInt());
      k = sliderValue.toInt();
      neuerWert = true;
    }
    if (bounce.changed()) {
      int deboucedInput = bounce.read();
      if (deboucedInput == LOW) {
        Serial.println("                button pressed");
        Serial.print("inputMessage = ");
        Serial.println(inputMessage);
        Serial.print("sliderValue = ");
        Serial.println(sliderValue);
        Serial.print("newPos = ");
        Serial.println(newPos);
        Serial.print("lastPos = ");
        Serial.println(lastPos);
        i = sliderValue.toInt();
        j = sliderValue.toInt();
        Serial.print("i = ");
        Serial.println(i);
        Serial.print("j = ");
        Serial.println(j);
      }

      if (i <= 0) {
        Serial.println("sliderValue <= 0");
        Serial.println("FadeIn");
        FadeIn = true;
        FadeIn1.reset();
        FadeIn1.start();
        j = lastPos;

        Serial.print(" Endwert k = ");
        Serial.println(k);
      }
      if (j >= 1) {
        Serial.println("sliderValue >= 1");
        Serial.println("FadeOut");
        FadeOut = true;
        FadeOut1.reset();
        FadeOut1.start();
        Serial.print("j = ");
        Serial.println(j);
      }
    }  //end of bounce.changed
    if (FadeOut) {
      if (FadeOut1.done()) {
        j = j - 1;
        analogWrite(Light, j);
        FadeOut1.reset();
        FadeOut1.start();
      }
      if (j <= 0) {
        FadeOut = false;
        FadeOut1.reset();
        Serial.print("j = ");
        Serial.println(j);
        inputMessage = j;
        sliderValue = j;
        newPos = j;
        encoder.setPosition(j / ROTARYSTEPS);  // start with the value of 0.
        Serial.print("inputMessage = ");
        Serial.println(inputMessage);
        Serial.print("sliderValue = ");
        Serial.println(sliderValue);
        Serial.print("newPos = ");
        Serial.println(newPos);
        Serial.print("lastPos = ");
        Serial.println(lastPos);
        Serial.print("k = ");
        Serial.println(k);
        i = sliderValue.toInt();
        j = sliderValue.toInt();
        Serial.print("i = ");
        Serial.println(i);
        lastPos = j;
      }  //if j <=0
    }    //end of fadeout
    if (FadeIn) {
      if (FadeIn1.done()) {
        i = i + 1;
        analogWrite(Light, i);
        FadeIn1.reset();
        FadeIn1.start();
      }
      if (i >= k) {
        FadeIn = false;
        FadeIn1.reset();
        Serial.print("i = ");
        Serial.println(i);
        Serial.print("k = ");
        Serial.println(k);
        inputMessage = i;
        sliderValue = i;
        newPos = i;
        encoder.setPosition(i / ROTARYSTEPS);  // start with the value of 0.
      }
    }             //end of FadeIn
    if (Blink) {  //wenn WiFi es
      if (HeartBeat1.done()) {
        //digitalWrite(LightAna, HIGH);  // turn LED ON
        analogWrite(LightAna, sensorValue);  // turn LED ON
        HeartBeat1.reset();
        HeartBeat2.reset();
        HeartBeat2.start();
      }
      if (HeartBeat2.done()) {
        digitalWrite(LightAna, LOW);  // turn LED OFF
        HeartBeat2.reset();
        HeartBeat3.reset();
        HeartBeat3.start();
      }
      if (HeartBeat3.done()) {
        analogWrite(LightAna, sensorValue);  // turn LED ON
        HeartBeat3.reset();
        HeartBeat4.reset();
        HeartBeat4.start();
      }
      if (HeartBeat4.done()) {
        digitalWrite(LightAna, LOW);  // turn LED OFF
        HeartBeat4.reset();
        HeartBeat1.reset();
        HeartBeat1.start();
      }
    }  //end of blink
  }    //end of loop
}  //end of runLoop
