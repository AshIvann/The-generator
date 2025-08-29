#include <Arduino.h>
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <math.h>
#include "stdint.h"
#include "GyverEncoder.h"
#include "Keypad.h"

//Пины для дислпея 
#define TFT_DC   8     //datacomand 
#define TFT_RST  9     //reset
#define TFT_CS   7     //chip select

//Для энкодера 
#define CLK 2
#define DT 3
#define SW 4

//Фрагмент из кода для SPI используются для передачи данных на LMX2595
#define DATAOUT 11//MOSI     
#define DATAIN  12//MISO     
#define SPICLOCK  13//sck    
#define CHIPSELECT 10//ss    
 
#define R0 0x00
#define R1 0x01
#define R7 0x07
#define R8 0x08
#define R9 0x09
#define R10 0x0A
#define R11 0x0B
#define R12 0x0C
#define R14 0x0E
#define R16 0x10
#define R17 0x11
#define R19 0x13
#define R20 0x14
#define R27 0x1B
#define R31 0x1F
#define R34 0x22
#define R36 0x24
#define R37 0x25
#define R38 0x26
#define R39 0x27
#define R40 0x28
#define R41 0x29
#define R42 0x2A
#define R43 0x2B
#define R44 0x2C 
#define R45 0x2D
#define R46 0x2E
#define R59 0x3B
#define R60 0x3C
#define R69 0x45
#define R70 0x46
#define R71 0x47
#define R72 0x48
#define R73 0x49
#define R74 0x4A
#define R75 0x4B
#define R78 0x4E
#define R79 0x4F
#define R80 0x50
#define R81 0x51
#define R82 0x52
#define R83 0x53
#define R84 0x54
#define R85 0x55
#define R86 0x56
#define R96 0x60
#define R97 0x61
#define R98 0x62
#define R99 0x63
#define R100 0x64
#define R101 0x65
#define R102 0x66
#define R103 0x67
#define R104 0x68
#define R105 0x69
#define R106 0x6A
#define R110 0x6E
#define R111 0x6F
#define R112 0x70
 
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Encoder enc1(CLK, DT, SW, TYPE2);

void set_generator();                                                     //устанавливает значения необходимых для работы регистров 
void turning_speed();                                                     //используется для более удобного использования энкодера. При быстром повороте  значение на которое увеличивается величина растет.          
void writeRegister(uint8_t address, uint16_t data);                       //передача дынных в регистр 
char spi_transfer(volatile uint8_t data);                                 //настройка SPI
void settings_spi();                                                      //настройка SPI
byte send_SPI_byte(uint8_t val1);                                         //передача байта по SPI
void set_freq(uint16_t fout, uint16_t power);                             //установка частоты до гигагерца 
uint16_t replace_bits_8_to_13(uint16_t original, uint8_t new_bits);       //перемещение 6 битов на место 8-13(используется для установки мощности), original изначальный адресс, в котором изменяется 8-13 байт, new_bits биты которые ставятся в 8-13 биты 
uint8_t fractional(float number);                                         //отделяет цифры после запятой
bool isInteger(float number);                                             //проверяет является ли число дробным или нет. Елс целое то возвращает 1, если нет то 0
void second_set_freq(uint64_t fout);                                      //установка частоты до герца 
int find_chdiv(uint64_t fout);                                            //определение значения делителя
uint16_t low_16bit(uint32_t);                                             //выделяет младшие 16 бит из 32 битной переменной
uint16_t high_16bit(uint32_t);                                            //выделяет старшие 16 бит из 32 битной переменной 
int getDigitCount(uint64_t);                                              //возвращает количество цифр в числе 
void print_freq(uint64_t numer, int x , int y);                           //корректный вывод частоты (без пропажи нулей)
void set_power(uint16_t power);                                           //установка частоты 
int find_power_level(uint64_t freq, float taregt_power);

byte clr;
uint8_t address=0;
 
unsigned int divider_values[18] = {2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 72, 96, 128, 192, 256, 384, 512, 768};
unsigned int reg_divider[18]  = {0b0000100000000000, 0b0000100001000000, 0b0000100010000000, 0b0000100011000000, 0b0000100100000000, 0b0000100101000000, 0b0000100110000000, 0b0000100111000000, 0b0000101000000000, 0b0000101001000000, 0b0000101010000000, 0b0000101011000000, 0b0000101100000000, 0b0000101101000000, 0b0000101110000000, 0b0000101111000000, 0b0000110000000000, 0b0000110001000000};

uint32_t check_freq[33] = {25000000, 50000000, 75000000, 100000000, 150000000, 200000000, 300000000, 400000000, 500000000, 600000000, 700000000, 800000000, 900000000, 1000000000, 1100000000, 1200000000, 1300000000, 1400000000, 1500000000, 1600000000, 1700000000, 1800000000, 1900000000, 2000000000, 2100000000, 2200000000, 2300000000, 2400000000, 2500000000, 2600000000, 2700000000, 2800000000, 2900000000};

