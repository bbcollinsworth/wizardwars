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
	kinect.setDepthClipping(600.0F,1200.0F);

	kinect.start();


	//==========================
	//GL + SHADER SETUP
	//==========================

	xResolution = 1024;
	yResolution = 768;
	//xResolution = 1280;
	//yResolution = 720;

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

	firstPress = false;
	spellCalled = false;
	spellExists = false;
	newFireCanBeCalled = true;

	spellFired = false;

	playSound = false;

	//=========================
	//SOUND SETUP
	//==========================

	fWhoosh.loadSound("sounds/fire_whoosh.wav");
	fWhoosh.setVolume(0.75);
	fWhoosh.setMultiPlay(true);

	fCrackle.loadSound("sounds/fire_crackle.wav");
	fCrackle.setVolume(0.2);
	fCrackle.setMultiPlay(true);

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



	/*for (int i=0; i<flames.size(); i++){
	if (flames[i]->pos.x > plane.getWidth()){
	flames[i]->deleteSparks();
	delete flames[i];
	flames.erase(flames.begin()+i);

	}
	}*/

	//motionEnergy = 0;




	//should access PLAYER 1 [ kinect.getSkeletons().size() - 1 ]
	// PLAYER 2 [ kinect.getSkeletons().size() - 2 ]
	if(kinect.isNewSkeleton()) {

		motionEnergy*= 0.99;
	} else {
		motionEnergy = 0;
	}

	if(kinect.isNewSkeleton()) {



		for( int i = 0; i < kinect.getSkeletons().size(); i++) 
		{

			/*int i = kinect.getSkeletons().size();
			cout << "skeletons: " << i << endl;
			i -= 1;*/

			//if(kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD) != kinect.getSkeletons().at(i).end())
			if(kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD) != kinect.getSkeletons().at(i).end())

			{

				// just get the first one

				//is z really necessary?
				SkeletonBone headBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second;
				SkeletonBone lHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second;
				SkeletonBone rHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second;
				SkeletonBone lWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_LEFT)->second;
				SkeletonBone rWristBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_WRIST_RIGHT)->second;
				ofVec3f hb( headBone.getScreenPosition().x, headBone.getScreenPosition().y, 0 );
				head = head.getInterpolated(hb, 0.5);
				head.z =  ofInterpolateCosine( head.z, headBone.getStartPosition().x, 0.5) + 0.1;
				ofVec3f lhb(lHandBone.getScreenPosition().x, lHandBone.getScreenPosition().y, 0);
				lHand = lHand.getInterpolated( lhb, 0.5);
				lHand.z = ofInterpolateCosine( lHand.z, lHandBone.getStartPosition().x, 0.5);
				ofVec3f rhb(rHandBone.getScreenPosition().x, rHandBone.getScreenPosition().y, 0);
				rHand = rHand.getInterpolated( rhb, 0.5);
				rHand.z = ofInterpolateCosine( rHand.z, rHandBone.getStartPosition().x, 0.5);
				ofVec3f lwb(lWristBone.getScreenPosition().x, lWristBone.getScreenPosition().y, 0);
				lWrist = lWrist.getInterpolated( lwb, 0.5);
				lWrist.z = ofInterpolateCosine( lWrist.z, lWristBone.getStartPosition().x, 0.5);
				ofVec3f rwb(rWristBone.getScreenPosition().x, rWristBone.getScreenPosition().y, 0);
				rWrist = rWrist.getInterpolated( rwb, 0.5);
				rWrist.z = ofInterpolateCosine( rWrist.z, rWristBone.getStartPosition().x, 0.5);

				/*cout << headBone.getScreenPosition()  << endl;
				cout << rHandBone.getScreenPosition() << endl;
				cout << lHandBone.getScreenPosition() << endl;*/

				//lHandPos = lHandBone.getScreenPosition();
				//rHandPos = rHandBone.getScreenPosition();

				//
				/*lHandAdj = ofVec3f(lHand.x*plane.getWidth()/640,lHand.y*plane.getHeight()/480);
				rHandAdj = ofVec3f(rHand.x*plane.getWidth()/640,rHand.y*plane.getHeight()/480);
				lWristAdj = ofVec3f(lWrist.x*plane.getWidth()/640,lWrist.y*plane.getHeight()/480);
				cout << "Left Wrist: " << lWristAdj << endl;
				rWristAdj = ofVec3f(rWrist.x*plane.getWidth()/640,rWrist.y*plane.getHeight()/480);
				cout << "Right Wrist: " << rWristAdj << endl;*/


				//----SPELL POSITION HERE:-----

				//ofVec3f spellPos = ofVec3f(lHandAdj + (rHandAdj-lHandAdj)*0.5);

				//for feeding to shader:


				float lDistMoved = lHand.squareDistance(prevLHand);
				float rDistMoved = rHand.squareDistance(prevRHand);
				float totalMoved = rDistMoved*0.1 + lDistMoved*0.1;
				totalMoved = ofClamp(totalMoved,0,50);


				//----ALTERNATIVE MOTION ENERGY APPROACH
				if (lDistMoved > 100 || rDistMoved > 100){
					motionEnergy += totalMoved;
				}

				if (lDistMoved > 100 || rDistMoved > 100){
					motion.push_back(totalMoved);
				}

				if (motion.size() > 120) {
					//delete motion[motion.size()-1];
					motion.erase(motion.begin());
				} 
				else if (motion.size() > 1 && totalMoved < 10) {
					motion.erase(motion.begin());
				}

				spellIntensity = ofMap(motionEnergy,0,6000,0.0,1.0,true);



				//for (auto & move : motion) {
				//	motionEnergy += move;
				//}

				cout << "Motion Energy: " << motionEnergy << endl;

				//=====SET A TIME GATE ON NEW FLAME???====//

				//------NEED TO REACTIVATE HAND SPACING FOR THIS??
				float handSpacing = lWrist.distance(rWrist);

				//cout << "Hand Spacing: " << handSpacing << endl;
				//cout << "Max Spacing: " << plane.getHeight()*0.1 << endl;
				//if (handSpacing < plane.getHeight()*0.1 && lHandAdj.x > plane.getWidth()*0.7){

				//if (handSpacing < plane.getHeight()*0.1 && lHandAdj.x > lWristAdj.x && rHandAdj.x > rWristAdj.x){

				if (lHand.x < rHand.x && !spellExists && !spellCalled) {

					newFireCanBeCalled = true;
				}


				if (handSpacing < plane.getHeight()*0.1 && lHand.x > lWrist.x && rHand.x > rWrist.x && abs(lWrist.y - rWrist.y) < 50 && spellExists){

					//if (ofGetElapsedTimeMillis() > spellCreateTime + 
					spellFired = true;
					fCrackle.stop();
					playSound = true;
					//playSound(fire);

					//for (int i=0; i<flames.size(); i++){
					//	flames[i]->launch();
					//}
				}

				if (playSound){
					fWhoosh.setVolume(ofMap(spellIntensity,0.0,1.0,0.5,1.0));
					fWhoosh.setSpeed(ofMap(spellIntensity,0.0,1.0,1.0,0.5));
					fWhoosh.play();
					playSound = false;
				}



				if (spellPos.x > 640) {
					spellExists = false;
					spellFired = false;
					motion.clear();
				}



				//---Put these in sep. function with pointer to hand positions?)
				cout << "Angle between hands: " << ofVec3f(1,0,0).angle(lHand - rHand) << " DEGREES" << endl;
				if (rHand.x - lHand.x < 0 && prevRHand.x - prevLHand.x > 0 && abs(rHand.y-lHand.y) < 50 && !spellCalled && !spellExists && newFireCanBeCalled) { //&& !flameExists  //abs(rHand.y-lHand.y)
					spellFired = false;
					spellCalled = true;
					//spellCreateDelay = ofGetElapsedTimef() +0.3;
					motionEnergy = 0;

				}

				if (spellCalled && !spellExists){
					spellFired = false;
					if (rHand.x - lHand.x > 5) {
						spellExists = true;
						fWhoosh.play();
						fCrackle.play();
						//fWhoosh.setSpeed( 0.1f );
						//fWhoosh.setPan(ofMap(x, 0, widthStep, -1, 1, true));
						spellCalled = false;

					}
				}

				if (fCrackle.getIsPlaying()){
					fCrackle.setVolume(ofMap(spellIntensity,0.0,1.0,0.2,1.0));
					fCrackle.setSpeed(ofMap(spellIntensity,0.0,1.0,1.0,0.5));
				}

				if (spellExists){
					newFireCanBeCalled = false;
				}

				//cout << flamePos << endl;

				//if (flames.size() < 1){
				//	Flame *f = new Flame();

				//	//ofVec3f startPos = ofVec3f(lHand.x,lHand.y);
				//	f->setup(&spellPos);
				//	flames.push_back(f);
				//}

				//cout << kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second.getScreenPosition() << endl;
				//cout << kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second.getScreenPosition() << endl;
				//cout << kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second.getScreenPosition() << endl;

				/*jointDistance = head.distance(rHand);
				jointDistance += lHand.distance(rHand);
				jointDistance += lHand.distance(head);*/

				hasSkeleton = true;

				

				for (int i=0; i<flames.size(); i++){
					/*ofVec3f clearForce = ofVec3f(0,0);
					flames[i]->updateThrowForce(&clearForce);
					motionEnergy = motion.size();*/
					//flames[i]->intensify(&motionEnergy);
				}


				for (int i=0; i<flames.size(); i++){
					//ofVec3f movedPos = ofVec3f(lHand.x,lHand.y);
					//flames[i]->flameMoved(&spellPos);
					//flames[i]->update();
				}

				//ofVec3f wristSpacingVec = lHandAdj-rHandAdj;
				//float handSpacing = handSpacingVec.length();

				if (spellExists){
					if (spellFired){
						spellPos.x *= 1.2;
					} else {
						spellPos = ofVec3f(lHand + (rHand-lHand)*0.5);
						//cout << rHand.x << endl;
					}
				}

				prevLHand = lHand;
				prevRHand = rHand;


				return;
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
	shader.setUniformTexture("videoTex", kinect.getColorTexture(), 0);
	//shader.setUniformTexture("videoTex", kinect.getDepthTexture(), 0);

	if(hasSkeleton)
	{

		// something in here blows up in release mode
		//shader.setUniform3f("headPoint", 1.0 + (head.x/-320.0), 1.0 + (head.y/-240.0) - 0.1,head.z);
		//shader.setUniform3f("lHandPoint", 1.0 + (lHand.x/-320.0), 1.0 + (lHand.y/-240.0), 0);
		//shader.setUniform3f("lHandPoint", 1.0 + (lHand.x/-320.0), 1.0 + (lHand.y/-240.0), 0);
		//shader.setUniform3f("rHandPoint", 1.0 + (rHand.x/-320.0), 1.0 + (rHand.y/-240.0), 0);

		//shader.setUniform3f("lHandPoint", 1.0 + (lHand.x/-320.0), 1.0 + (lHand.y/-240.0), 0);
		//cout << 1.0 + (lHand.x/-320.0) << " , " << 1.0 + (lHand.y/-240.0) << endl;

		//shader.setUniform3f("rHandPoint", 1.0 + (rHand.x/-320.0), 1.0 + (rHand.y/-240.0), 0);

		shader.setUniform3f("spellPoint", 1.0 + (spellPos.x/-320.0), 1.0 + (spellPos.y/-240.0), 0);

		//shader.setUniform1f("frequency", p4);
		//shader.setUniform1f("scalar", p5);
		//shader.setUniform1f("blobDensity", jointDistance/600.0);
		//shader.setUniform1f("blobDensity", 2.0);

		//float flameIntense = ofMap(motionEnergy,0,10000,0.5,0.01,true);
		if (spellExists){
			//if (spellType == "flame"){
			shader.setUniform1i("flame", 1);
		}else {
			shader.setUniform1i("flame", 0);
		}
		//}


		shader.setUniform1f("intensity", spellIntensity);

	} else {

		//shader.setUniform3f("headPoint", p1, p1, p1);
		//shader.setUniform3f("lHandPoint", p2, p2, p2);
		//shader.setUniform3f("rHandPoint", p3, p3, p3);
		shader.setUniform3f("spellPoint", p3, p3, p3);
		/*shader.setUniform1f("frequency", p4);
		shader.setUniform1f("scalar", p5);
		shader.setUniform1f("blobDensity", p6);*/
		shader.setUniform1f("intensity", 0.0);
	}

	plane.draw();
	shader.end();



	ofPopMatrix();

	ofPushStyle();
	for (int i=0; i<flames.size(); i++){
		//flames[i]->draw();
	}
	ofPopStyle();

	if(hasSkeleton)
	{
		ofSetColor(255,0,0);
		ofSetLineWidth(5);
		ofLine(lHand,rHand);

		ofSetColor(255,255,100);
		ofDrawBitmapString("SKELETON DETECTED", ofGetWidth()*0.5, 32);

	}


	if(!hasSkeleton) 
	{
		ofEnableAlphaBlending();
		gui.draw();
	}
}

//--------------------------------------------------------------
void testApp::setSpellPosition(ofVec3f *pos){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == 'f'){
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