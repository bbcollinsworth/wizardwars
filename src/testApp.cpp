#include "testApp.h"

#define USE_PROGRAMMABLE_RENDERER

void testApp::setup(){

	ofSetFrameRate(24);

	//==========================
	//KINECT SETUP + INITIALIZATION
	//==========================

	kResX = 320;
	kResY = 240;
	//kResX = 640;
	//kResY = 480;

	kinect.initSensor( 0 );

	//kinect.initColorStream(kResX, kResY);
	kinect.initDepthStream(kResX, kResY, true); //could I just change this to 640,480?
	//kinect.initDepthStream(640, 480, true); //could I just change this to 640,480?
	kinect.initSkeletonStream(false);
	kinect.setDepthClipping(1400.0F,2200.0F);

	kinect.start();


	//==========================
	//GL + SHADER SETUP
	//==========================

	xResolution = 1500;
	yResolution = kResY*xResolution/kResX;

	//ofToggleFullscreen();

#ifdef USE_PROGRAMMABLE_RENDERER
	//shader.load("shaders/blobs.vs", "shaders/blobs.fs");
	shader.load("shaders/blobs.vs", "shaders/frag_with_impact.fs");
	cout << "Shader in use is not gl2" << endl;
#else
	shader.load("shaders/blobs_gl2.vs", "shaders/blobs_gl2.fs");
	cout << "Shader in use IS gl2" << endl;
#endif

	GLint err = glGetError();
	if (err != GL_NO_ERROR){
		ofLogNotice() << "Load Shader came back with GL error:	" << err;
	}

	plane.set(xResolution, yResolution, 4, 4);
	plane.setPosition(0, 0, 0);
	plane.mapTexCoords(0, 0, xResolution, yResolution);


	gameFont.loadFont("fonts/Glastonbury.ttf", 60, true, true);
	gameFont.setLineHeight(18.0f);

	//==========================
	//BOOLEAN INITIALIZATIONS
	//==========================

	hasSkeleton = false;

	gameOver = false;
	winner = 0;

	playSound = false;

	player1Exists = false;
	player2Exists = false;
	p1Updated = false;
	p2Updated = false;


	//==========================

	lHandAdj = ofVec3f(plane.getWidth()*0.5,plane.getHeight()*0.5);
	rHandAdj = ofVec3f(0,0,0);
	lWristAdj = lHandAdj;
	rWristAdj = rHandAdj;

	/*head1 = &ofVec3f(0,0,0);
	lHand1 = &ofVec3f(0,0,0);
	rHand1 = &ofVec3f(0,0,0);
	lWrist1 = &ofVec3f(0,0,0);
	rWrist1 = &ofVec3f(0,0,0);
	lElbow1 = &ofVec3f(0,0,0);
	rElbow1 = &ofVec3f(0,0,0);
	lFoot1 = &ofVec3f(0,0,0);
	rFoot1 = &ofVec3f(0,0,0);

	head2 = &ofVec3f(0,0,0);
	lHand2 = &ofVec3f(0,0,0);
	rHand2 = &ofVec3f(0,0,0);
	lWrist2 = &ofVec3f(0,0,0);
	rWrist2 = &ofVec3f(0,0,0);
	lElbow2 = &ofVec3f(0,0,0);
	rElbow2 = &ofVec3f(0,0,0);
	lFoot2 = &ofVec3f(0,0,0);
	rFoot2 = &ofVec3f(0,0,0);*/


	jointDistance = 1.f;

}

