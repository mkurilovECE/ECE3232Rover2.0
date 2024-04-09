//Environmental Remediation Task Code
//(relay control code)
//Author: Mattias Floyd 

#include <xc.h> //CONFIG
#include <stdbool.h>

void pumpcontol(bool state) { // 1 on 0 off
    
    //RC2 for output
    TRISCbits.TRISC2 =state; //Make RC2 the state of the input parameter
    ANSELCbits.ANSC3 =0;
    return;
}
