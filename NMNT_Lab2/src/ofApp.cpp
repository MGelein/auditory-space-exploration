#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetBackgroundAuto(false);
	//Load the font
	planetNameFont.load("galaxy.ttf", 32);	
	//Calculate the dimensions of the screen and store half, this is used to set center of the coordinate space
	centerScreen.x = ofGetWidth() / 2;
	centerScreen.y = ofGetHeight() / 2;
	screenShake.set(0, 0);
	//Generate the planet
	generatePlanet();
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
}

//--------------------------------------------------------------
void ofApp::draw(){
	//Always translate the 0,0 of the coordinate space to the middle of the window
	ofTranslate(centerScreen + screenShake);

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
	ofDisableAlphaBlending();

	//Finally draw the UI
	ofSetColor(255);
	planetNameFont.drawString(planet.name, - planetNameFont.stringWidth(planet.name) / 2, -centerScreen.y + 50);
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
	for (int i = 0; i < max; i++) {
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
	//Make a random amount of new stars
	int max = ofRandom(200, 1000);
	for (int i = 0; i < max; i++) {
		Star s;
		s.c = ofColor::fromHsb(ofRandom(0, 255), ofRandom(0, 100), ofRandom(150, 255));
		s.x = ofRandom(-centerScreen.x * 2, centerScreen.x * 2);
		s.y = ofRandom(-centerScreen.y * 2, centerScreen.y * 2);
		s.vx = ofRandom(0, .05);
		s.vy = ofRandom(0, .05);
		s.r = ofRandom(0.2, 2);
		stars.push_back(s);
	}
}

//Generates the cloud layers
void ofApp::generateClouds() {
	//Remove all prev clouds
	clouds.clear();
	int max = ofRandom(10, 20);
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
		stars[i].x += stars[i].vx * starSpeedMult;
		stars[i].y += stars[i].vy;
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
		waterOctaves[i].x += 0.01;
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
	waterOctaves.clear();
	landOctaves.clear();
	//Create the noise octaves for land
	ofVec2f pt = rndPt();
	addOctave(landOctaves, 1, 0.7, pt.x, pt.y);
	pt = rndPt();
	addOctave(landOctaves, 0.5, 0.1, pt.x, pt.y);
	pt = rndPt();
	addOctave(landOctaves, 2, -.4, pt.x, pt.y);
	pt = rndPt();
	addOctave(landOctaves, 5, -.1, pt.x, pt.y);
	pt = rndPt();
	addOctave(landOctaves, 10, 0.05, pt.x, pt.y);
	//Create the noise octaves for water
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
}

//Sets the hyperdrive to the required status, also checks the minimum time has passed
void ofApp::setHyperDrive(bool enabled) {
	//Ignore if we're not changing anything
	if (enabled == recording) return;
	//Ignore if we haven't passed the required minimum recording time yet
	if (!enabled && recordingFrames < MIN_RECORDING_FRAMES) return;
	if (enabled) {
		recording = true;
		targetStarSpeedMult = 1000;
		targetShakeForce = 100;
		targetPlanetOffset = ofGetWidth() * 1.5;
		recordingFrames = 0;
		targetBgAlpha = 10;
	}
	else {
		recording = false;
		targetStarSpeedMult = 1;
		planetOffset = -targetPlanetOffset; //Swap sides so the new planet comes from the other side
		targetPlanetOffset = 0;
		targetShakeForce = 0;
		targetBgAlpha = 100;
		generatePlanet();//Generate a new planet
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 32) {//If space is pressed
		spacePressed = true;
		setHyperDrive(true);
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
