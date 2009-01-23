
#include "colors.inc"
#include "metals.inc"

camera
{
	angle 10
	location <0, 0, -50>
	look_at <0,0,0>
}

background
{
	colour Clear
}

light_source
{
	<-20,20,-20> colour White
}                                      

#declare metalfader =
union
{
difference
{              
       difference
       { box
        {
                <-2,0,2.5><2,2.8,-2.5>   
        
        } 
        cylinder
        {
                <-2.1,5,4.5><2.1,5,4.5>,4.6
        }          
        cylinder
        {
                <-2.1,5,-4.5><2.1,5,-4.5>,4.6
        }
                    pigment { LightGray }
 
        
        }   
        
        union
                 {
                        box
                        {
                               <-2.5,-.01,-.1><-1.95,4,.1>   
                              
                        }  
                        
                        box
                        {
                               <-2.5,2.7,-.1><2.5,4,.1>   
                               
                        }
                        
                        box
                        {
                               <2.5,-.01,-.1><1.95,4,.1>   
                              
                        }     
                        pigment { Black }
        		finish { reflection  0 }
                        
                }
}         

difference
{    
        box
        {
                <-5,0,13><5,-.5,-13>
                       
        }     
        box
        {
                <-2,1,10><2,-.2,-10>
             
        
        }  
        box
        {
                <-.2,1,8.75><.2,-1,-8.75>
        }
        
           pigment { White } 
        }   
} 
 
object 
{
        metalfader		
        
	//Rotation
	rotate y*0

        // Top View
        rotate -x*90 //scale .3 translate y*2.36          

        
        // Bottom View
     //    rotate x*45 scale .72 translate y*2.36
	
        // Side View
	// scale .68 translate y*-.8     
	        
	// Angle View 
	// rotate -x*45 scale .58 translate y*.63
}
