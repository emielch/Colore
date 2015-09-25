
#include "Arduino.h"
#include "Color.h"


Color::Color(){
	RGB_updated = false;
	HSB_updated = false;
}

Color::Color(int v1, byte v2, byte v3, byte mode){
	RGB_updated = false;
	HSB_updated = false;
	if(mode == 1){
		HSB_color.h = v1;
		HSB_color.s = v2;
		HSB_color.b = v3;
		HSB_updated = true;
	}else{
		RGB_color.r = v1;
		RGB_color.g = v2;
		RGB_color.b = v3;
		RGB_updated = true;
	}
}

void Color::setRGB(byte r, byte g, byte b){
	RGB_color.r = r;
	RGB_color.g = g;
	RGB_color.b = b;
	RGB_updated = true;
	HSB_updated = false;
}

void Color::setHSB(int h, byte s, byte b){
	HSB_color.h = h;
	HSB_color.s = s;
	HSB_color.b = b;
	RGB_updated = false;
	HSB_updated = true;
}

void Color::updateRGB(){
	RGB_color = HSBtoRGB(&HSB_color);
	RGB_updated = true;
}

void Color::updateHSB(){
	HSB_color = RGBtoHSB(&RGB_color);
	HSB_updated = true;
}

byte Color::red(){
	if(!RGB_updated){
		updateRGB();
	}
	return RGB_color.r;
}

byte Color::green(){
	if(!RGB_updated){
		updateRGB();
	}
	return RGB_color.g;
}

byte Color::blue(){
	if(!RGB_updated){
		updateRGB();
	}
	return RGB_color.b;
}

int Color::hue(){
	if(!HSB_updated){
		updateHSB();
	}
	return HSB_color.h;
}

byte Color::saturation(){
	if(!HSB_updated){
		updateHSB();
	}
	return HSB_color.s;
}

byte Color::brightness(){
	if(!HSB_updated){
		updateHSB();
	}
	return HSB_color.b;
}

void Color::add(Color toAdd, float fade){
	if(fade!=0.){
		byte r = constrain(red() + toAdd.red()*fade, 0,255);
		byte g = constrain(green() + toAdd.green()*fade, 0,255);
		byte b = constrain(blue() + toAdd.blue()*fade, 0,255);
		
		setRGB(r,g,b);
	}
};

//// convert hsb to rgb
RGB Color::HSBtoRGB(HSB *color){
  RGB output;
  int hue = color->h%360;
  byte bri = min(color->b,100)*2.55;
  if (color->s == 0) { // Acromatic color (gray). Hue doesn't mind.
    output.r=bri;
    output.g=bri;
    output.b=bri;  
  }
  else{
    byte base = ((255 - int(min(color->s,100)*2.55)) * bri)>>8;

    switch(hue/60) {
    case 0:
      output.r = bri;
      output.g = (((bri-base)*hue)/60)+base;
      output.b = base;
      break;

    case 1:
      output.r = (((bri-base)*(60-(hue%60)))/60)+base;
      output.g = bri;
      output.b = base;
      break;

    case 2:
      output.r = base;
      output.g = bri;
      output.b = (((bri-base)*(hue%60))/60)+base;
      break;

    case 3:
      output.r = base;
      output.g = (((bri-base)*(60-(hue%60)))/60)+base;
      output.b = bri;
      break;

    case 4:
      output.r = (((bri-base)*(hue%60))/60)+base;
      output.g = base;
      output.b = bri;
      break;

    case 5:
      output.r = bri;
      output.g = base;
      output.b = (((bri-base)*(60-(hue%60)))/60)+base;
      break;
    }
  }
  return output;
}

HSB Color::RGBtoHSB(RGB *color){
  byte max_rgb = max(max(color->r,color->g), color->b);
  byte min_rgb = min(min(color->r,color->g), color->b);
  
  HSB output;
  output.s = max_rgb - min_rgb;

  if (output.s == 0) output.h = 0;
  else if (max_rgb == color->r){
    if(color->b>color->g) output.h = 360+(0 + (float)(color->g-color->b) /  output.s) * 60;
    else output.h = (0 + (float)(color->g-color->b) /  output.s) * 60;
  } 
  else if (max_rgb == color->g) output.h = (2 + (float)(color->b-color->r) /  output.s) * 60;
  else if (max_rgb == color->b) output.h = (4 + (float)(color->r-color->g) /  output.s) * 60;

  output.s = (float)output.s/max_rgb*100;
  output.b = max_rgb/2.55;

  return output;
}