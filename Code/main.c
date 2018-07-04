
#include <18F2553.h>
#include "Definitions.h"
#fuses NOPROTECT, NOWDT, CCP2C1
#device adc = 12
//#use delay (clock = 4000000)
#use delay(clock=8000000) 
#use RS232(uart1,BAUD=9600, BITS=8, PARITY=N, XMIT=PIN_C6, RCV=PIN_C7,STREAM = DATA1)
#include <stdlib.h>
#include <stdio.h>
#define MAX_TIMER_PRESET 60000
#define DELAY_SET_ADC    10
#define MIN_ADC_MEASURE 242
#define MAX_POWER 255
#define LIMTED_POWER 180
#define ADC_SAMPLES 1
#define get_voltage(dataADC) (float) (dataADC * (5.0 / 1023.0))// 4095.0))

int counterLED = 0;
int samplesCounter = 0;
float meanfilter = 0;
int16 filterbuffer = 0;
int GPS_Cnt = 0, Us_Cnt = 0, GPS_Connected = 0;
int Save_flag = 0;
int GPS_Counter = 0;
char LatWrite[10];
char LonWrite[10];

enum Power
{
   POWER_OFF = 4, //80 % Off
   POWER_ON = 5, //20 % On
   VoltajeMax = 5.0,
   AdcCountMax = 4095.0
};

void Motor1_Start();
void start_alert();
void If_Message(VOID);
void Get_GPS(VOID);
void Bluetooth_Config(void);
void If_Message(void);

//!//UART1 Interrupt
#INT_rda

void rda_isr()
{
   c = getchar ();
   
   IF (c == '$')
   {
      i = 0;
      flag_buf = 1;
   }

   IF (c == '<')
   {
      i = 0;
      flag_buf = 2;
   }

   IF (flag_buf == 1)
   {
      //recibimos un entero sin signo
      IF (c == 0x0d)
      {
         //si recibe un enter --- o un salto de linea
         flag_buf = 0;
         str_flag = 1; //flag must be cleared by software
         c = 0; //fin de la trama
         strcpy (str, buf); //copia en str lo que est� en bufer
      }

      ELSE
      {
         buf[i] = c;
         i++;
      }
   }

   IF (flag_buf == 2)
   {
      IF (c == '>')
      {
         flag_buf = 0;
         str_flag2 = 1; //flag must be cleared by software
         c = 0; //fin de la trama
         strcpy (str, buf) ;
      }

      ELSE
      {
         buf[i] = c;
         i++;
      }
   }
}

//TIMER0 interrupt
#INT_TIMER0

void t0()
{
   counterLED++;

   IF (POWER_OFF == counterLED)
   {
      output_high (pin_a4);
   }

   else IF (POWER_ON == counterLED)
   {
      output_low (pin_a4);
      counterLED = 0;
   }

   GPS_Counter++;
   if(GPS_Counter == 80)
   {
      if(t_flag == 1)
      {
          t_flag = 0;
      }
      else
      {
          t_flag = 1;
      }
      GPS_Counter = 0;
   //   output_toggle(pin_e2);
   }
   
   set_timer0 (MAX_TIMER_PRESET);
}

