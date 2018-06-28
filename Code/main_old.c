#include <18F8527.h>
#include <stdio.h>
#include <string.h>
#fuses INTRC, NOPROTECT, NOWDT, CCP2E7
#use delay (crystal = 20000000)
//#use delay (clock = 4000000)
#include <stdlib.h>
#include <math.h>
#use RS232(uart1,BAUD=9600, BITS=8, PARITY=N, XMIT=PIN_C6, RCV=PIN_C7,STREAM = DATA1)
///String lenght
#define buf_length 255
#define trig pin_e5
#define echo input(pin_e6)
#define Send_Message input(pin_B3)
#define trig2 pin_E3
#define echo2 input(pin_E4)
///Global Variables
int duty = 0, t_flag = 0;
char buf[buf_length],str[buf_length],c;
int i=0, str_flag=0;
char buf2[buf_length],str2[buf_length];
int str_flag2=0,i2=0;
float latitud=0, longitud=0;
int flag_buf=0;
int16 centimetros = 0, Motor_CCP2 = 0, y = 0;
int16 centimetros2 = 0, Motor2_CCP3 = 0, y2 = 0, antCentimetros = 0, antCentimetros2 = 0;
int Us_Cnt2 = 0;
int GPS_Cnt = 0, Us_Cnt = 0, GPS_Connected = 0;
int Save_flag = 0;
char LatWrite[10], LatRead[10];
char LonWrite[10], LonRead[10];
char _Cel1[8], _Cel2[8];
float Num1, Num2;
int GPS_Counter = 0, timerCount = 0, timerCount2 = 0;

//Function declaration
void Motor1_Start();
void Get_GPS(void);
void Ultrasonic_1(void);
void Ultrasonic_2(void);
void Bluetooth_Config(void);
void If_Message(void);
void PruebaPwm(void);
//Equation y = 3E-05x^3 - 0.0104x^2 - 0.1837x + 255
//C function y = 0.00003*x*x*x - 0.0104*x*x - 0.1837*x + 255;

//!//UART1 Interrupt
#int_rda
void rda_isr()
{
       //  output_toggle(pin_e2);
        c = getchar();
        if(c=='$')
        {
           i=0;
           flag_buf = 1;
        }
        if(c == '<')
        {
       // output_toggle(pin_e2);
           i = 0;
           flag_buf = 2;
        }
        if(flag_buf == 1)
        {
          //recivimos un entero sin signo 
          if(c == 0x0d)    //si recive un enter --- o un salto de linea
          {
             flag_buf = 0;
             str_flag=1;  //flag must be cleared by software
             c=0;  //fin de la trama
             strcpy(str,buf); //copia en str lo que está en bufer
             //i=0;  // lo pone en cero
          }
          else
          {
            buf[i]=c;
            i++;
          }
        }
        if(flag_buf == 2)
        {
            if(c == '>')
            {
               flag_buf = 0;
               str_flag2 = 1;  //flag must be cleared by software
               c=0;  //fin de la trama
               strcpy(str,buf);
            }
            else
            {
               buf[i]=c;
               i++;
            }
        }
}

//UART2 Interrupt

//TIMER0 interrupt
#int_TIMER0
void t0()
{
//!   timerCount++;
//!   if(timerCount == y)
//!   {
//!      if(y > 2.4)
//!      {
//!         output_low(pin_E7);
//!        // output_low(pin_e2);
//!      }
//!      else
//!      {  
//!         output_toggle(pin_E7);
//!       //  output_toggle(pin_e2);
//!      }
//!      timerCount = 0;
//!   }
   
   timerCount2++;
   if(timerCount2 == y2)
   {
   if(y2 > 24)
      {
        // output_low(pin_G0);
        // output_low(pin_e2);
      }
      else
      {  
         output_toggle(pin_G0);
         output_toggle(pin_e2);
      }
      timerCount2 = 0;
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
   
  // output_toggle(pin_e2);
   //output_toggle(trig);
  // output_toggle(trig2);
   //set_timer0(10);
}

void main()
{
   enable_interrupts(INT_RDA);
   //enable_interrupts(INT_RDA2);
   enable_interrupts(INT_TIMER0);
   //enable_interrupts(INT_TIMER2);
   enable_interrupts(GLOBAL);
   //set_timer0(65535);
   setup_counters(T0_DIV_1, 1);
  // setup_timer_0(T0_DIV_1);
   
   //Smart cane is turned ON
   Motor1_Start();
   Output_low(trig);
   
   While (true)
   {
     // output_toggle(pin_e2); // PIN_E2 en PCB
      
      delay_ms(10);
      If_Message();
      ///<summary>
      /// Ultrasonic1 implementation
      /// Includes a filter to promediate the distance acquired by ultrasonic sensor
      /// and a control function for the PWM, in order to obtain a more adecuated output
   //   antCentimetros = centimetros;
      ///</summary>
     // Ultrasonic_1();
      // Filter implementation
     // if(Us_Cnt == 4)
     // {
     //    Us_Cnt = 0;
     //    Motor_CCP2 = aux/4;
     //    aux = 0;
     // y = 0.00003*Motor_CCP2*Motor_CCP2*Motor_CCP2 - 0.0104*Motor_CCP2*Motor_CCP2 - 0.1837*Motor_CCP2 + 255;
     // y = 0.00003*centimetros*centimetros*centimetros - 0.0104*centimetros*centimetros - 0.1837*centimetros + 255;
     //y = 255 - centimetros;
   
   //    y = centimetros;
  //     y = y*0.2;
      //set_pwm2_duty(y);
     // }
     // else
     // {
     //    aux = aux + centimetros;
         
     //    Us_Cnt++;
     // }
   //   centimetros = 0;
      ///<summary>
      /// Ultrasonic2 implementation
      ///</summary>
      antCentimetros2 = centimetros2;
   //   Ultrasonic_2();                         ///Segundo ultrasonico
      // Filter 
      //if(Us_Cnt2 == 1)
      //{
      //   Us_Cnt2 = 0;
      //   Motor2_CCP3 = aux2/4;
      //   aux2 = 0;
   /*   if(centimetros2 > 250 && centimetros2 < 500)
      {
         centimetros2 = 250;
      }
      else if(centimetros2 < 10)
      {
         centimetros2 = 10;
      }
      else if(centimetros2 > 500)
      {
         centimetros2 = antCentimetros2;
      }*/
      
        y2 = centimetros2;
        y2 = y2*0.2;
     // y2 = 0.00003*centimetros2*centimetros2*centimetros2 - 0.0104*centimetros2*centimetros2 - 0.1837*centimetros2 + 255;
     // set_pwm3_duty(y2);
      //}
      //else
      //{
       //  aux2 = aux2 + centimetros2;
       //  Us_Cnt2++;
      //}
   //   printf("%lu\n\r",centimetros2);
   //   printf("%lu\n\r",y2);
      centimetros2 = 0;
      ///<summary>
      /// Bluetooth implementation
      ///</summary>
      Bluetooth_Config();
      ///<summary>
      /// GPS implementation
      ///</summary>
      Get_GPS();
      ///<summary>
      /// GPS Connected
      ///</summary>
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
          //  printf("LatRead: %.6f\r\n", atof(LatRead));
            //delay_ms(5);
          //  printf("LonRead: %.6f\r\n", atof(LonRead));
            //delay_ms(5);
          //  printf("Numero: %s%s \r\n", _Cel1, _Cel2);
            //delay_ms(10);
            
            Save_flag = 1;
          }
      }
     //
   }
}

