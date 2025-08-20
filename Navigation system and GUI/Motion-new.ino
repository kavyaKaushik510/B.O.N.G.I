
//Code for ESP32 CAM
//Group 20
//B.O.N.G.I.
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "esp_camera.h"
#include <driver/ledc.h>
#include <ESP32Servo.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks

#define EAP_ANONYMOUS_IDENTITY "anonymous@tuke.sk" //anonymous@example.com, or you can use also nickname@example.com
#define EAP_IDENTITY "kshkav001@wf.uct.ac.za" //nickname@example.com, at some organizations should work nickname only without realm, but it is not recommended
#define EAP_PASSWORD "MeerutUP@200311" //password for eduroam account
#define EAP_USERNAME "kshkav001@wf.uct.ac.za" // the Username is the same as the Identity in most eduroam networks.

//const char* ssid = "Rachael iPhone";
//const char* password = "Rachael1234";
const char* ssid = "NSFW";
const char* password = "A904_wifi";

//const char* ssid = "eduroam"; // eduroam SSID

//Start up web server
WebServer server(80);

//Setup motor pins
#define IN1 12
#define IN2 13
#define IN3 14
#define IN4 15
#define CAMERA_MODEL_AI_THINKER //Camera module

//Servo Pins
#define TILT_PIN 2
#define SCOOPER_PIN1 4
Servo tiltServo;
Servo scooperR;

#define PWM_FREQ 30000
#define PWM_RES 8

//Max duty cycle
#define SPEED 180

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

void setupCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SVGA; //Setup camera configuration
  config.jpeg_quality = 8; //Setup JPEG quality
  config.fb_count = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  if (err == ESP_OK) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
  }
}

const char* guiHtml = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>EEE4113F Project</title>
  <link rel="icon" type="image/png" href="https://raw.githubusercontent.com/kavyaKaushik510/Penguin-Robot-Controller/201f9bd05b89d4a12c4acafada7e5a793f691cc9/penguin.png">
  <style>
    body {
      font-family: monofonto;
      text-align: center;
       margin:0;
      background-color: white;
    }

.header {
  background-color: #89CFF0;
  color: black;
  padding: 20px;
  font-family: Arial, sans-serif;

    }

.header-content {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 30px; /* or something similar */
}

    .header h1 {
      font-size: 60px;
      flex-direction: column;
      font-weight: bold;
      font-family: monofonto

    }


.text-group h1 {
  margin: 0;
  font-size: 36px;
  font-weight: bold;
  font-family: monofonto
  
}

.text-group p {
  margin: 15px 0 0 0; /* margin between title and subtitle */
  font-size: 16px;
 

}

.tagline {
  font-size: 14px;
  margin-top: 4px;
}


.penguin-icon {
  width: 90px;  /* increase from 60px */
  height: 90px; /* increase from 60px */
  background-color: white;
  border-radius: 50%;
  overflow: hidden;
  flex-shrink: 0;
}

.penguin-icon img {
  width: 100%;
  height: 100%;
  object-fit: contain;
}
    #status {
      padding: 10px 20px;
      background-color: #ffffff;
      border:2px inset #d3d3d3;
      border-radius: 8px;
      font-weight: 500;
      font-size: 16px;
      color: #333;
      box-shadow: inset 1px 1px 3px rgba(0, 0, 0, 0.1);
      text-align: center;
      min-width: 200px;
    }

    .status-wrapper {
      display: flex;
      justify-content: center;
      margin-bottom: 40px;
      margin-top:30px;
    }
    /* Button Grid */
    .button-grid {
      display: grid;
      grid-template-columns: repeat(3, 80px);
      gap: 20px;
      justify-content: center;
      align-items: center;
      margin: 30px auto;
    }

    .btn {
      background-color: white;
      border: none;
      border-radius: 12px;
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
      padding: 10px;
      cursor: pointer;
    }

    .btn img {
      width: 40px;
      height: 40px;
    }

    .btn:hover {
        background-color: #cce6ff;

    }

    .collect-btn {
      grid-column: 1 / span 3;
      background-color: white;
      color: black;
      padding: 10px;
      font-size: 18px;
      font-weight: bold;
      border-radius: 12px;
box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
      border: none;
      cursor: pointer;
    }

    .collect-btn:hover {
     background-color: #89CFF0;
  color: white;
}

.wifi-status {
  position: absolute;
  top: 5px;
  right: 15px;
  font-size: 14px;
  font-weight: bold;
  color: #787878;
  padding: 6px 12px;
  border-radius: 8px;

}

