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
 
 byte clr;
 int address=0;
 
 
 char spi_transfer(volatile char data)
 {
   SPDR = data;                    // Start the transmission
 
   while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
   {};
 
   return SPDR;                    // return the received byte
 }
 


void settings_spi()
{
  pinMode(SPICLOCK,OUTPUT); 
  pinMode(CHIPSELECT,OUTPUT);
  pinMode(DATAOUT,OUTPUT);

  // SPCR = 01010000
  //interrupt disabled,spi enabled,msb 1st,controller,clk low when idle,
  //sample on leading edge of clk,system clock/4 rate (fastest)

  SPCR = (1<<SPE)|(1<<MSTR);
  clr=SPSR;
  clr=SPDR;
}

 void setup()//аналог SPISettings
 {
    settings_spi();
}
   
 
 byte send_SPI_byte(uint8_t val1)
 {
   int data;
 
   digitalWrite(CHIPSELECT,LOW);
   data = spi_transfer(val1); //get data byte

   digitalWrite(CHIPSELECT,HIGH); //release chip, signal end transfer
 
   return data;
 }

 
 void loop()
 {
   
   address++;
  uint8_t a = send_SPI_byte(address);
 
   delay(100); //pause for readability
 }