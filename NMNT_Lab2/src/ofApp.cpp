#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//Do some setup stuff with drawing settings
	ofSetWindowPosition(0, 30);
	ofSetWindowTitle("Auditory Space Exploration");
	ofSetBackgroundAuto(false);
	ofEnableSmoothing();
	//Prepare audio settings
	audioSetup();
	//Load the font
	font.title.load("nasalization.ttf", 32);	
	font.debug.load("nasalization.ttf", 13);
	font.normal.load("nasalization.ttf", 14);
	//Calculate the dimensions of the screen and store half, this is used to set center of the coordinate space
	centerScreen.x = ofGetWidth() / 2;
	centerScreen.y = ofGetHeight() / 2;
	screenShake.set(0, 0);
	infoPos.set(-10, 300);
	infoDim.set(450, 400);
	//Generate the stars for the first background and a use-cue
	planet.name = "Press [SPACE] to start";
	generateStars();
	ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void ofApp::update(){
	//Calculate the terrain every frame, so we're ready for re-gen
	animateTerrain();
	//Animate the water
	animateWater();
	//Animate the planet rotation
	planet.angle -= planet.angleV;
	if (planet.angle < 0) planet.angle = 360;
	//Animate the stars, moons and clouds
	animateClouds();
	animateStars();
	animateMoons();
	//Ease all numbers
	easeNumbers();

	//Apply some screen shake proportionate to the shakeForce
	screenShake.x += ofRandom(-shakeForce, shakeForce);
	screenShake.y += ofRandom(-shakeForce, shakeForce);
	screenShake.x *= 0.95;
	screenShake.y *= 0.95;

	//Do some mic analysis
	if (recording) analyseMic();
}

//--------------------------------------------------------------
void ofApp::draw(){
	//Always translate the 0,0 of the coordinate space to the middle of the window
	ofPushMatrix();
	ofTranslate(centerScreen);
	ofTranslate(screenShake);

	//Set the background to black for now and draw some stars
	ofEnableAlphaBlending();
	ofSetColor(ofColor(0, 0, 0, bgAlpha));
	ofDrawRectangle(-1000, -1000, 3000, 3000);
	drawStars();

	//Now draw the planet and clouds
	ofTranslate(planetOffset, 0);
	drawPlanet();

	drawMoons();
	drawClouds();
	
	//Finally draw the UI
	ofPopMatrix();
	drawUI();
	ofDisableAlphaBlending();
}

//Draws all the UI elements
void ofApp::drawUI() {
	//First render the title
	ofSetColor(255);
	font.title.drawString(planet.name, -font.title.stringWidth(planet.name) / 2 + centerScreen.x, 50);
	//Now render some UI elements (bottom is 0, 0)
	ofTranslate(0, ofGetHeight());
	ofSetColor(200);
	font.debug.drawString("Silences: " + to_string(silences), 10, -10);
	font.debug.drawString("Average Volume: " + to_string(avgVolume), centerScreen.x / 3, -10);
	font.debug.drawString("Dynamic Range: " + to_string(dynamicRange), centerScreen.x, -10);
	font.debug.drawString("Mic Volume: ", centerScreen.x * 1.5, -10);
	micGainPos.set(centerScreen.x * 1.5 + 120, -20);
	drawMicGain();
	//Move 0,0 to top and draw fps meter
	ofSetColor(150);
	ofTranslate(0, -ofGetHeight());
	font.debug.drawString("fps: " + to_string((int) ofGetFrameRate()), 10, 20);

	//Draw the info screen (not the first time
	if (landVerts.size() == 0) return;
	//Draw the holder
	ofTranslate((recording ? -planetOffset : planetOffset) * 0.3, 0);
	ofSetColor(planet.landColor);
	ofDrawRectRounded(infoPos, infoDim.x, infoDim.y, 10);
	ofSetColor(ofColor::fromHsb(0, 0, 255, 50));
	ofDrawRectRounded(infoPos + 5, infoDim.x - 10, infoDim.y - 10, 10);
	ofTranslate(infoPos.x + 20, infoPos.y + 20);
	//Draw the info
	ofSetColor(ofColor::white);
	font.title.drawString("Size:", 10, 50);
	font.normal.drawString(planet.sizeClass, 10, 80);
	font.title.drawString("Resources: ", 10, 150);
	font.normal.drawString(planet.resources, 10, 180);
	font.title.drawString("Description: ", 10, 270);
	font.normal.drawString(planet.description, 10, 300);
}

