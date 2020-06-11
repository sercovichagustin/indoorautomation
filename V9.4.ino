//#include <mylib.h>
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
#define DELAYBUTTONMS 500
#define OPCMAX 8
#define countof(a) (sizeof(a) / sizeof(a[0]))                                    
const int sensorPin = A0;
bool BLSTATE = 1;
bool bombastate = LOW;         
unsigned long previousMillis = 0;        
unsigned long previousMillis1= 0;
unsigned long previousMillisBUTTON_TOUCH= 0;
unsigned long currentMillis;
unsigned long currentMillis3;
int humedad;
int a=0;
char HS_ON_VEGET,HS_OFF_VEGET,HS_ON_FLORA,HS_OFF_FLORA;
char MIN_ON_VEGET,MIN_OFF_VEGET,MIN_ON_FLORA,MIN_OFF_FLORA;
char periodo_actual;
char show_pantalla;
int DIA_ON_VEGET,MES_ON_VEGET,ANO_ON_VEGET,DIA_ON_FLORA,MES_ON_FLORA,ANO_ON_FLORA,DIA_OFF_FLORA,MES_OFF_FLORA,ANO_OFF_FLORA;
RtcDateTime now;


int flag;

#ifndef MYLIB
#define MYLIB
RtcDS3231<TwoWire> Rtc(Wire);
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPin, DHTTYPE);   

#endif // MYLIB_H
void setup() 
{
   Serial.begin(9600);
   dht.begin();
   Wire.begin(D2, D1);
   Rtc.Begin();
   lcd.begin(16,2);
   lcd.init();
   lcd.backlight();
   pinMode(BOMBA, OUTPUT);
   pinMode(LUZ, OUTPUT);
   pinMode(BUTTON_TOUCH, INPUT);
   pinMode(BUTTON_MAS, INPUT);
   pinMode(BUTTON_MENOS, INPUT);
   digitalWrite(BOMBA, LOW);
   EEPROM.begin(512);
   boot_config(lcd,Rtc);
   leer_valores_eeprom();
   cargar_periodo();
    }//    ---------------------   //   SETUP  //  ---------------------------------------------------------

void loop() 
{          
   
         currentMillis = millis();
         if (currentMillis - previousMillis1 >= 3000)
         {
              previousMillis1 = currentMillis;   
              leer_sensores(); 
              if(show_pantalla==1)
                  imprimir_config();
              else
                  imprimir_valores();
         } 
         if (currentMillis - previousMillis >= 600000)
         {
              previousMillis = currentMillis;   
              ejecutar_acciones();
         }      
         chequear_botones();
 
              
}   //    ---------------------//  LOOP  //-------------------------------------------------------------------- 
int ejecutar_acciones(void){
  bomba();
}
int bomba(void){      
    if (humedad>395)
    {
        digitalWrite(BOMBA, HIGH);
        delay(2000);
        digitalWrite(BOMBA, LOW);   
    } 
    else 
    {
        digitalWrite(BOMBA, LOW);
    } 
}
void chequear_botones(){
           
    if(IS_TOUCHED_BUTTON(BUTTON_TOUCH))
        change_backlight();
    if(!digitalRead(BUTTON_MAS))
    {
        if(show_pantalla==1){
             show_pantalla=0;
             imprimir_valores();
        }
        else if(show_pantalla==0){
             show_pantalla=1;
             imprimir_config();
        }
        delay(DELAYBUTTONMS);
    }
}   //    -----------------------------------------------------------------------------------------------------
void imprimir_texto_desplazamiento(String texto_fila)
{
int tam_texto=texto_fila.length();
  for(int i=0; i<tam_texto ; i++)
  {
    if(tam_texto-i<=11)
        return;
    String texto = texto_fila.substring(i);
//    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(texto);
    delay(600);
  }
 
  } //  ----------------------------------------------------------------------------------------------------------------------------------------
