
/*
 * File:   MotorVector.c
 * Author: Mattias Floyd
 *
 * Created on April 4, 2024, 5:15 PM
 */


#include <xc.h> //CONFIG
#include "MotorVector.h"



//This function take in a steering vector (0-100) and a power vector and converts them into a proportionality
//for power to left and right 


//for powervec, we need an input 0-100 (0 is backwards full, 50 is stopped and 100 is full forwards)
//for steeringvec, we need an input 0-100 (0 is left, 50 is straight and 100 is right)
//this code does not do tank style turning, it does pivot turning 



int motorvectorleft(int powervec, int steeringvec) {
    int outleft = 0;
    
    if ((50 <= powervec && powervec <= 100) && (0 <= steeringvec && steeringvec <= 100)){  //makes sure both values are in range (doesn't do anything if they aren't)
        
        
             //scales the left motor to go from full speed at center to stopped at far right
            
            outleft = (powervec/100) * (steeringvec);
       if (steeringvec<55 && steeringvec>45)  // this means stick is in the center and we just want it to go as fast as we push the stick straight
            outleft = powervec;
          
        }
    
    
    return outleft;
}

int motorvectorright(int powervec, int steeringvec) {
    int outright =0;
    if ((50 <= powervec && powervec <= 100) && (0 <= steeringvec && steeringvec <= 100)){  //makes sure both values are in range (doesn't do anything if they aren't)
       
      
           outright = (powervec/100) * (100 - steeringvec);
            
            
        
          if (steeringvec<55 && steeringvec>45)  // this means stick is in the center and we just want it to go as fast as we push the stick straight
            outright = powervec;
          
        }
    
    
    return outright;
}
