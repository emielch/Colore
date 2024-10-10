// ---- led segment ----

#ifndef Segment_h
#define Segment_h

#include "Arduino.h"
#include "Color.h"
#include "Beam.h"
#include "BeamControl.h"

#define BLACK 0
#define STATIC 1
#define RAINBOW 2
#define GRADIENT 3
#define SINES 4
#define FADE 5
#define FADEINOUT 6
#define WIPE 7
#define WHITE 8
#define FIRE 9
#define PULSATE 10
#define SEG_GRADIENT 11

#define NEURON 0
#define SYNAPSE 1

#define COMPLETE 0  //ledDefMode
#define STARTEND 1

#define ADD 0
#define NORMAL 1
#define MULTIPLY 2

class BeamControl; // forward declaration
class Beam;

class Segment{
	public:
		Segment(int _segLen, uint16_t *_ledArray);  // ledDefMode COMPLETE
		Segment(uint16_t _startLed, uint16_t _endLed); // ledDefMode STARTEND
		Segment(uint16_t _startsLen, uint16_t *_startLeds, uint16_t _segLen);
		void initStartEnd(uint16_t _startsLen, uint16_t *_startLeds, uint16_t _segLen);
		void setBlendMode(int _blendMode);
		void setNN(boolean _nnType, int _connAm, Segment **_connSeg);
		void draw(void (*setPixel)(int pixel, byte, byte, byte), Color (*getPixel)(int));
		void move(float dt);
		uint16_t getLen(){return segLen;}
		byte getEffectID() { return effectID; }
		// uint16_t getPixelID(uint16_t pixID);
		// uint16_t getPixelID(uint16_t pixID, uint16_t startLedID);
		void getPixelID(uint16_t pixID, uint16_t* arraySize, uint16_t* returnArray);

		void setRainbow(float spd, float len, byte bri);
		void setStaticColor(Color c);
		void setStaticBlack();
		void setStaticWhite();
		void setSines(Color c, float spd);
		void setFade(Color c, float spd);
		void setFadeInOut(Color cIn, Color cOut, float inSpd, float outSpd);
		void setGradient(Color c1, Color c2);
		void setSegGradient(Segment *_s1,Segment *_s2);
		void setWipe(Color c, float spd, boolean dir, float fadeLen, float accel);
		void setFire();
		void setPulsate(Color c, float spd);
		
		Color getCurrentColor();
		Color getOutColor();
		
		void setBeamControl(BeamControl *_beamControl);
		Beam* setBeam(boolean dir, float spd, byte spdMode, float len, Color col);
		void clearBeams();
		boolean setNNBeam(boolean dir, float spd, byte spdMode, float len, Color col, int power);
		
		void blendSetPixel(int segPixel, Color c, void (*setPixel)(int pixel, byte, byte, byte), Color (*getPixel)(int));
		
		//-- NN
		Segment **connSeg;  // neurons are connected to multiple synapses, synapses to two neurons
		byte connAm;
		boolean nnType;  // NEURON or SYNAPSE
		
		byte power = 0;
		
		//-- network search
		boolean visited = false;
		Segment *parent;
		
		Beam *beamAnchor; // linked list anchor
		
		
	private:
		boolean ledDefMode; // COMPLETE mode or STARTEND mode
		uint16_t segLen;  // segment length
		uint16_t *ledArray;  // COMPLETE: LUT for converting segment pixel ID's to strip pixel ID's / STARTEND: array containing the ID's of segment starts
		uint16_t startLed;   // save the start led when mode is STARTEND without copies
		uint16_t startsLen;  // amount of start leds when STARTEND
		
		byte effectID;
		
		int blendMode;
		
		void addBeam(Beam* beam);

		//-- effect parameters --
		float e_spd;
		float e_outSpd;
		float e_pos;
		float e_len;
		byte e_bri;
		Color e_color;
		Color e_fromColor;
		Color e_toColor;
		Color e_outColor;

		Segment *s1;
		Segment *s2;
		//---
		
		BeamControl *beamControl;
		
		
		void updateBeams(float dt);
		void drawBeams(void (*setPixel)(int pixel, byte, byte, byte), Color (*getPixel)(int));
};

#endif