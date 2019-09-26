#include <SoftwareSerial.h>                // Software Serial Port
#define RxD 7
#define TxD 6
#define DEBUG_ENABLED  1

String retSymb = "+RTINQ=";                // start symble when there's any return
String slaveName = ";Slave29";             // caution that ';'must be included, and make sure the slave name is right
int nameIndex = 0;
int addrIndex = 0;
String recvBuf;
String slaveAddr;
String connectCmd = "\r\n+CONN=";
SoftwareSerial blueToothSerial(RxD,TxD);
void setup()
{
    Serial.begin(9600);
    pinMode(RxD, INPUT);
    pinMode(TxD, OUTPUT);
    setupBlueToothConnection();
    //wait 1s and flush the serial buffer
    delay(1000);
    Serial.flush();
    blueToothSerial.flush();
    pinMode(3, INPUT); // OG code
}

int is_Pressed = 0; //establishing the is_pressed trigger

void loop() // main part of code that will loop continuously 
{
    char recvChar;
   
    while(1)
    {
      if(blueToothSerial.available())         //check if there's any data sent from the remote bluetooth shield
      {
        recvChar = blueToothSerial.read();

        if(recvChar == '1');
          {
            Serial.print("confirmation from slave: the program has started \t");  
            //confirming to user that the slave has sent feedback to master
          }
      }
        //if(Serial.available())                  
        //check if there's any data sent from the local serial terminal, you can add the other applications here
       
          int button = digitalRead(3);
        
          Serial.print(button);
        
          if (button == LOW)      //sets of is_pressed trigger
          {
            is_Pressed = 1;
          }

          if (is_Pressed == 1)
          {
            Serial.println("pressed"); //'pressed' will be printed every second while is_pressed has been set off
          }
          else
          {
           Serial.println("not pressed"); // not pressed will be printed otherwise
          }
          delay(1000); //delaying the loop 1000 milliseconds ( 1 second)

        if (is_Pressed == 1)
        {
          blueToothSerial.print("1"); 
          //if is pressed is triggered, the master will send the starting signal to the slave, starting the robot's movement
        }
    }
}

void setupBlueToothConnection()
{
    blueToothSerial.begin(38400);                               // Set BluetoothBee BaudRate to default baud rate 38400
   
    blueToothSerial.print("\r\n+STWMOD=1\r\n");                 // set the bluetooth work in master mode

    blueToothSerial.print("\r\n+STNA=Master29\r\n");            // set the bluetooth name as

    blueToothSerial.print("\r\n+STAUTO=0\r\n");                 // Auto-connection is forbidden here
   
    delay(2000);                                                // This delay is required.
 
    blueToothSerial.flush();
   
    blueToothSerial.print("\r\n+INQ=1\r\n");                    //make the master inquire
   
    Serial.println("Master is inquiring!");
  
    delay(2000); // This delay is required.


    //find the target slave
 
    char recvChar;
  
    while(1)
    {
        if(blueToothSerial.available())
        {
            recvChar = blueToothSerial.read();

            recvBuf += recvChar;

            nameIndex = recvBuf.indexOf(slaveName);     
            //get the position of slave name
            //decrease the ';' in front of the slave name, to get the position of the end of the slave address

            if ( nameIndex != -1 )
            {
                //Serial.print(recvBuf);

                addrIndex = (recvBuf.indexOf(retSymb,(nameIndex - retSymb.length()- 18) ) + retSymb.length());
                //get the start position of slave address

                slaveAddr = recvBuf.substring(addrIndex, nameIndex);//get the string of slave address

                break;
            }
        }
    }

    //form the full connection command

    connectCmd += slaveAddr;

    connectCmd += "\r\n";

    int connectOK = 0;

    Serial.print("Connecting to slave:");

    Serial.print(slaveAddr);

    Serial.println(slaveName);

    //connecting the slave till they are connected

    do

    {

        blueToothSerial.print(connectCmd);//send connection command

        recvBuf = "";

        while(1)
        {
            if(blueToothSerial.available()){

                recvChar = blueToothSerial.read();

                recvBuf += recvChar;

                if(recvBuf.indexOf("CONNECT:OK") != -1)
                
                    connectOK = 1;

                    Serial.println("Connected!");

                    blueToothSerial.print("Connected!");

                    break;
                
                else if(recvBuf.indexOf("CONNECT:FAIL") != -1)
                {
                    Serial.println("Connect again!");

                    break;
                }
            }
        }
        
    }
    
while(0 == connectOK);

}