#define power_level 30   

const float power_table[power_level][33] PROGMEM =  
{
  {-10.77, -5.79, -3.3, -2.07, -1.38, -0.888, -1.04, -0.307, -0.8, -0.258, -0.242, -0.642, -0.484, 0.55, -1.09, -0.218, -2.7, -1.13, -2.81, -1.6, -2.45, -1.8, -1.2, -1.57, -0.3, -2.09, 0.41, -2.31, -1.57, -2.2, -3.28, -0.45, -3.64 },
  {-9.8, -4.74, -2.27, -1.09, -0.358, 0.095, 0.023, 0.77, 0.215, 0.721, 0.651, 0.303, 0.437, 0.468, -0.175, 0.755, -1.79, -0.222, -1.87, -0.746, -1.58, -0.905, -0.258, -0.7, 0.654, -1.15, 1.3, -1.58, -0.6, -1.13, -2.26, 0.455, -2.68},
  {-8.9, -3.82, -1.4, -0.127, 0.642, 0.917, 0.98, 1.61, 1.08, 1.54, 1.5, 1.17, 1.32, 1.29, 0.751, 1.55, -0.905, 0.584, -1.03, 0.153, -0.768, 0.025, 0.462, 0.166, 1.42, -0.3, 2.13, -0.845, 0.272, -0.23, -1.43, 1.27, - 1.86},
  {-8.08, -3.07, -0.642, 0.616, 1.32, 1.81, 1.8, 2.39, 1.93, 2.42, 2.42, 1.97, 2.12, 2.04, 1.55, 2.33, -0.182, 1.38, -0.316, 0.986, 0.04, 0.738, 1.3, 0.916, 2.24, 0.4, 3.03, -0.206, 1.1, 0.605, -0.725, 2.03, -1.18},
  {-7.38, -2.29, 0.117, 1.33, 2.02, 2.49, 2.56, 3.1, 2.55, 3.14, 3.1, 2.69, 2.89, 2.72, 2.18, 3.05, 0.48, 2.09, 0.327, 1.6, 0.635, 1.29, 2.02, 1.6, 2.85, 1.15, 2.61, 0.47, 1.77, 1.18, -0.032, 2.71, -0.569},
  {-6.61, -1.66, 0.828, 1.93, 2.82, 3.18, 3.29, 3.86, 3.24, 3.84, 4.76, 3.39, 3.57, 3.48, 2.87, 3.75, 1.13, 2.71, 1.08, 2.18, 1.27, 1.98, 2.57, 2.24, 3.5, 1.78, 3.12, 1.04, 2.4, 2, 0.728, 3.33, 0.117},
  {-6.01, -0.997, 1.4, 2.59, 3.35, 3.73, 3.95, 4.48, 3.92, 4.33, 4.43, 4.02, 4.12, 4.03, 3.46, 4.39, 1.8, 3.31, 1.64, 2.8, 1.86, 2.52, 3.32, 2.8, 4.12, 2.36, 3.64, 1.62, 3.07, 2.62, 1.31, 3.92, 0.645},
  {-5.36, -0.358, 2.06, 3.22, 3.98, 4.44, 4.54, 5.04, 4.5, 4.94, 5.01, 4.63, 4.57, 4.59, 4, 4.94, 2.29, 3.89, 2.12, 3.29, 2.42, 3.12, 3.85, 3.42, 4.64, 2.92, 4.24, 2.16, 3.61, 3.21, 1.89, 4.41, 1.21},
  {-4.8, 0.126, 2.56, 3.74, 4.49, 4.9, 5.11, 5.61, 5.08, 5.5, 5.49, 5.15, 5.17, 5.16, 4.56, 5.56, 2.8, 4.42, 2.62, 3.89, 2.94, 3.6, 4.28, 4.03, 5.28, 3.4, 4.83, 2.64, 4.24, 3.73, 2.41, 5.03, 1.76},
  {-4.24, 0.67, 3.19, 4.25, 5, 5.49, 5.67, 6.08, 5.53, 6.04, 5.95, 5.63, 5.72, 5.67, 5.15, 6.07, 3.38, 4.87, 3.17, 4.37, 3.44, 4.13, 4.86, 4.38, 5.72, 3.89, 5.29, 3.12, 4.71, 4.24, 2.87, 5.46, 2.15},
  {-3.83, 1.18, 3.56, 4.75, 5.4, 5.98, 6.14, 6.56, 5.98, 6.45, 6.44, 6.12, 6.22, 6.17, 5.53, 6.45, 3.76, 5.37, 3.61, 4.77, 3.81, 4.55, 5.27, 4.85, 6.16, 4.35, 5.74, 3.48, 5.16, 4.72, 3.32, 5.92, 2.6},
  {-3.4, 1.64, 4.05, 5.2, 5.92, 6.31, 6.56, 7.14, 6.45, 6.97, 6.89, 6.62, 6.69, 6.64, 6.05, 6.88, 4.22, 5.76, 4.04, 5.23, 4.26, 5.07, 5.76, 5.29, 6.52, 4.79, 6.24, 4.35, 5.59, 5.18, 3.77, 6.44, 3.05},
  {-2.9, 2.03, 4.5, 5.65, 6.3, 6.8, 7.07, 7.57, 6.85, 7.43, 7.32, 7.05, 7.13, 6.97, 6.4, 7.32, 4.6, 6.25, 4.43, 5.57, 4.69, 5.45, 6.2, 5.74, 7.05, 5.22, 6.87, 4.78, 6.04, 5.58, 4.19, 6.81, 3.48},
  {-2.5, 2.42, 4.83, 6.16, 6.63, 7.24, 7.43, 7.91, 7.32, 7.72, 7.74, 7.3, 7.44, 7.39, 6.84, 7.71, 4.99, 6.6, 4.81, 5.93, 5.16, 5.77, 6.55, 6.15, 7.34, 5.65, 7.21, 5.23, 6.39, 6, 4.63, 7.21, 3.79},
  {-2.23, 2.8, 5.26, 6.51, 7.1, 7.57, 7.82, 8.3, 7.72, 8.17, 8.12, 7.73, 7.85, 7.8, 7.24, 8.04, 5.43, 6.94, 5.18, 6.36, 5.4, 6.13, 6.92, 6.41, 7.77, 6.16, 7.53, 5.59, 6.76, 6.37, 5, 7.55, 4.22},
  {-1.85, 3.25, 5.59, 6.88, 7.45, 8, 8.13, 8.69, 8.07, 8.48, 8.51, 8.12, 8.24, 8.15, 7.6, 8.48, 5.79, 7.37, 5.53, 6.69, 5.78, 6.57, 7.24, 6.81, 8.07, 5.18, 7.85, 5.94, 6.8, 6.78, 5.36, 7.98, 4.65},
  {-1.46, 3.61, 6.01, 7.26, 7.77, 8.41, 8.53, 9.13, 8.45, 8.84, 8.87, 8.5, 8.61, 8.53, 7.99, 8.83, 6.13, 7.68, 5.84, 7.06, 6.13, 6.82, 7.57, 7.17, 8.42, 5.62, 8.22, 6.31, 7.5, 7.15, 5.72, 8.3, 4.94},
  {-1.08, 3.94, 6.39, 7.6, 8.17, 8.72, 8.9, 9.46, 8.85, 9.25, 9.21, 8.79, 9.04, 8.95, 8.28, 9.16, 6.46, 7.98, 6.23, 7.3, 6.55, 7.26, 7.9, 7.52, 8.7, 5.84, 8.51, 6.53, 7.47, 7.44, 6.08, 8.7, 5.19},
  {-0.75, 4.23, 6.61, 7.88, 8.47, 9.08, 9.27, 9.81, 9.1, 9.59, 9.55, 9.12, 9.29, 9.18, 8.57, 9.47, 6.78, 8.37, 6.47, 7.65, 7.82, 7.62, 8.25, 7.84, 9.06, 6.21, 8.87, 6.8, 8.01, 7.77, 6.37, 8.96, 5.54},
  {-0.402, 4.53, 7., 8.22, 8.81, 9.44, 9.59, 10.17, 9.44, 9.86, 9.86, 9.4, 9.56, 9.5, 8.91, 9.77, 7.09, 8.64, 6.8, 7.91, 7.08, 7.78, 8.52, 8.16, 9.35, 6.55, 9.15, 7.15, 8.32, 8.1, 6.75, 9.33, 5.87},
  {-0.176, 4.83, 7.31, 8.51, 9.14, 9.64, 9.84, 10.48, 9.75, 10.24, 10.17, 9.66, 9.88, 9.8, 9.16, 10.1, 7.38, 8.91, 7.04, 8.23, 7.32, 8.12, 8.81, 8.36, 9.66, 6.84, 9.44, 7.45, 8.61, 8.38, 6.99, 9.6, 6.07},
  {0.196, 5.14, 7.53, 8.78, 9.37, 9.98, 10.16, 10.78, 9.98, 10.53, 10.47, 9.99, 10.22, 10.1, 9.48, 10.35, 7.67, 9.17, 7.41, 8.55, 7.68, 8.33, 9.09, 8.66, 9.94, 7.17, 9.7, 7.68, 8.9, 8.62, 7.34, 9.98, 6.37},
  {0.486, 5.42, 7.88, 9.06, 9.72, 10.22, 10.51, 11.04, 10.31, 10.73, 10.78, 10.16, 10.44, 10.32, 9.78, 10.66, 7.94, 9.51, 7.63, 8.72, 7.92, 8.65, 9.37, 8.95, 10.2, 7.39, 9.91, 7.86, 9.09, 8.91, 7.52, 10.22, 6.7},
  {0.672, 5.64, 8.09, 9.34, 10, 10.58, 10.75, 11.35, 10.6, 11.01, 10.96, 10.48, 10.73, 10.68, 10.02, 10.7, 8.18, 9.75, 7.87, 9.01, 8.15, 8.85, 9.64, 9.2, 10.48, 7.7, 10.15, 8.07, 9.37, 9.2, 7.75, 10.55, 6.98},
  {0.905, 5.89, 8.39, 9.61, 10.2, 10.88, 11, 11.61, 10.92, 11.32, 11.21, 10.69, 10.98, 10.93, 10.24, 11.18, 8.47, 10.02, 8.18, 9.17, 8.48, 9.14, 9.93, 9.38, 10.67, 7.97, 10.38, 8.29, 9.64, 9.39, 8.06, 10.79, 7.26},
  {1.25, 6.17, 8.58, 9.8, 10.43, 11.09, 11.28, 11.86, 11.13, 11.56, 11.47, 11.01, 11.23, 11.18, 10.5, 11.46, 8.73, 10.26, 8.38, 9.45, 8.65, 9.34, 10.17, 9.65, 10.88, 8.16, 10.64, 8.49, 9.81, 9.7, 8.3, 11.01, 7.5},
  {1.46, 6.39, 8.9, 10.06, 10.73, 11.3, 11.58, 12.11, 11.39, 11.82, 11.76, 11.14, 11.53, 11.41, 10.74, 11.55, 8.97, 10.44, 8.68, 9.66, 8.87, 9.62, 10.42, 9.94, 11.2, 8.52, 10.86, 8.66, 10.06, 9.95, 8.6, 11.31, 7.63},
  {1.7, 6.63, 9.08, 10.28, 10.98, 11.62, 11.87, 12.34, 11.65, 12.07, 12.01, 11.37, 11.75, 11.66, 11.04, 11.98, 9.21, 10.66, 8.87, 9.85, 9.17, 9.8, 10.66, 10.11, 11.44, 8.7, 11, 8.85, 10.23, 10.2, 8.72, 11.52, 7.93},
  {1.9, 6.92, 9.23, 10.54, 11.19, 11.75, 12, 12.51, 11.75, 12.32, 12.26, 11.62, 11.87, 11.79, 11.18, 12.1, 9.41, 10.89, 9.05, 10.02, 9.36, 10.02, 10.89, 10.36, 11.68, 8.98, 11.21, 9.12, 10.42, 10.34, 8.92, 11.73, 8.17},
  {2.19, 7.18, 9.51, 10.62, 11.43, 11.95, 12.26, 12.7, 12.02, 12.55, 12.5, 11.94, 12.12, 12.04, 11.43, 12.33, 9.64, 11.05, 9.16, 10.19, 9.55, 10.18, 11, 10.55, 10.67, 9.12, 11.35, 9.4, 10.66, 10.59, 9.26, 11.98, 8.26}
};


