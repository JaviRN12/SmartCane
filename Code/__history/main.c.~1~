#include <18F2553.h>
#fuses NOPROTECT, NOWDT, CCP2C1
#device adc = 12
//#use delay (clock = 4000000)
#use delay(clock=8000000) 
//#include "audio_buzzer.c"
//#use delay (crystal = 20000000)
#define MAX_TIMER_PRESET 65535
#define DELAY_SET_ADC    10
//#define set_zero(x) x=0 
//#define get_voltage(dataADC) (float) (dataADC * (5.0 / 4095.0))


int duty = 0, t_flag = 0;

int16 data_adc0, data_adc1, Pa, Pb;
float volt_adc0, volt_adc1;
int duty1, duty2;
int counterLED = 0;

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
   
   set_timer0(MAX_TIMER_PRESET);
}

void main()
{
   setup_oscillator( OSC_8MHZ | OSC_INTRC ); 
   setup_adc(ADC_CLOCK_INTERNAL);   
   setup_adc_ports (AN0 | VSS_VDD); //Read ADC0
   enable_interrupts(INT_RDA);
   enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);
   set_timer0(MAX_TIMER_PRESET);
   setup_timer_0(T0_DIV_1);
   
//Setup_Oscillator parameter not selected from Intr Oscillator Config tab
   Motor1_Start();
   start_alert();
   setup_timer_2(T2_DIV_BY_16,255,1);
   //int m = Salidas.a;
   // TODO: USER CODE!!
   While (TRUE) 
   {
      
      set_adc_channel(0);
      delay_us(10);
      data_adc0 = read_adc();
      
      volt_adc0 = (float) (data_adc0 * (5.0 / 4095.0));
      
      Pa = (int8)(data_adc0*(100.0/4095.0));
      duty1 = (int8) data_adc0;//*(99.0/255.0);
      set_pwm1_duty((int8)(volt_adc0*120.0));
      
      /*output_high(pin_a4);
      delay_ms(500);
      output_low(pin_a4);
      delay_ms(500);
      output_high(pin_a5);
      delay_ms(500);
      output_low(pin_a5);
      delay_ms(500);*/
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


