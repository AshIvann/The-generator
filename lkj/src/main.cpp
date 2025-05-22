#include <Arduino.h>
//#include <Adafruit_GFX.h>    // Core graphics library
 //#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
 //#include <SPI.h>



// put function declarations here:
//  #define SS   10     //chip select
//  #define TFT_DC   8     //datacomand 
//  #define TFT_RST  9     //reset
//  //mosi 11
 //sck 13


 
 #define DATAOUT 11//MOSI
 #define DATAIN  12//MISO
 #define SPICLOCK  13//sck
 #define CHIPSELECT 10//ss
 
 //opcodes
 #define WREN  6
 #define WRDI  4
 #define RDSR  5
 #define WRSR  1
 #define READ  3
 #define WRITE 2
 
 byte eeprom_output_data;
 byte eeprom_input_data=0;
 byte clr;
 int address=0;
 //data buffer
 char buffer [128];

void fill_buffer()
{

  for (int I=0;I<128;I++)

  {

    buffer[I]=I;

  }
}
 
 
 char spi_transfer(volatile char data)
 {
 
   SPDR = data;                    // Start the transmission
 
   while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
 
   {
 
   };
 
   return SPDR;                    // return the received byte
 }
 
 void setup()
 {
 
   Serial.begin(9600);
 
  //  pinMode(DATAOUT, OUTPUT);
 
  //  pinMode(DATAIN, INPUT);
 
   pinMode(SPICLOCK,OUTPUT);
 
   pinMode(CHIPSELECT,OUTPUT);
 
   digitalWrite(CHIPSELECT,HIGH); //disable device
 
   // SPCR = 01010000
   //interrupt disabled,spi enabled,msb 1st,controller,clk low when idle,
   //sample on leading edge of clk,system clock/4 rate (fastest)
 
   SPCR = (1<<SPE)|(1<<MSTR);
 
   clr=SPSR;
 
   clr=SPDR;
 
   delay(10);
 
  }
   
 
 byte read_eeprom(int EEPROM_address)
 {
   //READ EEPROM
 
   int data;
 
   digitalWrite(CHIPSELECT,LOW);

 
   data = spi_transfer(0xFF); //get data byte


   digitalWrite(CHIPSELECT,HIGH); //release chip, signal end transfer
 
   return data;
 }

 
 void loop()
 {
 
   eeprom_output_data = read_eeprom(address);

 
   delay(50); //pause for readability
 }