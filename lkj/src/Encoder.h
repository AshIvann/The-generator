#include "GyverEncoder.h"
#include "Keypad.h"

#include "Display.h"

extern Encoder enc1;
extern Keypad keypad;

//для экодера
#define CLK 2
#define DT 3
#define SW 4


extern uint64_t freq_set_by_encoder;
extern uint8_t power_counter;
extern bool freq_changed;
extern bool power_changed;

class My_encoder
{
    public:
        void freq_selection(void);
        void power_selection(void);
        boolean click(void);

        bool click_changed = true;

    private:
        uint8_t freq_increment = 1;
        uint8_t power_increment = 1;
};


extern const byte ROWS ;   // Количество рядов 
extern const byte COLS ;   // Количество строк 

extern char keys[4][4];
extern byte rowPins[4]  ;//подключение к строкам
extern byte colPins[4] ; // Выводы, подключение к столбцам  

class My_keybord
{
    public:
        void scan_key(uint8_t);
        uint64_t get_rem();

    private:
        uint64_t entered_number;
        uint64_t freq_set_by_key;
        uint64_t remember_freq;
};
