 #include "colors.inc"

camera
{
	angle 30
	location <-0, 40, 0>
	look_at <-0,0,0>
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
	<-10,30,10> colour White
}

union {
sphere {

 <0,0,0>7
 


} 

cylinder {

 <0,3,0><0,1,0>7.56
} 

 pigment {
  Green
 }         
 
 finish {
     //reflection .4
     ambient .4
 } 
 scale .45  
}