void main()
{
   setup_oscillator( OSC_8MHZ | OSC_INTRC ); 
   setup_adc (ADC_CLOCK_INTERNAL);   
   setup_adc_ports (AN0 | VSS_VDD); //Read ADC0
   
   enable_interrupts(INT_RDA);
   enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);
   set_timer0( MAX_TIMER_PRESET );
   setup_timer_0( T0_DIV_1 );
   
   //Setup_Oscillator parameter not selected from Intr Oscillator Config tab
   Motor1_Start ();
   start_alert ();
   setup_timer_2 (T2_DIV_BY_16, 255, 1) ;
   //INT m = Salidas.a;

   // TODO: USER CODE!!
   WHILE (TRUE)
   {
      
      set_adc_channel(0);
      delay_us(10);
      data_adc0 = read_adc();
     /* samplesCounter++;
      filterbuffer = filterbuffer + data_adc0;
      if(ADC_SAMPLES == samplesCounter)
      {
         meanfilter = (float)(filterbuffer / ADC_SAMPLES);
         filterbuffer = 0;
      }
      */
      
      if( MIN_ADC_MEASURE > data_adc0)
      {
         output_toggle(pin_a5);
         duty1 = MAX_POWER;
      }
      else
      {
         duty1 = (float) ( LIMTED_POWER ) - ( data_adc0 - 240 )*( 255.0/1023.0 );
      }
      //volt_adc0 = get_voltage(data_adc0);
      
      set_pwm1_duty((int8)duty1);
      
      // Bluetooth implementation
      Bluetooth_Config();
      // Get current GPS position
      Get_GPS();
      
      // Alert that GPS is Connected/Disconnected
      // NOTE: Change set_pwm1_duty to set_pwm2_duty when buzzer works properly
      if(latitud > 0.5 && GPS_Connected == 0)
      {
        //GPS Connected
        set_pwm1_duty(230);        
        delay_ms(100);
        set_pwm1_duty(0); 
        delay_ms(20);
        set_pwm1_duty(230);        
        delay_ms(100);
        set_pwm1_duty(0); 
        delay_ms(20);
        set_pwm1_duty(230);        
        delay_ms(100);
        set_pwm1_duty(0); 
        GPS_Connected = 1;
      }
      else if(latitud < 0.5 && GPS_Connected == 1)
      {
      //GPS Disconnected
        set_pwm1_duty(80);        
        delay_ms(500);
        set_pwm1_duty(0);
        GPS_Connected = 0;
      }
      
      // First store position into eeprom memory every 80 cycles of T0
      // then read it and store into arrays
      if(t_flag == 0)
      {
         //printf("Lat: %f, Lon: %f\n\r", latitud, longitud);
         if(Save_flag == 1)
         {
            sprintf(LatWrite,"%.6f",latitud*0.01);
            sprintf(LonWrite,"%.6f",longitud*0.01);
            for(int index = 1; index < 10; index++)
            {
               write_eeprom(index, LatWrite[index - 1]);
               write_eeprom(index + 10, LonWrite[index - 1]);
            }
            Save_flag = 0;
         }
      }
      else
      {
         // printf("%lu | %lu\r\n", Motor_CCP2, y2);
          if(Save_flag == 0)
          {
            for(int index = 1; index < 10; index++)
            {
                LatRead[index - 1] = read_eeprom(index);
                LonRead[index - 1] = read_eeprom(index + 10);
            }
            // Discomment this for testing purposes
          //  printf("LatRead: %.6f\r\n", atof(LatRead));
            //delay_ms(5);
          //  printf("LonRead: %.6f\r\n", atof(LonRead));
            //delay_ms(5);
          //  printf("Numero: %s%s \r\n", _Cel1, _Cel2);
            //delay_ms(10);
            
            Save_flag = 1;
          }
      }
      
      If_Message(); 
     
      
   }
}

void Motor1_Start()
{
   output_low(PIN_C2); //CCP1
   setup_ccp1(CCP_PWM);
   setup_timer_2(T2_DIV_BY_16, 255, 1);
   duty = 100;
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

void start_alert()
{
   output_low(PIN_C1); //ccp2
   setup_ccp2(CCP_PWM);
   set_pwm2_duty(250);        
   delay_ms(250);
   set_pwm2_duty(0);      
   delay_ms(80);
   set_pwm2_duty(100);      
   delay_ms(200);
   set_pwm2_duty(0);
   output_low(PIN_C2); //CCP2
}

void If_Message(VOID)
{
   for (INT indexM = 0; indexM < 5; IndexM++)
   {
      _Cel1[indexM] = read_eeprom (indexM + 25);
      _Cel2[indexM] = read_eeprom (indexM + 30);
   }

   
   Num1 = atof (_Cel1);
   Num2 = atof (_Cel2);
   
   IF (Send_Message)
   {
      printf ("AT + CMGF = 1\r") ;
      delay_ms (10);
      printf("AT+CMGS=\"+52%.0f%.0f\"\r",Num1,Num2);;
      delay_ms (10);
      printf("https://maps.google.com/?q=%.6f,-%.6f\x1a\r",atof(LatRead),atof(LonRead));
      delay_ms (1500);
   }
}

void Get_GPS(VOID)
{
   CHAR lat[9], lg[10];

   IF ( (str_flag == 1)&& (t_flag == 1) )
   {
      ///////////////////// Si el str[4] == G obtenemos de la trama los valores de la latitud
      IF (str[4] == 'G')
      {
         for (INT index = 0; index < 9; index++)
         {
            lat[index] = str[index + 18];
         }

         latitud = atof (lat);
         output_toggle (pin_b2);
      }

      /////////////////// Cuando str[4] = M obtenemos de la trama los valores de longitud en string
      else IF (str[4] == 'M')
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


void Bluetooth_Config(void)
{
   if(str_flag2 == 1)
   {
      if(str[1]=='@')
            {
              // output_toggle(pin_e2);
               for(int index = 0; index < 10; index++)
               {
                   write_eeprom(index + 25, str[index + 2]);
               }
            }
      //Cel = atoi(Num);
      //sprintf(Num,"%i",Cel);
   }
   str_flag2 = 0;
}
