#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

struct config_t
{
  String workerSsid;
  String workerPassword;
  uint16_t workerAddress = 1;
  bool isServer = true;
  String dispatcherSsid = "abc"; // TODO
  String dispatcherPassword = "123"; // TODO
  String dispatcherHost;
};

WiFiServer server( 80 );
config_t config;

void setup()
{
  Serial.begin( 115200 );
  Serial.println();
  Serial.println( "Configuring access point..." );

  if ( config.workerSsid.length() == 0 )
  {
    const uint64_t chipid = ESP.getEfuseMac();
    config.workerSsid = "WLok_Worker_" + String( ( uint16_t )( chipid>>32 ), HEX ) + String( ( uint32_t ) chipid, HEX );
  }

  WiFi.mode( WIFI_AP_STA );
  WiFi.softAP( config.workerSsid.c_str(),
               config.workerPassword.length() > 0 ? config.workerPassword.c_str() : nullptr );
  const IPAddress accessPointIP = WiFi.softAPIP();
  Serial.printf( "Access Point SSID: %s\n", config.workerSsid.c_str() );
  Serial.print( "Access Point IP address: " );
  Serial.println( accessPointIP );

  delay( 500 );
  WiFi.begin( config.dispatcherSsid.c_str(), config.dispatcherPassword.c_str() );
  while ( WiFi.status() != WL_CONNECTED )
  {
    delay( 500 );
    Serial.print( "." );
  }
  Serial.println("");
  Serial.printf( "Connected to WiFi: %s\n", config.dispatcherSsid );
  Serial.printf( "WiFi IP address: %s.\n", WiFi.localIP().toString().c_str() );

  server.begin();

  Serial.println( "Server started" );
}

void loop()
{
  WiFiClient client = server.available();   // listen for incoming clients

  if ( client )
  {
    Serial.println();
    Serial.println( "New Client." );

    // an http request ends with a blank line
    bool currentLineIsBlank = true;
    String reqStr = "";
    bool skip = true;

    while ( client.connected() )
    {
      if ( client.available() )
      {
        char c = client.read();
        reqStr += c;

        // End of the line (received a newline character) and the line is blank, the http request has ended,
        // so you can send a reply
        if ( c == '\n' && currentLineIsBlank && reqStr.startsWith( "GET" ) )
        {
          sendPage( client );
          break;
        }
        if ( c == '\n' && currentLineIsBlank && reqStr.startsWith( "POST" ) && skip )
        {
          if ( skip )
          {
            skip = false;
            String dummy = reqStr.substring( reqStr.indexOf( "Content-Length:" ) + 15 );
            dummy.trim();
            int dataLength = dummy.toInt();
            String postData;
            while ( dataLength-- > 0 )
            {
              c = client.read();
              postData += c;
            }
            parsePostData( postData );
            reqStr += postData;
          }
          sendPage( client );
          break;
        }

        if ( c == '\n' )
        {
          // starting a new line
          currentLineIsBlank = true;
        }
        else if ( c != '\r' )
        {
          // a character on the current line
          currentLineIsBlank = false;
        }
      }
    }

    Serial.println( reqStr );

    // close the connection:
    delay( 1 );
    client.stop();
    Serial.println( "Client Disconnected." );
  }
}