//Returns a couple of sentences of description
string ofApp::getDescription() {
	string sent1 = SENT1[(int) ofRandom(5)];
	string sent2 = SENT2[(int) ofRandom(5)];
	string sent3 = SENT3[(int) ofRandom(5)];
	return sent1 + "\n" + sent2 + "\n" + sent3;
}

//Returns a list of possible resources that could be on this planet
string ofApp::getResources() {
	int res1Index = ofRandom(10);
	int res2Index = ofRandom(10);
	int res3Index = ofRandom(10);
	//Make sure the indeces are unique
	while (res2Index == res1Index) res2Index = ofRandom(10);
	while (res2Index == res3Index || res1Index == res3Index) res3Index = ofRandom(10);
	//Get the corresponding strings and res numbers
	string res1 = RESOURCES[res1Index];
	string res2 = RESOURCES[res2Index];
	string res3 = RESOURCES[res3Index];
	return res1 + ", " + res2 + ", \nand " + res3;
}

//Returns the size classification based on the base height
string ofApp::getSizeClass() {
	if (BASE_HEIGHT > 120) {
		return "Gargantuan Cluster Giant"; 
	}
	else if (BASE_HEIGHT > 100) {
		return "Towering Planar Orb";
	}
	else if (BASE_HEIGHT > 80) {
		return "Large Astral Body";
	}
	else if (BASE_HEIGHT > 60) {
		return "Strikingly Normal Planet";
	}
	else if (BASE_HEIGHT > 40) {
		return "Cute Small Planet"; 
	}
	else {
		return "Mini Dwarf Moon";
	}
}

//Draws the little gain meter for the microphone
void ofApp::drawMicGain() {
	ofPushMatrix();
	ofTranslate(micGainPos);
	float w = ofMap(micVolume, 0, micMaxGain, 0, 1, true);
	float l = ofMap(micThreshold, 0, micMaxGain, 0, 1, true);
	ofSetColor(ofColor::fromHsb(100 - w * 100, 255, 255));
	ofDrawRectangle(0, 0, w * 200, 10);
	ofSetColor(ofColor::white);
	ofDrawLine(ofVec2f(l * 200, 0), ofVec2f(l * 200, 20));
	ofPopMatrix();
}

//Draws the complete planet including rotation and everything
void ofApp::drawPlanet() {
	ofPushMatrix();
	ofRotateDeg(planet.angle);
	//Start drawing the watrer
	ofSetColor(planet.waterColor);
	//Draw the circle from the list of verteces
	drawVerts(waterVerts);
	//Do the same for land
	ofSetColor(planet.landColor);
	//Draw the circle from the list of verteces
	drawVerts(landVerts);
	ofPopMatrix();
}

//Draws every star in the list of stars
void ofApp::drawStars() {
	int max = stars.size();
	ofVec2f future, current;
	for (int i = 0; i < max; i++) {
		future.x = stars[i].x + stars[i].vx * starSpeedMult;
		future.y = stars[i].y + stars[i].vy;
		current.x = stars[i].x;
		current.y = stars[i].y;
		ofSetColor(stars[i].c);
		ofDrawCircle(stars[i].x, stars[i].y, stars[i].r);
	}
}

//Draws every cloud that is on the surface of the planet
void ofApp::drawClouds() {
	int max = clouds.size();
	ofSetColor(planet.cloudColor);
	ofVec2f pos;
	for (int i = 0; i < max; i++) {
		pos = p2c(clouds[i].angle, clouds[i].height);
		ofDrawCircle(pos, clouds[i].radius);
	}
}

void ofApp::drawMoons() {
	int max = moons.size();
	ofVec2f pos;
	for (int i = 0; i < max; i++) {
		ofSetColor(moons[i].color);
		pos = p2c(moons[i].angle, moons[i].height);
		ofDrawCircle(pos, moons[i].radius);
	}
}

