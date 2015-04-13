#pragma once

#include "ofMain.h"
#include "ofxKinectCommonBridge.h"
#include "ofxGui.h"
#include "Flame.h"
#include "Spark.h"

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		void setSpellPosition(ofVec3f *pos);
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofxKinectCommonBridge kinect;
		ofShader shader;
		ofPlanePrimitive plane;

		float xResolution, yResolution;

		ofVec3f head, lHand, rHand, lWrist, rWrist, prevLHand, prevRHand;
		bool hasSkeleton;

		float jointDistance;
		ofxPanel gui;

		ofxFloatSlider p1, p2, p3, p4, p5, p6;

		ofVec3f lHandAdj, rHandAdj, lWristAdj, rWristAdj;
		ofVec3f spellPos;

		//spell creation logic
		bool spellFired;
		bool spellCalled, spellExists, newFireCanBeCalled;
		float spellCreateDelay;

		ofVec3f mousePos;
		float motionEnergy;
		float spellIntensity;

		bool firstPress;

		vector <Flame *> flames;

		//vector <float *> motion;
		vector <float> motion;

		ofSoundPlayer fCrackle;
		ofSoundPlayer fWhoosh;

		bool playSound;

};
