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
	kinect.setDepthClipping(1300.0F,2100.0F);

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


	if(kinect.isNewSkeleton()) {

		//motionEnergy*= 0.95;
	} else {
		motionEnergy = 0;
	}

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

		if (player1Exists){

			/*if (player1->impactCheckCalled){
			impactCheck(1);
			}*/

			//could be while !p1updated || !p2Updated
			for( int i = 0; i < kinect.getSkeletons().size(); i++) 
			{

				/*int i = kinect.getSkeletons().size();
				cout << "skeletons: " << i << endl;
				i -= 1;*/

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

					//cout << "p1 Left Foot: " << lFoot1 << endl;

					player1->updateSkeleton(&head1,&lHand1,&rHand1,&lWrist1,&rWrist1,&lElbow1,&rElbow1,&lFoot1,&rFoot1);		

					p1index = i;

				}
			}

		}

		if (player2Exists){

			/*if (player2->impactCheckCalled){
			impactCheck(2);
			}*/

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

					//cout << "p2 Left Foot: " << lFoot2 << endl;

					player2->updateSkeleton(&head2,&lHand2,&rHand2,&lWrist2,&rWrist2,&lElbow2,&rElbow2,&lFoot2,&rFoot2);
					//cout << "Player2 updated - elbow is:" << lElbow2.x << "," << lElbow2.y << endl;
					//p2Updated=true;

					return;

				}
			}
		}
	}



	//if (player2Exists && player2->impactCheckCalled && player2->spellPos.x < 320){
	//if (player2Exists && player2->impactCheckCalled && player2->spellPos.x < 320){	
	//	impactCheck(2);
	//}


	ofSoundUpdate();


}