//Returns a name made up of parts of names
string ofApp::getPlanetName() {
	int max = ofRandom(3, 6);
	string name = "";
	for (int i = 0; i < max; i++) {
		int index = ofRandom(0, 10);
		name += NAME_PARTS[index];
	};
	int index = ofRandom(0, 10);
	name += END_PARTS[index];
	return name;
}

void ofApp::generatePlanet() {
	generateStars();
	//Recreate the generator
	createGenerator();
	//Setup the base for the land verts and water verts
	landVerts.clear();
	waterVerts.clear();
	for (float a = 0; a < TWO_PI; a += INC) {
		ofVec2f v;
		ofVec2f v2;
		landVerts.push_back(v);
		waterVerts.push_back(v2);
	}
	//Generate the clouds and the moon
	generateClouds();
	generateMoons();
	//Get a nice name, and some water and land colors
	planet.name = getPlanetName();
	planet.angleV = ofRandom(0.05, 0.1);
	planet.waterColor = ofColor::fromHsb(ofRandom(255), ofRandom(50, 100), ofRandom(150, 200));
	planet.landColor = ofColor::fromHsb(ofRandom(255), ofRandom(100, 200), ofRandom(100, 150));
	planet.cloudColor = ofColor::fromHsb(ofRandom(255), ofRandom(50, 100), ofRandom(200, 250), 100);
	//Make the info for the display
	planet.sizeClass = getSizeClass();
	planet.resources = getResources();
	planet.description = getDescription();
}

void ofApp::generateMoons() {
	//Remove all previous moons
	moons.clear();
	//Random amt of moons
	int max = ofRandom(0, 4);//Max 3 moons
	for (int i = 0; i < max; i++) {
		Moon m;
		m.angle = ofRandom(TWO_PI);
		m.angleV = ofRandom(-0.001, 0.01);
		m.height = ofRandom(MAX_HEIGHT, MAX_HEIGHT + 50);
		m.radius = ofRandom(20, 30);
		m.color = ofColor(ofRandom(0, 255), ofRandom(50, 100), ofRandom(100, 150));
		moons.push_back(m);
		//Create the spots
		int num = ofRandom(1, 4);
		float deltaA = atan2(m.radius * .5, m.height);
		float offset[] = { deltaA, 0, -deltaA };
		for (int j = 0; j < num; j++) {
			Moon m2;
			m2.angle = m.angle + offset[j];
			m2.angleV = m.angleV;
			m2.height = m.height + offset[j] * 0.5;
			m2.radius = ofRandom(m.radius * .2, m.radius * .4);
			m2.color = ofColor(0, 0, 0, 80);
			moons.push_back(m2);
		}
	}
}

//Fills the stars array with a list of stars
void ofApp::generateStars() {
	//Remove all previous stars
	stars.clear();
	//Make a random amount of new stars (first time, if avg vol is 0, make at least 800);
	int max = ofRandom((avgVolume == 0) ? 800: 200, 1000);
	for (int i = 0; i < max; i++) {
		Star s;
		s.c = ofColor::fromHsb(ofRandom(0, 255), ofRandom(0, 100), ofRandom(150, 255));
		s.x = ofRandom(-centerScreen.x * 2, centerScreen.x * 2);
		s.y = ofRandom(-centerScreen.y * 2, centerScreen.y * 2);
		s.vx = ofRandom(0, .2);
		s.vy = ofRandom(0, .2);
		s.r = ofRandom(0.2, 2);
		stars.push_back(s);
	}
}

//Generates the cloud layers
void ofApp::generateClouds() {
	//Remove all prev clouds
	clouds.clear();
	int max = ofRandom(5, 30);
	for (int i = 0; i < max; i++) {
		Cloud c;
		c.angle = ofRandom(TWO_PI);
		c.height = BASE_HEIGHT + ofRandom(HEIGHT_RANGE);
		c.angleV = ofMap(c.height, BASE_HEIGHT, MAX_HEIGHT, 0.005, 0, true);
		c.radius = ofRandom(5, 10);
		//How many clouds are part of this
		int num = ofRandom(15, 25);
		for (int j = 0; j < num; j++) {
			Cloud c2;
			c2.angle = c.angle + ofRandom(-.005 * num, .005 * num);
			c2.height = c.height * ofRandom(0.95, 1.05);
			c2.angleV = c.angleV;
			c2.radius = c.radius * ofRandom(0.3, 1);
			clouds.push_back(c2);
		}
		clouds.push_back(c);
	}
}

