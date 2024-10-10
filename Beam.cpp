#include "Arduino.h"
#include "Beam.h"


Beam::Beam(){
	active = false;
}

void Beam::begin(Segment *seg, boolean _dir, float spd, byte _spdMode, float len, float _spread, Color col, byte _mode){    // speed in pixels/sec or segment/sec
	active = true;
	arrived = false;
	mode = _mode;
	onSegment = seg;
	length = len;
	color = col;
	dir = _dir;
	spdMode = _spdMode;
	if (spdMode==PIXEL_SPD){
		pixelSpd = spd;
		segSpd = spd / onSegment->getLen();
	}
	else if(spdMode==SEGMENT_SPD){
		segSpd = spd;
	}
	setSpread(_spread);
	startFac = 0 - spreadFac;
	endFac = 1 + spreadFac;
	posFactor = ((dir == UP) ? startFac : endFac);
	// linked list pointer
	nextBeam = NULL;
}

void Beam::setSpread(float _spread){
	spread = _spread;
	spreadFac = (spread/2)/onSegment->getLen();
	endFac = 1 + spreadFac;
}

void Beam::begin(Segment *seg, boolean dir, float spd, byte spdMode, float _spread, Color col, byte _mode, int _power){
	power = _power;
	begin(seg, dir, spd, spdMode, 0, _spread, col, _mode);
}

void Beam::begin(Segment *seg, boolean dir, float spd, byte spdMode, float _spread, Color col, byte _mode){
	begin(seg, dir, spd, spdMode, 0, _spread, col, _mode);
}

boolean Beam::move(float dt){
	posFactor += segSpd*dt * ((dir == UP) ? 1 : -1);
	if( posFactor>endFac || posFactor<startFac ){
		active = false;
		return false;
	}
	return true;
}

boolean Beam::justArrived(){
	if (arrived) return false;
	if( dir == UP ){
		if( posFactor >= 1-spreadFac/4 ) return true;
	}
	if( dir == DOWN ){
		if( posFactor <= 0+spreadFac/4 ) return true;
	}
	return false;
}

void Beam::arrive(){
	arrived = true;
}

boolean Beam::isNeuralMode(){
	if( mode==NEURAL ){
		return true;
	}
	return false;
}

void Beam::draw(void (*setPixel)(int pixel, byte, byte, byte), Color (*getPixel)(int)){
	if(mode == FLASH){
		float bri;
		if(posFactor<0.5){
			bri = posFactor*2;
		}else{
			bri = 1 - (posFactor*2 - 1);
		}
		
		Color c = color;
		c.multiply(bri);
		
		for(int i=0; i<=onSegment->getLen(); i++){
			onSegment->blendSetPixel(i,c,setPixel,getPixel);
		}
	}else if(mode == PULSE || mode == NEURAL){
		float position = posFactor * onSegment->getLen();
		int startLed = position - spread/2;
		int endLed = position + spread/2;
		
		for(int i=startLed; i<=endLed; i++){
			if( i>=0 && i<onSegment->getLen() ){
				float dist = constrain(1-abs(i-position)/spread*2, 0, 1);
				Color c = color;
				c.multiply(dist);
				onSegment->blendSetPixel(i,c,setPixel,getPixel);
			}
		}
	}
}

boolean Beam::isActive(){
	return active;
}