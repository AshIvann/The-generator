#include "Encoder.h"

// Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
Encoder enc1(CLK, DT, SW, TYPE2);


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

void My_encoder:: update_freq()
{
    Display dis;
    LMX2595 gen;

    bool freq_changed = false;
    if(enc1.isRight())
    {
        freq_set_by_encoder += freq_increment;
        if(freq_set_by_encoder >= 19e9) 
        {
            freq_set_by_encoder = 19e9;
        }
        freq_changed = true;
    }
    if(enc1.isLeft())
    {
        freq_set_by_encoder -= freq_increment;
        if(freq_set_by_encoder <= 12e6) 
        {
            freq_set_by_encoder = 12e6;
        }
        freq_changed = true;
    }

    if(freq_changed)
    {
        dis.print_freq(freq_set_by_encoder, 100, 30);  
        gen.set_freq(freq_set_by_encoder);
    }
}

void My_encoder:: update_power()
{
    Display dis;
    LMX2595 gen;
    My_encoder my_enc;

    bool power_changed = false;

    if(enc1.isRight())
    {
        power_counter += power_increment;
        if(power_counter >= 11) 
        {
            power_counter = 11;
        }
        power_changed = true;
    }
    if(enc1.isLeft())
    {
        power_counter -= power_increment;
        if(power_counter == 255) 
        {
            power_counter = 0;
        }
        power_changed = true;
    }
    if(power_changed)
    {
        uint8_t best_pow_level = dis.get_best_level(power_counter, freq_set_by_encoder);
        dis.power_print(dis.find_power_level(power_counter, freq_set_by_encoder));

        tft.setCursor(0, 150);
        tft.fillRect(0, 150, SCREEN_WIDTH, 20, ST77XX_BLACK);
        tft.print("pwr_count = ");
        tft.print(power_counter);                                           

        tft.setCursor(0, 190);
        tft.fillRect(0, 190, SCREEN_WIDTH, 20, ST77XX_BLACK);
        tft.print("set_pwr = ");
        tft.print(best_pow_level);

        gen.set_out_power(best_pow_level);
    }
}

boolean My_encoder:: click()
{
    //enc1.tick(); 
    if(enc1.isClick())
    {
        click_changed = !click_changed;
    }
    return click_changed;
}

void My_keybord::scan_key(uint8_t key)
{
    Display dis;
    LMX2595 gen;
    My_encoder my_enc;
    if(key >= '0' && key <= '9')
    {
        freq_set_by_key = (freq_set_by_key * 10) + (key - 48);
        tft.fillRect(0, 150, 200, 22, ST77XX_BLACK);       
        tft.setCursor(0, 150);
        tft.print((uint32_t)freq_set_by_key);
    }
    else if(key == 'A')
    {
        // tft.setCursor(100,30);
        // tft.fillRect(100, 30, SCREEN_WIDTH, 21, ST77XX_BLACK);
        if(freq_set_by_key < 12e6)
        {
            freq_set_by_key = 12e6;
            tft.print("12.000000 MHz");
        }

        else if(freq_set_by_key >= 19e9)
        {
            freq_set_by_key = 19e9;
            tft.print("19000.000000 MHz");
        }

        dis.print_freq(freq_set_by_key, 100, 30);
        gen.set_freq(freq_set_by_key);

        remember_freq = freq_set_by_key;
        freq_set_by_key = 0;
    }
    // else if(key == 'B')
    // {
    //     freq_set_by_key = freq_set_by_key * 1000000;
    //     // if(freq_set_by_key < 12e6)
    //     // {
    //     //     freq_set_by_key = 12e6;
    //     //     tft.print("12.000000 MHz");
    //     // }
    //     // if(freq_set_by_key >= 19e9)
    //     // {
    //     //     freq_set_by_key = 19e9;
    //     //     tft.print("19000.000000 MHz");
    //     // }
    //     dis.print_freq(freq_set_by_key, 100, 30);
    //     gen.set_freq(freq_set_by_key);
    //     remember_freq = freq_set_by_key;
    //     freq_set_by_key = 0;
    // }
    else if(key == 'D')
    {
        freq_set_by_key = freq_set_by_key / 10;
        dis.print_freq(freq_set_by_key, 0, 150);
    }
}


uint64_t My_keybord:: get_rem()
{
    return remember_freq;
}