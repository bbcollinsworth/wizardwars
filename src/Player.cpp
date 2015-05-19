#include "Player.h"


Player::Player(void)
{
	//spellExists = false;
}

//--------------------------------------------------------------
void Player::setup(int _playerNum, int _pIndex){

	playerNum = _playerNum;
	pIndex = _pIndex;

	kResX = 640;
	kResY = 480;
	//kResX*=2;

	startVel = 7;
	//vel = 5;

	boom.loadSound("sounds/impactEd.mp3");
	boom.setMultiPlay(false);

	fWhoosh.loadSound("sounds/fire_whoosh.wav");
	fWhoosh.setVolume(0.75);
	fWhoosh.setMultiPlay(false);

	fCrackle.loadSound("sounds/fire_crackle.wav");
	fCrackle.setVolume(0.2);
	fCrackle.setMultiPlay(true);
	fCrackle.setLoop(true);

	wWhoosh.loadSound("sounds/water_whoosh.wav");
	wWhoosh.setVolume(1.5);
	wWhoosh.setMultiPlay(false);

	wRunning.loadSound("sounds/water_flowing.wav");
	wRunning.setVolume(0.3);
	wRunning.setMultiPlay(true);
	wRunning.setLoop(true);

	spellWhoosh = &fWhoosh;
	spellNoise = &fCrackle;

	//spellNoise->setLoop(true);

	spellCalled = false;
	spellExists = false;
	newFireCanBeCalled = true;

	spellState = 0;
	startHealth = 20;
	health = startHealth;

	spellFired = false;
	spellCanBeFired = false;
	impactCheckCalled = false;
	impact = false;

	impactCounter = 0;

	prevLBehindHeadMax = 0;
	float prevRBehindHeadMax = 0;
	throwTimerMax = 60;
	throwTimer = throwTimerMax;

	playSound = false;
}

