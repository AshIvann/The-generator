#include "GyverEncoder.h"
#include "Keypad.h"

#include "Display.h"
#include "LMX2595.h"

extern Encoder enc1;
extern Keypad keypad;

//для экодера
#define CLK 2
#define DT 3
#define SW 4

class My_encoder
{
    public:
        void update_freq(void);
        void update_power(void);
        boolean click(void);

        bool click_changed = true;

        uint64_t freq_set_by_encoder = 425000000;
        uint8_t freq_increment = 1;

        uint8_t power_increment = 1;  
        uint8_t power_counter = 5;
};

class My_keybord
{
    public:
        void scan_key(uint8_t);
        uint64_t get_rem();
        uint64_t freq_set_by_key;
        uint64_t remember_freq;
};