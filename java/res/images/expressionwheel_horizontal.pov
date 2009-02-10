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

#declare reverse_switch =

 difference
 { 
difference{
 union{
 difference
        {
              difference
              {
                box
                {
                        <-2,0,-1.1><2,3.8,1.1>
                }
		box {
		<-2,0,-1.1><-3,4,1.1>
		rotate z*-15
		translate x*-.35
		}
				box {
		<2,0,-1.1><3,4,1.1>
		rotate z*15
		translate x*.35
		}
                 box
                {
                        <-2.1,-1,-1.6><2.1,3.9,-.7>
                        rotate x*5
                       translate z*-.2
                }
                box
                {
                        <-2.1,-1,1.6><2.1,3.9,.7>
                        rotate x*-5
                        translate z*.2
                }

		pigment { Gray95}                

        }          
        
        translate y*2 
        scale y*.5
        scale z*.7
        }
cylinder 
{
	<-2,0,0><2,0,0>,2
}
 }

/*   text {
    ttf "cyrvetic.ttf" "FWD".15, .1*x
    pigment { Black } 
    scale 1.6    
    rotate z*180
    rotate x*25
    translate x*1.75   
    translate y*1.6
    translate z*-1.5


  }      
  
     text {
    ttf "cyrvetic.ttf" "REV" .15, .1*x
    pigment { Black }
    scale 1.6    
    rotate z*-180  
    rotate y*180
    rotate x*-25
    translate x*-1.65   
    translate y*1.6
    translate z*1.5


  }  
  
       */

 //rotate x*-30  
 scale x*.26
}



  


/*box {
	<-2.1,.8,-2><2.1,-2,2>
	pigment { Black }



} */ 

}    


object
{
        reverse_switch
	 pigment { LightGray }
                        	finish
                        	{
                        		ambient .6
                        		phong .7
                                        brilliance 5
                                        reflection .2
                        	}
           
          scale 1.3 
rotate x*-90    
	//Rotation
	//rotate x*45
	

	rotate z*-90        
	//rotate y*-5
	//rotate x*-45
	//rotate z*90
	//scale 1.5      
	//scale x*.3
                     //rotate y*90
        // Top View
        // rotate -x*90 // scale 1.4 //translate y*2.36
        
        // Bottom View
        // rotate x*90 scale .72 translate y*2.36
	
        // Side View
	// scale .68 translate y*-.8     
	        
	// Angle View 
	// rotate -x*45 scale .58 translate y*.63
}

box {
<.8,2.55,1><-.8,-2.55,-1.3>
pigment {Black}
rotate z*90
}
