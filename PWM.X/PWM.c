

// PIC16F884 Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#define LED_IDLE 0
#define LED_ONOFF 1
#define LED_DIM_UP 2
#define LED_DIM_DOWN 3
unsigned char delay = 0;
void SystemInit(void);
void PowerLEDHandler(void);
int main(void)
{
    SystemInit();
    while(1)
    {
        PowerLEDHandler();
    }//while slut
}// slut main





void SystemInit(void)
{
    ANSEL=0;
    TRISC=0; // utg�ng
    RC2 = 0;
    TRISA=0xFF; // ing�ng
    OPTION_REG = 0b11010011;
    T0IE=1;
    T0IF=0;
    GIE=1;
    T2CKPS1=0;
    T2CKPS0=1;  // prescaler 4
    PR2=249;    // (20Mhz/(4*5khz*4))-1
    DC1B1=0; // ccp1con bit 5
    DC1B0=0; // ccp1con bit 4
    CCP1M3 =1;
    CCP1M2 =1;
    CCP1M1 =0;
    CCP1M0 =0;
    P1M1 =0;
    P1M0=0;
    CCP2M3=1;
    CCP2M2=1;
    CCPR1L=0;
    //
    TMR2IF =0;
    TMR2ON=1;
}
void PowerLEDHandler(void)
{
    static unsigned char dim = 0;
    static unsigned char state = LED_IDLE;
    switch (state)
    {
        case LED_IDLE:
            if (RA0 == 1)
                state=LED_DIM_UP;
            if (RA1 == 1)
                state=LED_DIM_DOWN;
            if (RA2 == 1)
                state=LED_ONOFF;
            break;
        case LED_ONOFF:
            if(RA2 == 0)
            {
            if (dim == 0)
            {
                dim = 255;
               // RC2 = 1;
            }

            else
            {
                dim = 0;
              //  RC2 =0;
            }
            CCPR1L = dim;
            state=LED_IDLE;
            }
            break;
        case LED_DIM_UP:
            if (delay==0)
            {
            delay= 12; // ca 10 ms

            if (dim <255)
                dim++;
            CCPR1L = dim;
             if (RA0 == 0)
                state=LED_IDLE;
            }
            break;
        case LED_DIM_DOWN:
            if (delay==0)
            {
            delay= 12; // ca 10 ms
            if (dim >0)
                dim--;
            CCPR1L = dim;
            if (RA1 == 0)
                state=LED_IDLE;
            }
            break;

    }
//LED_IDLE:
//Kontrollera om n�gon av switcharna trycks ner. Om ON/OFF switchen tryckts ner,
//hoppa till state LED_ONOFF. Om n�gon av upp/ner switcharna trycks ner, s�
//laddas en f�rdr�jningsvariabel (som backas ner och landar p� 0 i timerinterruptrutinen)
//f�ljt av byte till state LED_DIM_UP eller LED_DIM_DOWN.

//LED_ONOFF:
//N�r du detekterar att knappen sl�ppts upp skall du t�nda en sl�ckt diod eller
//sl�cka en t�nd diod. Ett s�tt att sl�cka dioden �r att skriva 0 till CCPR1L, och
//t�ndning sker genom att skriva ett v�rde <> 0. Ev. f�r du anv�nda en variabel f�r
//att veta om dioden �r t�nd eller sl�ckt.

//LED_DIM_UP:
//Kontrollera om f�rdr�jningsvariabeln landat p� 0. Om s� fallet, �ka dutycyclen ett
//steg f�r att �ka ljusstyrkan, f�ljt av omladdning av f�rdr�jningen. Avsikten �r att
//ligga kvar i detta state tills knappen sl�pps upp. Om du detekterar detta s� hoppa
//tillbaka till LED_IDLE. En viktig sak att inte gl�mma �r att kontrollera s� man inte
//�kar dutycyclen �ver maximalt till�tet v�rde.

//LED_DIM_DOWN:
//Kontrollera om f�rdr�jningsvariabeln landat p� 0. Om s� fallet, minska dutycyclen
//ett steg f�r att minska ljusstyrkan, f�ljt av omladdning av f�rdr�jningen. Avsikten
//�r att ligga kvar i detta state tills knappen sl�pps upp. Om du detekterar detta s�
//hoppa tillbaka till LED_IDLE. En viktig sak att inte gl�mma �r att kontrollera s�
//man inte minskar dutycyclen under 0.
//I de state d�r upp- och neddimning sker anv�nder du allts� en f�rdr�jning mellan
//varje �kning/minskning av duty cyclen, s� att man tydligt hinner se att lysdioden
//dimmas upp och ned. Testa dig fram med olika tider p� f�rdr�jningen, tills du
//tycker att dimkarakt�ristiken k�nns bra. En bra utg�ngspunkt �r att v�nta ca 10
//ms mellan varje duty cycle-steg.
}

void interrupt ISR(void)
{
 if(T0IE && T0IF)
 {
 T0IF = 0;
 if (delay>0)
     delay--;
 }
 if(TMR2IF == 1)
 {
     TMR2IF = 0;
     TRISC =0;
 }
}


