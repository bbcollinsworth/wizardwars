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
	flameCalled = false;
	flameExists = false;

	spellFired = false;

	//=========================

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

	motionEnergy = 0;

	if(kinect.isNewSkeleton()) {
		for( int i = 0; i < kinect.getSkeletons().size(); i++) 
		{

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

				//=====SET A TIME GATE ON NEW FLAME???====//
				if (spellFired){
					spellPos.x *= 1.2;
				} else {
					spellPos = ofVec3f(lHand + (rHand-lHand)*0.5);
					cout << rHand.x << endl;
				}

				if (spellPos.x > 640) {
					flameExists = false;
					spellFired = false;
					motion.clear();
				}


				//---Put these in sep. function with pointer to hand positions?)
				if (rHand.x - lHand.x < -20 && abs(rHand.y-lHand.y) < 20 && !flameCalled) { //&& !flameExists 
					flameCalled = true;
				}

				if (flameCalled && !flameExists && rHand.x - lHand.x > 5) {
					flameExists = true;
					flameCalled = false;
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

				float lDistMoved = lHand.squareDistance(prevLHand);
				float rDistMoved = rHand.squareDistance(prevRHand);
				float totalMoved = rDistMoved*0.1 + lDistMoved*0.1;
				totalMoved = ofClamp(totalMoved,0,50);

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


				/*for (int i = 0; i < motion.size(); i++){
				motionEnergy += motion[i];
				}*/

				for (auto & move : motion) {
					motionEnergy += move;
				}

				//cout << motionEnergy << endl;

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

				//------NEED TO REACTIVATE HAND SPACING FOR THIS??
				float handSpacing = lWrist.distance(rWrist);

				//cout << "Hand Spacing: " << handSpacing << endl;
				//cout << "Max Spacing: " << plane.getHeight()*0.1 << endl;
				//if (handSpacing < plane.getHeight()*0.1 && lHandAdj.x > plane.getWidth()*0.7){

				//if (handSpacing < plane.getHeight()*0.1 && lHandAdj.x > lWristAdj.x && rHandAdj.x > rWristAdj.x){
				
				if (handSpacing < plane.getHeight()*0.1 && lHand.x > lWrist.x && rHand.x > rWrist.x && abs(lWrist.y - rWrist.y) < 50 && flameExists){
					spellFired = true;
					//for (int i=0; i<flames.size(); i++){
					//	flames[i]->launch();
					//}
				}

				prevLHand = lHand;
				prevRHand = rHand;


				return;
			}
		}
	}


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
		if (flameExists){
			shader.setUniform1i("flame", 1);
		}else {
			shader.setUniform1i("flame", 0);
		}
		float flameIntense = ofMap(motionEnergy,0,6000,0.0,1.0,true);

		shader.setUniform1f("intensity", flameIntense);

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