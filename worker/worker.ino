#include <Preferences.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define CONFIG_NAMESPACE           "wlokWorker"
#define CONFIG_WORKER_SSID         "workerSsid"
#define CONFIG_WORKER_PASSWORD     "workerPwd"
#define CONFIG_WORKER_ADDRESS      "workerAddr"
#define CONFIG_IS_SERVER           "isServer"
#define CONFIG_DISPATCHER_SSID     "dispatchSsid"
#define CONFIG_DISPATCHER_PASSWORD "dispatchPwd"
#define CONFIG_DISPATCHER_HOST     "dispatchHost"

#define LED 2

struct config_t
{
  String workerSsid;
  String workerPassword;
  uint16_t workerAddress = 1;
  bool isServer = true;
  String dispatcherSsid;
  String dispatcherPassword;
  String dispatcherHost;
};

config_t config;
Preferences preferences;
WiFiServer server( 80 );

void setup()
{
  Serial.begin( 115200 );
  Serial.println();

  pinMode( LED, OUTPUT );

  Serial.println( "Reading preferences..." );
  preferences.begin( CONFIG_NAMESPACE, true );
  config.workerSsid         = preferences.getString( CONFIG_WORKER_SSID );
  config.workerPassword     = preferences.getString( CONFIG_WORKER_PASSWORD );
  config.workerAddress      = preferences.getUShort( CONFIG_WORKER_ADDRESS, 1 );
  config.isServer           = preferences.getBool  ( CONFIG_IS_SERVER, true );
  config.dispatcherSsid     = preferences.getString( CONFIG_DISPATCHER_SSID );
  config.dispatcherPassword = preferences.getString( CONFIG_DISPATCHER_PASSWORD );
  config.dispatcherHost     = preferences.getString( CONFIG_DISPATCHER_HOST );
  preferences.end();
  Serial.printf( "config.workerSsid         = %s\n", config.workerSsid.c_str() );
  Serial.printf( "config.workerPassword     = %s\n", config.workerPassword.c_str() );
  Serial.printf( "config.workerAddress      = %d\n", config.workerAddress );
  Serial.printf( "config.isServer           = %d\n", config.isServer );
  Serial.printf( "config.dispatcherSsid     = %s\n", config.dispatcherSsid.c_str() );
  Serial.printf( "config.dispatcherPassword = %s\n", config.dispatcherPassword.c_str() );
  Serial.printf( "config.dispatcherHost     = %s\n", config.dispatcherHost.c_str() );
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

  if ( !config.isServer )
  {
    delay( 500 );
    WiFi.begin( config.dispatcherSsid.c_str(), config.dispatcherPassword.c_str() );
    int tries = 20;
    while ( WiFi.status() != WL_CONNECTED && tries-- > 0 )
    {
      delay( 500 );
      Serial.print( "." );
    }
    Serial.println("");
    if ( tries > 0 )
    {
      Serial.printf( "Connected to WiFi: %s\n", config.dispatcherSsid );
      Serial.printf( "WiFi IP address: %s.\n", WiFi.localIP().toString().c_str() );
      digitalWrite( LED, HIGH );
    }
    else
    {
      Serial.printf( "Connecting to WiFi failed: %s\n", config.dispatcherSsid );
    }
  }

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
    bool valueChanged = false;

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
            valueChanged = parsePostData( postData );
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

    if ( valueChanged )
    {
      preferences.begin( CONFIG_NAMESPACE, false );
      preferences.putString( CONFIG_WORKER_SSID,         config.workerSsid );
      preferences.putString( CONFIG_WORKER_PASSWORD,     config.workerPassword );
      preferences.putUShort( CONFIG_WORKER_ADDRESS,      config.workerAddress );
      preferences.putBool  ( CONFIG_IS_SERVER,           config.isServer );
      preferences.putString( CONFIG_DISPATCHER_SSID,     config.dispatcherSsid );
      preferences.putString( CONFIG_DISPATCHER_PASSWORD, config.dispatcherPassword );
      preferences.putString( CONFIG_DISPATCHER_HOST,     config.dispatcherHost );
      preferences.end();

      ESP.restart();
    }
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

bool parsePostData( const String &line )
{
  int startPos = 0;
  int endPos = -1;
  bool valueChanged = false;

  //Serial.printf( "line=%s\n", line.c_str() );

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
        if ( config.dispatcherHost != value )
        {
          config.dispatcherHost = value;
          valueChanged = true;
        }
      }
      else if ( var == "dispatcherPassword" )
      {
        if ( config.dispatcherPassword != value )
        {
          config.dispatcherPassword = value;
          valueChanged = true;
        }
      }
      else if ( var == "dispatcherSsid" )
      {
        if ( config.dispatcherSsid != value )
        {
          config.dispatcherSsid = value;
          valueChanged = true;
        }
      }
      else if ( var == "workerAddress" )
      {
        if ( config.workerAddress != value.toInt() )
        {
          config.workerAddress = value.toInt();
          valueChanged = true;
        }
      }
      else if ( var == "workerMode" )
      {
        const bool isServer = ( value == "Server" );
        if ( config.isServer != isServer )
        {
          config.isServer = isServer;
          valueChanged = true;
        }
      }
      else if ( var == "workerPassword" )
      {
        if ( config.workerPassword != value )
        {
          config.workerPassword = value;
          valueChanged = true;
        }
      }
      else if ( var == "workerSsid" )
      {
        if ( config.workerSsid = value )
        {
          config.workerSsid = value;
          valueChanged = true;
        }
      }
      else
      {
        Serial.printf( "Unkown POST data: %s\n", var.c_str() );
      }
    }

    startPos = endPos + 1;
  }
  while ( endPos > 0 );

  return valueChanged;
}
