#pragma once

#include "ofMain.h"
#include "ofxKinectCommonBridge.h"
#include "ofxGui.h"
#include "Flame.h"
#include "Spark.h"
#include "Player.h"

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		void setSpellPosition(ofVec3f *pos);
		void spellsCollideCheck();
		void spellsCollide();
		void impactCheck(int _pNum);
		void drawHealthBar(Player* p, int _pNum);
		void gameOverCheck();
		float collideUniformSet();
		
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

		bool depth;

		Player * player1;
		Player * player2;
		bool player1Exists, player2Exists;
		bool p1Updated, p2Updated;

		bool gameStarted, gameOver;
		int winner;

		bool playOutCollide;
		float collideCounter;
		float collideIntensity;
		ofVec3f collidePos;

		ofTrueTypeFont gameFont;
		ofImage startPose;

		ofSoundPlayer collideSound;

		float xResolution, yResolution, kResX, kResY;

		ofVec3f head1, lHand1, rHand1, lWrist1, rWrist1, lElbow1, rElbow1, lFoot1, rFoot1;
		ofVec3f head2, lHand2, rHand2, lWrist2, rWrist2, lElbow2, rElbow2, lFoot2, rFoot2;
		bool hasSkeleton;

		float jointDistance;
		

		//spell creation logic
		bool spellFired;
		float spellCreateDelay;

		ofVec3f mousePos;
		float motionEnergy;
		float spellIntensity;

		bool firstPress;

		bool playSound;

};
