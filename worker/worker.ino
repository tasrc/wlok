#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

String ssid;
String password;

WiFiServer server( 80 );

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  if ( ssid.length() == 0 )
  {
    const uint64_t chipid = ESP.getEfuseMac();
    ssid = "WLok_Worker_" + String( ( uint16_t )( chipid>>32 ), HEX ) + String( ( uint32_t )chipid, HEX );
  }

  WiFi.softAP( ssid.c_str(), password.length() > 0 ? password.c_str() : nullptr );
  IPAddress myIP = WiFi.softAPIP();
  Serial.print( "AP SSID: " );
  Serial.println( ssid );
  Serial.print( "AP IP address: " );
  Serial.println( myIP );

  server.begin();

  Serial.println( "Server started" );
}

void loop()
{
  WiFiClient client = server.available();   // listen for incoming clients

  if ( client )
  {                             // if you get a client,
    Serial.println();
    Serial.println( "New Client." );           // print a message out the serial port
    String currentLine;                       // make a String to hold incoming data from the client
    while ( client.connected() )              // loop while the client's connected
    {
      if ( client.available() )             // if there's bytes to read from the client,
      {
        char c = client.read();             // read a byte, then
        Serial.write( c );                  // print it out the serial monitor
        if ( c == '\n' )                    // if the byte is a newline character
        {
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if ( currentLine.length() == 0 )
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println( "HTTP/1.1 200 OK" );
            client.println( "Content-type:text/html" );
            client.println();

            String message;
            message += "<!doctype html>\n";
            message += "<html>\n";
            message += "  <h1>WLok Worker Settings</h1>\n";
            message += "  <form method=\"post\">\n";

            message += "  <h2>Worker</h2>\n";
            message += "  <label for=\"address\">Address</label>\n";
            message += "  <input id=\"address\">\n";
            message += "  <p>Mode:</p>\n";
            message += "  <fieldset>\n";
            message += "    <input type=\"radio\" id=\"client\" name=\"mode\" value=\"Client\">\n";
            message += "    <label for=\"client\">Client</label>\n";
            message += "    <br>\n";
            message += "    <input type=\"radio\" id=\"server\" name=\"mode\" value=\"Server\" checked=\"checked\">\n";
            message += "    <label for=\"server\">Server</label>\n";
            message += "  </fieldset>\n";

            message += "  <h2>Access Point</h2>\n";
            message += "  <label for=\"ssid\">SSID</label>\n";
            message += "  <input id=\"ssid\" value=\"" + ssid + "\">\n";
            message += "  <br>\n";
            message += "  <label for=\"password\">Password</label>\n";
            message += "  <input id=\"password\" type=\"password\" value=\"" + password + "\">\n";

            message += "  <h2>Client</h2>\n";
            message += "  <label for=\"wifi\">WiFi</label>\n";
            message += "  <input id=\"wifi\">\n";
            message += "  <br>\n";
            message += "  <label for=\"wifi_password\">Password</label>\n";
            message += "  <input id=\"wifi_password\" type=\"password\">\n";
            message += "  <br>\n";
            message += "  <label for=\"server\">Server Address/Name</label>\n";
            message += "  <input id=\"Server\">\n";

            message += "  <p>\n";
            message += "  <button type=\"reset\">Cancel</button>\n";
            message += "  <button type=\"submit\">OK</button>\n";
            message += "  </form>\n";
            message += "</html>\n";
            Serial.print( message );
            client.print( message );

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else
          {
            // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if ( c != '\r' )
        {
          // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
//          digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
//          digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println( "Client Disconnected." );
  }
}