//Animates all the moons
void ofApp::animateMoons() {
	int max = moons.size();
	for (int i = 0; i < max; i++) {
		moons[i].angle += moons[i].angleV;
	}
}

void ofApp::animateClouds() {
	int max = clouds.size();
	for (int i = 0; i < max; i++) {
		clouds[i].angle += clouds[i].angleV;
	}
}

//Animates all the stars in the starfield
void ofApp::animateStars() {
	int max = stars.size();
	for (int i = 0; i < max; i++) {
		if (stars[i].x > centerScreen.x * 2) stars[i].x = -centerScreen.x;
		else if (stars[i].x < -centerScreen.x * 2) stars[i].x = centerScreen.x;
		if (stars[i].y > centerScreen.y * 2) stars[i].y = -centerScreen.x;
		else if (stars[i].y < -centerScreen.y * 2) stars[i].y = centerScreen.x;
	}
}

//Handles the animation of the water
void ofApp::animateWater() {
	//Animate the water
	int max = waterOctaves.size();
	for (int i = 0; i < max; i++) {
		if(waterOctaves[i].radius > 0) waterOctaves[i].x += 0.01;
	}
	//Regenerate water terrain
	max = waterVerts.size();
	float angle;
	for (int i = 0; i < max; i++) {
		angle = INC * i;
		//Get height and set to water vert
		float r = getNoise(angle, waterOctaves);
		float x = sin(angle) * r;
		float y = cos(angle) * r;
		waterVerts[i].x = x;
		waterVerts[i].y = y;
	}
}

//Calculates the terrain as it should be according to the current noise gen parameters
void ofApp::animateTerrain() {
	//Regenerate water terrain
	int max = landVerts.size();
	float angle;
	for (int i = 0; i < max; i++) {
		angle = INC * i;
		//Get height and set to water vert
		float r = getNoise(angle, landOctaves);
		float x = sin(angle) * r;
		float y = cos(angle) * r;
		landVerts[i].x = x;
		landVerts[i].y = y;
	}
}

//Draws the list of verteces that is given into a shape
void ofApp::drawVerts(vector<ofVec2f> &vec) {
	int max = vec.size();
	ofBeginShape();
	for (int i = 0; i < max; i++) {
		ofVertex(vec[i]);
	}
	ofEndShape();
}

//Obtains a noise value at the specified angle for the specified octave vector
float ofApp::getNoise(float angle, vector<NoiseOctave> &noiseOctaves) {
	int max = noiseOctaves.size();
	float sum = BASE_HEIGHT;
	for (int i = 0; i < max; i++) {
		float x = sin(angle) * noiseOctaves[i].radius + noiseOctaves[i].x;
		float y = cos(angle) * noiseOctaves[i].radius + noiseOctaves[i].y;
		sum += ofNoise(x, y) * noiseOctaves[i].amplitude;
	}
	return sum;
}

