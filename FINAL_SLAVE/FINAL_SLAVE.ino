#include <SoftwareSerial.h>   //Software Serial Port
#include <Servo.h> //part of black line program
#define RxD 7
#define TxD 6
Servo servoLeft;//part of black line program
Servo servoRight;//part of black line program
#define DEBUG_ENABLED  1
SoftwareSerial blueToothSerial(RxD,TxD);

void setup()
{
    pinMode(10, INPUT);  pinMode(9, OUTPUT); //part of black line program
    pinMode(3, INPUT);  pinMode(2, OUTPUT); //part of black line program
    Serial.begin(9600);//part of black line program
    pinMode(RxD, INPUT);
    pinMode(TxD, OUTPUT);
    setupBlueToothConnection();
}

boolean start_flag = 0;
boolean start_message = 0;

void loop()
{
  char recvChar;
  
      if(blueToothSerial.available())           //check if there's any data sent from the remote bluetooth shield
      {
        recvChar = blueToothSerial.read();
        
        if(recvChar == '1')
        {
          start_flag = 1;
        }
      }
      
  if(Serial.available())  //check if there's any data sent from the local serial terminal, you can add the other applications here
      {
        recvChar  = Serial.read();
        
        blueToothSerial.print(recvChar);
      }

    if(start_flag == 1)
    {

      if (start_message == 1)
      {
      Serial.print("the program has started");  //print to local serial monitor (this loop ensures it is only printed once)
    
      blueToothSerial.print("1");               // send confirmation signal to master serial monitor
     
      start_message = 1;
      }
      
      servoLeft.attach(12); //RIGHT WHEEL
    
      servoRight.attach(13); // THIS IS LEFT WHEEL
      
      int irLeft = irDetect(9, 10, 38000);    // Check for object on the left sensor
      
      int irRight = irDetect(2, 3, 38000);    // Check for object on the right sensor
      
      servoRight.writeMicroseconds(1700);
      
      servoLeft.writeMicroseconds(1300);
      
      if (irLeft == 0) 
      {
        servoRight.writeMicroseconds(1500);   // Left wheel stop
        
        servoLeft.writeMicroseconds(1300);    // Right wheel clockwise
        
        delay(200);                           // Maneuver for time ms
      }
      
      if (irRight == 0) 
      {
        servoRight.writeMicroseconds(1700);   // Left wheel clockwise
        
        servoLeft.writeMicroseconds(1500);    // Right wheel stop
        
        delay(200 );                          // Maneuver for time ms
      }
    }
}

void setupBlueToothConnection()
{
    blueToothSerial.begin(38400);                           // Set BluetoothBee BaudRate to default baud rate 38400
 
    blueToothSerial.print("\r\n+STWMOD=0\r\n");             // set the bluetooth work in slave mode
    
    blueToothSerial.print("\r\n+STNA=Slave29+\r\n");        // set the bluetooth name as "SeeedBTSlave"
    
    blueToothSerial.print("\r\n+STOAUT=1\r\n");             // Permit Paired device to connect me
    
    blueToothSerial.print("\r\n+STAUTO=0\r\n");             // Auto-connection should be forbidden here
    
    delay(2000);                                            // This delay is required.
    
    blueToothSerial.print("\r\n+INQ=1\r\n");                // make the slave bluetooth inquirable
    
    Serial.println("The slave bluetooth is inquirable!");
    
    delay(2000);                                            // This delay is required.
    
    blueToothSerial.flush();
}


// IR Object Detection Function

int irDetect(int irLedPin, int irReceiverPin, long frequency)
{
  tone(irLedPin, frequency, 8);              // IRLED 38 kHz for at least 1 ms
  
  delay(1);                                  // Wait 1 ms
  
  int ir = digitalRead(irReceiverPin);       // IR receiver -> ir variable
  
  delay(1);                                  // Down time before recheck
  
  return ir;                                 // Return 1 no detect, 0 detect
}
