#!/usr/bin/perl

use Gimp;
use Cwd 'abs_path';
Gimp::init;

# args below

$baseImage = $ARGV[0];
$noOfImages = 51;
$degreesOfRotation = 90;
$height = 71;
$axis = 'x';

# calculated variables below

@fn = split(/\./,$baseImage);
$original_image = abs_path($fn[0]);
$width = int $height*1.3333;
$crop = ($width - $height)/2;
$incrementAngle = $degreesOfRotation/($noOfImages-1);
$startingAngle = 0-$degreesOfRotation/2;

# begin below

open(POV, $baseImage);
@pov = <POV>;
close POV;

$image_strip = Gimp->gimp_image_new($noOfImages*$height,$height,0);

$back = $image_strip->gimp_layer_new($noOfImages*$height, $height, RGB, "Back", 100, NORMAL_MODE);

$image_strip->gimp_image_add_layer($back,0);

for ($imageNo = 0; $imageNo < $noOfImages; $imageNo++)
{
	$angle = $startingAngle + ($incrementAngle*$imageNo);	
	
	for($i = 0; $i < scalar(@pov); $i++)
	{
		if ($pov[$i] =~ /Rotation/)
		{
			$pov[$i+1] = "rotate $axis*$angle\n";
		}
	}
	
	$newImage = "$original_image$imageNo";
		
	open(OUTF, ">$newImage.pov");
	print OUTF @pov;
	close OUTF;
	system("povray $newImage.pov -H$height -W$width +A -D -V && rm -f $newImage.pov");

	$image = Gimp->file_png_load("$newImage.png", "$newImage.png");
	$image->gimp_crop($height,$height,$crop,0);	 

	$layer = $image->gimp_image_get_active_layer();	
	$layer->gimp_edit_copy(); 
			
	$float = $image_strip->gimp_image_get_active_layer()->gimp_edit_paste(1);			
	$float->gimp_layer_set_offsets($imageNo*$height,0);	
	
	$image_strip->gimp_image_merge_visible_layers(2);	
	
	system("rm -f $newImage.png");
}

$layer = $image_strip->gimp_image_get_active_layer();	
$layer->gimp_layer_add_alpha();
	
$layer->gimp_fuzzy_select(1,1,30,0,1,0,0,0);

$mask = $layer->gimp_layer_create_mask(0);	

$layer->gimp_layer_add_mask($mask);		
	
$mask->gimp_bucket_fill(0,0,100,0,0,1,1);

$layer->gimp_layer_remove_mask(0);

$final_image = $original_image."_strip.png";

Gimp->file_png_save($image_strip, $image_strip->gimp_image_active_drawable, $final_image, $final_image, 125, 0, 0, 0, 0, 0, 0);

