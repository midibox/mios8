 #include "colors.inc"

camera
{
	angle 30
	location <-0, 40, -20>
	look_at <-0,2,-2>
}                      

/*
          camera
{
	angle 10
	location <-10, 0, -50>
	look_at <0,0,0>
} */

background
{
	colour Clear
}

light_source
{
	<0,50,-5> colour White
}

#declare white_key =

                  box 
                  {
                        <.8,1.6,0><-.8,0,-8>     
  
                               
                                pigment { White }
                        	finish
                        	{
                        		ambient .75
                        		phong .9    
                                        brilliance 25  
                                        reflection .2
                        	}  
                  }         
                  
                  
#declare black_key =

                  box 
                  {
                        <.4,2.1,0><-.4,0,-4>     
  
                               
                                pigment { Black }
                        	finish
                        	{
                        		ambient .75
                        		phong .9    
                                        brilliance 25  
                                        reflection .2
                        	}      
                        	     rotate x *-10
                        	translate x*.8
                        	translate z*-.2
                  }   
                  
                  
#declare white_key_down =

                  box 
                  {
                        <.8,1.6,0><-.8,0,-8>     
  
                               
                                pigment { White }
                        	finish
                        	{
                        		ambient .75
                        		phong .9    
                                        brilliance 25  
                                        reflection .2
                        	}       
                        	
                        	rotate x *-10
                  }

        
        
        box 
        {
         <-5,-5,3><5,-2,-8>
        
        }

        object { white_key}   
        object { black_key}        
        object { white_key  translate x*1.8}  
        object { white_key  translate x*1.8 *2}
        object { white_key  translate x*1.8 *-1}
        object { white_key  translate x*1.8 *-2}

              