float power_counter = 10;
int click_counter = 1;

int time1 = 0;              //используется в функции turning_speed 
int time2;                  //используется в функции turning_speed 
int time_diff;              //используется в функции turning_speed 

uint16_t freq = 75;          //частота которая вызывается в setup
uint16_t power = 5;         //мощность которая вызывается в setup

uint32_t increase_value = 1;     //переменная отвечающая за изменение значения частоты 
float power_increment = 1;         //переменная отвечающая за изменение значения мощности 
int position = 8;                

uint64_t number = 25e6;            //выбор частоты для second_freq
uint32_t last_six = number % 1000000;
uint32_t first_five = number / 1000000;

uint64_t key_number = 0;               
uint32_t key_last_six = key_number % 1000000;
uint32_t key_first_five = key_number / 1000000;

const byte ROWS = 4;   // Количество рядов 
const byte COLS = 4;   // Количество строк 
 
char keys[ROWS][COLS] = 
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {A3, A2, A1, A0}; // Выводы, подключение к строкам
byte colPins[COLS] = {6, 5, A5, A4}; // Выводы, подключение к столбцам  

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

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

  print_freq(number, 100, 30);
}


void loop()
{
  enc1.tick(); 
  
  char key = keypad.getKey();
  if (key)
  {
    if(key >= '0' && key <= '9')
    {
      key_number = (key_number * 10) + (key - 48);

      tft.fillRect(0, 150, 200, 22, ST77XX_BLACK);       
      tft.setCursor(0, 150);
      tft.print((uint32_t)key_number);
    }

    else if(key == 'A')
    {
      if(key_number < 12e6)
      {
        tft.setCursor(100,30);
        tft.fillRect(100, 30, 320, 21, ST77XX_BLACK);
        key_number = 12e6;
        tft.print("12.000000MHz");
        second_set_freq(key_number);
      }
      else if(key_number >= 19e9)
      {
        tft.setCursor(100,30);
        tft.fillRect(100, 30, 320, 21, ST77XX_BLACK);
        key_number = 19e9;
        tft.print("19000.000000 MHz");
        second_set_freq(key_number);
      }
      else
      {
        key_number = key_number * 1000000;
        print_freq(key_number, 100, 30);

        second_set_freq(key_number);
        find_power_level(key_number, power);
      }
      number = key_number;
      key_number = 0;             //отчищает переменную для новой записи 
    }
    
    else if(key == 'B')
    {
      if(key_number < 12e6)
      {
        tft.setCursor(100,30);
        tft.fillRect(100, 30, 320, 21, ST77XX_BLACK);
        key_number = 12e6;
        tft.print("12.000000MHz");
        second_set_freq(key_number);
      }
      else if(key_number >= 19e9)
      {
        tft.setCursor(100,30);
        tft.fillRect(100, 30, 320, 21, ST77XX_BLACK);
        key_number = 19e9;
        tft.print("19000.000000 MHz");
        second_set_freq(key_number);
      }
      else
      {
        print_freq(key_number, 100, 30);

        second_set_freq(key_number);
        find_power_level(key_number, power);
      }
      number = key_number;
      key_number = 0;  
    }

    else if(key == 'D')
    {
      key_number = key_number / 10;
      tft.fillRect(0, 150, 200, 22, ST77XX_BLACK);
      tft.setCursor(0, 150);
      tft.print((uint32_t)key_number);
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
      number += increase_value;
      if(number > 19e9) 
      {
        number = 19e9;
      }
      print_freq(number, 100, 30);

      second_set_freq(number);
    }

    if(enc1.isLeft())
    {
      number -= increase_value;
      if(number < 12e6 )
      {
        number = 12e6;
      }
      print_freq(number, 100, 30);

      second_set_freq(number);
    } 
  }

  else                    //изменение мощности 
  {
    if(enc1.isRight())       //увеличение на 1
    {
      power_counter += power_increment;
      tft.setCursor(110, 85);
      tft.fillRect(110, 85, 320, 21, ST77XX_BLACK); 
    
      if(power_counter >= 30)
      {
        power_counter = 30;                                                                   
      }
      
      tft.print(power_counter);
      power = power_counter;
      tft.print(" Units");

      set_power(power);
      //set_power(find_power_level(number, power));
    }

    else if(enc1.isLeft())   //уменьшение на 1
    {
      power_counter -= power_increment;
      tft.setCursor(110, 85);
      tft.fillRect(110, 85, 320, 21, ST77XX_BLACK); 
      if(power_counter < 0 )                                                                          //нужно изменить ограничения мощности, от 0 до 11
      { 
        power_counter = 0;
      }
      
      tft.print(power_counter);
      power = power_counter;
      tft.print(" Units");

      set_power(power);
      //set_power(find_power_level(number, power));
    }
  }
  
}