//Create the noise generator with the multiple octaves for land and water
void ofApp::createGenerator() {
	//Clear the previous generators
	waterOctaves.clear();
	landOctaves.clear();
	/**
	We have the following params to play with
	- avgVolume [0.001 - 0.2]
	- recordLength [0-1]
	- dynamicRange [0.001 - 0.2]
	- silences (integer)
	**/
	//Normalize all variables (except for record length, which is already normalized)
	float avgVolumeN = ofMap(avgVolume, 0.0005, 0.3, 0, 1, true);
	float dynamicRangeN = ofMap(dynamicRange, 0.005, 0.3, 0, 1, true);
	float silencesN = ofMap(silences, 0, 40, 0, 1, true);

	//Depending on the recording length, set base height
	BASE_HEIGHT = sqrt(ofMap((recordLength + avgVolumeN) / 2, 0, 1, 2000, 40000, true));
	MAX_HEIGHT = ofMap((dynamicRangeN + avgVolumeN) / 2, 0, 1, BASE_HEIGHT, 300, true);
	HEIGHT_RANGE = MAX_HEIGHT - HEIGHT_RANGE;

	//Create the noise octaves for land
	ofVec2f pt = rndPt();
	addOctave(landOctaves, 1, 0.7, pt.x, pt.y);
	pt = rndPt();
	addOctave(landOctaves, 0.5, 0.4 * dynamicRange, pt.x, pt.y);
	pt = rndPt();
	addOctave(landOctaves, 2, -1 * silencesN, pt.x, pt.y);
	pt = rndPt();
	addOctave(landOctaves, 5, -.1, pt.x, pt.y);
	pt = rndPt();
	addOctave(landOctaves, 10, avgVolume, pt.x, pt.y);
	//Create the noise octaves for water
	pt = rndPt();
	addOctave(waterOctaves, 0, .5, pt.x, pt.y);//Main water level determiner
	pt = rndPt();
	addOctave(waterOctaves, 0.5, 0.04, pt.x, pt.y);
	pt = rndPt();
	addOctave(waterOctaves, 2, -.04, pt.x, pt.y);
	pt = rndPt();
	addOctave(waterOctaves, 4, -.02, pt.x, pt.y);
}

//Adds a new octave to the provided vector of noise octaves for the generation
void ofApp::addOctave(vector<NoiseOctave> &vec, float r, float a, float x, float y) {
	NoiseOctave oct;
	oct.radius = r;
	oct.amplitude = a * HEIGHT_RANGE;//Use a percentage [0-1] of the height range
	oct.x = x;
	oct.y = y;
	vec.push_back(oct);
}

//Return a random point between 100,100 and 1000,1000 for the perlin noise gen
ofVec2f ofApp::rndPt() {
	ofVec2f v(ofRandom(100, 1000), ofRandom(100, 1000));
	return v;
}

//Converts polar coordinates into carthesian coordinates
ofVec2f ofApp::p2c(float angle, float radius) {
	ofVec2f v;
	v.set(sin(angle) * radius, cos(angle) * radius);
	return v;
}

//Eases some numbers towards their target values
void ofApp::easeNumbers() {
	//Keep track ofrecording frames if we're recording
	if (recording) recordingFrames++;
	if (recordingFrames > MIN_RECORDING_FRAMES && !spacePressed) setHyperDrive(false);
	//Ease the actual numbers
	starSpeedMult += (targetStarSpeedMult - starSpeedMult) * 0.05;
	planetOffset += (targetPlanetOffset - planetOffset) * 0.05;
	shakeForce += (targetShakeForce - shakeForce) * 0.05;
	bgAlpha += (targetBgAlpha - bgAlpha) * 0.05;
	//Constantly pull micVolume lower, this 'erodes' mic volume, to make it pull to ground faster
	micVolume *= 0.90;

	//Ease all the volumes of the sounds
	hyperDrive.volume += (hyperDrive.tVolume - hyperDrive.volume) * .01;
	drone1.volume += (drone1.tVolume - drone1.volume) * .01;
	drone2.volume += (drone2.tVolume - drone2.volume) * .01;
	drone3.volume += (drone3.tVolume - drone3.volume) * .01;
	drone4.volume += (drone4.tVolume - drone4.volume) * .01;
	drone5.volume += (drone5.tVolume - drone5.volume) * .01;
	//And set the sound to the corresponding level on each of the soundplayers
	hyperDrive.player.setVolume(hyperDrive.volume);
	drone1.player.setVolume(drone1.volume);
	drone2.player.setVolume(drone2.volume);
	drone3.player.setVolume(drone3.volume);
	drone4.player.setVolume(drone4.volume);
	drone5.player.setVolume(drone5.volume);
	
	//Check playback position and if the active one nears the end, find a new drone
	Sound activeDrone;
	if (droneNum == 1) activeDrone = drone1;
	else if (droneNum == 2) activeDrone = drone2;
	else if (droneNum == 3) activeDrone = drone3;
	else if (droneNum == 4) activeDrone = drone4;
	else if (droneNum == 5) activeDrone = drone5;
	if (activeDrone.player.getPosition() > 0.95) {
		activeDrone.tVolume = DRONE_VOLUME;
		droneSound();
	}
}

