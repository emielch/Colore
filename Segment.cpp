#include "Arduino.h"
#include "Segment.h"


Segment::Segment(int _segLen, uint16_t *_ledArray){
	segLen = _segLen;
	ledArray = _ledArray;
	ledDefMode = COMPLETE;
	
	blendMode = ADD;
	setStaticBlack();  // no effect on segment
	e_spd = 0;
	e_pos = 0;
	e_len = 0;
}

Segment::Segment(uint16_t _startLed, uint16_t _endLed){
	uint16_t len = _endLed - _startLed + 1;
	startLed = _startLed;
	initStartEnd(1,&startLed,len);
}

Segment::Segment(uint16_t _startsLen, uint16_t *_startLeds, uint16_t _segLen){
	initStartEnd(_startsLen, _startLeds, _segLen);
}

void Segment::initStartEnd(uint16_t _startsLen, uint16_t *_startLeds, uint16_t _segLen){
	ledArray = _startLeds;
	startsLen = _startsLen;
	segLen = _segLen;
	ledDefMode = STARTEND;
	
	blendMode = ADD;
	effectID = BLACK;  // no effect on segment
	e_spd = 0;
	e_pos = 0;
	e_len = 0;
}

void Segment::setBlendMode(int _blendMode){
	blendMode = _blendMode;
}

// uint16_t Segment::getPixelID(uint16_t pixID){
	// getPixelID(pixID,0);
// }

// uint16_t Segment::getPixelID(uint16_t pixID, uint16_t startLedID){
	// if(ledDefMode == STARTEND){
		// if(startLedID >= startsLen) startLedID = startsLen-1;
		// return ledArray[startLedID] + pixID;
	// }
	// return ledArray[pixID];
// }

void Segment::getPixelID(uint16_t pixID, uint16_t* arraySize, uint16_t* returnArray){
	if(ledDefMode == STARTEND){
		for(int i=0; i<startsLen; i++){
			returnArray[i] = ledArray[i] + pixID;
		}
		*arraySize = startsLen;
	}else{
		returnArray[0] = ledArray[pixID];
		*arraySize = 1;
	}
}

void Segment::setNN(boolean _nnType, int _connAm, Segment **_connSeg){
	nnType = _nnType;
	connAm = _connAm;
	connSeg = _connSeg;
}

void Segment::setRainbow(float spd, float len, byte bri){
	if(effectID != RAINBOW) e_pos = 0;
	effectID = RAINBOW;
	e_spd = spd*360;
	e_len = len;
	e_bri = bri;
}

void Segment::setStaticColor(Color c){
	e_color = c;
	if(c.red() == 0 && c.green() == 0 && c.blue() == 0){
		setStaticBlack();
		return;
	}else if(c.red() == 255 && c.green() == 255 && c.blue() == 255){
		setStaticWhite();
		return;
	}
	effectID = STATIC;
}

void Segment::setStaticBlack(){
	effectID = BLACK;
	e_color.setRGB(0,0,0);
}

void Segment::setStaticWhite(){
	effectID = WHITE;
	e_color.setRGB(255,255,255);
}

void Segment::setSines(Color c, float spd){
	if(effectID != SINES) e_pos = 0;
	effectID = SINES;
	e_color = c;
	e_spd = spd;
}

void Segment::setFade(Color c, float spd){
	effectID = FADE;
	e_toColor = c;
	e_fromColor = e_color;
	e_spd = spd;
	e_pos = 0;
}

void Segment::setFadeInOut(Color cIn, Color cOut, float inSpd, float outSpd){
	effectID = FADEINOUT;
	e_toColor = cIn;
	e_outColor = cOut;
	e_fromColor = e_color;
	e_spd = inSpd;
	e_outSpd = outSpd;
	e_pos = 0;
}

void Segment::setGradient(Color c1, Color c2){
	effectID = GRADIENT;
	e_fromColor = c1;
	e_toColor = c2;
}

