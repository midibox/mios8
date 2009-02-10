#include "colors.inc"

camera
{
	angle 10
	location <0, 0, -50>
	look_at <0,0,0>
}

background
{
	colour Grey
}

light_source
{
	<-20,20,-20> colour Grey
}                                     

#declare plasticfader =
       
 union{
 
difference
        {
                                    /*  box {
                        <-2,0,-1.1><2,3.8,1.1>
                         pigment { Gray10 }
                        	finish
                        	{
                        		ambient .75
                        		phong .9    
                                        brilliance 25  
                                        reflection .06
                        	}  
              
                                     }  */      
                                     
                                     sphere_sweep {
  b_spline 
 5,

  <0,0,-1.5>,0   
    <0,0,-1.3>,1 
      <0,0,0>,1.2
       <0,0,1.3>,1
        <0,0,1.5>,0 

pigment { Gray10 }
                        	finish
                        	{
                        		ambient .75
                        		phong .9    
                                        brilliance 25  
                                        reflection .06
                        	}  
}   

box {
 <-1.3,-1.3,-2><1.3,-0.2,2>
} 

              difference
                 {
                        cylinder
                        {
                               <0,0,-.12><0,0,.12>,2
                              
                        }  
                        
                        cylinder
                        {
                               <0,0,-.2><0,0,.12>,1.1   
                               
                        }
                        
                       
                        pigment { White }
        		finish { reflection .8 }
                        
                }            
                
                        
        }                      
             
         /* difference
        {    
                box
                {
                        <-5,0,13><5,-.5,-13>
                               
                }     
              
                box
                {
                        <-.2,1,7><.2,-1,-7>
                }
                
                   pigment { White }    
        }  */    
}
 
object 
{
        plasticfader		
        
	//Rotation
	rotate y*90

        // Top View
        rotate -x*90// scale 1.4 //translate y*2.36
        
        // Bottom View
        // rotate x*90 scale .72 translate y*2.36
	
        // Side View
	// scale .68 translate y*-.8     
	        
	// Angle View 
	// rotate -x*45 scale .58 translate y*.63
}