void Motor1_Start()
{
   output_low(PIN_C2); //CCP1
   setup_ccp1(CCP_PWM);
   setup_timer_2(T2_DIV_BY_16, 255, 1);
   duty = 200;
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
   
   /*output_low(PIN_E7); //ccp2
   setup_ccp2(CCP_PWM);
   duty = 200;
   set_pwm2_duty(duty);        
   delay_ms(200);
   duty = 0;
   set_pwm2_duty(duty);      
   delay_ms(80);
   duty = 200;
   set_pwm2_duty(duty);      
   delay_ms(200);
   duty = 0;
   set_pwm2_duty(duty);*/
   
  /* output_low(PIN_G0); //ccp3
   setup_ccp3(CCP_PWM);
   duty = 200;
   set_pwm3_duty(duty);        
   delay_ms(200);
   duty = 0;
   set_pwm3_duty(duty);      
   delay_ms(80);
   duty = 200;
   set_pwm3_duty(duty);      
   delay_ms(200);
   duty = 0;
   set_pwm3_duty(duty); */
}

void Get_GPS(void)
{
   char lat[9],lg[10];
       if((str_flag == 1) && (t_flag == 1))
       {
   /////////////////////   Si el str[4] == G obtenemos de la trama los valores de la latitud
          if(str[4]=='G')
            {
               for(int index = 0; index < 9; index++)
               {
                  lat[index]=str[index+18];
               }
               latitud = atof(lat);
               output_toggle(pin_b2);
            }
      ///////////////////  Cuando str[4] = M obtenemos de la trama los valores de longitud en string
         else if(str[4]=='M')
            {
               for(int index = 0; index < 10; index++)
               {
                  lg[index]=str[index + 32];
               }
               longitud = atof(lg);
            }
      }
      str_flag = 0;
}

void Ultrasonic_1(void)
{
      output_high(trig);
      delay_ms(50);
      output_low(trig);
      while(echo==0){}
      //comienza a contar centimetros hasta que pin echo sea cero
      while(echo==1)
      {
            if(centimetros > 250)
            {
               centimetros = 250;
               break;
            }
            else
            {
               centimetros++;
               delay_us(58);
            }
      }
}

void Ultrasonic_2(void)
{
      output_high(trig2);
      delay_us(15);
      output_low(trig2);
      while(echo2==0){}
      //comienza a contar centimetros hasta que pin echo sea cero
      while(echo2 == 1)
      {
            if(centimetros2 > 250)
            {
               centimetros2 = 250;
               break;
            }
            else
            {
               centimetros2++;
               delay_us(58);
            }
      }
      delay_ms(50);
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


void If_Message(void)
{
   for(int indexM = 0; indexM < 5; IndexM++)
   {
      _Cel1[indexM] = read_eeprom(indexM + 25);
      _Cel2[indexM] = read_eeprom(indexM + 30);
   }
   
   Num1 = atof(_Cel1);
   Num2 = atof(_Cel2);
   
   if(Send_Message)
   {
      printf("AT+CMGF=1\r");
      delay_ms(10);
      printf("AT+CMGS=\"+52%.0f%.0f\"\r",Num1,Num2);
      delay_ms(10);
      printf("http://www.google.com/maps/place/%.6f,-%.6f\x1a\r",atof(LatRead),atof(LonRead));
      delay_ms(1500);
   }
}


void PruebaPwm()
{
   output_low(PIN_G0); //ccp3
   setup_ccp3(CCP_PWM);
   duty = 200;
   set_pwm3_duty(duty);        
   delay_ms(200);
   duty = 0;
   set_pwm3_duty(duty);      
   delay_ms(80);
   duty = 200;
   set_pwm3_duty(duty);      
   delay_ms(200);
   duty = 0;
   set_pwm3_duty(duty); 
}