int chdiv;        //используется для получения значения делителя из массива divider_values[]
int chdiv_reg;    //используется для получения значения, которое должно быть записано в регистр из массива reg_divider[]

int find_power_level(uint64_t target_freq, float taregt_power)
{
  
  int closest_index = 0;
  uint32_t min_diff = abs(target_freq - check_freq[0]);
  for(int i = 1; i < 33; i++)
  {
    uint32_t current_freq = check_freq[i];
    uint32_t diff = abs(target_freq - current_freq) ;
    if(abs(diff) <= abs(min_diff))                 
    {
      min_diff = diff;
      closest_index = i;
    }
  }

  int best_level = 0;
  if(taregt_power == 0)
  {
    best_level = 1;
  }
  else
  {
    float best_diff = abs(taregt_power - pgm_read_float(&power_table[0][closest_index]));                
    for(int level = 1; level <= power_level; level++ )
    {
      float current_dbm = pgm_read_float(&power_table[level][closest_index]);
      float diff = abs(taregt_power - current_dbm);
      if(abs(diff) <= abs(best_diff))
      {
        best_diff = diff;
        best_level = level;
      }
    }
  }
  tft.setCursor(0,200);
  tft.fillRect(0,200,320,20,ST77XX_BLACK);
  tft.print(best_level);
  tft.print(" ");
  tft.print(check_freq[closest_index]);
  return best_level;
}

