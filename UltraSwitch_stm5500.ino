#include "Adafruit_MCP23017.h"
#include <Ethernet_STM.h>

// remove "//" to uncomment DEBUG-Mode 
//#define DEBUG
//#define ULN_INVERT
//
//#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
//;
//#else
  byte mac[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
//#endif  

//////////////////////////////////////////////////////////////////// Change this IPs to your needs... /////////////////////////////////////
int numberOfRelayBoards = 2;            // how many boars you want to use...

IPAddress ip(192, 168, 1, 155);         // The IP Address you want to use...
IPAddress co(31, 31, 231, 42);          // Url where the .js and .css files are located!

// the dns server ip
IPAddress dnServer(192, 168, 1, 40);
// the router's gateway address:
IPAddress gateway(192, 168, 1, 40);
// the subnet:
IPAddress subnet(255, 255, 255, 0);

boolean useOwnJquery = false;           // if all has to be under your own control, also jquery must be local...
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long ctime;


/////////////////////////////// Nothing to change below this line ////////////////////////////////////////////////////////////////////////
String customUrl;
String contentUrl;

//int pinsOrder[16] = { 0,8,1,9,2,10,3,11,4,12,5,13,6,14,7,15 };
int pinsOrder[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
int isBoardInverted[8] = { 1,0,0,0,0,0,0,0 };

struct RelayCard
{
  Adafruit_MCP23017 mcp;
  
  int pins[16];
  int pinStatus[16];
};


RelayCard relay0;
RelayCard relay1;
RelayCard relay2;
RelayCard relay3;
RelayCard relay4;
RelayCard relay5;
RelayCard relay6;
RelayCard relay7;

RelayCard relayArray[8];

EthernetServer server(80);
char requestString[100];

void setup()
{
#if defined DEBUG
  Serial.begin(115200);
#endif

  for (int a = 0; a < 16; a++)
  {
    relay0.pins[a] = pinsOrder[a];
    relay1.pins[a] = pinsOrder[a];              
    relay2.pins[a] = pinsOrder[a];              
    relay3.pins[a] = pinsOrder[a];              
    relay4.pins[a] = pinsOrder[a];              
    relay5.pins[a] = pinsOrder[a];              
    relay6.pins[a] = pinsOrder[a];              
    relay7.pins[a] = pinsOrder[a];              
  }  
  
  relayArray[0] = relay0;
  relayArray[1] = relay1;
  relayArray[2] = relay2;
  relayArray[3] = relay3;
  relayArray[4] = relay4;
  relayArray[5] = relay5;
  relayArray[6] = relay6;
  relayArray[7] = relay7;

  for(int a = 0; a < numberOfRelayBoards; a++)
  {
    relayArray[a].mcp.begin(a);      // use default address 0
    
    for (int p = 0; p < 16; p++)
    {
      relayArray[a].mcp.pinMode(p, OUTPUT);

      if(isBoardInverted[a]==1)
          relayArray[a].mcp.digitalWrite(p, LOW);
      else      
          relayArray[a].mcp.digitalWrite(p, HIGH);
      delay(20);
    }
  }    

//#if defined(WIZ550io_WITH_MACADDRESS)
//  Ethernet.begin(ip);
//#else
  Ethernet.begin(mac, ip, dnServer, gateway, subnet);  
  server.begin();

#if defined DEBUG
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
#endif

}

void loop() {
  Webserver();
}

void Webserver(){
  EthernetClient client = server.available();
  if (client) {
    int charIndex = 0;

  ctime = millis() + 500;  // time within to get reply
    
/*
ctime = millis() + TIMEOUT_REPLY;  // time within to get reply
while ( client.connected() )
{
    if ( millis()>ctime )   // if no answer received within the prescribed time
        break;
    while ( client.available() )
        Time_ParseTCPData( client.read() );
}
client.stop();

*/

    while (client.connected()) {

      if ( millis()>ctime )   // if no answer received within the prescribed time
        break;
    
      if (client.available()) {
        char c = client.read();

        #if defined DEBUG
          Serial.write(c);
        #endif

        if (charIndex < 100) {
          requestString[charIndex] = c;
          charIndex++;
        }

        if (requestString[0] == 'G' && c == '\n') {
          char delimiter[] = " ";
          char *ptr;           
          char box[3][20];

          ptr = strtok(requestString, delimiter);
          
          for(int i=0;((ptr != NULL) && (i<3));i++) {
             strcpy(box[i], ptr);             
             ptr = strtok(NULL, delimiter);
          }
         
          unsigned long params[3];

          boolean cmdSet = strstr(box[1], "/Set");
          boolean cmdGet = strstr(box[1], "/Get"); // see if its a get request
          boolean cmdSa = strstr(box[1], "/Sa");

          if(cmdSet || cmdSa)
          {

            char limiter[] = "/";
            char *pr;           
            char paramsBox[4][6];
  
            pr = strtok(box[1], limiter);
            
            for(int i=0;((pr != NULL) && (i<4));i++) {
               strcpy(paramsBox[i], pr);             
               pr = strtok(NULL, limiter);
            }
  
            params[0] = strtoul(paramsBox[1], NULL, 10);
            params[1] = strtoul(paramsBox[2], NULL, 10);
            params[2] = strtoul(paramsBox[3], NULL, 10);
            
            #if defined DEBUG    
              Serial.print("Param0: ");
              Serial.println(params[0]);
              Serial.print("Param1: ");
              Serial.println(params[1]);
              Serial.print("Param2: ");
              Serial.println(params[2]);
            #endif
          }
        
          client.println("HTTP/1.1 200 OK");
          client.println("Access-Control-Allow-Origin: *");
          
          if(cmdSet>0 || cmdSa>0 || cmdGet >0)
          {
            if(cmdSet>0)
            {

            relayArray[params[0]].pinStatus[params[1]] = params[2]; // set the statusflag
            
            #if defined DEBUG
                  Serial.print("Pinstatus: ");
                  Serial.println(relayArray[params[0]].pinStatus[params[1]]);           
                  Serial.print("PinsNr: ");
                  Serial.println(relayArray[params[0]].pins[params[1]]);
            #endif
            
              if(isBoardInverted[params[0]]==1)
              {
                if(params[2] == 1) // write the correct pins
                  relayArray[params[0]].mcp.digitalWrite(relayArray[params[0]].pins[params[1]], HIGH);
              else
                relayArray[params[0]].mcp.digitalWrite(relayArray[params[0]].pins[params[1]], LOW);                
              }            
              else
              {                  
                if(params[2] == 1) // write the correct pins
                  relayArray[params[0]].mcp.digitalWrite(relayArray[params[0]].pins[params[1]], LOW);
                else
                relayArray[params[0]].mcp.digitalWrite(relayArray[params[0]].pins[params[1]], HIGH);     
              }
            }
            if(cmdSa>0)
            {
              for (int bits = 15; bits > -1; bits--) 
              {
                int value = 0;
        
                if (params[1] & (1 << bits))
                  value = 1;
                
                relayArray[params[0]].pinStatus[bits] = value;
        
                if(isBoardInverted[params[0]]==1)
                {
                  if(value == 1) // write the correct pins
                    relayArray[params[0]].mcp.digitalWrite(relayArray[params[0]].pins[bits], HIGH);
                  else
                    relayArray[params[0]].mcp.digitalWrite(relayArray[params[0]].pins[bits], LOW);        
                }
                else
                {
                  if(value == 1) // write the correct pins
                    relayArray[params[0]].mcp.digitalWrite(relayArray[params[0]].pins[bits], LOW);
                  else
                    relayArray[params[0]].mcp.digitalWrite(relayArray[params[0]].pins[bits], HIGH);        
                }
              }
            }
            client.println("Content-Type: application/json");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println();
            client.println(getPinStatus()); 
          }
          else
          { 
            String contentIP = co.toString();

            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println();
            client.println("<!DOCTYPE html><HTML><HEAD><meta http-equiv=\"Cache-control\" content=\"no-cache\"><meta http-equiv=\"Expires\" content=\"0\">");
            client.print("<script type=\"text/javascript\" src=\"http://");
            if(useOwnJquery)
            {
              client.print(contentIP);
              client.print("/h");
            }
            else
              client.print("code.jquery.com");
            client.println("/jquery-1.11.2.min.js\"></script>");
            client.println("<script>");          
            client.print("var configAddress='http://");
            client.print(ip.toString());
            client.print("'; var numberOfRelayBoards=");
            client.print(numberOfRelayBoards);
            client.println(";</script>");
            client.print("<script src=\"http://");
            client.print(contentIP);
            client.println("/h/ultraswitch_v1.js\"></script><link rel=\"shortcut icon\" href=\"http://h.mmmedia-online.de/favicon.ico\"><TITLE>THE UltraSwitch</TITLE>");
            client.print("<script src=\"http://");
            client.print(contentIP);
            client.println("/h/ultraswitch_content_v1.js\"></script>");
            client.print("<link rel=\"Stylesheet\" href=\"http://");
            client.print(contentIP);            
            client.println("/h/ultraswitch_v1.css\" type=\"text/css\">");
            client.println("</HEAD>");
            client.println("<BODY><div id=\"content\"></div><script>addContent();</script></BODY></HTML>");  
          }      
          break;          
        }
      }
    }
    delay(1);
    client.stop();
  }
}

String getPinStatus(){
  String result;
  result = "{\"B\":"+String(numberOfRelayBoards);
  result += ",\"V\":[";

  for(int s=0; s < numberOfRelayBoards;s++)
  {
    result += getEncodedSum(relayArray[s].pinStatus);

    if(s+1 < numberOfRelayBoards)
      result += ",";
    else
      result += "],\"M\":0}";
  }
  return result;
}

unsigned int getEncodedSum(int* bin)
{
	unsigned int i = 1;
	unsigned int sum = 0;

	for (int m = 0; m < 16; m++)  // do it from the back...
	{
    if(bin[m] == 1)
      sum = sum + pow(2, m);
	}
	return sum;
}