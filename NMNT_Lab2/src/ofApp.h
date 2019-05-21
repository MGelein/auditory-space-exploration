#pragma once

#include "ofMain.h"


struct NoiseOctave {
	float x;//The position this noise gen samples from
	float y;
	float radius;//The radius this noise samples in
	float amplitude;//The amplitude this noisegen makes for the actual planet
};

class ofApp : public ofBaseApp{

	public:
		
		ofVec2f centerScreen;

		const int BASE_HEIGHT = 150; //The base height of the planet
		const int RESOLUTION = 200; //The resolution of the circle both for land and water
		const float INC = TWO_PI / RESOLUTION;//Increment, resolution of the circle

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

		void addOctave(vector<NoiseOctave> &vec, float r, float a, float x, float y);
		float getNoise(float angle, vector<NoiseOctave> &noiseVector);
		
		int waterColor = 0x00CCFF;
		int landColor = 0x00FF44;
		vector<ofVec2f> landVerts;//The verteces that make up the circle of the land
		vector<ofVec2f> waterVerts;//The verteces that make up the circle of the water
		vector<NoiseOctave> landOctaves;//The settings for the generator for land
		vector<NoiseOctave> waterOctaves;//The settings for the generator for water
};