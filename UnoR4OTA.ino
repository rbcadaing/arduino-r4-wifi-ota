#include <WiFiS3.h>
#include <ArduinoOTA.h>
#include "arduino_secrets.h"

// Change this between uploads to see that an OTA update worked
const unsigned long BLINK_INTERVAL = 300;   // ms

unsigned long lastBlink = 0;
bool ledOn = false;

void setupOTA()
{
    // Static IP (outside router's DHCP pool on 192.168.254.0/24)
    IPAddress ip(192, 168, 254, 240);
    IPAddress gateway(192, 168, 254, 254);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress dns(192, 168, 254, 254);
    WiFi.config(ip, dns, gateway, subnet);

    Serial.println("[APP] Connecting to WiFi...");
    while (WiFi.begin(SECRET_SSID, SECRET_PASS) != WL_CONNECTED)
    {
        delay(2000);
    }

    // WiFiS3 can report connected before DHCP finishes, so wait for a real IP
    Serial.println("[APP] Waiting for IP address...");
    unsigned long start = millis();
    while (WiFi.localIP() == IPAddress(0, 0, 0, 0))
    {
        if (millis() - start > 15000)
        {
            Serial.println("[APP] No IP after 15 s, retrying WiFi...");
            WiFi.disconnect();
            delay(1000);
            WiFi.begin(SECRET_SSID, SECRET_PASS);
            start = millis();
        }
        delay(250);
    }

    Serial.print("[APP] Connected, IP: ");
    Serial.println(WiFi.localIP());

    // name shown in the IDE, upload password
    ArduinoOTA.begin(WiFi.localIP(), "UnoR4WiFi", "password", InternalStorage);
    Serial.println("[APP] OTA ready");
}

void setup()
{
    Serial.begin(115200);
    delay(2000);

    pinMode(LED_BUILTIN, OUTPUT);

    setupOTA();

    Serial.print("[APP] Blink interval: ");
    Serial.print(BLINK_INTERVAL);
    Serial.println(" ms");
}

void loop()
{
    ArduinoOTA.poll();   // must run often, so no delay() here

    // Non-blocking blink
    unsigned long now = millis();
    if (now - lastBlink >= BLINK_INTERVAL)
    {
        lastBlink = now;
        ledOn = !ledOn;
        digitalWrite(LED_BUILTIN, ledOn ? HIGH : LOW);
    }
}