#include <String.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
byte ip[] = { 192, 168, 0, 247 }; // indirizzo IP della shield
byte gateway[] = { 192, 168, 0, 1 }; // indirizzo ip del router
byte subnet[] = { 255, 255, 255, 0 }; //subnet mask
EthernetServer server(80); 

int ledPin = 9; // LED pin
String readString; 
boolean LEDON = false; 

void setup() {
  Ethernet.begin(mac, ip, gateway, subnet);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        readString.concat(c); 

        if (c == '\n' && currentLineIsBlank) {
          Serial.print(readString);
          if (readString.indexOf("Led=1") > 0) {
            digitalWrite(ledPin, HIGH); 
            LEDON = true;
          } else {
            digitalWrite(ledPin, LOW); 
            LEDON = false;
          }
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.print("<html><head><title>Megalab.it by MGTeck & Gabry 1609</title><meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1' ></head><body>");
          if (LEDON) {
            client.println("<img src=http://img849.imageshack.us/img849/1513/accesao.jpg />");
          } else {
            client.println("<img src=http://img10.imageshack.us/img10/4719/spenta.jpg />");
          }
          client.print("<h2><a href='/?Led=1'>ACCENDI</a> | <a href='/?Led=0'>SPEGNI</a></h2>");
          client.println("</body></html>");
          readString="";
          client.stop();
        } 
      } 
    } 
  } 
}
