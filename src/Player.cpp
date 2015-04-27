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

	spellPan = ofMap(spellPos.x,0,640,-1.0,1.0);

	//cout << "Motion Energy: " << motionEnergy << endl;

	//=====SET A TIME GATE ON NEW FLAME???====//

	float handSpacing = lWrist.distance(rWrist);

	if (lHand.x < rHand.x - 30 && !spellExists && !spellCalled) {
		newFireCanBeCalled = true;
		spellState = 1;
	}


	if (spellCanBeFired){
		//cout << "Can be Fired is true" << endl;
		//cout << "PlayerNum: " << playerNum << endl;

		//handSpacing testApp.plane.getHeight()*0.1
		if (playerNum == 1){
			//cout << "playerNum is 1" << endl;
			//cout << "Handspacing is: " << handSpacing << endl;
			//cout << "LWrist: " << lWrist.x << "LElbow+20: " << lElbow.x+20 << endl;
			if (handSpacing < 50 && lWrist.x > lElbow.x+20 && rWrist.x > rElbow.x+20 && abs(lWrist.y - rWrist.y) < 50){

				//if (ofGetElapsedTimeMillis() > spellCreateTime + 
				fireSpell(vel);
				//impactCheckCalled = true;
				//cout << "fireSpell called" << endl;

			}
		} else if (playerNum == 2){

			if (handSpacing < 50 && lWrist.x < lElbow.x-20 && rWrist.x < rElbow.x-20 && abs(lWrist.y - rWrist.y) < 50){

				fireSpell(vel);
				//impactCheckCalled = true;


			}
		}
	}

	if (playSound){
		fWhoosh.setVolume(ofMap(spellIntensity,0.0,1.0,0.5,1.0));
		fWhoosh.setSpeed(ofMap(spellIntensity,0.0,1.0,1.0,0.5));
		fWhoosh.setPan(spellPan);
		fWhoosh.play();
		playSound = false;
	}



	if (playerNum == 1){
		if (spellPos.x > 640 || impactComplete) {
			clearSpell();
			//spellExists = false;
			//spellFired = false;
			//impactCheckCalled = false;
			//motion.clear();
		}
	}

	if (playerNum == 2){
		if (spellPos.x < 0) {
			clearSpell();
		}
	}

	//---Put these in sep. function with pointer to hand positions?)
	//cout << "Angle between hands: " << ofVec3f(1,0,0).angle(lHand - rHand) << " DEGREES" << endl;
	if (rHand.x - lHand.x < 0 && prevRHand.x - prevLHand.x > 0 && abs(rHand.y-lHand.y) < 50 && !spellCalled && !spellExists && newFireCanBeCalled) { //&& !flameExists  //abs(rHand.y-lHand.y)
		spellFired = false;
		spellCalled = true;
		//spellCreateDelay = ofGetElapsedTimef() +0.3;
		motionEnergy = 0;

		//cout << "P1 Spell Called" << endl;

	}

	if (spellCalled && !spellExists){
		spellFired = false;
		if (rHand.x - lHand.x > 5) {
			spellExists = true;
			fWhoosh.play();
			fCrackle.play();
			spellCalled = false;

			//cout << "P1 Spell Exists" << endl;

		}
	}

	if (fCrackle.getIsPlaying()){
		fCrackle.setVolume(ofMap(spellIntensity,0.0,1.0,0.1,1.0));
		fCrackle.setSpeed(ofMap(spellIntensity,0.0,1.0,0.5,1.5));
		fCrackle.setPan(spellPan);
	}

	if (fWhoosh.getIsPlaying()){
		fWhoosh.setPan(spellPan);
	}

	if (spellExists && !spellFired){
		newFireCanBeCalled = false;
		spellCanBeFired = true;
		//impact = false;
	}


	hasSkeleton = true;



	//if (spellExists){
	if (spellFired){
		if (!impact){
			vel*=1.2;

			if (playerNum == 1){
				spellPos.x += vel;

			} else if (playerNum == 2){
				//vel *= -1;
				spellPos.x -= vel;
			}
			//spellPos.x+=vel;

		} else if (impact) {
			vel = 0;
			spellPos = spellPos;
		} 
	//} else if (spellExists && !spellFired && !impact) {
	//	spellPos = ofVec3f(lHand + (rHand-lHand)*0.5);
		//cout << rHand.x << endl;
	} else {
		spellPos = ofVec3f(lHand + (rHand-lHand)*0.5);
	}

	//cout << "SPELLPOS: " << spellPos << endl;


	if (impact){
		//ofDegtoRad(180)
		if (impactCounter < PI){
			impactCounter += PI/ofGetFrameRate();
			//cout << "Impact counter: " << impactCounter << endl;
			impactSize = sin(impactCounter);//*powerAtImpact;
			impactSize = ofMap(impactSize,0,1,10,1-powerAtImpact);
			//impactSize = 1-impactSize;
			//impactSize*=10;
			cout << "Impact size: " << impactSize << endl;
		} else {
			impactSize = 0;
			clearSpell();
			impact = false;
		}
	}

	//} 

	prevLHand = lHand;
	prevRHand = rHand;

	//ofSoundUpdate();

}

//--------------------------------------------------------------


void Player::fireSpell(float startVel){
	vel = startVel;
	spellFired = true;
	fCrackle.stop();
	playSound = true;
	impactCheckCalled = true;
	spellCanBeFired = false;

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
	boom.setVolume(ofMap(spellIntensity,0.0,1.0,0.5,2.0));
	boom.setSpeed(ofMap(spellIntensity,0.0,1.0,0.3,1.0));
	boom.setPan(spellPan);
	boom.play();
}

//--------------------------------------------------------------

void Player::startImpact(){
	playBoom();
	// = false;
	powerAtImpact = spellIntensity;
	impactCounter = 0;
	impact = true;
	impactCheckCalled = false;
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
