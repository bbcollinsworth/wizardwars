#include "testApp.h"

#define USE_PROGRAMMABLE_RENDERER

void testApp::setup(){

	ofSetFrameRate(60);

	//==========================
	//KINECT SETUP + INITIALIZATION
	//==========================

	kinect.initSensor( 0 );

	//kinect.initColorStream(640, 480);
	//kinect.initDepthStream(320, 240, true); //could I just change this to 640,480?
	kinect.initDepthStream(640, 480, true); //could I just change this to 640,480?
	kinect.initSkeletonStream(true);
	kinect.setDepthClipping(1000.0F,1500.0F);

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
	shader.load("shaders/blobs.vs", "shaders/fire_clean.fs");
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


	if(kinect.isNewSkeleton()) {

		//motionEnergy*= 0.95;
	} else {
		motionEnergy = 0;
	}

	if(kinect.isNewSkeleton()) {

		//p1Updated = false;
		//p2Updated = false;

		//if (!player1Exists || !player2Exists){

		for( int i = 0; i < kinect.getSkeletons().size(); i++) 
		{

			/*int i = kinect.getSkeletons().size();
			cout << "skeletons: " << i << endl;
			i -= 1;*/

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
			//}
		}

		int p1index = -1;

		if (player1Exists){

			//could be while !p1updated || !p2Updated
			for( int i = 0; i < kinect.getSkeletons().size(); i++) 
			{

				/*int i = kinect.getSkeletons().size();
				cout << "skeletons: " << i << endl;
				i -= 1;*/

				if(kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD) != kinect.getSkeletons().at(i).end()
					&& kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second.getScreenPosition().x < 320 )
				{

					//if (i=player1->pIndex){
					// just get the first one
					//is z really necessary?

					SkeletonBone headBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second;
					SkeletonBone lHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second;
					SkeletonBone rHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second;
					SkeletonBone lWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_LEFT)->second;
					SkeletonBone rWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_RIGHT)->second;
					SkeletonBone lElbowBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_LEFT)->second;
					SkeletonBone rElbowBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_RIGHT)->second;

					lHand1 = getBone(lHandBone,lHand1);
					rHand1 = getBone(rHandBone,rHand1);
					lWrist1 = getBone(lWristBone,lWrist1);
					rWrist1 = getBone(rWristBone,rWrist1);
					lElbow1 = getBone(lElbowBone,lElbow1);
					rElbow1 = getBone(rElbowBone,rElbow1);

					player1->updateSkeleton(&lHand1,&rHand1,&lWrist1,&rWrist1,&lElbow1,&rElbow1);

					//cout << "Player1 updated - elbow is:" << lElbow1.x << "," << lElbow1.y << endl;
					//p1Updated=true;

					//ofVec3f hb( headBone.getScreenPosition().x, headBone.getScreenPosition().y, 0 );
					//head = head.getInterpolated(hb, 0.5);
					////head.z =  ofInterpolateCosine( head.z, headBone.getStartPosition().x, 0.5) + 0.1;
					//ofVec3f lhb(lHandBone.getScreenPosition().x, lHandBone.getScreenPosition().y, 0);
					//lHand = lHand.getInterpolated( lhb, 0.5);
					////lHand.z = ofInterpolateCosine( lHand.z, lHandBone.getStartPosition().x, 0.5);
					//ofVec3f rhb(rHandBone.getScreenPosition().x, rHandBone.getScreenPosition().y, 0);
					//rHand = rHand.getInterpolated( rhb, 0.5);
					////rHand.z = ofInterpolateCosine( rHand.z, rHandBone.getStartPosition().x, 0.5);
					//ofVec3f lwb(lWristBone.getScreenPosition().x, lWristBone.getScreenPosition().y, 0);
					//lWrist = lWrist.getInterpolated( lwb, 0.5);
					////lWrist.z = ofInterpolateCosine( lWrist.z, lWristBone.getStartPosition().x, 0.5);
					//ofVec3f rwb(rWristBone.getScreenPosition().x, rWristBone.getScreenPosition().y, 0);
					//rWrist = rWrist.getInterpolated( rwb, 0.5);
					////rWrist.z = ofInterpolateCosine( rWrist.z, rWristBone.getStartPosition().x, 0.5);
					//ofVec3f leb(lElbowBone.getScreenPosition().x, lElbowBone.getScreenPosition().y, 0);
					//lElbow = lElbow.getInterpolated( leb, 0.5);
					////lWrist.z = ofInterpolateCosine( lWrist.z, lWristBone.getStartPosition().x, 0.5);
					//ofVec3f reb(rElbowBone.getScreenPosition().x, rElbowBone.getScreenPosition().y, 0);
					//rElbow = rElbow.getInterpolated( reb, 0.5);

					p1index = i;

					//return;

					//break;
					//}
				}
			}
		}

		if (player2Exists){
			for( int i = 0; i < kinect.getSkeletons().size(); i++) 
			{
				//if (i!=player1->pIndex){
				//if (i!=p1index){
					if(kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD) != kinect.getSkeletons().at(i).end()
						&& kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second.getScreenPosition().x >= 320 && i!=p1index)
					{

						//if (i=player2->pIndex){

						SkeletonBone headBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second;
						SkeletonBone lHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second;
						SkeletonBone rHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second;
						SkeletonBone lWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_LEFT)->second;
						SkeletonBone rWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_RIGHT)->second;
						SkeletonBone lElbowBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_LEFT)->second;
						SkeletonBone rElbowBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_ELBOW_RIGHT)->second;

						lHand2 = getBone(lHandBone,lHand2);
						rHand2 = getBone(rHandBone,rHand2);
						lWrist2 = getBone(lWristBone,lWrist2);
						rWrist2 = getBone(rWristBone,rWrist2);
						lElbow2 = getBone(lElbowBone,lElbow2);
						rElbow2 = getBone(rElbowBone,rElbow2);

						player2->updateSkeleton(&lHand2,&rHand2,&lWrist2,&rWrist2,&lElbow2,&rElbow2);
						cout << "Player2 updated - elbow is:" << lElbow2.x << "," << lElbow2.y << endl;
						//p2Updated=true;

						return;
						//break;
						//}
					//}
				}
			}
		}
	}

	ofSoundUpdate();


}