//Sets the hyperdrive to the required status, also checks the minimum time has passed
void ofApp::setHyperDrive(bool enabled) {
	//Ignore if we're not changing anything
	if (enabled == recording) return;
	//Ignore if we haven't passed the required minimum recording time yet
	if (!enabled && recordingFrames < MIN_RECORDING_FRAMES) return;
	if (enabled) {
		recording = true;
		micVolume = 0;//Reset volume to 0 for now
		avgVolume = 0;
		planet.name = "Recording...";
		micHistory.clear();
		targetStarSpeedMult = 100;
		targetShakeForce = 30;
		targetPlanetOffset = ofGetWidth() * 1.5;
		recordingFrames = 0;
		targetBgAlpha = 10;
		silences = 0;
		hyperSound();
	}
	else {
		recording = false;
		targetStarSpeedMult = 1;
		avgVolume = getAvgVolume();
		planetOffset = -targetPlanetOffset; //Swap sides so the new planet comes from the other side
		targetPlanetOffset = 0;
		targetShakeForce = 0;
		targetBgAlpha = 100;
		recordingFrames = (recordingFrames > MAX_RECORDING_FRAMES) ? MAX_RECORDING_FRAMES : recordingFrames;
		recordLength = ofMap(recordingFrames, MIN_RECORDING_FRAMES, MAX_RECORDING_FRAMES, 0, 1, true);
		cout << "There were " << silences << " silence(s)" << endl; 
		cout << "Average volume was " << avgVolume << endl;
		cout << "Dynamic range was " << dynamicRange << endl;
		cout << "Recording took " << recordingFrames << " frames, which means " << recordLength << endl;
		generatePlanet();//Generate a new planet
		hit1.play();
		droneSound();
	}
}

//Makes a new balance of a couple of drone sounds
void ofApp::droneSound() {
	//Mute the hyperdrive
	hyperDrive.tVolume = 0;
	int pick = ofRandom(1, 6);
	while (pick == droneNum) pick = ofRandom(1, 6);//Never pick the same drone twice
	droneNum = pick;
	if (droneNum == 1) {
		drone1.tVolume = DRONE_VOLUME;
		drone1.player.setPositionMS(0);
	}
	else if (droneNum == 2) {
		drone2.tVolume = DRONE_VOLUME;
		drone2.player.setPositionMS(0);
	}
	else if (droneNum == 3) { 
		drone3.tVolume = DRONE_VOLUME;
		drone3.player.setPositionMS(0);
	}
	else if (droneNum == 4) {
		drone4.tVolume = DRONE_VOLUME;
		drone4.player.setPositionMS(0);
	}
	else if (droneNum == 5) {
		drone5.tVolume = DRONE_VOLUME;
		drone5.player.setPositionMS(0);
	}
}

//Turns of all drones and goes into hyper
void ofApp::hyperSound() {
	//Mute all drones
	drone1.tVolume = drone2.tVolume = drone3.tVolume = drone4.tVolume = drone5.tVolume = 0;
	//Set hyperdrive to full volume
	hyperDrive.tVolume = HYPERDRIVE_VOLUME;
	//Play the second hit 
	hit2.play();
}

//Returns the avg volume of the complete recording
float ofApp::getAvgVolume() {
	float sum = 0;
	float max = 0;
	float min = 1;
	float eased = 0;
	int num = micHistory.size();
	for (int i = 0; i < num; i++) {
		sum += micHistory[i];
		eased += (micHistory[i] - eased) * 0.1;
		if (eased < min) min = micHistory[i];
		if (eased > max) max = micHistory[i];
	}
	dynamicRange = max - min;
	return sum / num;
}

//Analyses the change in the micVolume variable
void ofApp::analyseMic() {
	if (micVolume == 0) return;//If we're just starting, ignore for now
	//Push latest volume into the list of mic volumes
	micHistory.push_back(micVolume);
	//If we're currently being very silent
	if (micVolume < micThreshold) {
		if (!silenceRegistered) {
			silenceRegistered = true;
			silences++;
		}
	}
	else {
		silenceRegistered = false;
	}
}