void set_power(uint16_t power)
{
  writeRegister(R44, replace_bits_8_to_13(0x1EA3, power));
}

void print_freq(uint64_t number, int x ,int y)
{
  last_six = number % (uint32_t)1e6;
  first_five = number / 1e6;
  tft.setCursor(x,y);
  tft.fillRect(x, y, 320, 21, ST77XX_BLACK);
  tft.print(first_five);
  tft.print(".");
  for(int i = 0; i < 6 - getDigitCount(last_six); i++)
  {
    tft.print(0) ;
  }
  tft.print(last_six);
  tft.print("MHz");
}

void second_set_freq(uint64_t fout)
{

  if(fout > 19000000000)
  {
    //ошибка, больше 19 Ghz нельзя 
  }

  else if(fout < 19000000000 && fout > 15000000000)   //VCO doubler
  {
    //VCO doubler

   
    writeRegister(R45, 0b1101000011011110);   //переключил выход A на VCO Doubler
    writeRegister(R27, 0b0000000000000011);   //включил VCO2X_EN
  }

   else if(fout <15000000000 && fout >7500000000 )    //VCO
  {
    //VCO
    writeRegister(R46, 0b0000011111111101);   //переключил выход B на VCO
    writeRegister(R45, 0b1100100011011110);   //переключил выход A на VCO
    writeRegister(R27, 0b0000000000000010);   //включил VCO2X_EN    
  }

else        //<7500
  {
    writeRegister(R46, 0b0000011111111100);   //переключил выход B на Channel Divider
    writeRegister(R45, 0b1100000011011110);   //переключил выход A на Channel Divider
    writeRegister(R31, 0b0100001111101100);   //включил CHDIV

    first_five = fout / 1000000;
    last_six = fout % 1000000;

    chdiv = divider_values[find_chdiv(fout)];
    chdiv_reg = reg_divider[find_chdiv(fout)];

    uint32_t pll_den = 10000000;                              
    int NUM = (last_six * chdiv) / pll_den;
    uint16_t pll_n = (first_five * chdiv) / 10 + NUM;   //10 это частота фазового детектора

    long int num_fractional_part = (last_six * chdiv) - NUM * 1e7 + fractional((((float)first_five * (float)chdiv) / 10.0)) * 1e6;        //дробная часть от NUM. (int) NUM * 1e7 необходимо, чтобы убрать целую часть(если она есть), так как ее я уже прибавил к pll_n

    // tft.setCursor(0,58);                         параметры делителей для установки частоты 
    // tft.setTextColor(ST77XX_YELLOW);  
    // tft.fillRect(0, 58, 320, 21, ST77XX_BLACK);
    // tft.print("pll_n = ");
    // tft.print(pll_n);
    // tft.setCursor(0,83);
    // tft.fillRect(0, 83, 320, 21, ST77XX_BLACK);
    // tft.print("PLL_NUM=");
    // tft.print(num_fractional_part);
    // tft.setCursor(0,108);
    // tft.fillRect(0, 108, 320, 21, ST77XX_BLACK);
    // tft.print("chdiv=");
    // tft.print(chdiv);

    writeRegister(R75, chdiv_reg);   

    writeRegister(R36, pll_n);
    writeRegister(R43, low_16bit(num_fractional_part));
    writeRegister(R42, high_16bit(num_fractional_part));

    writeRegister(R0, 0b0010010000011100);

  }
}

