#pragma once

#include "ofMain.h"

class Player
{
public:
	Player(void);

	void setup(int _playerNum, int _pIndex);
	void updateSkeleton(ofVec3f *_lHand, ofVec3f *_rHand, ofVec3f *_lWrist, ofVec3f *_rWrist, ofVec3f *_lElbow, ofVec3f *_rElbow);
	void fireSpell(float startVel);
	void draw();
	ofVec3f getSpellPos();
	int doesSpellExist();

	int playerNum, pIndex;


	ofVec3f lHand, rHand, lWrist, rWrist, lElbow, rElbow; 
	ofVec3f prevLHand, prevRHand;
	bool hasSkeleton;

	ofVec3f spellPos;
	float motionEnergy;
	float spellIntensity;

	//spell creation logic
	bool spellFired;
	bool spellCalled, spellExists, newFireCanBeCalled, spellCanBeFired;
	float spellCreateDelay;

	float vel;


	bool playSound;
	ofSoundPlayer fCrackle;
	ofSoundPlayer fWhoosh;

	~Player(void);
};