//--------------------------------------------------------------
void testApp::update()
{
	kinect.update();
	//ofPixels d = kinect.getDepthPixelsRef();

	//cout << d.size() << endl;


	//if(kinect.isNewSkeleton()) {

	//	//motionEnergy*= 0.95;
	//} else {
	//	motionEnergy = 0;
	//}

	if(kinect.isNewSkeleton()) {

		//p1Updated = false;
		//p2Updated = false;

		//I JUST UNCOMMENTED THIS
		if (!player1Exists || !player2Exists){

			for( int i = 0; i < kinect.getSkeletons().size(); i++) 
			{

				if(kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD) != kinect.getSkeletons().at(i).end())

				{

					if (!player1Exists){
						player1 = new Player();
						player1->setup(1,i,&kResX);
						player1Exists = true;
						cout << "PLAYER 1 set up at Index: " << i << endl; 
					}

					if (!player2Exists && player1Exists){
						if (i != player1->pIndex){
							player2 = new Player();
							player2->setup(2,i,&kResX);
							player2Exists = true;
							cout << "PLAYER 2 set up at Index: " << i << endl; 
						}
					}
				}
			}
		}

		int p1index = -1;

		int skelCounter = 0;

		if (player1Exists){

			for( int i = 0; i < kinect.getSkeletons().size(); i++) 
			{

				if(kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD) != kinect.getSkeletons().at(i).end()
					&& kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second.getScreenPosition().x < 320 )
				{

					head1 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second.getScreenPosition();
					lHand1 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second.getScreenPosition();
					rHand1 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second.getScreenPosition();
					lWrist1 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_LEFT)->second.getScreenPosition();
					rWrist1 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_RIGHT)->second.getScreenPosition();
					lElbow1 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_LEFT)->second.getScreenPosition();
					rElbow1 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_RIGHT)->second.getScreenPosition();
					lFoot1 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_FOOT_LEFT)->second.getScreenPosition();
					rFoot1 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_FOOT_RIGHT)->second.getScreenPosition();

					/*SkeletonBone headBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second;
					SkeletonBone lHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second;
					SkeletonBone rHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second;
					SkeletonBone lWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_LEFT)->second;
					SkeletonBone rWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_RIGHT)->second;
					SkeletonBone lElbowBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_LEFT)->second;
					SkeletonBone rElbowBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_RIGHT)->second;
					SkeletonBone lFootBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_FOOT_LEFT)->second;
					SkeletonBone rFootBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_FOOT_RIGHT)->second;

					head1 = headBone.getScreenPosition();
					lHand1 = lHandBone.getScreenPosition();
					rHand1 = rHandBone.getScreenPosition();
					lWrist1 = lWristBone.getScreenPosition();
					rWrist1 = rWristBone.getScreenPosition();
					lElbow1 = lElbowBone.getScreenPosition();
					rElbow1 = rElbowBone.getScreenPosition();
					lFoot1 = lFootBone.getScreenPosition();
					rFoot1 = rFootBone.getScreenPosition();*/

					cout << "p1 Left Foot: " << lFoot1 << endl;

					player1->updateSkeleton(&head1,&lHand1,&rHand1,&lWrist1,&rWrist1,&lElbow1,&rElbow1);		

					p1index = i;

					skelCounter++;
					cout << "skeleton p1 setup has run " << skelCounter << " times." << endl;
					break;

				}
				cout << "skeleton p1 check has run " << i << " times." << endl;
			}

		}

		if (player2Exists){

			for( int i = 0; i < kinect.getSkeletons().size(); i++) 
			{

				if(kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD) != kinect.getSkeletons().at(i).end()
					&& kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second.getScreenPosition().x >= 320 && i!=p1index)
				{

					head2 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second.getScreenPosition();
					lHand2 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second.getScreenPosition();
					rHand2 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second.getScreenPosition();
					lWrist2 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_LEFT)->second.getScreenPosition();
					rWrist2 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_RIGHT)->second.getScreenPosition();
					lElbow2 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_LEFT)->second.getScreenPosition();
					rElbow2 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_RIGHT)->second.getScreenPosition();
					lFoot2 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_FOOT_LEFT)->second.getScreenPosition();
					rFoot2 = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_FOOT_RIGHT)->second.getScreenPosition();

					/*SkeletonBone headBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second;
					SkeletonBone lHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second;
					SkeletonBone rHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second;
					SkeletonBone lWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_LEFT)->second;
					SkeletonBone rWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_RIGHT)->second;
					SkeletonBone lElbowBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_LEFT)->second;
					SkeletonBone rElbowBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_RIGHT)->second;
					SkeletonBone lFootBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_FOOT_LEFT)->second;
					SkeletonBone rFootBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_FOOT_RIGHT)->second;

					head2 = headBone.getScreenPosition();
					lHand2 = lHandBone.getScreenPosition();
					rHand2 = rHandBone.getScreenPosition();
					lWrist2 = lWristBone.getScreenPosition();
					rWrist2 = rWristBone.getScreenPosition();
					lElbow2 = lElbowBone.getScreenPosition();
					rElbow2 = rElbowBone.getScreenPosition();
					lFoot2 = lFootBone.getScreenPosition();
					rFoot2 = rFootBone.getScreenPosition();*/

					/*head2 = getBone(headBone,head2);
					lHand2 = getBone(lHandBone,lHand2);
					rHand2 = getBone(rHandBone,rHand2);
					lWrist2 = getBone(lWristBone,lWrist2);
					rWrist2 = getBone(rWristBone,rWrist2);
					lElbow2 = getBone(lElbowBone,lElbow2);
					rElbow2 = getBone(rElbowBone,rElbow2);
					lFoot2 = getBone(lFootBone,lFoot2);
					rFoot2 = getBone(rFootBone,rFoot2);*/

					cout << "p2 Left Foot: " << lFoot2 << endl;

					player2->updateSkeleton(&head2,&lHand2,&rHand2,&lWrist2,&rWrist2,&lElbow2,&rElbow2);
					//cout << "Player2 updated - elbow is:" << lElbow2.x << "," << lElbow2.y << endl;
					//p2Updated=true;

					return;

				}
			}
		}
	}

	ofSoundUpdate();


}