//Handles the audio setup, so sets up the mic for recording
void ofApp::audioSetup() {
	int channelsOut = 0;        // number of requested output channels (i.e. 2 for stereo).
	int channelsIn = 1;         // number of requested input channels.
	int sampleRate = 44100;     // requested sample rate (44100 is typical).
	int bufferSize = 512;       // requested buffer size (256 is typical).
	int numOfBuffers = 4;       // number of buffers to queue, less buffers will be more responsive, but less stable.
	mic.setup(this, channelsOut, channelsIn, sampleRate, bufferSize, numOfBuffers);
	samples.assign(bufferSize, 0.0);
	//Load the two hits
	hit1.load("hit1.ogg");
	hit1.setVolume(0.5);
	hit2.load("hit2.ogg");
	hit2.setVolume(0.5);

	//Prepare all soudn files
	hyperDrive.player.load("hyperdrive.ogg");
	hyperDrive.volume = 0;
	hyperDrive.tVolume = 0;
	hyperDrive.player.setVolume(0);
	drone1.player.load("drone1.ogg");
	drone1.volume = 0;
	drone1.tVolume = 0;
	drone1.player.setVolume(0);
	drone2.player.load("drone2.ogg");
	drone2.volume = 0;
	drone2.tVolume = 0;
	drone2.player.setVolume(0);
	drone3.player.load("drone3.ogg");
	drone3.volume = 0;
	drone3.tVolume = 0;
	drone3.player.setVolume(0);
	drone4.player.load("drone4.ogg");
	drone4.volume = 0;
	drone4.tVolume = 0;
	drone4.player.setVolume(0);
	drone5.player.load("drone5.ogg");
	drone5.volume = 0;
	drone5.tVolume = 0;
	drone5.player.setVolume(0);

	//Play all sounds and loop them all
	hyperDrive.player.play();
	hyperDrive.player.setLoop(true);
	drone1.player.play();
	drone1.player.setLoop(true);
	drone2.player.play();
	drone2.player.setLoop(true);
	drone3.player.play();
	drone3.player.setLoop(true);
	drone4.player.play();
	drone4.player.setLoop(true);
	drone5.player.play();
	drone5.player.setLoop(true);

	//Start the drone with app setup
	droneSound();
}

//Handles incoming audio buffers
void ofApp::audioIn(float * input, int bufferSize, int nChannels) {
	float bufferVolume = 0;
	int numSamples = 0;//Number of valid samples (overflow samples are ignored)
	for (int i = 0; i < bufferSize; i++) {
		samples[i] = input[i];
		if (samples[i] < 0) samples[i] = -samples[i];//Only positive sample values
		if (samples[i] <= 1) {
			numSamples++;
			bufferVolume += samples[i];
		}
	}
	bufferVolume /= numSamples;
	bufferVolume *= micGain;
	micVolume += (bufferVolume - micVolume) * 0.1;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 32) {//If space is pressed
		spacePressed = true;
		setHyperDrive(true);
	}
	else if (key == OF_KEY_LEFT) {
		micThreshold -= 0.001;
		if (micThreshold < 0.005) micThreshold = 0.005;
	}
	else if (key == OF_KEY_RIGHT) {
		micThreshold += 0.001;
		if (micThreshold > micMaxGain) micThreshold = micMaxGain;
	}
	else if (key == OF_KEY_UP) {
		micGain += 0.05;
		if (micGain > 2) micGain = 2;
	}
	else if (key == OF_KEY_DOWN) {
		micGain -= 0.05;
		if (micGain < 0.5) micGain = 0.5;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == 32) {//If space is released
		spacePressed = false;
		setHyperDrive(false);
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}
void ofApp::mouseDragged(int x, int y, int button){}
void ofApp::mousePressed(int x, int y, int button){}
void ofApp::mouseReleased(int x, int y, int button){}
void ofApp::mouseEntered(int x, int y){}
void ofApp::mouseExited(int x, int y){}
void ofApp::windowResized(int w, int h){}
void ofApp::gotMessage(ofMessage msg){}
void ofApp::dragEvent(ofDragInfo dragInfo){}