int getDigitCount(uint64_t number)     
{
  if (number == 0) 
  {
    return 1;
  }
  return (int)(floor(log10(number)) + 1);
}

void set_generator()
{
  //выдает 25 МГц                        
  // Program RESET = 1 to reset registers
  writeRegister(R0, 0b0010010000011110);
  //Program RESET = 0 to remove reset
  writeRegister(R0, 0b0010010000011100);

  writeRegister(R78, 0x0003);
  writeRegister(R75, 0x0BC0);       //channel divider = 384
  writeRegister(R74, 0x0000);
  writeRegister(R73, 0x003F);
  writeRegister(R72, 0x0001);
  writeRegister(R71, 0x0081);
  writeRegister(R70, 0xC350);
  writeRegister(R69, 0x0000);
  writeRegister(R60, 0x0000);
  writeRegister(R59, 0x0001);
  writeRegister(R46, 0x07FC);
  //writeRegister(R45, 0b1101000011011110);   //No output power boost
  writeRegister(R45, 0b1100000011011110);  //Maximum output power boost
  //writeRegister(R44, 0b0001111010100011);   //OUTA_PWR =30
  set_power(5);
  writeRegister(R43, 0x0000);
  writeRegister(R42, 0x0000);
  writeRegister(R41, 0x0000);
  writeRegister(R40, 0x0000);
  writeRegister(R39, 0x9680);
  writeRegister(R38, 0x0098);
  writeRegister(R37, 0x0304);
  writeRegister(R36, 960);                  //N dIVEDER
  writeRegister(R34, 0x0000);
  writeRegister(R31, 0x43EC);
  writeRegister(R27, 0x0002);
  writeRegister(R20, 0xE048);
  writeRegister(R19, 0x27B7);
  writeRegister(R17, 0x012C);
  writeRegister(R16, 0x0080);        
  writeRegister(R14, 0x1E70);
  writeRegister(R12, 0x5002);
  writeRegister(R11, 0x0018);
  writeRegister(R10, 0x10D8);
  writeRegister(R9,  0x1604);
  writeRegister(R8,  0x2000);
  writeRegister(R7,  0x40B2);
  writeRegister(R1,  0x0808);
  writeRegister(R0, 0b0010010000011100);            
}