void Segment::setWipe(Color c, float spd, boolean dir, float fadeLen, float accel){
	e_outSpd = constrain(accel,-0.99,0.99);
	if(!dir) e_outSpd = -e_outSpd;
	e_len = fadeLen;
	
	if(effectID == WIPE){   // if the current effect is already a wipe
		boolean currDir = e_spd > 0;
		if(currDir != dir) e_fromColor = e_color;
		e_color = c;
		if(dir) e_spd = spd;
		else e_spd = -spd;
	}else{
		effectID = WIPE;
		e_fromColor = e_color;
		e_color = c;
		if(dir){
			e_spd = spd;
			e_pos = 0;
		}else{
			e_spd = -spd;
			e_pos = 1;
		}
	}
}

void Segment::setFire(){
	effectID = FIRE;
}

void Segment::setPulsate(Color c, float spd) {
	effectID = PULSATE;
	e_toColor = c;
	e_spd = spd;
	e_pos = 0;
}

Color Segment::getCurrentColor(){
	return e_color;
}

Color Segment::getOutColor(){
	return e_outColor;
}

void Segment::setBeamControl(BeamControl *_beamControl){
	beamControl = _beamControl;
}

Beam* Segment::setBeam(boolean dir, float spd, byte spdMode, float len, Color col){
	Beam* newBeam = beamControl->freeBeam();
	if(newBeam == NULL) return false;
	newBeam->begin(this, dir, spd, spdMode, len, col, PULSE);
	addBeam(newBeam);
	return newBeam;
}

boolean Segment::setNNBeam(boolean dir, float spd, byte spdMode, float len, Color col, int power){
	Beam* newBeam = beamControl->freeBeam();
	if(newBeam == NULL) return false;
	newBeam->begin(this, dir, spd, spdMode, len, col, NEURAL, power);
	addBeam(newBeam);
	return true;
}

void Segment::addBeam(Beam* beam){  // add the beam to the end of the linked list
	if(beamAnchor == NULL){
		beamAnchor = beam;
	}else{
		Beam* beamWalker = beamAnchor;
		while( beamWalker->nextBeam != NULL ){
			beamWalker = beamWalker->nextBeam;
		}
		beamWalker->nextBeam = beam;
	}
}

void Segment::updateBeams(float dt){
	Beam* beamWalker = beamAnchor;  // initing the walker
	Beam* beamWalker_previous = NULL; // there is no previous beam yet..
	while( beamWalker != NULL ){ // loop thought the list of beams and move them.
		if( !beamWalker->move(dt) ){   //if the beam is finished, remove it from the list
			if(beamWalker_previous == NULL) beamAnchor = beamWalker->nextBeam;  // set the previous' beam next beam to the current's beam next beam, if the current is the first use the anchor
			else beamWalker_previous->nextBeam = beamWalker->nextBeam;
		}else{									// the previous beam remains the same when the current is removed therefore only change the preious' next beam
			beamWalker_previous = beamWalker;  // otherwise the previous beam becomes the current one
		}
		beamWalker = beamWalker->nextBeam;  // go to the next beam
	}
}

void Segment::drawBeams(void (*setPixel)(int pixel, byte, byte, byte), Color (*getPixel)(int)){
	Beam* beamWalker = beamAnchor;
	while( beamWalker != NULL ){
		beamWalker->draw(setPixel,getPixel);
		beamWalker = beamWalker->nextBeam;
	}
}

void Segment::clearBeams(){
	Beam* beamWalker = beamAnchor;  // initing the walker
	Beam* beamWalker_previous = NULL; // there is no previous beam yet..
	while( beamWalker != NULL ){ // loop thought the list of beams and move them.
		beamWalker->disable();
		if(beamWalker_previous == NULL) beamAnchor = beamWalker->nextBeam;  // set the previous' beam next beam to the current's beam next beam, if the current is the first use the anchor
		else beamWalker_previous->nextBeam = beamWalker->nextBeam;
		beamWalker = beamWalker->nextBeam;  // go to the next beam
	}
}