void imprimir_fecha(void)
{
     char datestring[20];
        snprintf_P(datestring, 
            countof(datestring),
            PSTR(" %02u/%02u/%02u"),
            now.Day(),
            now.Month(),
            now.Year()-2000);
     lcd.setCursor(0, 1);
    lcd.print(String(datestring));
    return;
}     //    -----------------------------------------------------------------------------------------------------
void cargar_periodo()
{
  char dat,dat1;
    dat=periodo(DIA_ON_VEGET,MES_ON_VEGET,ANO_ON_VEGET,DIA_ON_FLORA,MES_ON_FLORA,ANO_ON_FLORA);
    dat1=periodo(DIA_ON_FLORA,MES_ON_FLORA,ANO_ON_FLORA,DIA_OFF_FLORA,MES_OFF_FLORA,ANO_OFF_FLORA);
    if(dat==1&&dat1==0)
        periodo_actual=1;
    else if(dat==0&&dat1==1)
        periodo_actual=2;
    else if(dat==0&&dat1==0)
        periodo_actual=0;  
   // else
       //error_periodo();
    
}   //    -----------------------------------------------------------------------------------------------------
void leer_sensores(void){
    tiempo_luz(); //lee rtc y maneja luz. error devuelve -1
    humedad = analogRead(sensorPin);  //leo humedad
}   //    -----------------------------------------------------------------------------------------------------
int imprimir_valores(void){                                                 
    lcd.clear();
    imprimir_hora(); 
   // imprimir_fecha();
    lcd.setCursor(9, 0);
    lcd.print("HS "); 
    lcd.print(int(humedad)); 
}   //    -----------------------------------------------------------------------------------------------------

int tiempo_luz(void){
    if (!Rtc.IsDateTimeValid()){
        lcd.setCursor(0, 0);
        lcd.print("ERROR T");
        return -1;
    }
    else
    {
        now = Rtc.GetDateTime();
        if(periodo_actual==1){
            if(alrm(now,(int)HS_ON_VEGET,(int)MIN_ON_VEGET,(int)HS_OFF_VEGET,(int)MIN_OFF_VEGET))
              digitalWrite(LUZ,HIGH);
            else
                digitalWrite(LUZ,LOW); 
        }                   
        else if(periodo_actual==2){
            if(alrm(now,(int)HS_ON_FLORA,(int)MIN_ON_FLORA,(int)HS_OFF_FLORA,(int)MIN_OFF_FLORA))
              digitalWrite(LUZ,HIGH);
            else
              digitalWrite(LUZ,LOW);
        } 
        return 1;  
    }
}   //    -----------------------------------------------------------------------------------------------------
void imprimir_config(){
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("LUZ ");
          lcd.print(int(HS_ON_VEGET));
          lcd.print(":");
          lcd.print(int(MIN_ON_VEGET));
          lcd.print("-"); 
          lcd.print(int(HS_OFF_VEGET));
          lcd.print(":");
          lcd.print(int(MIN_OFF_VEGET));
  return;
}
void change_backlight(){
    if(BLSTATE==1){
      lcd.noBacklight();
      BLSTATE=0;
   }
   else{
      lcd.backlight();
      BLSTATE=1;
   }
  
}   //    -----------------------------------------------------------------------------------------------------

/*
void read_dht(void)
{
          interv1();  
         Reading temperature or humidity takes about 250 milliseconds!
         float h = dht.readHumidity();
        float t = dht.readTemperature();
         if (isnan(h) || isnan(t)) {
            Serial.println("Failed to read from DHT sensor!");
          return;
      }



          lcd.setCursor(0, 1);
   lcd.print("HA ");
    lcd.print(String(int(h)));
   lcd.print("% T");
    lcd.print(String(t));
    lcd.print(" C");
}   */    //    -----------------------------------------------------------------------------------------------------


bool IS_TOUCHED_BUTTON(const int BUTTON_TOUCH)
{
    bool d;
    if (currentMillis - previousMillisBUTTON_TOUCH >= 500)
    {
        if(d=digitalRead(BUTTON_TOUCH))
        {
                previousMillisBUTTON_TOUCH = currentMillis;
                return d; 
        }
    }
    return 0;
}     //    -----------------------------------------------------------------------------------------------------