//--------------------------------------------------------------
void testApp::draw(){

	if (player1Exists && player1->impactCheckCalled){// && player1->spellPos.x > 320){
		impactCheck(1);
		ofDrawBitmapString("P1 CHECKING IMPACT", ofGetWidth()*0.25, 100);
	}

	if (player2Exists && player2->impactCheckCalled){// && player1->spellPos.x > 320){
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

		if (player1->spellExists && player1->impact == false){

			//cout << "Shader Knows Player 1 spell called" << endl;
			//if (spellType == "flame"){

			//set and pass a number for each spell type
			shader.setUniform1i("p1SpellType", 1);
		} else {
			shader.setUniform1i("p1SpellType", 0);
		}

		if (player1->impact){
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

		if (player2->spellExists && player2->impact == false){
			//if (spellType == "flame"){
			//set and pass a number for each spell type
			shader.setUniform1i("p2SpellType", 1);
		} else {
			shader.setUniform1i("p2SpellType", 0);
		}

		if (player2->impact){
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
		ofSetColor(255,0,0);
		ofSetLineWidth(5);
		ofLine(head1,rFoot1);
		ofLine(head1,lFoot1);
		ofCircle(player1->spellPos,10);

		ofSetColor(255,255,100);
		ofDrawBitmapString("SKELETON 1 DETECTED", ofGetWidth()*0.25, 32);

		if (player1->impactCheckCalled){
			//ofDrawBitmapString("P1 IMPACT CHECK CALLED", ofGetWidth()*0.25, 100);
			//cout << "P1 SPELLPOS: " << player1->spellPos.x << endl;
		}

	}

	if(player2Exists)
	{
		ofSetColor(0,0,255);
		ofSetLineWidth(5);
		ofLine(head2,rFoot2);
		ofLine(head2,lFoot2);
		ofCircle(player2->spellPos,10);
		//ofLine(player->lHand,player->rHand);

		ofSetColor(255,255,100);
		ofDrawBitmapString("SKELETON 2 DETECTED", ofGetWidth()*0.75, 32);

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
}

//--------------------------------------------------------------

ofVec3f testApp::getBone(SkeletonBone bone, ofVec3f bodyPart){
	ofVec3f tempBone( bone.getScreenPosition().x, bone.getScreenPosition().y, 0);
	//cout << bodyPart << endl;
	//cout << tempBone.x << ", " << tempBone.y << endl;
	bodyPart = bodyPart.getInterpolated(tempBone, 0.5);
	return bodyPart;
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
		spellX = player1->spellPos.x;
		spellY = player1->spellPos.y;
		//CHECK AGAINST OPPOSITE PLAYER
		head = head2;
		leftFoot = lFoot2;
		rightFoot = rFoot2;
	} else if (_pNum == 2) {
		spellX = player2->spellPos.x;
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
					player1->startImpact();
				}
			} else if (_pNum == 2){
				if (spellX < rightFoot.x){
					cout << "IMPACT!!! (P2>P1)" << endl;
					player2->startImpact();
				}

			}
		}
	}

	//	cout << "Head: x - " << head.x << ", y - " << head.y << endl;
	//cout << "L Foot: x - " << leftFoot.x << ", y - " << leftFoot.y << endl;


	//if(spellX>640*0.8){
	//	player1->startImpact();
	//}
	//THIS IS A REALLY ROUGH IMPACT CHECK BUT SHOULD WORK FOR NOW...
	//if (player1->spellPos.y > head2.y){
	//	cout << "SPELL BELOW HEAD" << endl;
	//	if (player1->spellPos.y < lFoot2.y || player1->spellPos.y < rFoot2.y) {
	//		cout << "SPELL ABOVE FEET" << endl;
	//		//if (spellX < rightFoot.x && spellX > leftFoot.x){

	//		//if (_pNum == 1){
	//		if (player1->spellPos.x > lFoot2.x){
	//			cout << "IMPACT!!! (P1>P2)" << endl;
	//			player1->startImpact();
	//		}
	//		//} else if (_pNum == 2){
	//		//if (spellX < rightFoot.x){
	//		//cout << "IMPACT!!! (P2>P1)" << endl;
	//		//player2->startImpact();
	//		//}

	//	}
	//}
	////}



	//ofPixels d = kinect.getDepthPixelsRef();
	//int pixelIndex;
	//if (_pNum == 1){
	//	pixelIndex = player1->spellPos.x + (player1->spellPos.y*640);
	//	cout << "p1 pixelIndex" << pixelIndex << endl;
	//} else if (_pNum == 2) {
	//	pixelIndex = player2->spellPos.x + (player2->spellPos.y*640);
	//	cout << "p2 pixelIndex" << pixelIndex << endl;
	//}
	//pixelIndex *= 3;
	//int pixelColor = (int)d[pixelIndex];
	//cout << "First Px color: " << pixelColor << endl;
	////int summedPix = 0;

	//if (pixelColor > 0){
	//	int nextPix = 0;
	//	int nextNextPix = 0;
	//	if (_pNum == 1){
	//		nextPix = (int)d[pixelIndex + 15];
	//		nextNextPix = (int)d[pixelIndex + 30];
	//		cout << "p1 NextPx: " << nextPix << ", NextNextPx: " << nextNextPix << endl;
	//	}
	//	if (_pNum == 2){
	//		nextPix = (int)d[pixelIndex - 15];
	//		nextNextPix = (int)d[pixelIndex - 30];
	//		cout << "p2 NextPx: " << nextPix << ", NextNextPx: " << nextNextPix << endl;
	//	}


	//	//vector <int> nextPixels;
	//	//for (int i=3; i<=30; i+=3){
	//	//	//NOTE: WILL GOING OFF EDGE OF SCREEN & LOOPING EVER BE ISSUE?
	//	//	if (_pNum == 1){
	//	//		nextPixels.push_back((int)d[pixelIndex + i]);
	//	//	}
	//	//	if (_pNum == 2){
	//	//		nextPixels.push_back((int)d[pixelIndex - i]);
	//	//	}
	//	//}

	//	//for (auto & px : nextPixels){
	//	//	summedPix += px;	
	//	//	cout << summedPix << endl;
	//	//}


	//	//if (summedPix/26 > 10){
	//	if (nextPix > 5 && nextNextPix > 5){
	//		if (_pNum == 1){
	//			player1->startImpact();
	//		}
	//		if (_pNum == 2){
	//			player2->startImpact();
	//		}
	//	}



	//}

	//int next

	//if (_pNum == 1){
	//player1->
	//}
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