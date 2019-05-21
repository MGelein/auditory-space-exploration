#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//Create the noise octaves for land
	addLandOct(1, 20, 100, 100);

	//Create the noise octaves for water
	addWaterOct(1, 2, 100, 100);
	addWaterOct(2, 2, 200, 200);

	//Setup the base for the land verts and water verts
	for (float a = 0; a < TWO_PI; a += INC) {
		//Get height and add to water vert
		float r = getWaterNoise(a);
		float x = sin(a) * r;
		float y = cos(a) * r;
		ofVec2f v(x, y);
		waterVerts.push_back(v);
		//Do the same for land
		r = getLandNoise(a);
		x = sin(a) * r;
		y = cos(a) * r;
		landVerts.push_back(v);
	}	

	//Calculate the dimensions of the screen and store half, this is used to set center of the coordinate space
	halfWidth = ofGetWidth() / 2;
	halfHeight = ofGetHeight() / 2;
}

//--------------------------------------------------------------
void ofApp::update(){
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	//Always translate the middle of the coordinate space to the middle of the window
	ofTranslate(halfWidth, halfHeight);

	//Set the background to black for now
	ofBackground(0);

	//Start drawing the watrer
	ofSetHexColor(waterColor);
	//Draw the circle from the list of verteces
	int max = waterVerts.size();
	ofBeginShape();
	for (int i = 0; i < max; i++) {
		ofVertex(waterVerts[i]);
	}
	ofEndShape();

	//Do the same for land
	//ofSetHexColor(landColor);
	//Draw the circle from the list of verteces
	//int max = landVerts.size();
	//ofBeginShape();
	//for (int i = 0; i < max; i++) {
	//ofVertex(landVerts[i]);
	//}
	//ofEndShape();
}

//Gets a sampling of the noise for this specific angle of the planet
float ofApp::getLandNoise(float angle) {
	int max = landOctaves.size();
	float sum = BASE_HEIGHT;
	for (int i = 0; i < max; i++) {
		float x = sin(angle) * landOctaves[i].radius + landOctaves[i].x;
		float y = cos(angle) * landOctaves[i].radius + landOctaves[i].y;
		sum += ofNoise(x, y) * landOctaves[i].amplitude;
	}
	return sum;
}

// Gets a sampling of the noise for this specific angle of the planet for WATER
float ofApp::getWaterNoise(float angle) {
	int max = waterOctaves.size();
	float sum = BASE_HEIGHT;
	for (int i = 0; i < max; i++) {
		float x = sin(angle) * waterOctaves[i].radius + landOctaves[i].x;
		float y = cos(angle) * waterOctaves[i].radius + landOctaves[i].y;
		sum += ofNoise(x, y) * waterOctaves[i].amplitude;
	}
	return sum;
}

//Adds a water octave to the terrain gen
void ofApp::addWaterOct(float r, float a, float x, float y) {
	NoiseOctave oct;
	oct.radius = r;
	oct.amplitude = a;
	oct.x = x;
	oct.y = y;
	waterOctaves.push_back(oct);
}

//Adds a new octave to the terrain land gen
void ofApp::addLandOct(float r, float a, float x, float y) {
	NoiseOctave oct;
	oct.radius = r;
	oct.amplitude = a;
	oct.x = x;
	oct.y = y;
	landOctaves.push_back(oct);
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