#define countof(a) (sizeof(a) / sizeof(a[0]))
void imprimir_hora(void)
{
     char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR(" %02u:%02u"),
            now.Hour(),
            now.Minute());
    lcd.setCursor(0, 0);
    lcd.print(String(datestring));
        snprintf_P(datestring, 
            countof(datestring),
            PSTR(" %02u/%02u/%02u"),
            now.Day(),
            now.Month(),
            now.Year()-2000);
     lcd.setCursor(0, 1);
    lcd.print(String(datestring));
    return;
}     //    -----------------------------------------------------------------------------------------------------
void leer_valores_eeprom(void)
{
    String aux;
    String DAT_EEPROM;
    aux=leer(100);
    HS_ON_VEGET=aux[0];
    MIN_ON_VEGET=aux[1];
    aux=leer(120);
    HS_OFF_VEGET=(int)aux[0];
    MIN_OFF_VEGET=(int)aux[1];
    aux=leer(140);
    HS_ON_FLORA=(int)aux[0];
    MIN_ON_FLORA=(int)aux[1];
    aux=leer(160);
    HS_OFF_FLORA=(int)aux[0];
    MIN_OFF_FLORA=(int)aux[1];
    DAT_EEPROM=leer(180);
    ANO_ON_VEGET=DAT_EEPROM.toInt();
    DAT_EEPROM=leer(185);
    MES_ON_VEGET=DAT_EEPROM.toInt();
    DAT_EEPROM=leer(190);
    DIA_ON_VEGET=DAT_EEPROM.toInt();
    DAT_EEPROM=leer(200);
    ANO_ON_FLORA=DAT_EEPROM.toInt();
    DAT_EEPROM=leer(205);
    MES_ON_FLORA=DAT_EEPROM.toInt();
    DAT_EEPROM=leer(210);
    DIA_ON_FLORA=DAT_EEPROM.toInt();
    DAT_EEPROM=leer(240);
    ANO_OFF_FLORA=DAT_EEPROM.toInt();
    DAT_EEPROM=leer(245);
    MES_OFF_FLORA=DAT_EEPROM.toInt();
    DAT_EEPROM=leer(250);
    DIA_OFF_FLORA=DAT_EEPROM.toInt();
}   //    -----------------------------------------------------------------------------------------------------

bool alrm (const RtcDateTime& dt,int hon,int mon,int hof,int mof)
{
      uint8_t m=dt.Minute();
      uint8_t h=dt.Hour();        
      if(hon==hof)
      {
          if(hon==h)
          {
              if(mon==mof)
                  return 0;
              else
              {
                  if(mon<mof)
                  {
                      if(mon<=m&&m<mof)
                          return 1;
                      else
                          return 0;
                  }
                  else
                  {   
                      if(mon>m&&mof<=m)
                          return 0;
                      else
                          return 1;
                  }
              }
          }
          else
            return 0;
      }
      else if(hon<hof)
      {
            if(hon<h&&h<hof)
                return 1;
            else if(hon==h)
            {
              if(mon<=m)
                  return 1;
              else
                 return 0;
            }
            else if(hof==h)
            {
              if(mof>m)
                return 1;
              else
                return 0;
            }
            else
                return 0;
      }
      else if(hon>hof)
      {
            if(hon<h||hof>h)
                return 1;
            else if(hon>h&&h>hof)
                return 0;
            else if(hon==h)
            {
              if(mon<=m)
                return 1;
              else
                 return 0;
            }
            else if(hof=h)
            {
              if(mof<m)
                return 0;
              else
                return 1;
            }
      }
}   //    -----------------------------------------------------------------------------------------------------