//--------------------------------------------------------------
void Player::updateSkeleton(ofVec3f *_head, ofVec3f *_lHand, ofVec3f *_rHand, ofVec3f *_lWrist, ofVec3f *_rWrist, ofVec3f *_lElbow, ofVec3f *_rElbow){

	head = *_head;
	lHand = *_lHand; // will these work as local variables?
	rHand = *_rHand;
	lWrist = *_lWrist;
	rWrist = *_rWrist;
	lElbow = *_lElbow;
	rElbow = *_rElbow;
	//lFoot = *_lFoot;
	//rFoot = *_rFoot;

	//lHand = _lHand; // will these work as local variables?
	//rHand = _rHand;
	//lWrist = _lWrist;
	//rWrist = _rWrist;
	//lElbow = _lElbow;
	//rElbow = _rElbow;


	//cout << "Motion Energy: " << motionEnergy << endl;

	//=====SET A TIME GATE ON NEW FLAME???====//

	float handSpacing = lWrist.distance(rWrist);

	//==========RESET SPELL, PREPARE FOR NEW CALL==================

	if (spellState == 0){ 
		impactSize = 0;
		impactCounter = 0;
		motionEnergy = 0;
		spellIntensity = 0;
		spellPos = ofVec3f(lHand + (rHand-lHand)*0.5);
		//CLEARS PREVSPELLPOS
		//prevSpellPos = ofVec3f(lHand + (rHand-lHand)*0.5);
		spellPrevs.clear();

		if (lHand.x < rHand.x - 30) {
			spellState = 1; //spell can be called
		}
	}

	//---Put these in sep. function with pointer to hand positions?)
	//cout << "Angle between hands: " << ofVec3f(1,0,0).angle(lHand - rHand) << " DEGREES" << endl;

	//==========SPELL CAN BE CALLED==================

	if (spellState == 1){

		//float DistToFeet = abs((lHand.y + rHand.y)*0.5 - (lFoot.y+rFoot.y)*0.5);
		float avgHandYPos = (lHand.y + rHand.y)*0.5;

		if (rHand.x - lHand.x < 0 && prevRHand.x - prevLHand.x > 0 && abs(rHand.y-lHand.y) < 80) {
			spellType = "fire";
			spellState = 2; 

		} else if (avgHandYPos > 280){
			spellType = "water";
			spellState = 2;
		}
	}

	//==========SPELL CALLED==================

	if (spellState == 2){

		if (spellType == "fire"){
			if (rHand.x - lHand.x > 10) {

				spellWhoosh = &fWhoosh;
				spellNoise = &fCrackle;
				//fWhoosh.play();
				//fCrackle.play();
				//spellState = 3; 

				callSpell();
				//spell exists
				//cout << "P1 Spell Exists" << endl;
			}
		} else if (spellType == "water"){
			if ((lHand.y + rHand.y)*0.5 < 260) {	
				spellWhoosh = &wWhoosh;
				spellNoise = &wRunning;
				callSpell();
				//wWhoosh.play();
				//wRunning.play();
				//spellState = 3; //spell exists
				//cout << "P1 Spell Exists" << endl;
			}
		}
	}

	//==========SPELL EXISTS==================

	if (spellState == 3){
		//cout << "Can be Fired is true" << endl;//cout << "PlayerNum: " << playerNum << endl;
		//spellPos = ofVec3f(lHand + (rHand-lHand)*0.5);

		float lDistMoved = lHand.squareDistance(prevLHand);
		float rDistMoved = rHand.squareDistance(prevRHand);
		float totalMoved = rDistMoved*0.1 + lDistMoved*0.1;
		totalMoved = ofClamp(totalMoved,0,50);

		if (lDistMoved > 100 || rDistMoved > 100){
			motionEnergy += totalMoved;
		} else {
			motionEnergy *= 0.95;
		}

		spellIntensity = ofMap(motionEnergy,0,6000,0.0,1.0,true);


		if (prevLBehindHeadMax < -20 && prevRBehindHeadMax < -20){
			if (playerNum == 1){
				if (lHand.x - head.x > abs((lHand.y - head.y)*0.1) && rHand.x - head.x > abs((rHand.y - head.y)*0.1)) { 

					fireSpell();
				}
			} else if (playerNum == 2){
				if (head.x - lHand.x > abs((lHand.y - head.y)*0.1) && head.x - rHand.x > abs((rHand.y - head.y)*0.1)) { 

					fireSpell();
				}
			}
		}

		//if (handSpacing < 50 && lWrist.x > lElbow.x+20 && rWrist.x > rElbow.x+20 && abs(lWrist.y - rWrist.y) < 50){
		//	fireSpell();
		//}
		/*} else if (playerNum == 2){
		if (handSpacing < 50 && lWrist.x < lElbow.x-20 && rWrist.x < rElbow.x-20 && abs(lWrist.y - rWrist.y) < 50){
		fireSpell();
		}
		}*/
	}

	//==========SET PRE-FIRE SPELLPOS==================
	if (spellState < 4){
		spellPos = ofVec3f(lHand + (rHand-lHand)*0.5);
	}

	//==========SPELL FIRED==================

	if (spellState == 4){
		spellIntensity = spellIntensity;
		cout << spellIntensity << endl;

		vel*=1.2;

		if (playerNum == 1){
			spellPos.x += vel;
			if (spellPos.x > kResX) {
				spellState = 0;
			}

		} else if (playerNum == 2){
			//vel *= -1;
			spellPos.x -= vel;
			if (spellPos.x < 0) {
				spellState = 0;
			}
		}
	}

	//==========SPELL IS IMPACTING==================

	if (spellState == 5){
		vel = 0;
		spellPos = spellPos;
		if (impactCounter < PI){
			float frameDivisor = floor(ofGetFrameRate());
			float impactLength = ofMap(spellIntensity,0,1,frameDivisor*0.5,frameDivisor*1.5);
			impactCounter += PI/impactLength;//ofGetFrameRate();
			//cout << "Impact counter: " << impactCounter << endl;
			impactSize = sin(impactCounter);
			impactSize = ofMap(impactSize,0,1,10.0,1.0-(spellIntensity*1.5));//powerAtImpact);
			cout << "Impact size: " << impactSize << endl;

		} else {
			spellState = 0;
		}
	}

	//==========SOUND ADJUSTMENTS==================

	spellPan = ofMap(spellPos.x,0,kResX,-1.0,1.0);

	//if (spellState > 1){

		if (spellNoise->getIsPlaying()){
			spellNoise->setPan(spellPan);

			if (spellType == "fire"){
				spellNoise->setSpeed(ofMap(spellIntensity,0.0,1.0,0.5,1.5));
				spellNoise->setVolume(ofMap(spellIntensity,0.0,1.0,0.1,1.0));
			} else if (spellType == "water"){
				spellNoise->setSpeed(ofMap(powf(spellIntensity,4.0f),0.0,1.0,1.0,0.8));
				spellNoise->setVolume(ofMap(spellIntensity,0.0,1.0,0.2,2.5));
			}
			
		}

		if (spellWhoosh->getIsPlaying()){
			spellWhoosh->setPan(spellPan);
		}
	//}

	hasSkeleton = true;

	prevLHand = lHand;
	prevRHand = rHand;

	//NEED TO CLEAR WHEN SPELL THROWN

	float lHandFromHead;
	float rHandFromHead;

	if (playerNum == 1){
		lHandFromHead = lHand.x - head.x;
		rHandFromHead = rHand.x - head.x; 
	}

	if (playerNum == 2){
		lHandFromHead = head.x - lHand.x;
		rHandFromHead = head.x - rHand.x;
	}

	if (lHandFromHead < prevLBehindHeadMax && rHandFromHead < prevRBehindHeadMax) {
		prevLBehindHeadMax = lHandFromHead;
		prevRBehindHeadMax = rHandFromHead;
		throwTimer = throwTimerMax;
	}

	throwTimer--;

	if (throwTimer <= 0 || spellState != 3){
		prevLBehindHeadMax = 0;
		prevRBehindHeadMax = 0;
	}

	spellPrevs.push_back(spellPos);
	if (spellPrevs.size() > ofGetFrameRate()*0.25){
		spellPrevs.erase(spellPrevs.begin());
	}

	//if (spellState > 2){
	prevSpellPos = spellPrevs[0];
	//}

}

