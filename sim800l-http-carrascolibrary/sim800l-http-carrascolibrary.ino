//--------------------------------Start-Libraries Section----------------------------------------------------//

#include <gprs.h>
#include <SoftwareSerial.h>

//--------------------------------End-Libraries Section----------------------------------------------------//


//--------------------------------Start-Definition Section--------------------------------------------------//

#define TIMEOUT    15000
#define LED_PIN    13
#define TIMEOUTINTERNET 30000

//--------------------------------End-Definition Section--------------------------------------------------//


//--------------------------------Start-Object Creation--------------------------------------------------//

GPRS gprs;

//--------------------------------End-Object Creation--------------------------------------------------//


//--------------------------------Start-Variable Declaration-------------------------------------------//

//Boolean to be set to false if number is on contact
bool ledStatus = HIGH;

////Variable to hold last line of serial output from SIM800
char currentLine[500] = "";
int currentLineIndex = 0;

//Boolean to be set to true if number is found on contact
bool isIncontact = false;

// Variable to hold contact from call
char contact[13]; // changes this if needed (a contact name with more than 13 characters)

// Variable to hold number from call
char phonenumber[13];

// Variable to send SMS
char *number = NULL;

////Boolean to be set to true if call notificaion was found and next line is NOT empty
//bool nextLineIsCall = false;
bool nextValidLineIsCall = false;

////Boolean to be set to true if message notificaion was found and next
////line of serial output is the actual SMS message content
bool nextLineIsMessage = false;

//Boolean to be set to true if number is found on phonebook
bool isInPhonebook = false;

// String which holds the last line read from Serial activitiy
String lastLine = "";

// Integer indexes
int firstDollar = -1;
int secondDollar = -1;
int thirdDollar = -1;
int forthDollar = -1;
int fifthDollar = -1;
int firstPound = -1;
int secondQuote = -1;
int len = -1;
int j = -1;
int i = -1;
int f = -1;
int r = -1;

// Contact Counters
int OldCounter = 0;
int NewCounter = 0;

// Temporal variable when using LoadWhiteList() ClearWhiteList() whitelist
String tmp = "";

// String where whitelist will be definitely saved
String BuildString = "";

int out = false;

char response [200];
char whitelist [200];     // Might ge bigger if phone number is huge.


//--------------------------------End-Variable Declaration-------------------------------------------//



//------------------------------End-Libraries Section--------------------------------------------------------//


