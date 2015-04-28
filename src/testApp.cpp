#include "testApp.h"

#define USE_PROGRAMMABLE_RENDERER

void testApp::setup(){

	//ofSetFrameRate(30);

	//==========================
	//KINECT SETUP + INITIALIZATION
	//==========================

	kinect.initSensor( 0 );

	//kinect.initColorStream(640, 480);
	//kinect.initDepthStream(320, 240, true); //could I just change this to 640,480?
	kinect.initDepthStream(640, 480, true); //could I just change this to 640,480?
	kinect.initSkeletonStream(false);
	kinect.setDepthClipping(1500.0F,2300.0F);

	kinect.start();


	//==========================
	//GL + SHADER SETUP
	//==========================

	//xResolution = 1024;
	//yResolution = 768;
	xResolution = 1500;
	yResolution = 480*xResolution/640;
	//xResolution = 1600;
	//yResolution = 480*xResolution/640;

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


	//==========================
	//BOOLEAN INITIALIZATIONS
	//==========================

	hasSkeleton = false;

	gameOver = false;
	winner = 0;

	/*firstPress = false;
	spellCalled = false;
	spellExists = false;
	newFireCanBeCalled = true;

	spellFired = false;
	spellCanBeFired = false;*/

	playSound = false;

	player1Exists = false;
	player2Exists = false;
	p1Updated = false;
	p2Updated = false;

	//=========================
	//SOUND SETUP
	//==========================

	/*fWhoosh.loadSound("sounds/fire_whoosh.wav");
	fWhoosh.setVolume(0.75);
	fWhoosh.setMultiPlay(true);

	fCrackle.loadSound("sounds/fire_crackle.wav");
	fCrackle.setVolume(0.2);
	fCrackle.setMultiPlay(true);*/

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


	gui.setup();
	gui.add(p1.setup("head", 0.0, -1.0, 1.0));
	gui.add(p2.setup("leftHand", 0.0, -1.0, 1.0));
	gui.add(p3.setup("rightHand", 0.0, -1.0, 1.0));
	gui.add(p4.setup("blobDensity", 0.8, 0.0, 2.0));
	gui.add(p5.setup("frequency", 0.18, 0.0, 2.0));
	gui.add(p6.setup("scalar", 2.5, -1.0, 3.0));

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
						player1->setup(1,i);
						player1Exists = true;
						cout << "PLAYER 1 set up at Index: " << i << endl; 
					}

					if (!player2Exists && player1Exists){
						if (i != player1->pIndex){
							player2 = new Player();
							player2->setup(2,i);
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

					SkeletonBone headBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second;
					SkeletonBone lHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second;
					SkeletonBone rHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second;
					SkeletonBone lWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_LEFT)->second;
					SkeletonBone rWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_RIGHT)->second;
					SkeletonBone lElbowBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_LEFT)->second;
					SkeletonBone rElbowBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_RIGHT)->second;
					SkeletonBone lFootBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_FOOT_LEFT)->second;
					SkeletonBone rFootBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_FOOT_RIGHT)->second;

					head1 = getBone(headBone,head1);
					lHand1 = getBone(lHandBone,lHand1);
					rHand1 = getBone(rHandBone,rHand1);
					lWrist1 = getBone(lWristBone,lWrist1);
					rWrist1 = getBone(rWristBone,rWrist1);
					lElbow1 = getBone(lElbowBone,lElbow1);
					rElbow1 = getBone(rElbowBone,rElbow1);
					lFoot1 = getBone(lFootBone,lFoot1);
					rFoot1 = getBone(rFootBone,rFoot1);

					cout << "p1 Left Foot: " << lFoot1 << endl;

					player1->updateSkeleton(&lHand1,&rHand1,&lWrist1,&rWrist1,&lElbow1,&rElbow1);		

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


					SkeletonBone headBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second;
					SkeletonBone lHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second;
					SkeletonBone rHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second;
					SkeletonBone lWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_LEFT)->second;
					SkeletonBone rWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_RIGHT)->second;
					SkeletonBone lElbowBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_LEFT)->second;
					SkeletonBone rElbowBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_RIGHT)->second;
					SkeletonBone lFootBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_FOOT_LEFT)->second;
					SkeletonBone rFootBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_FOOT_RIGHT)->second;

					head2 = getBone(headBone,head2);
					lHand2 = getBone(lHandBone,lHand2);
					rHand2 = getBone(rHandBone,rHand2);
					lWrist2 = getBone(lWristBone,lWrist2);
					rWrist2 = getBone(rWristBone,rWrist2);
					lElbow2 = getBone(lElbowBone,lElbow2);
					rElbow2 = getBone(rElbowBone,rElbow2);
					lFoot2 = getBone(lFootBone,lFoot2);
					rFoot2 = getBone(rFootBone,rFoot2);

					cout << "p2 Left Foot: " << lFoot2 << endl;

					player2->updateSkeleton(&lHand2,&rHand2,&lWrist2,&rWrist2,&lElbow2,&rElbow2);
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
		//ofTranslate(512, 768/2);
		ofTranslate(xResolution*0.5, yResolution*0.5);

		shader.begin();
		shader.setUniform1f("time", ofGetElapsedTimef());///2.0);
		shader.setUniform2f("resolution", xResolution, yResolution);
		//shader.setUniformTexture("videoTex", kinect.getColorTexture(), 0);
		shader.setUniformTexture("videoTex", kinect.getDepthTexture(), 0);

		//if(hasSkeleton)
		if (player1Exists)
		{

			shader.setUniform3f("p1SpellPoint", 1.0 + (player1->spellPos.x/-320.0), 1.0 + (player1->spellPos.y/-240.0), 0);
			shader.setUniform1f("p1Intensity", player1->spellIntensity);

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
		}

		if (player2Exists)
		{

			shader.setUniform3f("p2SpellPoint", 1.0 + (player2->spellPos.x/-320.0), 1.0 + (player2->spellPos.y/-240.0), 0);
			shader.setUniform1f("p2Intensity", player2->spellIntensity);

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
			//float healthBarX = ofGetWidth()*0.25-45;
			//float healthBarW = (player1->health)*3;
			//ofRect(healthBarX,70,healthBarW,20);
			//ofDrawBitmapString("P1 HEALTH: " + ofToString(player1->health), ofGetWidth()*0.25, 70);

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

			//float healthBarX = ofGetWidth()*0.75-45;
			//float healthBarW = (player2->health)*3;
			//ofRect(healthBarX,70,healthBarW,20);
			//ofDrawBitmapString("P2 HEALTH: " + ofToString(player2->health), ofGetWidth()*0.75, 70);

			if (player2->impactCheckCalled){
				//ofDrawBitmapString("P2 IMPACT CHECK CALLED", ofGetWidth()*0.75, 100);
				//cout << "P2 SPELLPOS: " << player2->spellPos.x << endl;
			}

		}

		float frameRate = ofGetFrameRate();

		ofDrawBitmapString("FRAMERATE: " + ofToString(frameRate), ofGetWidth()*0.5, 32);


		if(!hasSkeleton) 
		{
			/*ofEnableAlphaBlending();
			gui.draw();*/
		}

	} else {
		ofBackground(0);
		ofSetColor(255,255,0);
		if (winner == 1){
			ofDrawBitmapString("PLAYER 1 WINS!", ofGetWidth()*0.5, ofGetHeight()*0.5);
		} else if (winner == 2){
			ofDrawBitmapString("PLAYER 2 WINS!", ofGetWidth()*0.5, ofGetHeight()*0.5);
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
		yResolution = 480*xResolution/640;
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