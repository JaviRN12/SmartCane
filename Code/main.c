
#include <18F2553.h>
#include "Definitions.h"
#fuses NOPROTECT, NOWDT, CCP2C1
#device adc = 12
//#use delay (clock = 4000000)
#use delay(clock=8000000) 
#use RS232(uart1,BAUD=9600, BITS=8, PARITY=N, XMIT=PIN_C6, RCV=PIN_C7,STREAM = DATA1)
#include <stdlib.h>
#include <stdio.h>
//#include "audio_buzzer.c"
//#use delay (crystal = 20000000)
//#define set_zero(x) x=0 
//#define get_voltage(dataADC) (FLOAT) (dataADC * (5.0 / 4095.0))
   
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
         strcpy (str, buf); //copia en str lo que está en bufer
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

   
   set_timer0 (MAX_TIMER_PRESET);
}

void main()
{
   setup_oscillator (OSC_8MHZ|OSC_INTRC);
   setup_adc (ADC_CLOCK_INTERNAL);
   setup_adc_ports (AN0|VSS_VDD); //Read ADC0
   enable_interrupts (INT_RDA);
   enable_interrupts (INT_TIMER0);
   enable_interrupts (GLOBAL);
   set_timer0 (MAX_TIMER_PRESET);
   setup_timer_0 (T0_DIV_1);
   
   //Setup_Oscillator parameter not selected from Intr Oscillator Config tab
   Motor1_Start ();
   start_alert ();
   setup_timer_2 (T2_DIV_BY_16, 255, 1) ;
   //INT m = Salidas.a;

   // TODO: USER CODE!!
   WHILE (TRUE)
   {
      
      set_adc_channel (0);
      delay_us (10);
      data_adc0 = read_adc ();
      
      volt_adc0 = (FLOAT) (data_adc0 * (VoltajeMax / AdcCountMax));
      
      Pa = (INT8) (data_adc0 * (100.0 / AdcCountMax));
      duty1 = (INT8) data_adc0; //* (99.0 / 255.0);
      set_pwm1_duty ( (INT8) (volt_adc0 * 120.0));
      
      /* output_high (pin_a4) ;
      delay_ms (500);
      output_low (pin_a4);
      delay_ms (500);
      output_high (pin_a5);
      delay_ms (500);
      output_low (pin_a5);
      delay_ms (500); */
   }
}

void Motor1_Start()
{
   output_low (PIN_C2); //CCP1
   setup_ccp1 (CCP_PWM);
   setup_timer_2 (T2_DIV_BY_16, 255, 1);
   duty = 100;
   set_pwm1_duty (duty);
   delay_ms (250);
   duty = 0;
   set_pwm1_duty (duty);
   delay_ms (80);
   duty = 200;
   set_pwm1_duty (duty);
   delay_ms (200);
   duty = 0;
   set_pwm1_duty (duty);
   output_low (PIN_C2); //CCP1
}

void start_alert()
{
   output_low (PIN_C1); //ccp2
   setup_ccp2 (CCP_PWM);
   set_pwm2_duty (250);
   delay_ms (250);
   set_pwm2_duty (0);
   delay_ms (80);
   set_pwm2_duty (100);
   delay_ms (200);
   set_pwm2_duty (0);
   output_low (PIN_C2); //CCP2
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

