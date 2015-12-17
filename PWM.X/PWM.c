

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
    TRISC=0; // utgång
    RC2 = 0;
    TRISA=0xFF; // ingång
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
//Kontrollera om någon av switcharna trycks ner. Om ON/OFF switchen tryckts ner,
//hoppa till state LED_ONOFF. Om någon av upp/ner switcharna trycks ner, så
//laddas en fördröjningsvariabel (som backas ner och landar på 0 i timerinterruptrutinen)
//följt av byte till state LED_DIM_UP eller LED_DIM_DOWN.

//LED_ONOFF:
//När du detekterar att knappen släppts upp skall du tända en släckt diod eller
//släcka en tänd diod. Ett sätt att släcka dioden är att skriva 0 till CCPR1L, och
//tändning sker genom att skriva ett värde <> 0. Ev. får du använda en variabel för
//att veta om dioden är tänd eller släckt.

//LED_DIM_UP:
//Kontrollera om fördröjningsvariabeln landat på 0. Om så fallet, öka dutycyclen ett
//steg för att öka ljusstyrkan, följt av omladdning av fördröjningen. Avsikten är att
//ligga kvar i detta state tills knappen släpps upp. Om du detekterar detta så hoppa
//tillbaka till LED_IDLE. En viktig sak att inte glömma är att kontrollera så man inte
//ökar dutycyclen över maximalt tillåtet värde.

//LED_DIM_DOWN:
//Kontrollera om fördröjningsvariabeln landat på 0. Om så fallet, minska dutycyclen
//ett steg för att minska ljusstyrkan, följt av omladdning av fördröjningen. Avsikten
//är att ligga kvar i detta state tills knappen släpps upp. Om du detekterar detta så
//hoppa tillbaka till LED_IDLE. En viktig sak att inte glömma är att kontrollera så
//man inte minskar dutycyclen under 0.
//I de state där upp- och neddimning sker använder du alltså en fördröjning mellan
//varje ökning/minskning av duty cyclen, så att man tydligt hinner se att lysdioden
//dimmas upp och ned. Testa dig fram med olika tider på fördröjningen, tills du
//tycker att dimkaraktäristiken känns bra. En bra utgångspunkt är att vänta ca 10
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


