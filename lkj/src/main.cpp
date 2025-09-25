#include <Arduino.h>
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <math.h>
#include "stdint.h"
#include "GyverEncoder.h"
#include "Keypad.h"

//#include "power_table.h"
#include "func_for_generator.h"
#include "support_func.h"



// //Для энкодера 
// #define CLK 2
// #define DT 3
// #define SW 4

// //Фрагмент из кода для SPI используются для передачи данных на LMX2595
// #define DATAOUT 11//MOSI     
// #define DATAIN  12//MISO     
// #define SPICLOCK  13//sck    
// #define CHIPSELECT 10//ss    
 

 
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


  
void settings_spi();                                                      //настройка SPI


byte clr;
uint8_t address=0;
 

float power_counter = 10;
int click_counter = 1;


uint16_t freq = 75;          //частота которая вызывается в setup
uint16_t power = 5;         //мощность которая вызывается в setup

uint32_t increase_value = 1;     //переменная отвечающая за изменение значения частоты 
float power_increment = 1;         //переменная отвечающая за изменение значения мощности 
int position = 8;                

uint64_t freq_set_by_key = 0;      


// uint32_t key_last_six = freq_set_by_key % 1000000;
// uint32_t key_first_five = freq_set_by_key / 1000000;

// const byte ROWS = 4;   // Количество рядов 
// const byte COLS = 4;   // Количество строк 
 
// char keys[ROWS][COLS] = 
// {
//   {'1','2','3','A'},
//   {'4','5','6','B'},
//   {'7','8','9','C'},
//   {'*','0','#','D'}
// };

// byte rowPins[ROWS] = {A3, A2, A1, A0}; // Выводы, подключение к строкам
// byte colPins[COLS] = {6, 5, A5, A4}; // Выводы, подключение к столбцам  

// Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() 
{
  //settings_spi();
  tft.init(240, 320);
  tft.fillScreen(ST77XX_BLACK);    
  tft.setTextColor(ST77XX_WHITE);
  tft.setRotation(1);
  tft.setTextSize(3);
  
  Serial.begin(9600);
  enc1.setTickMode(TYPE2);

  set_generator();                                          
  tft.fillRect(165 , 31, 20, 3, ST77XX_BLUE);

  tft.setCursor(10, 30);
  tft.fillRect(8, 51, 240, 2, ST77XX_BLUE);
  tft.setTextColor(ST77XX_WHITE);                                    
  tft.print("Freq: ");
  tft.fillRect(8, 106, 265, 2, ST77XX_BLACK);

  tft.setCursor(10, 85);
  tft.setTextColor(ST77XX_WHITE);                                    
  tft.print("Power: ");
  tft.print(power);
  tft.fillRect(8, 51, 240, 2, ST77XX_BLACK);

  print_freq(freq_set_by_encoder, 100, 30);
}


void loop()
{
  enc1.tick(); 
  
  char key = keypad.getKey();
  if (key)
  {
    if(key >= '0' && key <= '9')
    {
      freq_set_by_key = (freq_set_by_key * 10) + (key - 48);

      tft.fillRect(0, 150, 200, 22, ST77XX_BLACK);       
      tft.setCursor(0, 150);
      tft.print((uint32_t)freq_set_by_key);
    }

    else if(key == 'A')
    {
      tft.setCursor(100,30);
      tft.fillRect(100, 30, 320, 21, ST77XX_BLACK);
      if(freq_set_by_key < 12e6)
      {
        freq_set_by_key = 12e6;
        tft.print("12.000000MHz");
        second_set_freq(freq_set_by_key);
      }
      else if(freq_set_by_key >= 19e9)
      {
        freq_set_by_key = 19e9;
        tft.print("19000.000000 MHz");
        second_set_freq(freq_set_by_key);
      }
      else
      {
        
        print_freq(freq_set_by_key, 100, 30);
        //set_power(find_power_level(number, power));
        second_set_freq(freq_set_by_key);
        //find_power_level(key_number, power);

      }
      freq_set_by_encoder = freq_set_by_key;
      freq_set_by_key = 0;             //отчищает переменную для новой записи 
    }
    


    else if(key == 'D')
    {
      freq_set_by_key = freq_set_by_key / 10;
      tft.fillRect(0, 150, 200, 22, ST77XX_BLACK);
      tft.setCursor(0, 150);
      tft.print((uint32_t)freq_set_by_key);
    }

  }

  if(enc1.isClick())
  {
    // при нажатии происходит выбор десятки на которую увеличивается число 
    // increase_value *= 10;
    // position--;
    // if(increase_value >= 1000000000)
    // {
    //   position = 8;
    //   increase_value = 1;
    // }
    // tft.fillRect(0, 31, 320, 3, ST77XX_BLACK);
    // tft.fillRect(position * 22, 31, 20, 3, ST77XX_BLUE);

    click_counter = click_counter + 1;
    if(click_counter % 2 == 1)
    {
      tft.setCursor(10, 30);
      tft.fillRect(8, 51, 240, 2, ST77XX_BLUE);
      tft.setTextColor(ST77XX_WHITE);
      //tft.setTextSize(2);
      tft.print("Freq: ");
      tft.fillRect(8, 106, 265, 2, ST77XX_BLACK);
    }
    else
    {
      tft.setCursor(10, 85);
      tft.fillRect(8, 106, 265, 2, ST77XX_BLUE);
      tft.setTextColor(ST77XX_WHITE);
      //tft.setTextSize(2);
      tft.print("Power: ");
      tft.fillRect(8, 51, 240, 2, ST77XX_BLACK);
    }


  }    

  if(click_counter % 2 == 1)    //изменение частоты 
  {
    if(enc1.isRight())
    {
      freq_set_by_encoder += increase_value;
      if(freq_set_by_encoder > 19e9) 
      {
        freq_set_by_encoder = 19e9;
      }
      print_freq(freq_set_by_encoder, 100, 30);

      second_set_freq(freq_set_by_encoder);
    }

    if(enc1.isLeft())
    {
      freq_set_by_encoder -= increase_value;
      if(freq_set_by_encoder < 12e6 )
      {
        freq_set_by_encoder = 12e6;
      }
      print_freq(freq_set_by_encoder, 100, 30);

      second_set_freq(freq_set_by_encoder);
    } 
  }

  else                    //изменение мощности 
  {
    if(enc1.isRight())       //увеличение на 1
    {
      power_counter += power_increment;
      tft.setCursor(110, 85);
      tft.fillRect(110, 85, 320, 21, ST77XX_BLACK); 
    
      if(power_counter >= 11)
      {
        power_counter = 11;                                                                   
      }
      
      tft.print(power_counter);
      power = power_counter;
      tft.print(" Units");

      //set_power(power);
      //set_out_power(find_power_level(freq_set_by_encoder, power, data_of_rigth_freq&, data_of_left_freq&));
      //tft.print();
      //find_power_level(number, power);
    }

  //   else if(enc1.isLeft())   //уменьшение на 1
  //   {
  //     power_counter -= power_increment;
  //     tft.setCursor(110, 85);
  //     tft.fillRect(110, 85, 320, 21, ST77XX_BLACK); 
  //     if(power_counter < 0 ) 
  //     { 
  //       power_counter = 0;
  //     }
      
  //     tft.print(power_counter);
  //     power = power_counter;
  //     tft.print(" Units");

  //     //set_power(power);
  //     set_out_power(find_power_level(freq_set_by_encoder, power));
  //     //find_power_level(number, power);
  //   }
   }
  
}