//--------------------------------Start-Setup Section-----------------------------------------------//

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  while (!Serial);

  // initialize digital pin 13 as an output.
  pinMode(LED_PIN, OUTPUT);

  //Write current status to LED pin
  digitalWrite(LED_PIN, ledStatus);

  Serial.println("Starting SIM800 Http Development");
  gprs.preInit();
  delay(1000);

  while (0 != gprs.init())
  {
    delay(1000);
    Serial.print("init error\r\n");
  }

  Serial.println("Init success");
  number = NULL;
  Serial.print("Initial value of number: ");
  Serial.println(strlen(number));
  number = "OKAA";
  Serial.print("Actual value of number: ");
  Serial.println(strlen(number));
  gprs.sendCmd("AT\r\n");
  //  if (0 != gprs.sendCmdAndWaitForResp("AT\r\n", "OK", 50000))  //ALWAYS CLEANS BUFFERRRRRRRRRRRRRRRRRRRR
  //    {
  //      ERROR("ERROR:");
  //      return;
  //    }
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  gprs.sendCmd("AT+CREG?\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  gprs.sendCmd("AT+SAPBR=3,1,\"ConType\",\"GPRS\"\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  gprs.sendCmd("AT+SAPBR=3,1,\"APN\",\"internet.movistar.ve\"\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  if (0 != gprs.sendCmdAndWaitForResp("AT+SAPBR=1,1\r\n", "OK", 50000)) //("AT+CMGF=1\r\n", "OK", TIMEOUT))
  {
    ERROR("ERROR:SAPBR-3");
    return;
  }
  Serial.println("3-Passed!");
  gprs.sendCmd("AT+SAPBR=2,1\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  gprs.sendCmd("AT+HTTPINIT\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  gprs.sendCmd("AT+HTTPPARA=\"URL\",\"www.castillolk.com.ve/WhiteList.txt\"\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  gprs.sendCmd("AT+HTTPACTION=0\r\n");
  Serial.println("Getting data, please wait...");
  delay(10000);
  //  Serial.println("Getting data, please wait...");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);

  //Data received Error Checking
  tmp = String(response);
  if (tmp.indexOf("200") >= 0)     //string you are expecting                   
  {
    Serial.println("Data Succesfully Received!");
  }
  else
  {
    Serial.println("There was a network error");
    gprs.sendCmd("AT+CPOWD=1\r\n"); // Normal power off
    gprs.cleanBuffer(response, sizeof(response));
    gprs.readBuffer(response, sizeof(response));
    Serial.println("System will restart, please wait...");
    Serial.println(response);
    delay(25000);
  }
  
  //"AT+HTTPREAD\r\n"
  gprs.sendCmd("AT+HTTPREAD\r\n");
  //delay(5000);
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  tmp = String(response);
  Serial.println("Printing tmp...");
  Serial.println(tmp);
  //AT+HTTPTERM
  gprs.sendCmd("AT+HTTPTERM\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  //AT+SAPBR=0,1
  if (0 != gprs.sendCmdAndWaitForResp("AT+SAPBR=0,1\r\n", "OK", 50000)) //("AT+CMGF=1\r\n", "OK", TIMEOUT))
  {
    ERROR("ERROR:SAPBR-4");
    return;
  }
  Serial.println("4-Passed!");

  // Parsing tmp to determine whitelist
  Serial.println("Processing White List");
  firstDollar = tmp.indexOf('$');
  Serial.println(firstDollar);  //For debugging
  fifthDollar = tmp.indexOf('$', firstDollar + 17);
  Serial.println(fifthDollar); //For debugging
  firstPound = tmp.indexOf('#');
  Serial.println(firstPound);  //For debugging

  // Extracting White List
  j = 0;
  for (int i = fifthDollar + 1; i < firstPound; ++i)
  {
    whitelist[j] = tmp[i];
    ++j;
  }
  whitelist[j] = '\0'; // whitelist as a full string
  Serial.println("Printing White List");
  Serial.println(whitelist); //For Debugging
  BuildString = whitelist;
  Serial.println("Printing BuilString");
  Serial.println(BuildString);


  //  thirdDollar = tmp.indexOf('$', secondDollar + 1);
  //  Serial.println(thirdDollar);  //For debugging
  //  forthDollar = tmp.indexOf('$', thirdDollar + 1);
  //  Serial.println(forthDollar); //For debugging
  //  fifthDollar = tmp.indexOf('$', forthDollar + 1);
  //  Serial.println(fifthDollar); //For debugging


  //"AT+HTTPPARA=\"URL\",\"www.castillolk.com.ve/WhiteList.txt\"\r\n";

  //  gprs.sendCmd("AT+SAPBR=1,1\r\n");
  //  gprs.cleanBuffer(response, sizeof(response));
  //  gprs.readBuffer(response, sizeof(response));
  //  Serial.println("Printing Response");
  //  Serial.println(response);
  //

}
void loop() {
  // put your main code here, to run repeatedly:
  
  //ClearWhiteList();
  LoadWhiteList();
  Serial.println("Added...");
  delay(180000);
  

}

///////////////////////////////////////////////////////////////////////////
void ClearWhiteList()
{
  Serial.println("On ClearWhiteList()");
  
  String jj = "";
  j = 1 ;     // lleva la cuenta de los nros a borrar
  while( j <= OldCounter) 
  {
    jj = j;
    tmp   = "AT+CPBW="+jj+"\r\n";
    if(0 != gprs.sendCmdAndWaitForResp(tmp.c_str(), "OK", TIMEOUT)) 
    {
     ERROR("ERROR:CPBW");
     return;
    }     
    Serial.println(tmp);       // comando AT a ejecutar ??    
    j = j+1;
  }
}

////////////////////////////////////////////////////////////////////
void LoadWhiteList()
{
  Serial.println("On LoadWhiteList()");
  Serial.print("Value of BuildString");
  Serial.println(BuildString);
  
  // *************************
  // *************************
  // ojo  colocar  las intrucciones get  del sim800l

  //HttpClient client;

  ////String BuildString = "";   //Initialize string

  ////////////// Here GOES HTTP GET ROUTINE ////////////

  //client.get("http://castillolk.com.ve/WhiteList.txt");
  //
  // ejemplo WhiteList.txt
  // 10,05,04265860622,04275860622,04285860622,04295860622,04305860622,####
  //
  //while (client.available())
  //{
  //char      c = client.read();
  //BuildString = BuildString + c;
  //}

  ////////////// Here GOES NEW IMPLEMENTATION ////////////
 
  // This is what I will receive from "http://castillolk.com.ve/WhiteList.txt"
  // and will save in BuildString 10,05,04265860622,04275860622,04285860622,04295860622,04305860622,####

  ////BuildString = "03,05,04168262667,04275860622,04285860622,04295860622,04305860622,####";  //Deletes 3 contacts

  String jj   = ""; 
  tmp = BuildString.substring(0,2);  // Saves Old number of contacts in SIM in tmp
  Serial.println(tmp);
  OldCounter  = tmp.toInt();         // Converts String to Integer
  //Serial.println(OldCounter);
  tmp = BuildString.substring(3,5);  // Saves New number of contacts in SIM
  NewCounter = tmp.toInt();          // Converts String to Integer
  ClearWhiteList();

  ////////////// Here Adds New Contacts ////////////
  
  f = 6;         // aqui comienzan los nros de telefono
  j = 1;         // lleva la cuenta de los nros a cargar
  
  while(j <= NewCounter)
  {
    r = f+11; //  nros son de largo 11 ejm 04265860622
    tmp = BuildString.substring(f, r);
    jj = j;
    tmp   = "AT+CPBW="+jj+",\""+tmp+"\",129,\""+jj+"\"\r\n";
    if(0 != gprs.sendCmdAndWaitForResp(tmp.c_str(), "OK", TIMEOUT)) 
    {
     ERROR("ERROR:CPBW");
     return;
    }     
    Serial.println(tmp);
    f = f+12;  //  12 para saltar la coma ,
    j = j+1;
  }                  
}

//////////////////////////////////////////////////////////////////////////////////////////////
