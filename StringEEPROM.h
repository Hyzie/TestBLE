#ifndef _STRING_EEPROM_
#define _STRING_EEPROM_

#include <EEPROM.h>

#define EEPROM_SIZE 40



void WriteStringToEEPROM(int addr, const String &str) {
  int len = str.length();
  Serial.print("Writing string: ");
  Serial.print(str);
  Serial.print(" at address: ");
  Serial.println(addr);

  EEPROM.write(addr, len); 
  for (int i = 0; i < len; i++) {
    EEPROM.write(addr + 1 + i, str[i]); 
  }
  EEPROM.commit();
}

String ReadStringFromEEPROM(int addr) {
  int len = EEPROM.read(addr);
  char data[len + 1]; 
  for (int i = 0; i < len; i++) {
    data[i] = EEPROM.read(addr + 1 + i); 
  }
  data[len] = '\0'; 

  Serial.print("Read string of length: ");
  Serial.print(len);
  Serial.print(" from address: ");
  Serial.println(addr);

  return String(data);
}

#endif