.header {
  background-color: #89CFF0;
  color: black;
  padding: 20px;
  text-align: center;
}

.title-row {
  display: inline-flex;
  align-items: center;
  gap: 20px;
  justify-content: center;
}

.title-row h1 {
  font-size: 40px;
  font-family: monofonto;
  font-weight: bold;
  margin: 0;
}

.penguin-icon {
  width: 70px;
  height: 70px;
  background-color: white;
  border-radius: 50%;
  overflow: hidden;
}

.penguin-icon img {
  width: 100%;
  height: 100%;
  object-fit: contain;
}

.subtitle {
  font-size: 16px;
  margin-top: 10px;
  font-family: monofonto;
}

  </style>
</head>
<body>

<div class="header">
  <div class="title-row">
    <h1>B.O.N.G.I.</h1>
    <div class="penguin-icon">
      <img src="https://raw.githubusercontent.com/kavyaKaushik510/Penguin-Robot-Controller/main/p.png" alt="Penguin">
    </div>
  </div>
  <p class="subtitle">Biological Observation and Navigation for Guano Investigationn</p>
</div>


 <div class="status-wrapper">
      <div id="status">Ready</div>
    </div>

    <div class="wifi-status" id="wifiStatus">WiFi Connected: {{ssid}} </div>


    <div class="button-grid">
    <!-- Row 1: Forward -->
    <div></div>
    <button class="btn" onclick="sendCommand('/forward')">
      <img src="https://raw.githubusercontent.com/kavyaKaushik510/Penguin-Robot-Controller/52195a19be3bafac938f77263055c1611ea13e56/up.png" alt="Forward">
    </button>
    <div></div>
    <!-- Row 2: Left, Stop, Right -->
    <button class="btn" onclick="sendCommand('/left')">
      <img src="https://raw.githubusercontent.com/kavyaKaushik510/Penguin-Robot-Controller/52195a19be3bafac938f77263055c1611ea13e56/left.png" alt="Left">
    </button>
    <button class="btn" onclick="sendCommand('/stop')">
      <img src="https://raw.githubusercontent.com/kavyaKaushik510/Penguin-Robot-Controller/6d677f7ca550da6934342a4fbf9ab26e3b37446a/stop.png" alt="Stop">
    </button>
    <button class="btn" onclick="sendCommand('/right')">
      <img src="https://raw.githubusercontent.com/kavyaKaushik510/Penguin-Robot-Controller/52195a19be3bafac938f77263055c1611ea13e56/right.png" alt="Right">
    </button>
    <!-- Row 3: Backward -->
    <div></div>
    <button class="btn" onclick="sendCommand('/backward')">
      <img src="https://raw.githubusercontent.com/kavyaKaushik510/Penguin-Robot-Controller/52195a19be3bafac938f77263055c1611ea13e56/down.png" alt="Backward">
    </button>
    <div></div>

    <!-- Row 4: Collect Sample button centered -->
    <div></div>
    <button class="collect-btn" onclick="sendCommand('/collect')">
      Collect Sample
    </button>
    <div></div>
  </div>
</body>
</html>

  <script>
    function updateStatus(message) {
      document.getElementById("status").innerHTML = message;
    }

    function sendCommand(command) {
      fetch(command)
        .then(response => response.text())
        .then(data => updateStatus(data))
        .catch(error => updateStatus("Error: " + error));
    }

    document.addEventListener('keydown', function(event) {
      event.preventDefault();
      switch(event.key) {
        case "ArrowUp": sendCommand('/forward'); break;
        case "ArrowDown": sendCommand('/backward'); break;
        case "ArrowLeft": sendCommand('/left'); break;
        case "ArrowRight": sendCommand('/right'); break;
        case " ": sendCommand('/stop'); break;
        case "c": sendCommand('/collect'); break; 
      }
    });
  </script>
</body>
</html>
)rawliteral";

//Increase linear motion of motor slowly
void rampMotors(int in1, int in2, int in3, int in4, bool forward, int maxSpeed=180, int step = 10, int delayMs = 70) {
  for (int speed = 0; speed <= maxSpeed; speed += step) {
    if (forward) {
      analogWrite(in1, 0);
      analogWrite(in2, speed);
      analogWrite(in3, 0);
      analogWrite(in4, speed);
    } else {
      analogWrite(in1, speed);
      analogWrite(in2, 0);
      analogWrite(in3, speed);
      analogWrite(in4, 0);
    }
    delay(delayMs);
  }
}

