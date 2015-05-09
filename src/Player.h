#pragma once

#include "ofMain.h"

class Player
{
public:
	Player(void);

	void setup(int _playerNum, int _pIndex);
	void updateSkeleton(ofVec3f *_head, ofVec3f *_lHand, ofVec3f *_rHand, ofVec3f *_lWrist, ofVec3f *_rWrist, ofVec3f *_lElbow, ofVec3f *_rElbow);
	void callSpell();
	void fireSpell();
	void clearSpell();
	void draw();
	void playBoom();
	void startImpact(float* _spellPosX);
	void damage(float dmg);

	ofVec3f getSpellPos();

	float kResX;
	float kResY;

	int playerNum, pIndex;

	//COMMENTED THIS TO KEEP VARS LOCAL
	ofVec3f head, lHand, rHand, lWrist, rWrist, lElbow, rElbow, lFoot, rFoot; 
	ofVec3f prevLHand, prevRHand;
	bool hasSkeleton;

	ofVec3f spellPos, prevSpellPos;
	float motionEnergy;
	float spellIntensity;

	int spellState;

	string spellType;

	//spell creation logic
	bool spellFired;
	bool spellCalled, spellExists, newFireCanBeCalled, spellCanBeFired, impactCheckCalled;
	//	bool startImpact;
	bool impact, impactComplete;
	float spellCreateDelay;


	float prevLBehindHeadMax;
	float prevRBehindHeadMax;
	int throwTimer;
	int throwTimerMax;

	float vel;
	float startVel;

	float health;
	float startHealth;

	float impactCounter, impactSize;
	float powerAtImpact;

	vector <ofVec3f> spellPrevs;

	bool playSound;
	ofSoundPlayer fCrackle;
	ofSoundPlayer fWhoosh;
	ofSoundPlayer wWhoosh, wRunning;
	ofSoundPlayer boom;

	ofSoundPlayer* spellWhoosh;
	ofSoundPlayer* spellNoise;
	float spellPan;

	~Player(void);
};

