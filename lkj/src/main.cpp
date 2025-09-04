#include <Arduino.h>
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <math.h>
#include "stdint.h"
#include "GyverEncoder.h"
#include "Keypad.h"

#include "power_table.h"


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
        //key_number = key_number * 1000000;
        print_freq(key_number, 100, 30);
        //set_power(find_power_level(number, power));
        second_set_freq(key_number);
        //find_power_level(key_number, power);

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
    
      if(power_counter >= 11)
      {
        power_counter = 11;                                                                   
      }
      
      tft.print(power_counter);
      power = power_counter;
      tft.print(" Units");

      //set_power(power);
      set_power(find_power_level(number, power));
      //tft.print();
      //find_power_level(number, power);
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

      //set_power(power);
      set_power(find_power_level(number, power));
      //find_power_level(number, power);
    }
  }
  
}


int chdiv;        //используется для получения значения делителя из массива divider_values[]
int chdiv_reg;    //используется для получения значения, которое должно быть записано в регистр из массива reg_divider[]

int find_power_level(uint64_t target_freq, float target_power)
{
  
  //определение ближайшей частоты из таблицы к той, которую задали 
  int closest_index = 0;
  uint32_t left_freq_index;
  uint32_t right_freq_index;
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


  if(min_diff == 0)                                                       //переделать, чтобы без if, if else  и тд
  {
    left_freq_index  = closest_index;
    right_freq_index = closest_index;
  }
  else
  {
    if(target_freq < check_freq[0])
    {
      left_freq_index = right_freq_index = 0;
    }
    else if(target_freq < check_freq[closest_index])
    {
      right_freq_index = closest_index;
      left_freq_index = closest_index - 1;
    }
     else if(target_freq > check_freq[closest_index])
    {
      right_freq_index = closest_index + 1;
      left_freq_index = closest_index;
    }
   
  }
  tft.setCursor(0,200);
  tft.fillRect(0,200,320,20,ST77XX_BLACK);

  int best_left_level;
  int best_right_level;
  float best_left_diff;
  float best_right_diff;
  int best_level;

     best_left_diff = abs(target_power - pgm_read_float(&power_table[0][left_freq_index]));
     for(int level = 1; level <= power_level; level++ )
    {
      float current_dbm = pgm_read_float(&power_table[level][left_freq_index]);
      float diff = abs(target_power - current_dbm);
      if(abs(diff) <= abs(best_left_diff))
      {
        best_left_diff = diff;
        best_left_level = level;
      }
    }


    best_right_diff = abs(target_power - pgm_read_float(&power_table[0][right_freq_index]));
     for(int level = 1; level <= power_level; level++ )
    {
      float current_dbm = pgm_read_float(&power_table[level][right_freq_index]);
      float diff = abs(target_power - current_dbm);
      if(abs(diff) <= abs(best_right_diff))
      {
        best_right_diff = diff;
        best_right_level = level;
      }
    }

     if(best_right_diff <= best_left_diff)
    {
      best_level = best_right_level;
    }
    else
    { 
      best_level = best_left_level;
    }

    long after_map = map(target_freq, check_freq[left_freq_index], check_freq[right_freq_index], power_table[best_level][left_freq_index], power_table[best_level][right_freq_index]);


  // if(abs(best_right_level - best_left_level) >=2 || (abs(target_freq - check_freq[left_freq_index] >= 15000000) && abs(target_freq - check_freq[right_freq_index] >= 15000000)))
  // {
  //   best_level = (best_right_level + best_left_level) / 2;
  // }
  // else
  // {
   
// }

  


  tft.setCursor(0,200);
  tft.fillRect(0,200,320,20,ST77XX_BLACK);
  tft.print(best_left_level);
  tft.print(" ");
  tft.print(left_freq_index);
  tft.print(" ");
  tft.print(" ");
  tft.print(best_right_level);
  tft.print(" ");
  tft.print(right_freq_index);
  tft.print(" ");
  tft.print(after_map);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print(best_level);
  tft.setTextColor(ST77XX_WHITE);

  // tft.setCursor(110, 85);
  // tft.fillRect(110, 85, 320,20,ST77XX_BLACK);
 // tft.print(after_map);

  return best_left_level;
}

void set_power(uint16_t power)
{
  writeRegister(R44, replace_bits_8_to_13(0x1EA3, power));
}

void print_freq(uint64_t number, int x ,int y)
{
  first_five = number / 1e6;
  last_six = number % (uint32_t)1e6;
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