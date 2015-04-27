#include "Player.h"


Player::Player(void)
{
	//spellExists = false;
}

//--------------------------------------------------------------
void Player::setup(int _playerNum, int _pIndex){

	playerNum = _playerNum;
	pIndex = _pIndex;

	vel = 5;

	boom.loadSound("sounds/impactEd.mp3");
	boom.setMultiPlay(false);

	fWhoosh.loadSound("sounds/fire_whoosh.wav");
	fWhoosh.setVolume(0.75);
	fWhoosh.setMultiPlay(false);

	fCrackle.loadSound("sounds/fire_crackle.wav");
	fCrackle.setVolume(0.2);
	fCrackle.setMultiPlay(true);
	fCrackle.setLoop(true);

	spellCalled = false;
	spellExists = false;
	newFireCanBeCalled = true;

	spellState = 0;

	spellFired = false;
	spellCanBeFired = false;
	impactCheckCalled = false;
	impact = false;

	impactCounter = 0;

	playSound = false;
}

//--------------------------------------------------------------
void Player::updateSkeleton(ofVec3f *_head, ofVec3f *_lHand, ofVec3f *_rHand, ofVec3f *_lWrist, ofVec3f *_rWrist, ofVec3f *_lElbow, ofVec3f *_rElbow, ofVec3f *_lFoot, ofVec3f *_rFoot){

	head = *_head;
	lHand = *_lHand; // will these work as local variables?
	rHand = *_rHand;
	lWrist = *_lWrist;
	rWrist = *_rWrist;
	lElbow = *_lElbow;
	rElbow = *_rElbow;
	lFoot = *_lFoot;
	rFoot = *_rFoot;

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

		if (lHand.x < rHand.x - 30) {
			spellState = 1; //spell can be called
		}
	}

	//---Put these in sep. function with pointer to hand positions?)
	//cout << "Angle between hands: " << ofVec3f(1,0,0).angle(lHand - rHand) << " DEGREES" << endl;

	//==========SPELL CAN BE CALLED==================

	if (spellState == 1){
		if (rHand.x - lHand.x < 0 && prevRHand.x - prevLHand.x > 0 && abs(rHand.y-lHand.y) < 50) {// && !spellCalled && !spellExists && newFireCanBeCalled) { //&& !flameExists  //abs(rHand.y-lHand.y)
			//motionEnergy = 0;
			spellState = 2; //spell called

			//cout << "P1 Spell Called" << endl;

		}
	}

	//==========SPELL CALLED==================

	if (spellState == 2){

		if (rHand.x - lHand.x > 5) {	
			fWhoosh.play();
			fCrackle.play();
			spellState = 3; //spell exists
			//cout << "P1 Spell Exists" << endl;
		}
	}

	//==========SPELL EXISTS==================

	if (spellState == 3){
		//cout << "Can be Fired is true" << endl;
		//cout << "PlayerNum: " << playerNum << endl;
		spellPos = ofVec3f(lHand + (rHand-lHand)*0.5);

		float lDistMoved = lHand.squareDistance(prevLHand);
		float rDistMoved = rHand.squareDistance(prevRHand);
		float totalMoved = rDistMoved*0.1 + lDistMoved*0.1;
		totalMoved = ofClamp(totalMoved,0,50);


		//----ALTERNATIVE MOTION ENERGY APPROACH
		if (lDistMoved > 100 || rDistMoved > 100){
			motionEnergy += totalMoved;
		} else {
			motionEnergy *= 0.95;
		}

		spellIntensity = ofMap(motionEnergy,0,6000,0.0,1.0,true);

		if (playerNum == 1){

			if (handSpacing < 50 && lWrist.x > lElbow.x+20 && rWrist.x > rElbow.x+20 && abs(lWrist.y - rWrist.y) < 50){
				fireSpell();
			}
		} else if (playerNum == 2){

			if (handSpacing < 50 && lWrist.x < lElbow.x-20 && rWrist.x < rElbow.x-20 && abs(lWrist.y - rWrist.y) < 50){
				fireSpell();
			}
		}
	}


	//==========SPELL FIRED==================

	if (spellState == 4){
		vel*=1.2;

		if (playerNum == 1){
			spellPos.x += vel;
			if (spellPos.x > 640) {
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
			impactCounter += PI/ofGetFrameRate();
			//cout << "Impact counter: " << impactCounter << endl;
			impactSize = sin(impactCounter);//*powerAtImpact;
			impactSize = ofMap(impactSize,0,1,10,0.5-powerAtImpact);
			cout << "Impact size: " << impactSize << endl;
		} else {
			
			spellState = 0;
		}

	}


	//==========SOUND ADJUSTMENTS==================

	spellPan = ofMap(spellPos.x,0,640,-1.0,1.0);

	if (fCrackle.getIsPlaying()){
		fCrackle.setVolume(ofMap(spellIntensity,0.0,1.0,0.1,1.0));
		fCrackle.setSpeed(ofMap(spellIntensity,0.0,1.0,0.5,1.5));
		fCrackle.setPan(spellPan);
	}

	if (fWhoosh.getIsPlaying()){
		fWhoosh.setPan(spellPan);
	}


	hasSkeleton = true;

	prevLHand = lHand;
	prevRHand = rHand;

}

//--------------------------------------------------------------


void Player::fireSpell(){
	vel = 5;
	fCrackle.stop();
	fWhoosh.setVolume(ofMap(spellIntensity,0.0,1.0,0.5,1.0));
	fWhoosh.setSpeed(ofMap(spellIntensity,0.0,1.0,1.0,0.5));
	fWhoosh.setPan(spellPan);
	fWhoosh.play();
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
	boom.setVolume(ofMap(spellIntensity,0.0,1.0,0.5,3.0));
	boom.setSpeed(ofMap(spellIntensity,0.0,1.0,0.2,0.8));
	boom.setPan(spellPan);
	boom.play();
}

//--------------------------------------------------------------

void Player::startImpact(){
	playBoom();
	powerAtImpact = spellIntensity;
	impactCounter = 0;
	spellState = 5;
}

//--------------------------------------------------------------
ofVec3f Player::getSpellPos(){
	return spellPos;
}

//--------------------------------------------------------------
int Player::doesSpellExist(){
	if (spellExists){
		return 1;
	} else {
		return 0;
	}
}

//--------------------------------------------------------------
void Player::draw(){

}


Player::~Player(void)
{
}
