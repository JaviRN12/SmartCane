#include <18F2553.h>
#fuses NOPROTECT, NOWDT, CCP2C1
#device adc = 10
//#use delay (clock = 4000000)
#use delay(clock=8000000) 
//#include "audio_buzzer.c"
//#use delay (crystal = 20000000)
#define MAX_TIMER_PRESET 60000
#define DELAY_SET_ADC    10
#define MIN_ADC_MEASURE 242
#define MAX_POWER 255
#define LIMTED_POWER 180
#define ADC_SAMPLES 1
#define get_voltage(dataADC) (float) (dataADC * (5.0 / 1023.0))// 4095.0))


int duty = 0, t_flag = 0;

int16 data_adc0, data_adc1, Pa, Pb;
float volt_adc0, volt_adc1;
int duty1, duty2;
int counterLED = 0;
int samplesCounter = 0;
float meanfilter = 0;
int16 filterbuffer = 0;

enum Power
{
   POWER_OFF = 4,  //80% Off
   POWER_ON  = 5   //20% On
};

void Motor1_Start();
void start_alert();

//TIMER0 interrupt
#int_TIMER0
void t0()
{
   counterLED++;
   if( POWER_OFF == counterLED )
   {
      output_high(pin_a4);
   }
   else if ( POWER_ON == counterLED )
   {
      output_low(pin_a4);
      counterLED = 0;
   }
   
   set_timer0( MAX_TIMER_PRESET );
}

void main()
{
   setup_oscillator( OSC_8MHZ | OSC_INTRC ); 
   setup_adc (ADC_CLOCK_INTERNAL);   
   setup_adc_ports (AN0 | VSS_VDD); //Read ADC0 and ADC1
   
   //enable_interrupts(INT_RDA);
   enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);
   set_timer0( MAX_TIMER_PRESET );
   setup_timer_0( T0_DIV_1 );
   
//Setup_Oscillator parameter not selected from Intr Oscillator Config tab
   Motor1_Start();
   delay_ms(500);
   //start_alert();
   setup_timer_2(T2_DIV_BY_16,255,1);
   //int m = Salidas.a;
   // TODO: USER CODE!!
   //output_high(pin_a5);
   While (TRUE) 
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
      
      /*output_high(pin_a4);
      delay_ms(500);
      output_low(pin_a4);
      delay_ms(500);
      output_high(pin_a5);
      delay_ms(100);
      output_low(pin_a5);
      delay_ms(100);*/
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