//--------------------------------------------------------------
void testApp::draw(){

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

		if (player1->spellExists){

			//cout << "Shader Knows Player 1 spell called" << endl;
			//if (spellType == "flame"){

			//set and pass a number for each spell type
			shader.setUniform1i("p1SpellType", 1);
		} else {
			shader.setUniform1i("p1SpellType", 0);
		}
		//}

	} else {

		shader.setUniform3f("p1SpellPoint", p3, p3, p3);
		shader.setUniform1f("p1Intensity", 0.0);
		shader.setUniform1i("p1SpellType", 0);
	}

	if (player2Exists)
	{

		shader.setUniform3f("p2SpellPoint", 1.0 + (player2->spellPos.x/-320.0), 1.0 + (player2->spellPos.y/-240.0), 0);

		shader.setUniform1f("p2Intensity", player2->spellIntensity);

		if (player2->spellExists){
			//if (spellType == "flame"){

			//set and pass a number for each spell type
			shader.setUniform1i("p2SpellType", 1);
		} else {
			shader.setUniform1i("p2SpellType", 0);
		}
		//}

	} else {

		shader.setUniform3f("p2SpellPoint", p3, p3, p3);
		shader.setUniform1f("p2Intensity", 0.0);
		shader.setUniform1i("p2SpellType", 0);
	}

	plane.draw();
	shader.end();



	ofPopMatrix();

	//ofPushStyle();
	//for (int i=0; i<flames.size(); i++){
	//flames[i]->draw();
	//}
	//ofPopStyle();

	if(player1Exists)
	{
		ofSetColor(255,0,0);
		ofSetLineWidth(5);
		//ofLine(player->lHand,player->rHand);

		ofSetColor(255,255,100);
		ofDrawBitmapString("SKELETON 1 DETECTED", ofGetWidth()*0.25, 32);

	}

	if(player2Exists)
	{
		ofSetColor(255,0,0);
		ofSetLineWidth(5);
		//ofLine(player->lHand,player->rHand);

		ofSetColor(255,255,100);
		ofDrawBitmapString("SKELETON 2 DETECTED", ofGetWidth()*0.75, 32);

	}


	if(!hasSkeleton) 
	{
		/*ofEnableAlphaBlending();
		gui.draw();*/
	}
}

//--------------------------------------------------------------

ofVec3f testApp::getBone(SkeletonBone bone, ofVec3f bodyPart){
	ofVec3f tempBone( bone.getScreenPosition().x, bone.getScreenPosition().y, 0);
	bodyPart = bodyPart.getInterpolated(tempBone, 0.5);
	return bodyPart;
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