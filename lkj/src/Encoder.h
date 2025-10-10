#include "GyverEncoder.h"
#include "Display.h"
#include "LMX2595.h"
 #include "Keypad.h"

extern Encoder enc1;

#define CLK 2
#define DT 3
#define SW 4


class My_encoder //: public Display, public LMX2595 
{
    public:
        void update_freq(void);
        void update_power(void);
        bool click(void);

        uint64_t freq_set_by_encoder;
        bool click_changed = true;
        

    private:
        uint8_t freq_increment, power_increment = 1;  
        uint8_t power_counter = 5;
        
};

class My_keybord : public LMX2595, public Display
{
    public:
        void scan_key(uint8_t);
        uint64_t freq_set_by_key;
};