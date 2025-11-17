#include <Arduino.h>
#include <math.h>
#include "stdint.h"
#include "Print.h"

#include "LMX2595.h"
#include "Display.h"
#include "Encoder_keypad.h"

LMX2595 gen;
Display dis;

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
My_keybord my_key;

bool is_freq_changed = false;
bool is_power_changed = false;
bool click_changed = true;
uint8_t power_value = 5;
uint64_t freq_value = 7500000000;           //7.5 GHz

uint64_t full_number;
void setup()
{

    //gen.set_generator(7500, 10);
    //gen.set_ramp2();
    
    // gen.set_ramp();
    // gen.reset();
    

    dis.set_display();
    tft.fillRect(165, 31, 20, 3, ST77XX_BLUE);
    tft.setCursor(10, 30);
                                      
    tft.print("Freq: ");
    tft.fillRect(8, 106, 265, 2, ST77XX_BLACK);

    tft.setCursor(10, 85);                          
    tft.print("Power: ");
    tft.print(power_value);
    tft.fillRect(8, 51, 240, 2, ST77XX_BLACK);

    dis.print_freq(freq_value, 100, 30);
    enc1.setTickMode(TYPE2);
    gen.set_generator(freq_value, power_value); 
    Serial.begin(9600);
    
}



void loop()
{
    uint32_t pll_n = gen.calculation_of_pll_n(30005500);
    // Serial.print("pll_n = ");
    // Serial.println(pll_n );
    uint32_t pll_num = gen.calculation_of_pll_num(30005500);
    // Serial.print("pll_num = ");
    // Serial.println(pll_num);

Serial.end();
    /*
    enc1.tick(); 
    char key = keypad.getKey();

    if(key >= '0' && key <= '9')
    {
        full_number = my_key.build_number(key);
        tft.fillRect(0,150,200,22, ST77XX_BLACK);
        tft.setCursor(0,150);
        tft.print((uint32_t)full_number);
        //dis.my_print(full_number, 50, SCREEN_HEIGHT);
    }
    else if(key == 'A')
    {
        freq_value = full_number;
        my_key.entered_number = 0;
        if(freq_value < 12e6)
        {
            freq_value = 12e6;
        }
        else if(freq_value >= 19e9)
        {
            freq_value = 19e9;
        }
        dis.print_freq(freq_value, 100, 30);
        gen.set_freq(freq_value);
        tft.fillRect(0,150,200,22, ST77XX_BLACK);
    }
    else if(key == 'B')
    {
        freq_value = full_number * 1000000;
        my_key.entered_number = 0;
        if(freq_value < 12e6)
        {
            freq_value = 12e6;
        }
        else if(freq_value >= 19e9)
        {
            freq_value = 19e9;
        }
        dis.print_freq(freq_value, 100, 30);
        gen.set_freq(freq_value);
    }
    else if(key == 'C')
    {
        full_number = my_key.delete_digit();

    }
    else if(key == '*')
    {
        gen.set_generator(freq_value, power_value); 
        gen.set_ramp1();
        tft.fillRect(150,120,200,22, ST77XX_BLACK);
        tft.setCursor(150, 120);
        tft.setTextColor(ST77XX_RED);
        tft.print("RAMP1 ON");
    }
        else if(key == '#')
    {
        gen.set_generator(freq_value, power_value); 
        gen.set_ramp2();
        tft.fillRect(150,120,200,22, ST77XX_BLACK);
        tft.setCursor(150, 120);
        tft.setTextColor(ST77XX_RED);
        tft.print("RAMP2 ON");
    }
         else if(key == 'D')
    {
        gen.set_generator(freq_value, power_value); 
        // gen.set_ramp3();
        gen.dif_ramp(100000000, 10000);
        tft.fillRect(150,120,200,22, ST77XX_BLACK);
        tft.setCursor(150, 120);
        tft.setTextColor(ST77XX_RED);
        tft.print("RAMP3 ON");
    }

    if(enc1.isClick())
    {
        click_changed = !click_changed;
    }


    if(click_changed)                                                       //изменение частоты                                 
    {
        // Serial.println("first");
        tft.fillRect(8, SCREEN_HEIGHT/3, 240, 2, ST77XX_BLUE);
        tft.fillRect(8, SCREEN_HEIGHT/2, SCREEN_WIDTH, 2, ST77XX_BLACK);
        if(enc1.isRight())
        {
            gen.freq_increas();
            if(freq_value >= 19e9) 
            {
                freq_value = 19e9;
            }
            is_freq_changed = !is_freq_changed;
        }
        if(enc1.isLeft())
        {
            gen.freq_decreas();
            if(freq_value <= 12e6) 
            {
                freq_value = 12e6;
            }
            is_freq_changed = !is_freq_changed;
        }
        if(is_freq_changed)
        {
            dis.print_freq(freq_value, 100, 30); 
            gen.set_freq(freq_value);
            is_freq_changed = !is_freq_changed;
        }
    }
    
    uint32_t step = 50000000;
    uint32_t len = 10000;
    if(!click_changed)                                                       
    {

        tft.fillRect(8, SCREEN_HEIGHT/2, 265, 2, ST77XX_BLUE);                       //изменение мощности
        tft.fillRect(8, SCREEN_HEIGHT/3, SCREEN_WIDTH, 2, ST77XX_BLACK);
        // Serial.println("second");
        for(uint8_t i = 0; i < 8; i++)
        {
            gen.set_generator(freq_value, power_value); 
             gen.dif_ramp(step, len);
             len -=1000;  
            step += 500000000;
            tft.fillRect(100, 100, SCREEN_WIDTH, 20, ST77XX_BLACK);
            tft.setCursor(100,100);
            tft.print(i);
            delay(5000);
        }
        // click_changed = !click_changed;
    //     if(enc1.isRight())
    //     {
    //         gen.power_increas();
    //         if(power_value >= 11) 
    //         {
    //             power_value = 11;
    //         }
    //         is_power_changed = !is_power_changed;
    //     }
    //     if(enc1.isLeft())
    //     {
    //         gen.power_decreas();
    //         if(power_value == 255) 
    //         {
    //             power_value = 0;
    //         }
            
    //         is_power_changed = !is_power_changed;
    //     }
    //     if(is_power_changed)
    //     {
    //         uint8_t best_pow_level = gen.get_best_level(power_value, freq_value);
    //         dis.power_print(gen.find_power_level(power_value, freq_value));

    //         tft.setCursor(0, 150);                                                                          //контроль устанавливаемых параметров, потом убрать
    //         tft.fillRect(0, 150, SCREEN_WIDTH, 20, ST77XX_BLACK);
    //         tft.print("pwr_count = ");
    //         tft.print(power_value);                                           

    //         tft.setCursor(0, 190);                                                                          //контроль устанавливаемых параметров, потом убрать
    //         tft.fillRect(0, 190, SCREEN_WIDTH, 20, ST77XX_BLACK);
    //         tft.print("set_pwr = ");
    //         tft.print(best_pow_level);

    //         gen.set_out_power(best_pow_level);
    //         is_power_changed = !is_power_changed;
    //     }
    }
    */
}
