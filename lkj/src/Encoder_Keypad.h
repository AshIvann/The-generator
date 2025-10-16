#include "GyverEncoder.h"
// #include "Keypad.h"

// //Для энкодера 
// #define CLK 2
// #define DT 3
// #define SW 4

// extern Encoder enc1;
// extern Keypad keypad ;


// extern uint64_t freq_set_by_encoder;            
// extern uint8_t power_counter;

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

// Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
// Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
// Encoder enc1(CLK, DT, SW, TYPE2);