//--------------------------------------------------------------
void testApp::draw(){

	if (player1Exists && player2Exists){
		gameOverCheck();
	}

	if (!gameOver){


		if (player1Exists && player1->spellState == 4){ //impactCheckCalled){// && player1->spellPos.x > 320){
			impactCheck(1);
			ofDrawBitmapString("P1 CHECKING IMPACT", ofGetWidth()*0.25, 100);
		}

		if (player2Exists && player2->spellState == 4){//impactCheckCalled){// && player1->spellPos.x > 320){
			impactCheck(2);
			ofDrawBitmapString("P1 CHECKING IMPACT", ofGetWidth()*0.25, 100);
		}

		ofDisableAlphaBlending();

		ofPushMatrix();
		ofTranslate(xResolution*0.5, yResolution*0.5);

		shader.begin();
		shader.setUniform1f("time", ofGetElapsedTimef());///2.0);
		shader.setUniform2f("resolution", xResolution, yResolution);
		shader.setUniform2f("kRes", kResX, kResY);
		//shader.setUniformTexture("videoTex", kinect.getColorTexture(), 0);
		shader.setUniformTexture("videoTex", kinect.getDepthTexture(), 0);

		//if(hasSkeleton)
		if (player1Exists)
		{

			shader.setUniform3f("p1SpellPoint", 1.0 + (player1->spellPos.x/(-320)), 1.0 + (player1->spellPos.y/(-240)), 0);
			shader.setUniform3f("p1PrevSpell", 1.0 + (player1->prevSpellPos.x/(-320)), 1.0 + (player1->prevSpellPos.y/(-240)), 0);
			shader.setUniform1f("p1Intensity", player1->spellIntensity);
			shader.setUniform1f("p1Health", player1->health/30);

			if (player1->spellState > 2 && player1->spellState < 5){

				//cout << "Shader Knows Player 1 spell called" << endl;
				//if (spellType == "flame"){

				//set and pass a number for each spell type
				shader.setUniform1i("p1SpellType", 1);
			} else {
				shader.setUniform1i("p1SpellType", 0);
			}

			if (player1->spellState == 5){
				shader.setUniform1f("p1Impact", player1->impactSize);
			} else {
				shader.setUniform1f("p1Impact", 0);
			}

		} else {

			shader.setUniform3f("p1SpellPoint", p3, p3, p3);
			shader.setUniform1f("p1Intensity", 0.0);
			shader.setUniform1i("p1SpellType", 0);
			shader.setUniform1i("p1Impact", 0);
			shader.setUniform1f("p1Health", 0);
		}

		if (player2Exists)
		{

			shader.setUniform3f("p2SpellPoint", 1.0 + (player2->spellPos.x/(-320.0)), 1.0 + (player2->spellPos.y/(-240.0)), 0);
			shader.setUniform3f("p2PrevSpell", 1.0 + (player2->prevSpellPos.x/(-320.0)), 1.0 + (player2->prevSpellPos.y/(-240.0)), 0);
			shader.setUniform1f("p2Intensity", player2->spellIntensity);
			shader.setUniform1f("p2Health", player2->health/30);

			if (player2->spellState > 2 && player2->spellState < 5){
				//if (spellType == "flame"){
				//set and pass a number for each spell type
				shader.setUniform1i("p2SpellType", 1);
			} else {
				shader.setUniform1i("p2SpellType", 0);
			}

			if (player2->spellState == 5){
				shader.setUniform1f("p2Impact", player2->impactSize);
			} else {
				shader.setUniform1f("p2Impact", 0);
			}


		} else {

			shader.setUniform3f("p2SpellPoint", p3, p3, p3);
			shader.setUniform1f("p2Intensity", 0.0);
			shader.setUniform1i("p2SpellType", 0);
			shader.setUniform1i("p2Impact", 0);
			shader.setUniform1f("p2Health", 0);
		}

		plane.draw();
		shader.end();



		ofPopMatrix();


		if(player1Exists)
		{
			/*ofSetColor(255,0,0);
			ofSetLineWidth(5);
			ofLine(head1,rFoot1);
			ofLine(head1,lFoot1);
			ofCircle(player1->spellPos,10);*/

			ofSetColor(255,255,100);
			ofDrawBitmapString("SKELETON 1 DETECTED", ofGetWidth()*0.25, 32);
			drawHealthBar(player1, 1);

			if (player1->impactCheckCalled){
				//ofDrawBitmapString("P1 IMPACT CHECK CALLED", ofGetWidth()*0.25, 100);
				//cout << "P1 SPELLPOS: " << player1->spellPos.x << endl;
			}

		}

		if(player2Exists)
		{
			/*ofSetColor(0,0,255);
			ofSetLineWidth(5);
			ofLine(head2,rFoot2);
			ofLine(head2,lFoot2);
			ofCircle(player2->spellPos,10);*/
			//ofLine(player->lHand,player->rHand);

			ofSetColor(255,255,100);
			ofDrawBitmapString("SKELETON 2 DETECTED", ofGetWidth()*0.75, 32);

			drawHealthBar(player2, 2);

			if (player2->impactCheckCalled){
				//ofDrawBitmapString("P2 IMPACT CHECK CALLED", ofGetWidth()*0.75, 100);
				//cout << "P2 SPELLPOS: " << player2->spellPos.x << endl;
			}

		}

		float frameRate = ofGetFrameRate();

		string framerateString = "FrameRate: " + ofToString(frameRate);
		gameFont.drawString(framerateString, ofGetWidth()*0.5-(gameFont.stringWidth(framerateString)*0.5), 32);

		//ofDrawBitmapString("FRAMERATE: " + ofToString(frameRate), ofGetWidth()*0.5, 32);


		if(!hasSkeleton) 
		{
			/*ofEnableAlphaBlending();
			gui.draw();*/
		}

	} else {
		ofBackground(0);
		ofSetColor(255,255,0);

		string winString = "Victory is Yours!";
		if (winner == 1){
			float winTextX = ofGetWidth()*0.25-(gameFont.stringWidth(winString)*0.5);
			gameFont.drawString(winString, winTextX, ofGetHeight()*0.5);
			//ofDrawBitmapString("PLAYER 1 WINS!", ofGetWidth()*0.5, ofGetHeight()*0.5);
		} else if (winner == 2){
			float winTextX = ofGetWidth()*0.75-(gameFont.stringWidth(winString)*0.5);
			gameFont.drawString(winString, winTextX, ofGetHeight()*0.5);
			//ofDrawBitmapString("PLAYER 2 WINS!", ofGetWidth()*0.5, ofGetHeight()*0.5);
		}

	}
}

