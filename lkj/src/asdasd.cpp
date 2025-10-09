
#pragma once
#include "Arduino.h"
#include "asdasd.h"

uint8_t find_chdiv()
{
  for(int N = 0; N <= 17; N++)
  {
    // uint64_t left = 7500 * 1e6 / divider_values[N];
    uint64_t left = 7500;
    if(1 >= left) 
    {
      return N;
    }
  }
  return 0;     
}