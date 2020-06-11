#ifndef MYLIB_H
#define MYLIB_H


#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <RtcDS3231.h>    
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
    #define BUTTON_TOUCH D8 
#define BOMBA D6
#define LUZ D7
        const int DHTPin = D7;
#define BUTTON_MENOS D4     
#define BUTTON_MAS D3
      #define DELAYBUTTONMS 250
                     #define OPCMAX 8
                     #define countof(a) (sizeof(a) / sizeof(a[0]))

void menu(LiquidCrystal_I2C,RtcDS3231<TwoWire>);
void print_config_options(int opcion,LiquidCrystal_I2C);
String config_hora(String dat,LiquidCrystal_I2C);
String leer(int addr);
void grabar(int addr, String a);
int ejecutar_opcion(int selecc, int op, LiquidCrystal_I2C ,RtcDS3231<TwoWire>);
int leer_opcion(void);
void boot_config(LiquidCrystal_I2C,RtcDS3231<TwoWire>);
void config_fecha(LiquidCrystal_I2C lcd1,RtcDS3231<TwoWire> Rtc,int *);

#endif // MYLIB_H
