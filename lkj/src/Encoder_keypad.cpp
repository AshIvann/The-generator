#include "Encoder_keypad.h"


Encoder enc1(CLK, DT, SW, TYPE2);

// const byte ROWS = 4;   // Количество рядов 
// const byte COLS = 4;   // Количество строк 

char keys[ROWS][COLS] = 
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

uint8_t rowPins[ROWS] = {A3, A2, A1, A0}; // Выводы, подключение к строкам
uint8_t colPins[COLS] = {6, 5, A5, A4}; // Выводы, подключение к столбцам  


uint64_t My_keybord:: build_number(uint8_t key)
{
  entered_number = (entered_number * 10) + (key - 48);
  return entered_number;
}

uint64_t My_keybord:: delete_digit()
{
    return (entered_number / 10);
}