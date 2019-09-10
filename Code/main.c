#include <18F2553.h>
#fuses NOPROTECT, NOWDT
#device adc = 10
#use delay(clock=8000000)
#use RS232(BAUD=9600, BITS=8, PARITY=N, XMIT=PIN_C6, RCV=PIN_C7)

#include "Definitions.h"

void setup();
void start_alert();
void Motor1_Start();
void If_Message(void);
void process_tel_number();
void Get_GPS();
void gps_info_processor(void);
void dummyFunction(char characterRecived);
int16 AdcReadAndProcessing();

#INT_TIMER0
void t0()
{
   timerCounter++;
   if(timerCounter == ONE_SEC)
   {
      output_toggle(pin_a5);
      timerCounter = 0;
   }
   timerGPS++;
   if(timerGPS == TEN_SEC)
   {
      savePosition = 1;
      timerGPS = 0;
   }
}

#INT_rda
void rda_isr()
{
   volatile char characterRecived = getchar ();
   dummyFunction(characterRecived);
}

void main()
{
   int16 distancePower;
   setup();
   start_alert();
   Motor1_Start(); // Starts motor and confgures timer2
   set_adc_channel(0);
   
   while(TRUE)
   {    
      distancePower = AdcReadAndProcessing();
      set_pwm1_duty(distancePower);
      Get_GPS();
      gps_info_processor();
      If_Message();
      process_tel_number(); 
   }
}

void setup()
{
   setup_oscillator( OSC_8MHZ | OSC_INTRC ); 
   setup_adc (ADC_CLOCK_INTERNAL);
   setup_adc_ports (AN0 | VSS_VDD); //Read ADC0
   enable_interrupts(INT_TIMER0);
   enable_interrupts(INT_RDA);       
   enable_interrupts(GLOBAL);
   set_timer0(65535);
   setup_counters(T0_DIV_1, 1);
   setup_timer_0(T0_DIV_1);
}

void start_alert()
{
   setup_ccp1(CCP_PWM);
   setup_timer_2(T2_DIV_BY_16, 200, 1);
   output_low(PIN_C1); //ccp2
   setup_ccp2(CCP_PWM);
   set_pwm2_duty(190);        
   delay_ms(100);
   set_pwm2_duty(0);      
   delay_ms(40);
   set_pwm2_duty(100);      
   delay_ms(100);
   set_pwm2_duty(0);
   output_low(PIN_C1); //CCP2
}

void Motor1_Start()
{
   int duty = 100;

   output_low(PIN_C2); //CCP1
   setup_ccp1(CCP_PWM);
   setup_timer_2(T2_DIV_BY_16, 255, 1);
   set_pwm1_duty(duty);        
   delay_ms(250);
   duty = 0;
   set_pwm1_duty(duty);      
   delay_ms(80);
   duty = 200;
   set_pwm1_duty(duty);      
   delay_ms(200);
   duty = 0;
   set_pwm1_duty(duty);    
   output_low(PIN_C2); //CCP1
}

void If_Message(VOID)
{
   char _Cel1[8], _Cel2[8];

   for (INT indexM = 0; indexM < 5; IndexM++)
   {
      _Cel1[indexM] = read_eeprom (indexM + 25);
      _Cel2[indexM] = read_eeprom (indexM + 30);
   }

   float Num1 = atof (_Cel1);
   float Num2 = atof (_Cel2);
   
   for (int index = 1; index < 10; index++)
   {
      LatRead[index - 1] = read_eeprom(index);
      LonRead[index - 1] = read_eeprom(index + 10);
   }
   
   if (Send_Message)
   {
      set_pwm2_duty(5);   
      output_high(pin_a4);
      printf ("AT+CMGF=1\r") ;
      delay_ms(10);
      printf("AT+CMGS=\"+52%.0f%.0f\"\r",Num1,Num2);
      //printf("AT+CMGS=\"+52%.0f%.0f\"\r",Num1,Num2);
      delay_ms(10);
      printf("https://maps.google.com/?q=%.6f,-%.6f\x1a\r",atof(LatRead),atof(LonRead));
      delay_ms(1500);
      set_pwm2_duty(0);
      output_low(pin_a4);
   }
}

void process_tel_number()
{
   if(1 == str_flag2)
   {
      if(str[1]=='@')
            {
               set_pwm2_duty(150); // Beep when a telephone number is configured
               delay_ms(40);
               set_pwm2_duty(0);   // End beep
               delay_ms(40);
               set_pwm2_duty(150); // Beep when a telephone number is configured
               delay_ms(40);
               set_pwm2_duty(0);   // End beep
               delay_ms(40);
               set_pwm2_duty(150); // Beep when a telephone number is configured
               delay_ms(40);
               set_pwm2_duty(0);   // End beep
               for(int index = 0; index < 10; index++)
               {
                   write_eeprom(index + 25, str[index + 2]);
               }
            }
   }
   str_flag2 = 0;
}