char periodo(int diaon,int meson, int anoon,int diaof,int mesof,int anoof)
{
  RtcDateTime now = Rtc.GetDateTime();
  int ano_now=now.Year();
  int mes_now=now.Month();
  int dia_now=now.Day();
     if(anoof==anoon)
   {
    
       if(anoon==ano_now)
       {
               if(meson<mes_now||mes_now<mesof) return 1;

               if(meson==mes_now&&meson==mesof)
               {
                  if(diaon<=dia_now&&dia_now<diaof) return 1;
                  if(diaon==diaof) return -1;
                  else      return 0;
               }
              if(meson==mes_now)
               {
                  if(diaon<=dia_now) return 1;
                  else      return 0;
               }
               if(mesof==mes_now)
               {
                  if(diaof>dia_now) return 1;
                  else      return 0;
               }
               else return 0;
       }
   }
  if(anoon<anoof)
  {
          if(anoon==ano_now)
          {
                if(meson<mes_now) return 1;
                if(meson==mes_now)
                {
                  if(diaon<=dia_now) return 1;
                  else        return 0;
                }
                return 0;
          }
          if(anoof==ano_now)
          {
                if(mesof>mes_now) return 1;
                if(mesof==mes_now)
                {
                  if(diaof>dia_now) return 1;
                  else return 0;
                }
          }
          else return 0;
  }
  else return -1;
}   //    -----------------------------------------------------------------------------------------------------
void boot_config(LiquidCrystal_I2C lcd1, RtcDS3231<TwoWire> Rtc1)
{
      lcd1.setCursor(0, 0);
      lcd1.print("Inicializando...");
      while(1)
      {
          currentMillis3=millis();
          if (currentMillis3 >= 5000) 
          {       
                if( !digitalRead( BUTTON_MAS) && !digitalRead( BUTTON_MENOS) )
                {
                    menu(lcd1,Rtc1);
                    lcd1.clear();
                    break;
                }
                else
                {
                    lcd1.clear();
                     break;
                }        
          }  
          delay(250);     

      }
      return;
}

void menu(LiquidCrystal_I2C lcd1,RtcDS3231<TwoWire> rtc)
{
      int selecc=0;      //0 = Hora y Fecha ; 1 = Hora LUZ ON ; 2= Hora LUZ OFF; 3=Salir;
      int OPERACION;
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("-Configuracion-");
      delay(2000);
      print_config_options(selecc,lcd1);
      while(1)
      { 
                
                 if((OPERACION=leer_opcion())!=0)    // 1=MENOS;2=MAS;3=OK.
                {
                  
                    selecc=ejecutar_opcion(selecc,OPERACION,lcd1,rtc);
                    print_config_options(selecc,lcd1); 
                  if (selecc==-1){
                      delay(500);
                      return; 
                  }
                }
  //        }               
      }
      return;
} //-----------------------------------------------------

