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

struct Font {
	ofTrueTypeFont title;
	ofTrueTypeFont normal;
	ofTrueTypeFont debug;
};

struct Sound {
	ofSoundPlayer player;
	float volume;
	float tVolume;
};

struct Planet {
	float angle;
	float angleV;
	string name;
	ofColor waterColor;
	ofColor landColor;
	ofColor cloudColor;
	string sizeClass;
	string resources;
	string description;
};

class ofApp : public ofBaseApp{

	public:
		
		ofVec2f centerScreen;
		Font font;
		int BASE_HEIGHT = 150; //The base height of the planet
		int MAX_HEIGHT = 250;//The max height (includes the baseheigth)
		int HEIGHT_RANGE = MAX_HEIGHT - BASE_HEIGHT;
		const int MIN_RECORDING_FRAMES = 120;//Minimum amount of times for a recording
		const int MAX_RECORDING_FRAMES = 600;//Maximum amount of time for a recording (at least for scaling)
		const int RESOLUTION = 200; //The resolution of the circle both for land and water
		const float INC = TWO_PI / RESOLUTION;//Increment, resolution of the circle
		const string NAME_PARTS[10] = {"XI", "XO" , "XT", "TL", "BU", "BY", "CH", "KR", "AN", "PO"};
		const string END_PARTS[10] = {"TL", "BIN", "BAR", "BOF", "KYT", "BER", "NF", "TIR", "NC64", "BEG"};
		const string RESOURCES[10] = {"Unobtainium", "Barium", "Uranium", "Mithril", "Adamantium", "Aluminium", "Ammonia", "Draconium", "Porrilium", "Xythor"};
		const string SENT1[5] = {
			"The galaxy surrounding you is bare.",
			"Stars around you glisten and twinkle.",
			"Your ship comes to a screeching halt.",
			"The beauty of the cosmos dawns on you.",
			"You arrive in an isolated galactic corner."
		};
		const string SENT2[5] = {
			"Your hyperdrive spins down noisily.",
			"You look upon the peaceful planet.",
			"This planet reminds you of home.",
			"The planet's color looks bleaker.",
			"All movement looks harmonious."
		};
		const string SENT3[5] = {
			"Those clouds look very fluffy!",
			"Are those clouds poisonous?",
			"You see movement on the surface!",
			"Several lights shine from the planet!",
			"Have we been here before?"
		};

		bool recording = false;//If we're currently recording
		int recordingFrames = 0;
		bool spacePressed = false;
		float starSpeedMult = 1;
		float targetStarSpeedMult = 15;
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
		void drawUI();

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
		float getAvgVolume();
		ofSoundStream mic;
		vector<float> samples;
		vector<float> micHistory;
		float micVolume = 0;
		float micGain = 1;//The amount of gain on the microphone
		float micMaxGain = 0.2;//Anything over this is considered max gain
		float micThreshold = 0.03;//This is the threshold below which is considered silence
		float avgVolume = -1;
		float dynamicRange = -1;
		float recordLength = -1;
		void analyseMic();
		bool silenceRegistered = true;
		int silences = 0;
		ofVec2f micGainPos;
		void drawMicGain();

		//The sound files we use for background audio
		Sound hyperDrive;
		Sound drone1;
		Sound drone2;
		Sound drone3;
		Sound drone4;
		Sound drone5;
		const float DRONE_VOLUME = 0.5f;
		const float HYPERDRIVE_VOLUME = 0.5f;
		ofSoundPlayer hit1;
		ofSoundPlayer hit2;
		int droneNum;//Number of the currently active drone

		void droneSound();
		void hyperSound();

		ofVec2f infoPos;
		ofVec2f infoDim;
		string getSizeClass();
		string getResources();
		string getDescription();
};