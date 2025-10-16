 #pragma once
#include <Arduino.h>

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



class LMX2595 
{
    public:
        void set_freq(uint64_t);
        void set_out_power(uint16_t);
        void set_generator(uint64_t, uint8_t); 
        
    private:
        void writeRegister(uint8_t, uint16_t); 
        byte send_SPI_byte(uint8_t);
        char spi_transfer(volatile uint8_t);
        uint32_t calculation_of_pll_n(uint64_t);
        uint32_t calculation_of_pll_num(uint64_t);
        uint16_t low_16bit(uint32_t);
        uint16_t high_16bit(uint32_t); 
        uint8_t find_chdiv(uint64_t);
        struct st_freq_params
        {
            uint32_t first_five_of_freq;
            uint32_t last_six_of_freq;
            uint8_t chdiv;
            uint32_t int_part_of_frac_div;
        };
        st_freq_params get_divider_value(uint64_t);
        uint16_t replace_bits_8_to_13(uint16_t, uint8_t);

        uint16_t divider_values[18] = {2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 72, 96, 128, 192, 256, 384, 512, 768};
        uint16_t reg_divider[18] = {0b0000100000000000, 0b0000100001000000, 0b0000100010000000, 0b0000100011000000, 0b0000100100000000, 0b0000100101000000, 0b0000100110000000, 0b0000100111000000, 0b0000101000000000, 0b0000101001000000, 0b0000101010000000, 0b0000101011000000, 0b0000101100000000, 0b0000101101000000, 0b0000101110000000, 0b0000101111000000, 0b0000110000000000, 0b0000110001000000};
        uint16_t chdiv_reg;
};