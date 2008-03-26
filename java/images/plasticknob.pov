#include "colors.inc"

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


#declare plasticknob =    
        difference
        {  
                #declare d_shaft =  difference
                {
                        cylinder
                        {
                                <0,-0.001,0><0,5,0>1.375
                        }
                
                        box
                        {
                                <1.4,-0.01,-2><.9,6,2>
                        }  
                        
                        rotate y*90
                }
                        
                #declare body = difference
                {
                        merge
                        {
                                #declare grip = union
                                {
                                	cylinder
                                	{
                                	<-3.2,0.1,0>,<-3.0,8,0>.5
                                	}
                                
                                	cylinder
                                	{
                                	<3.2,0.1,0>,<3.0,8,0>.5
                                	}    
                                }           
                        
                                #declare centre = 
                                	difference
                                	{
                                		lathe
                                		{
                                			linear_spline
                                			7,
                                			<0,0>,<4,0>,<4,.5>,<3.5,1><3.3,8>,<3,8.3><0,8.3>
                                		}
                                		
                                		box
                                		{
                                			<1,8.4,-4>,<-1,8.001,4>
                                		}
                                	}    
        
                                object { centre  }
                        	object { grip rotate y*(22.5*0) }
                        	object { grip rotate y*(22.5*1) }
                        	object { grip rotate y*(22.5*2) }
                        	object { grip rotate y*(22.5*3) }
                        	object { grip rotate y*(22.5*4) }
                        	object { grip rotate y*(22.5*5) }
                        	object { grip rotate y*(22.5*6) }
                        	object { grip rotate y*(22.5*7) }
                        	object { grip rotate y*(22.5*8) }        
                        }
                                                        		
                        object { d_shaft }
                        cylinder { <0,-0.001,0><0,.4,0>,3 }
                }   
                        
                #declare pointer =
                        sphere
                        {
        		        <0,8,2>,.5
        		}        
                
                
                object 
                { 
                        body 
                        pigment { Gray40 } 
                }
        
                object
                {
                        pointer
        		pigment { White }
        		finish { reflection .8 }
                }      
        }			

object 
{
        plasticknob		
        
	//Rotation
	rotate y*0

        // Top View
         rotate -x*90 scale .72 //translate y*2.36
        
        // Bottom View
        // rotate x*90 scale .72 translate y*2.36
	
        // Side View
	// scale .68 translate y*-.8     
	        
	// Angle View 
	// rotate -x*45 scale .58 translate y*.63
}
