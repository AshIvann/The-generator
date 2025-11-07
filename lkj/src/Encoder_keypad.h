#include "GyverEncoder.h"
#include "Keypad.h"

#define ROWS 4
#define COLS 4

//для экодера
#define CLK 2
#define DT 3
#define SW 4

extern Encoder enc1;
extern Keypad keypad;
extern char keys[ROWS][COLS];
extern uint8_t rowPins[ROWS];     //подключение к строкам
extern uint8_t colPins[COLS];     // Выводы, подключение к столбцам  

class My_keybord
{
    public:
        uint64_t build_number(uint8_t);
        uint64_t delete_digit();

        uint64_t entered_number = 0;
};