void Segment::move(float dt){
	updateBeams(dt);
	
	switch (effectID) {
		case SINES:{
			e_pos += e_spd*dt;
			break;
		}
		case RAINBOW:{
			e_pos += e_spd*dt;
			break;
		}
		case FADE:{
			e_pos += e_spd*dt;
			if(e_pos >= 1){
				setStaticColor(e_toColor);
			}
			break;
		}
		case FADEINOUT:{
			e_pos +=  ((e_pos <= 1) ? e_spd : e_outSpd) * dt;
			if(e_pos >= 2){
				setStaticColor(e_outColor);
			}
			break;
		}
		case WIPE:{
			float spdBoost = e_spd;
			if(e_outSpd>0){
				spdBoost = (1-e_pos)*e_spd*(e_outSpd) + e_spd*(1-e_outSpd);
			}else if(e_outSpd<0){
				spdBoost = e_pos*e_spd*(-e_outSpd) + e_spd*(1+e_outSpd);
			}
			e_pos += spdBoost*dt;
			if(e_pos >= 1 || e_pos <= 0){
				setStaticColor(e_color);
			}
			break;
		}
		case PULSATE: {
			e_pos += e_spd * dt;
			if (e_pos >= 2) {
				e_pos -= 2;
			}
			break;
		}
		default:{
			
		}
		break;
	}
}

