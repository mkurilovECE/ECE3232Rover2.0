
// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config ALTI2C1 = OFF            // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF            // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25           // Watchdog Window Select bits (WDT Window is 25% of WDT period)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Mode Select bits (Primary Oscillator disabled)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config IOL1WAY = ON             // Peripheral pin select configuration (Allow only one reconfiguration)
#pragma config FCKSM = CSDCMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FOSCSEL
#pragma config FNOSC = FRC         // Oscillator Source Selection (Internal Fast RC (FRC) Oscillator with postscaler)
#pragma config PWMLOCK = ON             // PWM Lock Enable bit (Certain PWM registers may only be written after key sequence)
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GCP = OFF                // General Segment Code-Protect bit (General Segment Code protect is Disabled)


#include <xc.h>
#include <p33EP32MC202.h>
#include <dsp.h>
#include <stdint.h>
#include <math.h>
#define N 64 // Number of points for FFT
#define N_LOG2 6 // Log2(N), assuming N is a power of 2
#define FP 3685000
#define BAUDRATE 9600
#define BRGVAL ((FP/BAUDRATE)/16) - 1



fractcomplex input[N] __attribute__((space(ymemory),aligned(128))); // Input buffer
fractional output[N] __attribute__((__eds__)); // Output buffer
fractcomplex w[N/2] __attribute__((space(xmemory))); //twiddles
int count=0;
void InitializeInputBuffer(void);
void FFT(void);
int fftyippee(void);

int main(void)
{   
    //setting B9 for an LED to know system is on, and B15 as an input for good measure
    TRISBbits.TRISB9=0 ;    
    LATBbits.LATB9=1;
//freq at 8MHZ for fosc
//getting rid of timer so i can unplug debugger
RCONbits.SWDTEN=0;

//stuff from document
U1BRG = 23; // Baud Rate setting for 9600
U1MODEbits.STSEL = 0; // 1-stop bit
U1MODEbits.PDSEL = 0; // No Parity, 8-data bits
U1MODEbits.ABAUD = 0; // Auto-Baud disabled
U1MODEbits.BRGH = 0; // Standard-Speed mode


U1MODEbits.UARTEN = 1; // Enable UART
U1STAbits.UTXEN=1;

// #pragma config statements should precede project file includes.


//random setting of bits

    
    //setting input to RPI47 for uart
    RPINR18bits.U1RXR=0b0101111;


while(1)
{
char ReceivedChar;
/* Check for receive errors */
if(U1STAbits.FERR == 1)
{
continue;
}
/* Must clear the overrun error to keep UART receiving */
if(U1STAbits.OERR == 1)
{
U1STAbits.OERR = 0;
continue;
}
/* Get the data */
if(U1STAbits.URXDA == 1)
{
ReceivedChar = U1RXREG;
        input[count].real = Q15(ReceivedChar);
        count++;

IFS0bits.U1RXIF=0;
IFS0bits.T1IF=0;
IEC0bits.U1RXIE=0;
IEC0bits.T1IE=0;
if (count==64){
    fftyippee();
    count =0; }

}
}}


int fftyippee(void)
{
 
    TwidFactorInit(N_LOG2,&w[0],0);
    // Initialize input buffer with your data
    

    // Perform FFT
    
    FFT();
    BitReverseComplex(N_LOG2,&input[0]);
    // Find the dominant frequency
    SquareMagnitudeCplx(N_LOG2,&input[0],&output[0]);
    int16_t finval;
    fractional fftmax;
    fftmax=VectorMax(32, (fractional*)&output[0],&finval);
    uint16_t peakhz;
    peakhz=finval*9600/64;
    
  

    return 0;
}




void FFT(void)
{
    // Perform FFT with input data and twiddle factors
    FFTComplexIP(N_LOG2, &input[0], &w[0], 0xFF00);
    
}