void turning_speed()
{
  time2 = millis();
  time_diff = time2 - time1;
  if(time_diff < 400)                          //потом вернуть к 150
  {
    increase_value += increase_value + 1;
   increase_value;
    tft.setTextColor(ST77XX_RED);                           //вывод значения increase_value, по сути не нужно, убрать
    if(increase_value >= 100000000)
    {
      increase_value = 100000000;
    }
  }
  else
  {
    increase_value = 1;
    tft.setTextColor(ST77XX_BLUE);                          //вывод значения increase_value, по сути не нужно, убрать
  }
  time1 = time2;
  tft.setCursor(20, 120);
  tft.fillRect(20, 120, 150, 40, ST77XX_BLACK);
  tft.print(increase_value);
}

char spi_transfer(volatile uint8_t data)
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

byte send_SPI_byte(uint8_t val1)
 {
   uint8_t data_byte;
 
   //digitalWrite(CHIPSELECT,LOW);

   data_byte = spi_transfer(val1); //get data byte

   //digitalWrite(CHIPSELECT,HIGH); //release chip, signal end transfer
 
   return data_byte;
 }

void writeRegister(uint8_t addr, uint16_t data)
{
  struct st_packet 
  {
    uint8_t addr : 7;
    uint8_t rw : 1;             
    uint16_t data;
  };

  st_packet packet;
  packet.rw = 0;           // Режим записи при 0, чтение при 1                  
  packet.addr = addr;       // Адрес регистра
  packet.data = data;       // данные 

  uint8_t* bytes = (uint8_t*)&packet;

  //digitalWrite(CS, LOW);

  digitalWrite(CHIPSELECT,LOW);
  send_SPI_byte(bytes[0]);
  send_SPI_byte(bytes[2]);
  send_SPI_byte(bytes[1]);
  digitalWrite(CHIPSELECT,HIGH); //release chip, signal end transfer

 // digitalWrite(CS, HIGH);
}

bool isInteger(float number) 
{
  return number == (int)number;
}

