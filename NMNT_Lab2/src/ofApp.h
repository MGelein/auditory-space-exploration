#pragma once

#include "ofMain.h"


struct NoiseOctave {
	float x;//The position this noise gen samples from
	float y;
	float radius;//The radius this noise samples in
	float amplitude;//The amplitude this noisegen makes for the actual planet
};

struct Star{
	float x;//The x-coordainte of this star
	float y;//The y coordainte of this star
	float vx;
	float vy;
	float r;//The radius of the star
	ofColor c;//The color of this star
};

class ofApp : public ofBaseApp{

	public:
		
		ofVec2f centerScreen;
		ofTrueTypeFont font;

		const int BASE_HEIGHT = 150; //The base height of the planet
		const int RESOLUTION = 200; //The resolution of the circle both for land and water
		const float INC = TWO_PI / RESOLUTION;//Increment, resolution of the circle
		const string NAME_PARTS[10] = {"XI", "XO" , "XT", "TL", "BU", "BY", "CH", "KR", "AN", "PO"};
		const string END_PARTS[10] = {"TL", "BIN", "BAR", "BOF", "KYT", "BER", "NF", "TIR", "NC64", "BEG"};

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void drawVerts(vector <ofVec2f> &vec);
		void addOctave(vector<NoiseOctave> &vec, float r, float a, float x, float y);
		float getNoise(float angle, vector<NoiseOctave> &noiseVector);
		void drawPlanet();
		void drawStars();
		void animateWater();
		void animateStars();
		void calcTerrain();
		void generateStars();

		string getPlanetName();
		
		ofColor waterColor = 0x00CCFF;
		ofColor landColor = 0x00FF44;
		float planetRotation = 0;
		string planetName = "OXITHAXATL";
		vector<ofVec2f> landVerts;//The verteces that make up the circle of the land
		vector<ofVec2f> waterVerts;//The verteces that make up the circle of the water
		vector<NoiseOctave> landOctaves;//The settings for the generator for land
		vector<NoiseOctave> waterOctaves;//The settings for the generator for water
		vector<Star> stars;//List of all stars that are visible in the area
};