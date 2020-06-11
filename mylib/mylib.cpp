#include "mylib.h"
unsigned long currentMillis3;


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
      char OPERACION;
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("-Configuracion-");
      delay(2000);
      while(1)
      {
          currentMillis3=millis();
          if (currentMillis3 >= 200) 
          { 
                print_config_options(selecc,lcd1);
                 OPERACION=leer_opcion();    // 0=MENOS;1=MAS;2=OK.
                selecc=ejecutar_opcion(selecc,OPERACION,lcd1,rtc); 
                if (selecc==-1){
                    delay(500);
                    return; 
                }
          }               
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
	volatile int flag;
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
      if(flag==2){
        return 2;
      }
  }
  else if(digitalRead(BUTTON_TOUCH))
  {
      flag++;
      if(!(flag%4)&&flag>50){
        return 3;
      }
      if(flag==2){
        return 3;
      }
  }
  else
      flag=0;
  delay(20);
  return 0;
}   //    ---------
/*int leer_opcion(void)
{          
        while(1)
        {
                    delay(100);
                    if(!digitalRead(BUTTON_MAS))
                    {
                         delay(DELAYBUTTONMS);
                         return 1;
                    }
                    if(!digitalRead(BUTTON_MENOS))
                    {
                        delay(DELAYBUTTONMS);
                        return 0;
                    }
                    if(digitalRead(BUTTON_TOUCH))
                    {
                         delay(DELAYBUTTONMS);
                        return 2;
                    }
          }
} *///-------------------------------------------------------------------------------------------------------------------
int ejecutar_opcion(int selecc, int op,LiquidCrystal_I2C lcd1,RtcDS3231<TwoWire> Rtc ) // 0=MENOS;1=MAS;2=OK.
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
            if(accion==0)
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
            if(accion==1)
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
            if(accion==2)
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
        char accion;
          ano=fecha_dat[0];
          mes=fecha_dat[1];
          dia=fecha_dat[2];
          lcd.blink();     
          while(1)
          {
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
            accion=leer_opcion();
            if(accion==0)
            {
                  if(cursor1==0)
                  {
                    if(dia==1)
                      dia=31;
                    else
                        dia--;;                
                  }
                  else if(cursor1==1)
                  {
                    if(mes==1)
                        mes=12;
                    else
                        mes--;
                  }
                   else if(cursor1==2)
                  {
                    if(ano<=2018)
                        ano=2150;
                    else
                        ano--;
                  }

             }     
      if(accion==1)
      { 
          if(cursor1==0)
          {
            if(dia==31)
                  dia=1;
            else
                dia++;
          }
          else if(cursor1==1)
          {
            if(mes==12)
                mes=1;
            else
                mes++;    
          }
          else if(cursor1==2)
          {
            if(ano>=2150)
                ano=2018;
            else
                ano++;    
          }

          
      }            
      if(accion==2)
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
   //-------------------------------------------------------------------


/*  int num=257;
    String leido;
    String snum=String(num);
    grabar(10,snum);

}
      String leido;
      leido=leer(10);
      lcd.setCursor(4, 1);
      lcd.print(int(leido.toInt())); */