void Segment::draw(void (*setPixel)(int pixel, byte, byte, byte), Color (*getPixel)(int)){
	
	switch (effectID) {
		
		case BLACK:{
			if(blendMode!=ADD){
				for(int i=0; i<segLen; i++){
					blendSetPixel(i,e_color,setPixel,getPixel);
				}
			}
			break;
		}
		
		case WHITE:{
			if(blendMode!=MULTIPLY){
				for(int i=0; i<segLen; i++){
					blendSetPixel(i,e_color,setPixel,getPixel);
				}
			}
			break;
		}
		
		case SINES:{
			for(int i=0; i<segLen; i++){
				float multi = (sin(i*0.5 + e_pos)+1) * (sin(i*0.3 + e_pos*-1.2)+1) /4 * 0.7 + 0.3;
				Color sineCol(e_color.hue(), e_color.saturation(), e_color.brightness()*multi, HSB_MODE);
				
				blendSetPixel(i,sineCol,setPixel,getPixel);
			}
			break;
		}
			
		case RAINBOW:{
			Color rainCol;
			for(int i=0; i<segLen; i++){
				rainCol.setHSB( e_pos+i*(10/e_len), 100, e_bri);
				blendSetPixel(i,rainCol,setPixel,getPixel);
			}
			break;
		}
			
		case STATIC:{
			for(int i=0; i<segLen; i++){
				blendSetPixel(i,e_color,setPixel,getPixel);
			}
			break;
		}
		
		case FADE: case FADEINOUT:{
			if( e_pos <= 1 ){
				e_color.fade(e_fromColor, e_toColor, e_pos);
			}else{
				e_color.fade(e_toColor, e_outColor, e_pos-1);
			}
			for(int i=0; i<segLen; i++){
				blendSetPixel(i,e_color,setPixel,getPixel);
			}
			break;
		}
		
		case GRADIENT:{
			for(int i=0; i<segLen; i++){
				float fade = i*1./(segLen-1);
				e_color.fade(e_fromColor, e_toColor, fade);
				
				blendSetPixel(i,e_color,setPixel,getPixel);
			}
			break;
		}
		
		case WIPE:{
			boolean dir = e_spd > 0;
			float endLed = (segLen+e_len) * e_pos;
			float startLed = endLed - e_len;
			Color wipeCol;
			for(int i=startLed; i<=endLed; i++){
				if(i>=0 && i<segLen){
					float dist = constrain((i-startLed)/e_len,0,1);
					if(!dir) dist = 1-dist;
					wipeCol.fade(e_color,e_fromColor,dist);
					blendSetPixel(i,wipeCol,setPixel,getPixel);
				}
			}
			for(int i=endLed+1; i<segLen; i++){
				if(dir) blendSetPixel(i,e_fromColor,setPixel,getPixel);
				else blendSetPixel(i,e_color,setPixel,getPixel);
			}
			for(int i=0; i<startLed-1; i++){
				if(dir) blendSetPixel(i,e_color,setPixel,getPixel);
				else blendSetPixel(i,e_fromColor,setPixel,getPixel);
			}
			break;
		}
		
		////// FIRE needs to be reimplemented, doesnt support copies
		case FIRE:{/*
			int Cooling = 20;
			unsigned int Sparking = 80;
			// Step 1.  Cool down every cell a little
			int cooldown;
			for(int i=0; i<segLen; i++){
				int pixelID = getPixelID(i);
				Color c = getPixel(pixelID);
				
				cooldown = random(0, ((Cooling * 10) / segLen) + 2);
				
				if(cooldown>c.brightness()) {
					c.setHSB(0, 0, 0);
				} else {
					c.setHSB(0, 0, c.brightness()-cooldown);
				}
				setPixel( pixelID, c.red(), c.green(), c.blue() );
			}
			
			// Step 2.  Heat from each cell drifts 'up' and diffuses a little
			for( int k= segLen - 1; k >= 2; k--) {
				int pixelID = getPixelID(k);
				Color c = getPixel(pixelID);
				
				getPixel(getPixelID(k-1)).brightness();
				
				int heat = (getPixel(getPixelID(k-1)).brightness() + getPixel(getPixelID(k-2)).brightness()*2) / 3;
				c.setHSB(0, 0, heat);
				
				setPixel( pixelID, c.red(), c.green(), c.blue() );
			}
			
			// Step 3.  Randomly ignite new 'sparks' near the bottom
			if( random(255) < Sparking ) {
				int y = random(7);
				int pixelID = getPixelID(y);
				Color c = getPixel(pixelID);
				
				c.setHSB(0, 0, c.brightness()+random(60,100));
				
				setPixel( pixelID, c.red(), c.green(), c.blue() );
			}
			
			for(int i=0; i<segLen; i++){
				int pixelID = getPixelID(i);
				Color c = getPixel(pixelID);
				
				int temp = c.brightness();
				int hue = constrain(map(temp,0,100,-15,30),0,60);
				c.setHSB(hue, 100, temp);
				
				setPixel( pixelID, c.red(), c.green(), c.blue() );
			}
				*/
			break;
		}
		case PULSATE: {
			float fade = e_pos;
			if (fade > 1) fade = 1 - (fade - 1);
			e_color.fade(e_toColor, Color(0, 0, 0, RGB_MODE), fade);
			for (int i = 0; i<segLen; i++) {
				blendSetPixel(i, e_color, setPixel, getPixel);
			}
			break;
		}
			
		default:{
		}
					
		break;
	}
	
	drawBeams(setPixel,getPixel);
}

void Segment::blendSetPixel(int segPixel, Color _c, void (*setPixel)(int pixel, byte, byte, byte), Color (*getPixel)(int)){
	uint16_t pixelIDs[startsLen];
	uint16_t arraySize = 0;
	getPixelID(segPixel,&arraySize,pixelIDs);
	
	for(int i=0; i<arraySize; i++){
		Color c = _c;
		if(blendMode != NORMAL){
			Color prevCol = getPixel(pixelIDs[i]);
			
			if(blendMode == ADD) c.add(prevCol);
			else if(blendMode == MULTIPLY) c.multiply(prevCol);
		}
		setPixel( pixelIDs[i], c.red(), c.green(), c.blue() );
	}
	
	
}



