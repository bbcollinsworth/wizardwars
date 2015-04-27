#pragma once

#include "ofMain.h"

class Player
{
public:
	Player(void);

	void setup(int _playerNum, int _pIndex);
	void updateSkeleton(ofVec3f *_head, ofVec3f *_lHand, ofVec3f *_rHand, ofVec3f *_lWrist, ofVec3f *_rWrist, ofVec3f *_lElbow, ofVec3f *_rElbow, ofVec3f *_lFoot, ofVec3f *_rFoot);
	void fireSpell();
	void clearSpell();
	void draw();
	void playBoom();
	void startImpact();

	ofVec3f getSpellPos();

	int doesSpellExist();

	int playerNum, pIndex;

	//COMMENTED THIS TO KEEP VARS LOCAL
	ofVec3f head, lHand, rHand, lWrist, rWrist, lElbow, rElbow, lFoot, rFoot; 
	ofVec3f prevLHand, prevRHand;
	bool hasSkeleton;

	ofVec3f spellPos;
	float motionEnergy;
	float spellIntensity;

	int spellState;

	//spell creation logic
	bool spellFired;
	bool spellCalled, spellExists, newFireCanBeCalled, spellCanBeFired, impactCheckCalled;
	//	bool startImpact;
	bool impact, impactComplete;
	float spellCreateDelay;

	float vel;

	float impactCounter, impactSize;
	float powerAtImpact;


	bool playSound;
	ofSoundPlayer fCrackle;
	ofSoundPlayer fWhoosh;
	ofSoundPlayer boom;
	float spellPan;

	~Player(void);
};

