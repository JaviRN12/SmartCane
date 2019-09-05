#define ONE_SEC 33  // 1000/((4/8000)*(65535)(1)) = 32.767
#define TEN_SEC ONE_SEC*10 
#define buf_length 255
#define TELEPHONE_LENGHT 10
#define TELEPHONE_ADDRESS 25
#define DISTANCE_LIMIT 512
// Send message stuff
#define Send_Message input(PIN_b0)

volatile unsigned int8 timerCounter = 0;
volatile unsigned int16 timerGPS    = 0;

// GPS Variables
char LatRead[10], LonRead[10];
int savePosition =  0;
int GPS_Connected = 0;
float latitud = 0, longitud = 0;
char LatWrite[10];
char LonWrite[10];
float latitudConverted  = 0;
float longitudConverted = 0;

// Power Moduled variables
int duty = 0;

// Phone message variables
char _Cel1[8], _Cel2[8];
float Num1, Num2;

// Buffer variables 
volatile char c, str[buf_length], buf[buf_length];
int str_flag = 0, flag_buf = 0, str_flag2=0;
int i = 0;

// ADC Variables
int16 data_adc0 = 0, mmDistance = 0, distancePower = 0; 
float voltage = 0;

/////////////////////////////////////////////////////////////////////
int str_flag = 0, t_flag = 0, flag_buf = 0, str_flag2=0;
int duty = 0, duty1, duty2;
int i = 0;
char c, str[buf_length], buf[buf_length];
char LatRead[10], LonRead[10];
int16 data_adc0, data_adc1, Pa, Pb;
float volt_adc0, volt_adc1;
float latitud=0, longitud=0;
char _Cel1[8], _Cel2[8];
float Num1, Num2;