//Increment turning motion slowly
void rampTurn(bool left, int maxSpeed = 180, int step = 10, int delayMs = 70) {
  for (int speed = 0; speed <= maxSpeed; speed += step) {
    if (left) {
      analogWrite(IN1, 0);     // Left motor backward
      analogWrite(IN2, speed);
      analogWrite(IN3, speed); // Right motor forward
      analogWrite(IN4, 0);
    } else {
      analogWrite(IN1, speed); // Left motor forward
      analogWrite(IN2, 0);
      analogWrite(IN3, 0);     // Right motor backward
      analogWrite(IN4, speed);
    }
    delay(delayMs);
  }
}


// Navigation command - Forward
void moveForward(int speed) {
  rampMotors(IN1, IN2, IN3, IN4, true);
  Serial.print("Move Forward");
}
// Navigation command - Backwards
void moveBackward(int speed) {
  rampMotors(IN1, IN2, IN3, IN4, false);
   Serial.print("Move Backward");
}
// Navigation command - Turn Left
void turnLeft(int speed) {
 rampTurn(true, speed);
  Serial.print("Turn Left");
}
// Navigation command - Turn Right
void turnRight(int speed) {
   rampTurn(false, speed);
    Serial.print("Turn Right");
}
// Navigation command - Stop
void stopMotors() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
}

// Slowly increment servo angle
void smoothMove(Servo& servo1, Servo& servo2 ,int fromAngle, int toAngle, int delayPerStep = 25) {
  if (fromAngle < toAngle) {
    for (int pos = fromAngle; pos <= toAngle; pos++) {
      servo1.write(pos);
      servo2.write(180-pos);
      delay(delayPerStep);
    }
  } else {
    for (int pos = fromAngle; pos >= toAngle; pos--) {
      servo1.write(pos);
      servo2.write(180-pos);
      delay(delayPerStep);
    }
  }
}

//Setup processor 
void setup() {
  Serial.begin(115200);
  WiFi.disconnect(true);  //disconnect from WiFi to set new WiFi connection
  
  //esp network
  //WiFi.softAP(ssid, password);

    // Setup Wi-Fi hotspot
  WiFi.begin(ssid, password);

  //Setup eduroam
  //WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD); // without CERTIFICATE, RADIUS server EXCEPTION "for old devices" required

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  //setup with esp
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  //Serial.println(WiFi.softAPIP());

  setupCamera();

  //Setup input pins to output mode
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  //Set all motor pins to low
  stopMotors();


 //Setup servo pins
  tiltServo.attach(TILT_PIN);
  if (!tiltServo.attached()) {
  Serial.println("Tilt servo not attached!");
} else {
  tiltServo.write(30);
}


  scooperR.attach(SCOOPER_PIN1);
  if (!scooperR.attached()) {
  Serial.println("scooperR not attached!");
} else {
  scooperR.write(150);
}

  //get server commmands to interface GUI with esp server
  server.on("/", HTTP_GET, []() {
  String htmlContent = guiHtml;
  htmlContent.replace("{{ssid}}", ssid);  // Replace placeholder with actual SSID
  server.send(200, "text/html", htmlContent);
});

  server.on("/forward", []() { moveForward(180); server.send(200, "text/plain", "Moving Forward"); });
  server.on("/backward", []() { moveBackward(180); server.send(200, "text/plain", "Moving Backward"); });
  server.on("/left", []() { turnLeft(180); server.send(200, "text/plain", "Turning Left"); });
  server.on("/right", []() { turnRight(180); server.send(200, "text/plain", "Turning Right"); });
  server.on("/stop", []() { stopMotors(); server.send(200, "text/plain", "Stopped"); });

  server.on("/collect", []() {
    stopMotors(); //safety
    server.send(200, "text/plain", "Collecting Sample");
    stopMotors();

    //Lower the scooper
    smoothMove(tiltServo, scooperR, 70, 92, 25);
    delay(1000);
    //Scoop the samples by moving forward
    moveForward(180);
    delay(1000);
    //Lift the scoopers back up to initial position
    smoothMove(tiltServo,scooperR, 92,30, 25);
    delay(500);
    server.send(200, "text/plain", "Collected Sample");

  });

 //End point to send camera images
  server.on("/capture", HTTP_GET, []() {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) { server.send(500, "text/plain", "Camera capture failed"); return; }
    server.send_P(200, "image/jpeg", (char *)fb->buf, fb->len);
    esp_camera_fb_return(fb);
  });

  //Start server
  server.begin();
}

void loop() {
  server.handleClient();
}

