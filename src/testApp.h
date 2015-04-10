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

		ofVec3f head, lHand, rHand, lWrist, rWrist, prevLHand, prevRHand;
		bool hasSkeleton;

		float jointDistance;
		ofxPanel gui;

		ofxFloatSlider p1, p2, p3, p4, p5, p6;

		ofVec3f lHandAdj, rHandAdj, lWristAdj, rWristAdj;

		ofVec3f mousePos;
		float motionEnergy;

		bool firstPress;

		vector <Flame *> flames;

		//vector <float *> motion;
		vector <float> motion;

};
