/*
 * File:   MotorVector.c
 * Author: Mattias Floyd
 *
 * Created on April 4, 2024, 5:15 PM
 */


#include <xc.h> //CONFIG


//This function take in a steering vector (0-100) and a power vector and converts them into a proportionality
//for power to left and right 


volatile int powervec = 0; //we need an input 0-100 (0 is backwards full, 50 is stopped and 100 is full forwards)
volatile int steeringvec = 0;  //we need an input 0-100 (0 is left, 50 is straight and 100 is right)
volatile int outleft = 0;
volatile int outright =0;


int motorvectorleft(powervec, steeringvec) {
    if (0 <= powervec <= 100 && 0 <= steeringvec <= 100){  //makes sure both values are in range (doesn't do anything if they aren't)
        if (steeringvec >> 50){ // this is for right turns
        
            outleft = (powervec/50) * (100 - steeringvec) * 2; //scales the left motor to go from full speed at center to stopped at far right
            
            
        } else if(steeringvec << 50){ //this is for left turns
        
            outleft = (powervec/50) * (steeringvec) * 2;
         
        
        } else {  // this means stick is in the center and we just want it to go as fast as we push the stick straight
            outleft = powervec;
          
        }
    
    }
    return outleft;
}

int motorvectorright(powervec, steeringvec) {
    if (0 <= powervec <= 100 && 0 <= steeringvec <= 100){  //makes sure both values are in range (doesn't do anything if they aren't)
        if (steeringvec >> 50){ // this is for right turns
      
            outright = (powervec/50) * (steeringvec) *2;
            
        } else if(steeringvec << 50){ //this is for left turns
        
           
            outright = (powervec/50) * (100 - steeringvec) *2;
        
        } else {  // this means stick is in the center and we just want it to go as fast as we push the stick straight
            
            outright = powervec;
        }
    
    }
    return outright;
}
