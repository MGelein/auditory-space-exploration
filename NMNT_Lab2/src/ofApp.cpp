#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
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
}

//--------------------------------------------------------------
void ofApp::update(){
	//Calculate the terrain every frame, so we're ready for re-gen
	calcTerrain();
	//Animate the water
	animateWater();
	//Animate the planet rotation
	planetRotation -= 0.1;
	if (planetRotation < 0) planetRotation = 360;
}

//--------------------------------------------------------------
void ofApp::draw(){
	//Always translate the 0,0 of the coordinate space to the middle of the window
	ofTranslate(centerScreen);

	//Set the background to black for now
	ofBackground(0);

	//Now draw the planet
	drawPlanet();
}

//Draws the complete planet including rotation and everything
void ofApp::drawPlanet() {
	ofPushMatrix();
	ofRotateDeg(planetRotation);
	//Start drawing the watrer
	ofSetHexColor(waterColor);
	//Draw the circle from the list of verteces
	drawVerts(waterVerts);
	//Do the same for land
	ofSetHexColor(landColor);
	//Draw the circle from the list of verteces
	drawVerts(landVerts);
	ofPopMatrix();
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
void ofApp::calcTerrain() {
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