void print_config_options(int opcion,LiquidCrystal_I2C lcd1)
{
    if(opcion==0)
    {
         lcd1.setCursor(0, 0);
         lcd1.print("                  ");
         lcd1.setCursor(1, 0);
         lcd1.print("Fecha y Hora");       
    }
    else if(opcion==1)
    {
         lcd1.setCursor(0, 0);
         lcd1.print("                  ");
         lcd1.setCursor(0, 0);
         lcd1.print("HS LUZ ON VEGET.");        
    }
    else if(opcion==2)
    {
         lcd1.setCursor(0, 0);
         lcd1.print("                  ");
         lcd1.setCursor(0, 0);
         lcd1.print("HS LUZ OFF VEGET.");        
    }
            else if(opcion==3)
    {
         lcd1.setCursor(0, 0);
         lcd1.print("                  ");
         lcd1.setCursor(0, 0);
         lcd1.print("FECHA INICIO VEGET."); 
    }
            else if(opcion==4)
    {
         lcd1.setCursor(0, 0);
         lcd1.print("                  ");
         lcd1.setCursor(0, 0);
         lcd1.print("FECHA FIN VEGET."); 
    }
        else if(opcion==5)
    {
         lcd1.setCursor(0, 0);
         lcd1.print("                  ");
         lcd1.setCursor(0, 0);
         lcd1.print("HS LUZ ON FLORA");
    }  
        else if(opcion==6)
    {
         lcd1.setCursor(0, 0);
         lcd1.print("                  ");
         lcd1.setCursor(0, 0);
         lcd1.print("HS LUZ OFF FLORA"); 
    }   
            else if(opcion==7)
    {
         lcd1.setCursor(0, 0);
         lcd1.print("                  ");
         lcd1.setCursor(0, 0);
         lcd1.print("FECHA FIN FLORA"); 
    } 
    else if(opcion==OPCMAX)
    {
         lcd1.setCursor(0, 0);
         lcd1.print("                  ");
         lcd1.setCursor(0, 0);
         lcd1.print("Salir.");        
    }
    return;        
}   //-----------------------------------------------------------------------------------------------------------------
int leer_opcion(void){
  if(!digitalRead(BUTTON_MENOS))
  {
      flag++;
      if(!(flag%4)&&flag>50){
        return 1;
      }
      if(flag==4){
        return 1;
      }
  }
  else if(!digitalRead(BUTTON_MAS))
  {
      flag++;
      if(!(flag%4)&&flag>50){
        return 2;
      }
      if(flag==4){
        return 2;
      }
  }
  else if(digitalRead(BUTTON_TOUCH))
  {
      flag++;
      if(!(flag%4)&&flag>50){
        return 3;
      }
      if(flag==4){
        return 3;
      }
  }
  else
      flag=0;
  delay(20);
  return 0;
}   //    ---------
int ejecutar_opcion(int selecc, int op,LiquidCrystal_I2C lcd1,RtcDS3231<TwoWire> Rtc ) // 1=MENOS;2=MAS;3=OK.
{
      int fecha_dat[3];
      String HS_DATO;
      String DAT_EEPROM;
      if(op==2)
      {
          if(selecc==OPCMAX)    //si era salir
              return 0;
          else
            return (selecc+1);
      }
      else if(op==1)
      {
          if(selecc==0)
              return OPCMAX;
          else
            return (selecc-1);
      }
      else if(op==3)
          if(selecc==0){    //configurar la hora
             
              if (!Rtc.IsDateTimeValid()){
                lcd1.setCursor(0, 1);
                lcd1.print("TimeE ");
              }
              else{
                  RtcDateTime now = Rtc.GetDateTime();
                  fecha_dat[0]=(int)now.Year();
                   fecha_dat[1]=(int)now.Month();
                   fecha_dat[2]=(int)now.Day();
                  config_fecha(lcd1,Rtc,fecha_dat);
                  HS_DATO[0]=(char)(int)now.Hour();
                  HS_DATO[1]=(char)(int)now.Minute();
                   HS_DATO=config_hora(HS_DATO,lcd1);
                  RtcDateTime compiled = RtcDateTime(fecha_dat[0],fecha_dat[1],fecha_dat[2],HS_DATO[0],HS_DATO[1],0);
                  Rtc.SetDateTime(compiled);
              }
            }
          else if(selecc==1){   //configurar hora encendido luz veget
            HS_DATO=leer(100);
            HS_DATO=config_hora(HS_DATO,lcd1);
            grabar(100,HS_DATO);
            
          }
          else if(selecc==2){   //configurar hora apagado luz veget
            HS_DATO=leer(120);
            HS_DATO=config_hora(HS_DATO,lcd1);
            grabar(120,HS_DATO);
          }
          else if(selecc==3){   //INICIO VEGET
            DAT_EEPROM=leer(180);
            fecha_dat[0]=DAT_EEPROM.toInt();
            DAT_EEPROM=leer(185);
            fecha_dat[1]=DAT_EEPROM.toInt();
            DAT_EEPROM=leer(190);
            fecha_dat[2]=DAT_EEPROM.toInt();
            config_fecha(lcd1,Rtc,fecha_dat);                                        ////////////
            DAT_EEPROM=String(fecha_dat[0]);
            grabar(180,DAT_EEPROM);
            DAT_EEPROM=String(fecha_dat[1]);
            grabar(185,DAT_EEPROM);
            DAT_EEPROM=String(fecha_dat[2]);
            grabar(190,DAT_EEPROM);
          }
          else if(selecc==4){   //FIN VEGET
            DAT_EEPROM=leer(200);
            fecha_dat[0]=DAT_EEPROM.toInt();
            DAT_EEPROM=leer(205);
            fecha_dat[1]=DAT_EEPROM.toInt();
            DAT_EEPROM=leer(210);
            fecha_dat[2]=DAT_EEPROM.toInt();
            config_fecha(lcd1,Rtc,fecha_dat);                                        ////////////
            DAT_EEPROM=String(fecha_dat[0]);
            grabar(200,DAT_EEPROM);
            DAT_EEPROM=String(fecha_dat[1]);
            grabar(205,DAT_EEPROM);
            DAT_EEPROM=String(fecha_dat[2]);
            grabar(210,DAT_EEPROM);
          }                    
          else if(selecc==5){   //configurar hora encend luz flora
            HS_DATO=leer(140);
            HS_DATO=config_hora(HS_DATO,lcd1);
            grabar(140,HS_DATO);
          }
          else if(selecc==6){   //configurar hora apagado luz flora
            HS_DATO=leer(160);
            HS_DATO=config_hora(HS_DATO,lcd1);
            grabar(160,HS_DATO);
          }
          else if(selecc==7){   //FIN FLORA
            DAT_EEPROM=leer(240);
            fecha_dat[0]=DAT_EEPROM.toInt();
            DAT_EEPROM=leer(245);
            fecha_dat[1]=DAT_EEPROM.toInt();
            DAT_EEPROM=leer(250);
            fecha_dat[2]=DAT_EEPROM.toInt();
            config_fecha(lcd1,Rtc,fecha_dat);                                        ////////////
            DAT_EEPROM=String(fecha_dat[0]);
            grabar(240,DAT_EEPROM);
            DAT_EEPROM=String(fecha_dat[1]);
            grabar(245,DAT_EEPROM);
            DAT_EEPROM=String(fecha_dat[2]);
            grabar(250,DAT_EEPROM);
          }
          else if(selecc==OPCMAX){   //salir
            return -1;
          }
          return selecc+1; 
}   //------------------------------------------------------------------------------------------------------
String leer(int addr) {
   byte lectura;
   String strlectura;
   for (int i = addr; i < addr+4; i++) {
      lectura = EEPROM.read(i);
      if (lectura != 255) {
        strlectura += (char)lectura;
      }
   }
   return strlectura;
}   //-----------------------------------------------------------------------------------------------------
void grabar(int addr, String a) {
  int tamano = a.length(); 
  char inchar[4]; 
  a.toCharArray(inchar, tamano+1);
  for (int i = 0; i < tamano; i++) {
    EEPROM.put(addr+i, inchar[i]);
  }
  for (int i = tamano; i < 4; i++) {
    EEPROM.put(addr+i, 255);
  }
  EEPROM.commit();
}   //-----------------------------------------------------------------------------------------------------
String config_hora(String dat,LiquidCrystal_I2C lcd1)
{
      bool cursor1=0;
      char accion;
      char hs=dat[0];
      char mins=dat[1];
      char D[2];
      lcd1.setCursor(0, 1);
      lcd1.print("                  ");
      lcd1.setCursor(5, 1);
      lcd1.print(int(hs));
      lcd1.setCursor(7, 1);
      lcd1.print(":");
      lcd1.print(int(mins));
      lcd1.setCursor(5, 1);
      lcd1.blink();     
      while(1)
      {
            accion=leer_opcion();
            if(accion==1)
            {
                if(cursor1==0)
                {
                  if(hs<=0)
                  {
                        hs=23;
                  }
                  else
                      hs--;;
                  lcd1.setCursor(5, 1);
                  lcd1.print(int(hs));
                  if(hs<10)
                    lcd1.print(" ");
                  lcd1.setCursor(5, 1);                  

                }
                else if(cursor1==1)
                {
                  if(mins<=0)
                      mins=59;
                  else
                      mins--;
                  lcd1.setCursor(7, 1); 
                  lcd1.print("       ");  
                  lcd1.setCursor(7, 1);                 
                  lcd1.print(":");
                  lcd1.print(int(mins));
                  lcd1.setCursor(8, 1);
                }

            }
            if(accion==2)
            { 
                if(cursor1==0)
                {
                  if(hs>=23)
                        hs=0;
                  else
                      hs++;

                  lcd1.setCursor(5, 1);
                  lcd1.print(int(hs));
                  if(hs<10)
                    lcd1.print(" ");
                  lcd1.cursor();                  
                  lcd1.setCursor(5, 1); 
                }
                else if(cursor1==1)
                {
                  if(mins>=59)
                      mins=0;
                  else
                      mins++;
                    lcd1.setCursor(7, 1);
                    lcd1.print("       ");  
                    lcd1.setCursor(7, 1);                   
                    lcd1.print(":");
                    lcd1.print(int(mins));
                    lcd1.setCursor(8, 1);     
                }

                
            }            
            if(accion==3)
            {
                  if(cursor1==0)
                  {
                      cursor1++;
                      lcd1.setCursor(8, 1); 
                  }
                  else if(cursor1==1)
                  {                    
                    lcd1.setCursor(0, 1);
                    lcd1.print("                  ");
                    lcd1.noBlink();
                    lcd1.noCursor();
                    D[0]=hs;
                    D[1]=mins;
                    return D;
                  }
            }
      }
      
}   //------------------------------------------------------------------------------------------------------------
void config_fecha(LiquidCrystal_I2C lcd,RtcDS3231<TwoWire> Rtc,int * fecha_dat)                                                 //RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__); 
{                                                                     //Rtc.SetDateTime(compiled);
        int ano,dia,mes;
        char cursor1=0;
        char accion=0;
          ano=fecha_dat[0];
          mes=fecha_dat[1];
          dia=fecha_dat[2];
          lcd.blink();     

              lcd.setCursor(3, 1);
              lcd.print("                     ");
              lcd.setCursor(3, 1);
              lcd.print(int(dia));
              if(dia<10)
                    lcd.print(" ");
              lcd.print("/");
              lcd.print(int(mes));
              if(mes<10)
                    lcd.print(" ");
              lcd.print("/");
              lcd.print(int(ano));
              if(cursor1==0)
                  lcd.setCursor(4, 1);
              else if(cursor1==1)
                  lcd.setCursor(7, 1);
              else if(cursor1==2)
                  lcd.setCursor(12, 1);
          while(1)
          {
                          if(cursor1==0)
                  lcd.setCursor(4, 1);
              else if(cursor1==1)
                  lcd.setCursor(7, 1);
              else if(cursor1==2)
                  lcd.setCursor(12, 1);
            accion=leer_opcion();
            if(accion==1)
            {
                  if(cursor1==0)
                  {
                    if(dia==1)
                      dia=31;
                    else
                        dia--;
                    lcd.setCursor(3, 1);
                    lcd.print(int(dia));
                    if(dia<10)
                          lcd.print(" ");                
                  }
                  else if(cursor1==1)
                  {
                    if(mes==1)
                        mes=12;
                    else
                        mes--;
                    lcd.setCursor(6, 1);
                    lcd.print(int(mes));
                    if(mes<10)
                        lcd.print(" ");
                  }
                   else if(cursor1==2)
                  {
                    if(ano<=2018)
                        ano=2150;
                    else
                        ano--;
                    lcd.setCursor(9, 1);
                    lcd.print(int(ano));
                  }

             }     
      if(accion==2)
      { 
          if(cursor1==0)
          {
            if(dia==31)
                  dia=1;
            else
                dia++;
            lcd.setCursor(3, 1);
            lcd.print(int(dia));
            if(dia<10)
                  lcd.print(" ");                
          }
          else if(cursor1==1)
          {
            if(mes==12)
                mes=1;
            else
                mes++; 
            lcd.setCursor(6, 1);
            lcd.print(int(mes));
            if(mes<10)
                lcd.print(" ");   
          }
          else if(cursor1==2)
          {
            if(ano>=2150)
                ano=2018;
            else
                ano++; 
            lcd.setCursor(9, 1);
            lcd.print(int(ano));   
          }

          
      }            
      if(accion==3)
      {
            if(cursor1==0)
            {
                cursor1++;
            }
            else if(cursor1==1)
            {
                cursor1++;
            }
            else if(cursor1==2)
            {                    
              lcd.setCursor(0, 1);
              lcd.print("                  ");
              lcd.noBlink();
              lcd.noCursor();
              break;
            }
        }       //cierro if
            }   //cierra while1
              fecha_dat[0]=ano;
              fecha_dat[1]=mes;
              fecha_dat[2]=dia;
            return;
}