//--------------------------------------------------------------

ofVec3f testApp::getBone(SkeletonBone bone, ofVec3f bodyPart){
	ofVec3f tempBone( bone.getScreenPosition().x, bone.getScreenPosition().y, 0);
	//cout << bodyPart << endl;
	//cout << tempBone.x << ", " << tempBone.y << endl;
	//ofVec3f tempBodyPart = *bodyPart;
	bodyPart = bodyPart.getInterpolated(tempBone, 0.5);
	return bodyPart;
}

//--------------------------------------------------------------

void testApp::drawHealthBar(Player* p, int _pNum){
	int scale = 4;

	float healthBarX = p->startHealth*-0.5;
	healthBarX*= scale;
	if (_pNum == 1){
		healthBarX += ofGetWidth()*0.25;
	} else if (_pNum == 2){
		healthBarX += ofGetWidth()*0.75;
	}

	float healthBarW = (p->health)*scale;

	ofRect(healthBarX,70,healthBarW,20);
}

//--------------------------------------------------------------

void testApp::gameOverCheck(){

	if (player1->health <= 0.0){
		gameOver = true;
		winner = 2;
	} else if (player2->health <= 0.0){
		gameOver = true;
		winner = 1;
	}
}


//--------------------------------------------------------------

void testApp::impactCheck(int _pNum){

	cout << "Checking for impact" << endl;

	float spellX;
	float spellY;
	ofVec3f head;
	ofVec3f leftFoot;
	ofVec3f rightFoot;

	if (_pNum == 1){
		spellX = player1->spellPos.x+player1->vel; //using vel to get NEXT position
		spellY = player1->spellPos.y;
		//CHECK AGAINST OPPOSITE PLAYER
		head = head2;
		leftFoot = lFoot2;
		rightFoot = rFoot2;
	} else if (_pNum == 2) {
		spellX = player2->spellPos.x-player2->vel;
		spellY = player2->spellPos.y;
		//CHECK AGAINST OPPOSITE PLAYER
		head = head1;
		leftFoot = lFoot1;
		rightFoot = rFoot1;
	}

	if (spellY > head.y){
		cout << "SPELL BELOW HEAD" << endl;
		if (spellY < leftFoot.y || spellY < rightFoot.y) {
			cout << "SPELL ABOVE FEET" << endl;
			//if (spellX < rightFoot.x && spellX > leftFoot.x){

			if (_pNum == 1){
				if (spellX > leftFoot.x){
					cout << "IMPACT!!! (P1>P2)" << endl;
					player1->startImpact(&spellX);
					player2->damage(player1->spellIntensity);
				}
			} else if (_pNum == 2){
				if (spellX < rightFoot.x){
					cout << "IMPACT!!! (P2>P1)" << endl;
					player2->startImpact(&spellX);
					player1->damage(player2->spellIntensity);
				}
			}
		}
	}

}

//--------------------------------------------------------------
void testApp::setSpellPosition(ofVec3f *pos){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == 'f'){
		xResolution = 1600;
		yResolution = kResY*xResolution/kResX;
		plane.set(xResolution, yResolution, 4, 4);
		plane.setPosition(0, 0, 0);
		plane.mapTexCoords(0, 0, xResolution, yResolution);
		ofToggleFullscreen();
	}

	if (key == 's'){
		string frameCount = ofToString(20000+ofGetFrameNum());
		string fileName = "Images/" + frameCount + ".jpg";
		ofSaveScreen(fileName);
	}

	if (key == 'r'){
		shader.load("shaders/blobs.vs", "shaders/frag_with_impact.fs");
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}