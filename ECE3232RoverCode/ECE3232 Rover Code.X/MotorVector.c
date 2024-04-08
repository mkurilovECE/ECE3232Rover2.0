int abs(int v) 
{
  return v * ((v>0) - (v<0));
}
//power = 100
//serring vec = 58
int motorvectorleft(int powervec, int steeringvec) {
    volatile int outleft = 0;
   
    if ((0 <= powervec && powervec <= 100) && (0 <= steeringvec && steeringvec <= 100)){  //makes sure both values are in range (doesn't do anything if they aren't)
        if (steeringvec > 50){ // this is for right turns
        
            outleft = ((float)powervec/100) * (100 - steeringvec) ; //scales the left motor to go from full speed at center to stopped at far right
            
            
        } else if(steeringvec < 50){ //this is for left turns
        
            outleft = ((float)powervec/100) * (100-steeringvec) ;
         
        
        } else {  // this means stick is in the center and we just want it to go as fast as we push the stick straight
            outleft = powervec;
          
        }
    
    }
    return outleft;
}

int motorvectorright(int powervec, int steeringvec) {
    
    volatile float outright =0;
    if ((0 <= powervec && powervec <= 100) && (0 <= steeringvec && steeringvec <= 100)){  //makes sure both values are in range (doesn't do anything if they aren't)
        if (steeringvec > 50){ // this is for right turns
      
            outright = ((float)powervec/100) * (steeringvec) ;
            
        } else if(steeringvec < 50){ //this is for left turns
        
           
            outright = ((float)powervec/100) * (steeringvec) ;
        
        } else {  // this means stick is in the center and we just want it to go as fast as we push the stick straight
            
            outright = powervec;
        }
    
    }
    return outright;
}

int direction(int powervec){
    volatile int direction = 0;
    if (powervec > 50) {
        direction = 1; //forwards
    
    } else if (powervec < 50 ){
        direction = 2; //backwards
    } else {
    
        direction = 0; //brake
    }

    return direction;
}