//--------------------------------------------------------------

void Player::callSpell(){
	spellWhoosh->play();
	spellNoise->play();

	spellState = 3;
	//impactCheckCalled = false;
}

//--------------------------------------------------------------


void Player::fireSpell(){
	vel = startVel;
	spellNoise->stop();
	spellWhoosh->setVolume(ofMap(spellIntensity,0.0,1.0,0.5,1.5));
	spellWhoosh->setSpeed(ofMap(spellIntensity,0.0,1.0,1.5,0.9));
	spellWhoosh->setPan(spellPan);
	spellWhoosh->play();
	spellState = 4;

	cout << "fireSpell completed" << endl;
}

//--------------------------------------------------------------

void Player::clearSpell(){
	spellExists = false;
	spellFired = false;
	//impactCheckCalled = false;
}

//--------------------------------------------------------------

void Player::playBoom(){
	//boom.play();
	boom.setVolume(ofMap(spellIntensity*spellIntensity,0.0,1.0,0.1,3.0));
	boom.setSpeed(ofMap(spellIntensity,0.0,1.0,4.0,1.0));
	boom.setPan(spellPan);
	boom.play();
}

//--------------------------------------------------------------

void Player::startImpact(float *_spellPosX){
	playBoom();
	//powerAtImpact = spellIntensity;
	impactCounter = 0;
	spellPos.x = *_spellPosX;
	spellState = 5;
}

//--------------------------------------------------------------
void Player::damage(float dmg){
	health -= dmg*10;
}

//--------------------------------------------------------------
ofVec3f Player::getSpellPos(){
	return spellPos;
}

//--------------------------------------------------------------
void Player::draw(){

}


Player::~Player(void)
{
}
