#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//Load the font
	planetNameFont.load("galaxy.ttf", 32);

	//Create the noise octaves for land
	addOctave(landOctaves, 1, 70, 100, 100);
	addOctave(landOctaves, 0.5, 10, 50, 50);
	addOctave(landOctaves, 2, -40, 150, 150);
	addOctave(landOctaves, 5, -10, 170, 170);
	addOctave(landOctaves, 10, 5, 200, 200);
	//Create the noise octaves for water
	addOctave(waterOctaves, 0.5, 4, 100, 100);
	addOctave(waterOctaves, 2, -4, 200, 200);
	addOctave(waterOctaves, 4, -2, 250, 250);
	
	//Setup the base for the land verts and water verts
	for (float a = 0; a < TWO_PI; a += INC) {
		ofVec2f v;
		ofVec2f v2;
		landVerts.push_back(v);
		waterVerts.push_back(v2);
	}
	
	//Calculate the dimensions of the screen and store half, this is used to set center of the coordinate space
	centerScreen.x = ofGetWidth() / 2;
	centerScreen.y = ofGetHeight() / 2;
	//Generate the stars and clouds
	generateStars();
	generateClouds();
	//Get a nice name, and some water and land colors
	planetName = getPlanetName();
	waterColor = ofColor::fromHsb(ofRandom(255), ofRandom(50, 100), ofRandom(150, 200));
	landColor = ofColor::fromHsb(ofRandom(255), ofRandom(100, 200), ofRandom(100, 150));
	cloudColor = ofColor::fromHsb(ofRandom(255), ofRandom(0, 50), ofRandom(200, 250));
}

//--------------------------------------------------------------
void ofApp::update(){
	//Calculate the terrain every frame, so we're ready for re-gen
	animateTerrain();
	//Animate the water
	animateWater();
	//Animate the planet rotation
	planetRotation -= 0.1;
	if (planetRotation < 0) planetRotation = 360;
	//Animate the stars and clouds
	animateClouds();
	animateStars();
}

//--------------------------------------------------------------
void ofApp::draw(){
	//Always translate the 0,0 of the coordinate space to the middle of the window
	ofTranslate(centerScreen);

	//Set the background to black for now
	ofBackground(0);
	drawStars();

	//Now draw the planet and clouds
	drawPlanet();
	drawClouds();

	//Finally draw the UI
	ofSetColor(255);
	planetNameFont.drawString(planetName, - planetNameFont.stringWidth(planetName) / 2, -centerScreen.y + 50);
}

//Draws the complete planet including rotation and everything
void ofApp::drawPlanet() {
	ofPushMatrix();
	ofRotateDeg(planetRotation);
	//Start drawing the watrer
	ofSetColor(waterColor);
	//Draw the circle from the list of verteces
	drawVerts(waterVerts);
	//Do the same for land
	ofSetColor(landColor);
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
	ofSetColor(cloudColor);
	ofVec2f pos;
	for (int i = 0; i < max; i++) {
		pos = p2c(clouds[i].angle, clouds[i].height);
		ofDrawCircle(pos, 5);
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

//Fills the stars array with a list of stars
void ofApp::generateStars() {
	//Remove all previous stars
	stars.clear();
	//Make a random amount of new stars
	int max = ofRandom(50, 500);
	for (int i = 0; i < max; i++) {
		Star s;
		s.c = ofColor::fromHsb(ofRandom(0, 255), ofRandom(0, 100), ofRandom(150, 255));
		s.x = ofRandom(-centerScreen.x, centerScreen.x);
		s.y = ofRandom(-centerScreen.y, centerScreen.y);
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
	int max = ofRandom(5, 20);
	for (int i = 0; i < max; i++) {
		Cloud c;
		c.angle = ofRandom(TWO_PI);
		c.angleV = ofRandom(0.0001, 0.001);
		c.height = BASE_HEIGHT + ofRandom(50);
		clouds.push_back(c);
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
		stars[i].x += stars[i].vx;
		stars[i].y += stars[i].vy;
		if (stars[i].x > centerScreen.x) stars[i].x = -centerScreen.x;
		else if (stars[i].x < -centerScreen.x) stars[i].x = centerScreen.x;
		if (stars[i].y > centerScreen.y) stars[i].y = -centerScreen.x;
		else if (stars[i].y < -centerScreen.y) stars[i].y = centerScreen.x;
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

//Adds a new octave to the provided vector of noise octaves for the generation
void ofApp::addOctave(vector<NoiseOctave> &vec, float r, float a, float x, float y) {
	NoiseOctave oct;
	oct.radius = r;
	oct.amplitude = a;
	oct.x = x;
	oct.y = y;
	vec.push_back(oct);
}

//Converts polar coordinates into carthesian coordinates
ofVec2f ofApp::p2c(float angle, float radius) {
	ofVec2f v;
	v.set(sin(angle) * radius, cos(angle) * radius);
	return v;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
