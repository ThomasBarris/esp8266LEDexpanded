#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <DNSServer.h>

// ESP8266 NodeMCU v3 has the LED on Pin 2
#define LED_BUILTIN 2 

// let's create a webserver and DNS server
ESP8266WebServer server(80);
DNSServer dnsServer;

const byte DNS_PORT = 53;

// our IP address. Since there is no connection to the rest of the world, we can choose any
IPAddress apIP(192, 168, 1, 1);

// state of the LED
int stateLED = LOW;

// generates the html based on value of variable stateLED
void response();

//for document root '/' calls response() to build the html
void handleRoot();
//for .../LEDOn -> turning LED on and calls response() to build the html
void handleLedOn();
//for .../LEDOff -> turning LED on and calls response() to build the html
void handleLedOff();

void setup() {

    //start serial for some output 
    Serial.begin(115200);
    Serial.println();

    // setup the ESP8266 as access point with given IP and SSID
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP("lalilu");
    
    Serial.print("visit: \n");
    Serial.println(apIP);

    // modify TTL associated  with the domain name (in seconds)
    // default is 60 seconds, 
    dnsServer.setTTL(300);
    // set which return code will be used for all other domains (e.g. sending
    // ServerFailure instead of NonExistentDomain will reduce number of queries
    // sent by clients)
    // default is DNSReplyCode::NonExistentDomain
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

  // start DNS server for a specific domain name
    dnsServer.start(DNS_PORT, "/", apIP);

    // define the functions for the different links 
    server.on("/", handleRoot);
    server.on("/LEDOn", handleLedOn);
    server.on("/LEDOff", handleLedOff);

    // start the Webserver
    server.begin();
    
    Serial.println("HTTP server beginned");

    // set LED Pin to output mode and set it to Low
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, stateLED);
}

// keep the webserver and DNS server listening. Everything else is made by the two servers
void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}

// if document root is requested...just generate response with current state
void handleRoot() {
    response();
}

// if LED Off button is clicked, change LED state to low, turn the LED off and generate a html accordingly
void handleLedOn() {
  stateLED = LOW;
  digitalWrite(LED_BUILTIN, stateLED);
  response();
}

// if LED On button is clicked, change LED state to high, turn the LED on and generate a html accordingly
void handleLedOff() {
  stateLED = HIGH;
  digitalWrite(LED_BUILTIN, stateLED);
  response();
}

// defines the different parts of the html
const String HtmlHtml = "<html><head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" /></head>";
const String HtmlHtmlClose = "</html>";
const String HtmlTitle = "<h1>Arduino-er: ESP8266 AP WebServer exercise</h1><br/>\n";
const String HtmlLedStateLow = "<big>LED is now <b>ON</b></big><br/>\n";
const String HtmlLedStateHigh = "<big>LED is now <b>OFF</b></big><br/>\n";
const String HtmlButtons = 
    "<a href=\"LEDOn\"><button style=\"display: block; width: 100%;\">ON</button></a><br/>"
    "<a href=\"LEDOff\"><button style=\"display: block; width: 100%;\">OFF</button></a><br/>";


// build the html
void response(){
  String htmlRes = HtmlHtml + HtmlTitle;
  if(stateLED == LOW){
    htmlRes += HtmlLedStateLow;
  }else{
    htmlRes += HtmlLedStateHigh;
  }

  htmlRes += HtmlButtons;
  htmlRes += HtmlHtmlClose;

  server.send(200, "text/html", htmlRes);
}