void sendPage( WiFiClient &client )
{
  String message;

  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  message += "HTTP/1.1 200 OK\n";
  message += "Content-type: text/html\n";
  message += "Connection: close\n";
  message += "\n";

  message += "<!doctype html>\n";
  message += "<html>\n";
  message += "  <h1>WLok Worker Settings</h1>\n";
  message += "  <form method=\"post\">\n";

  message += "    <h2>Worker</h2>\n";
  message += "    <table border=0>\n";
  message += "      <tr>\n";
  message += "        <td>Address</td>\n";
  message += "        <td><input name=\"workerAddress\" type=\"number\" min=\"1\" max=\"255\" value=\"" +
             String( config.workerAddress ) + "\"></td>\n";
  message += "      </tr>\n";
  message += "      <tr>\n";
  message += "        <td>SSID</td>\n";
  message += "        <td><input name=\"workerSsid\" value=\"" + config.workerSsid + "\"></td>\n";
  message += "      </tr>\n";
  message += "      <tr>\n";
  message += "        <td>Password</td>\n";
  message += "        <td><input name=\"workerPassword\" type=\"password\" value=\"" + config.workerPassword +
             "\"></td>\n";
  message += "      </tr>\n";
  message += "      <tr>\n";
  message += "        <td>Mode</td>\n";
  message += "        <td><fieldset>\n";
  message += "          <input type=\"radio\" id=\"client\" name=\"workerMode\" value=\"Client\"" +
             String( !config.isServer ? " checked=\"checked\"" : "" ) + ">\n";
  message += "          <label for=\"client\">Client</label>\n";
  message += "          <br>\n";
  message += "          <input type=\"radio\" id=\"server\" name=\"workerMode\" value=\"Server\"" +
             String( config.isServer ? " checked=\"checked\"" : "" ) + ">\n";
  message += "          <label for=\"server\">Server</label>\n";
  message += "        </fieldset></td>\n";
  message += "      </tr>\n";
  message += "    </table>\n";

  message += "    <h2>Dispatcher</h2>\n";
  message += "    <table border=0>\n";
  message += "      <tr>\n";
  message += "        <td>SSID</td>\n";
  message += "        <td><input name=\"dispatcherSsid\" value=\"" + config.dispatcherSsid + "\"></td>\n";
  message += "      </tr>\n";
  message += "      <tr>\n";
  message += "        <td>Pasword</td>\n";
  message += "        <td><input name=\"dispatcherPassword\" type=\"password\" value=\"" + config.dispatcherPassword +
             "\">\n";
  message += "      </tr></td>\n";
  message += "      <tr>\n";
  message += "        <td>Host</td>\n";
  message += "        <td><input name=\"dispatcherHost\" value=\"" + config.dispatcherHost + "\"></td>\n";
  message += "      </tr>\n";
  message += "    </table>\n";

  message += "    <p>\n";
  message += "    <button type=\"reset\">Cancel</button>\n";
  message += "    <button type=\"submit\">OK</button>\n";
  message += "  </form>\n";
  message += "</html>\n";
  // The HTTP response ends with another blank line:
  message += "\n";

  Serial.println( "Send:" );
  Serial.println( message );
  client.print( message );
}

void parsePostData( const String &line )
{
  int startPos = 0;
  int endPos = -1;

Serial.printf( "line=%s\n", line.c_str() );
  do
  {
    String part;
    endPos = line.indexOf( "&", startPos );

    if ( endPos < 0 )
    {
      part = line.substring( startPos, endPos );
    }
    else
    {
      part = line.substring( startPos, endPos );
    }
    //Serial.printf( "part=%s\n", part.c_str() );

    const int pos = part.indexOf( "=" );
    if ( pos > 0 )
    {
      String var = part.substring( 0, pos );
      String value = part.substring( pos + 1 );
      //Serial.printf( "var=%s value=%s\n", var.c_str(), value.c_str() );

      if ( var == "dispatcherHost" )
      {
        config.dispatcherHost = value;
      }
      else if ( var == "dispatcherPassword" )
      {
        config.dispatcherPassword = value;
      }
      else if ( var == "dispatcherSsid" )
      {
        config.dispatcherSsid = value;
      }
      else if ( var == "workerAddress" )
      {
        config.workerAddress = value.toInt();
      }
      else if ( var == "workerMode" )
      {
        config.isServer = ( value == "Server" );
      }
      else if ( var == "workerPassword" )
      {
        config.workerPassword = value;
      }
      else if ( var == "workerSsid" )
      {
        config.workerSsid = value;
      }
      else
      {
        Serial.printf( "Unkown POST data: %s\n", var.c_str() );
      }
    }

    startPos = endPos + 1;
  }
  while ( endPos > 0 );
}