void set_freq(uint16_t fout, uint16_t power)
{
  int chdiv;

  if(fout > 19000)
  {
    //ошибка, больше 19 Ghz нельзя 
  }

  else if(fout < 19000 && fout >15000)   //VCO doubler
  {
    //VCO doubler
    writeRegister(R45, 0b1101000011011110);   //переключил выход A на VCO Doubler
    writeRegister(R27, 0b0000000000000011);   //включил VCO2X_EN

  
  }

  else if(fout <15000 && fout >7500 )    //VCO
  {
    //VCO
    writeRegister(R46, 0b0000011111111101);   //переключил выход B на VCO
    writeRegister(R45, 0b1100100011011110);   //переключил выход A на VCO
    writeRegister(R27, 0b0000000000000010);   //включил VCO2X_EN  
    
    int PLL_N = ((fout * chdiv) / 10);   
    int PLL_NUM;
  
    if(isInteger(PLL_N) == 0)           //исплбзуется для проверки являеся ли PLL_num дробным или нет. Если целое то можно получить частоты без использования дроби
    {
  
        PLL_NUM = fractional(((fout * chdiv) / 10));
    }                                   
    else
    {
       PLL_NUM = 0;
    }
  
    
    
    writeRegister(R36, PLL_N);
    writeRegister(R43, PLL_NUM);
    writeRegister(R39, 10);
    
    //writeRegister(R44, replace_bits_8_to_13(0x1EA3, dec_to_bin(power)));
    writeRegister(R0, 0b0010010000011100);
  
  }

  else if(fout < 7500)//начало диапазона Channel Divider
  {
    writeRegister(R46, 0b0000011111111101);   //переключил выход B на VCO
    writeRegister(R45, 0b1100100011011110);   //переключил выход A на VCO
    writeRegister(R27, 0b0000000000000010);   //выключил VCO2X_EN        (проверить)!!!!!!!!!!!!
    
  
    if(fout < 7500 && fout >= 3750 )
    {
     // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      writeRegister(R31, 0b0000001111101100);   //выключил CHDIV, при 2 можно выключить, при остальных всегда включен
      writeRegister(R75, 0b0000100000000000);   //chdiv  = 2
      chdiv = 2;

    }
  
    else if(fout < 3750 && fout >= 1875 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100001000000);   //chdiv  = 4
      chdiv = 4;
    }
  
    else if(fout <1875 && fout >= 1250 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100010000000);   //chdiv  = 6
      chdiv = 6;
    }
    
    else if(fout < 1250 && fout >= 936 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100011000000);   //chdiv  = 8
       chdiv = 8;
     
    }
  
    else if(fout < 936 && fout >= 625 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100100000000);   //chdiv  = 12
      chdiv = 12;
    }
  
    else if(fout < 625 && fout >= 468 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100101000000);   //chdiv  = 16
      chdiv = 16;
    }
  
    else if(fout < 468 && fout >= 312 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100110000000);   //chdiv  = 24
      chdiv = 24;
    }
  
    else if(fout < 312 && fout >= 234 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000100111000000);   //chdiv  = 32
      chdiv = 32;
    }
  
    else if(fout < 234 && fout >= 156 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101000000000);   //chdiv  = 48
      chdiv = 48;
    }
  
    else if(fout < 156 && fout >= 117 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101001000000);   //chdiv  = 64
      chdiv = 64;
    }
  
    else if(fout < 117 && fout >= 104 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101010000000);   //chdiv  = 72
      chdiv = 72;
    }
  
    else if(fout < 104 && fout >= 78 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101011000000);   //chdiv  = 96
      chdiv = 96;
    }
  
    else if(fout < 78 && fout >= 58 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101100000000);   //chdiv  = 128
      chdiv = 128;

    
    }
  
    else if(fout < 58 && fout >= 39 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101101000000);   //chdiv  = 192
      chdiv = 192;
    }

    else if(fout < 39 && fout >= 29 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101110000000);   //chdiv  = 256
      chdiv = 256;
    }

    else if(fout < 29 && fout >= 19 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000101111000000);   //chdiv  = 384
      chdiv = 384;
    }
    
    else if(fout < 19 && fout >= 14 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000110000000000);   //chdiv  = 512
      chdiv = 512;
    }
    
    else if(fout < 14 && fout >= 9 )
    {
      // writeRegister(R45, 0b1100000011011110);   //переключил выход на Channel Divider
      // writeRegister(R31, 0b0100001111101100);   //включил CHDIV
      writeRegister(R75, 0b0000110001000000);   //chdiv  = 768
      chdiv = 768;
    }
  }

  float PLL_N = (((float)fout * (float)chdiv) / 10);   
  int PLL_NUM;
           
 
  if(isInteger(PLL_N) == 0 )                //проверяет является ли число дробным или нет. Если целое то возвращает 1, если нет то 0 
  {
    PLL_NUM = fractional(PLL_N);
  }
  else 
  {
    PLL_NUM = 0;
  }

  writeRegister(R36, trunc(PLL_N));         //trunc(функция из math.h) использую для удаления дробной части, можно заменить на (int)PLL_N, но я сделал так
  writeRegister(R43, PLL_NUM);
  writeRegister(R42, 0);
  writeRegister(R39, 10);
  
  //writeRegister(R44, replace_bits_8_to_13(0x1EA3, dec_to_bin(power)));
  writeRegister(R0, 0b0010010000011100);

}

uint16_t replace_bits_8_to_13(uint16_t original, uint8_t new_bits) 
{
  // Маска для очистки битов 8-13: 0b1100000111111111
  uint16_t mask = 0xC0FF;
  new_bits &= 0x3F;
  // Сдвигаем новые биты на позицию 8
  uint16_t shifted_bits = (uint16_t)new_bits << 8;
  // Очищаем биты 8-13 и вставляем новые
  uint16_t final = (original & mask) | shifted_bits;

  return final;
}

uint8_t fractional(float number)          //отделяет цифру после запятой
{
  
    int integer_part = (int)floor(number); // Целая часть
    float fractional_part = number - integer_part + 0.01; // Дробная часть
    int fractional_digits = (int)(fractional_part * 10); // Умножаем на 100, чтобы получить 1
    return fractional_digits;
}

int find_chdiv(uint64_t fout)
{
  for(int N = 0; N <= 17; N++)
  {
    uint64_t left = 7500 * 1e6 / divider_values[N];
    if(fout >= left) 
    {
      return N;
    }
  }
  //return 0;     
}

uint16_t low_16bit(uint32_t value) 
{
    // Младшая 16-битная часть
    uint16_t low  = (uint16_t)(value & 0xFFFF);
    return low; 
}

uint16_t high_16bit(uint32_t value) 
{
    // Старшая часть (в примере для 32-битного числа хватит одного сдвига)
    uint16_t high = (uint16_t)((value >> 16) & 0xFFFF);    
    return high;
}