void Get_GPS(void)
{
   char lat[9], lg[10];

   if ( (str_flag == 1 ) && (savePosition == 1) )
   {
      ///////////////////// Si el str[4] == G obtenemos de la trama los valores de la latitud
      if (str[4] == 'G')
      {
         for (INT index = 0; index < 9; index++)
         {
            lat[index] = str[index + 18];
         }

         latitud = atof (lat);
        // output_toggle (pin_a4);
      }

      /////////////////// Cuando str[4] = M obtenemos de la trama los valores de longitud en string
      else if (str[4] == 'M')
      {
         for (INT index = 0; index < 10; index++)
         {
            lg[index] = str[index + 32];
         }

         longitud = atof (lg);
      }
   }

   str_flag = 0;
}

void gps_info_processor(void)
{
   // Alert that GPS is Connected/Disconnected
      // NOTE: Change set_pwm1_duty to set_pwm2_duty when buzzer works properly
      if( (latitud > 0.5) && (GPS_Connected == 0) )
      {
        //GPS Connected
        set_pwm2_duty(130);        
        delay_ms(100);
        set_pwm2_duty(0); 
        delay_ms(20);
        set_pwm2_duty(130);        
        delay_ms(100);
        set_pwm2_duty(0); 
        delay_ms(20);
        set_pwm2_duty(130);        
        delay_ms(100);
        set_pwm2_duty(0); 
        GPS_Connected = 1;
      }
      else if( (latitud < 0.5) && (GPS_Connected == 1) )
      {
      //GPS Disconnected
        set_pwm2_duty(80);        
        delay_ms(500);
        set_pwm2_duty(0);
        GPS_Connected = 0;
      }
      
      // First store position into eeprom memory every 10 seconds
      if( (savePosition == 1) && (GPS_Connected == 1) )
      {
         // sprintf(LatWrite,"%.6f",latitud*0.01);
         // sprintf(LonWrite,"%.6f",longitud*0.01);
         latitudConverted  = (float) ( (int8) (latitud * 0.01)  + (float) (((latitud * 0.01)  - (int8) (latitud * 0.01))  / 60.0)*100 );
         longitudConverted = (float) ( (int8) (longitud * 0.01) + (float) (((longitud * 0.01) - (int8) (longitud * 0.01)) / 60.0)*100 );
         sprintf(LatWrite,"%.6f", latitudConverted);
         sprintf(LonWrite,"%.6f", longitudConverted);
         for(int index = 1; index < 10; index++)
         {
            write_eeprom(index, LatWrite[index - 1]);
            write_eeprom(index + 10, LonWrite[index - 1]);
         }
         savePosition = 0;
      }
}

void dummyFunction(char characterRecived)
{
   if (characterRecived == '$')
   {
      i = 0;
      flag_buf = 1;
   }

   if (characterRecived == '<')
   {
      i = 0;
      flag_buf = 2;
   }

   if (flag_buf == 1)
   {
      //recibimos un entero sin signo
      if (characterRecived == 0x0d)
      {
         //si recibe un enter --- o un salto de linea
         flag_buf = 0;
         str_flag = 1; //flag must be cleared by software
         characterRecived = 0; //fin de la trama
         strcpy (str, buf); //copia en str lo que est� en bufer
      }

      else
      {
         buf[i] = characterRecived;
         i++;
      }
   }

   if (flag_buf == 2)
   {
      if (characterRecived == '>')
      {
         flag_buf = 0;
         str_flag2 = 1; //flag must be cleared by software
         characterRecived = 0; //fin de la trama
         strcpy (str, buf) ;
      }

      else
      {
         buf[i] = characterRecived;
         i++;
      }
   }
}

int16 AdcReadAndProcessing()
{
   int16 data_adc0 = 0, mmDistance = 0, distancePower = 0; 

   delay_us(20);
   data_adc0  = read_adc();     // 0 - 1024 bits
   
   if(data_adc0 > DISTANCE_LIMIT)
   {
      data_adc0 = DISTANCE_LIMIT;
   }
   
   mmDistance = 7680 - ((data_adc0*2) * 5.0); // 0 - 2500mm

   voltage    = (mmDistance/(1000.0));  // convert distance to voltage, the higher the distance, the higher the voltage.
                                          // *2 due to we want the volteage to reach up to 5 volts
   
   distancePower = (int16)(voltage * 49);

   return(distancePower);
}
