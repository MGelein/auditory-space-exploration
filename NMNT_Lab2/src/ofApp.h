#pragma once

#include "ofMain.h"	

struct NoiseOctave {
	float x;//The position this noise gen samples from
	float y;
	float radius;//The radius this noise samples in
	float amplitude;//The amplitude this noisegen makes for the actual planet
};

struct Cloud {
	float angle;
	float angleV;
	float height;
	float radius;
};

struct Moon {
	float angle;
	float angleV;
	float height;
	float radius;
	ofColor color;
};

struct Star{
	float x;//The x-coordainte of this star
	float y;//The y coordainte of this star
	float vx;
	float vy;
	float r;//The radius of the star
	ofColor c;//The color of this star
};

struct Planet {
	float angle;
	float angleV;
	string name;
	ofColor waterColor;
	ofColor landColor;
	ofColor cloudColor;
};

class ofApp : public ofBaseApp{

	public:
		
		ofVec2f centerScreen;
		ofTrueTypeFont planetNameFont;

		const int BASE_HEIGHT = 150; //The base height of the planet
		const int MAX_HEIGHT = 250;//The max height (includes the baseheigth)
		const int HEIGHT_RANGE = MAX_HEIGHT - BASE_HEIGHT;
		const int MIN_RECORDING_FRAMES = 120;//Minimum amount of times for a recording
		const int RESOLUTION = 200; //The resolution of the circle both for land and water
		const float INC = TWO_PI / RESOLUTION;//Increment, resolution of the circle
		const string NAME_PARTS[10] = {"XI", "XO" , "XT", "TL", "BU", "BY", "CH", "KR", "AN", "PO"};
		const string END_PARTS[10] = {"TL", "BIN", "BAR", "BOF", "KYT", "BER", "NF", "TIR", "NC64", "BEG"};

		bool recording = false;//If we're currently recording
		int recordingFrames = 0;
		bool spacePressed = false;
		float starSpeedMult = 1;
		float targetStarSpeedMult = 1;
		float planetOffset = 0;
		float targetPlanetOffset = 0;
		ofVec2f screenShake;
		float shakeForce = 0;
		float targetShakeForce = 0;
		float bgAlpha = 255;
		float targetBgAlpha = 255;

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
		ofVec2f p2c(float angle, float radius);
		ofVec2f rndPt();//Returns a random point for the noise generator
		void easeNumbers();
		void setHyperDrive(bool enabled);
		void createGenerator();//Creates the noise generator for the land and water

		void drawVerts(vector <ofVec2f> &vec);
		void drawPlanet();
		void animateWater();
		void animateTerrain();
		void generatePlanet();

		void generateMoons();
		void animateMoons();
		void drawMoons();
		vector<Moon> moons;//List of all the moons that this planet has
		
		void generateStars();
		void animateStars();
		void drawStars();
		vector<Star> stars;//List of all stars that are visible in the area
		
		void generateClouds();
		void animateClouds();
		void drawClouds();
		vector<Cloud> clouds;//List of all the clouds
		
		string getPlanetName();
		Planet planet; //Stores all planet related information
		vector<ofVec2f> landVerts;//The verteces that make up the circle of the land
		vector<ofVec2f> waterVerts;//The verteces that make up the circle of the water
		vector<NoiseOctave> landOctaves;//The settings for the generator for land
		vector<NoiseOctave> waterOctaves;//The settings for the generator for water

		void audioSetup();
		void audioIn(float * input, int bufferSize, int nChannels);
		ofSoundStream mic;
		vector<float> samples;
		float micVolume = 0;
};