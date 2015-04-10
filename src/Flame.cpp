#include "Flame.h"

Flame::Flame()
{
	//ctor
}

void Flame::setup(ofVec3f *startPos){
	pos = *startPos;
	throwForce = ofVec3f(0,0);
	energy = 1;
	flameLaunched = false;
}

void Flame::update(){

	if (flameLaunched){
		pos.x += 20.0f;
	}
	//pos += throwForce;

	if (sparks.size() < 500){

		if (energy < 5){
			newSparks = floor(energy)+1;
		} else {
			newSparks = 5;
		}

		for (int i = 0; i < energy; i++){
			Spark *s = new Spark();

			float sizeMultiplier = energy*0.2+5;
			s->setup(&pos, &sizeMultiplier);
			sparks.push_back(s);
		}
	}

	for (int i=0; i<sparks.size(); i++){
		sparks[i]->update();
		if (sparks[i]->pos.y < 0 || sparks[i]->color.a <= 0){
			delete sparks[i];
			sparks.erase(sparks.begin()+i);
			cout << sparks.size() << endl;
		}
	}


}

void Flame::draw(){
	for (int i=0; i<sparks.size(); i++){
		sparks[i]->draw();
	}
}

void Flame::flameMoved(ofVec3f *flamePos){
	if (!flameLaunched){
		pos = *flamePos;
	}
}

void Flame::updateThrowForce(ofVec3f *force){
	throwForce = *force;
}

void Flame::intensify(float *_energy){
	energy = *_energy + 1;
	energy = energy*0.0005; //energy is now a squareLength value
	/*
	for (int i = 0; i < energy; i++){
	Spark *s = new Spark();
	s->setup(&pos);
	sparks.push_back(s);
	}
	*/
}

void Flame::launch(){

	flameLaunched = true;
}

void Flame::deleteSparks(){
	for (auto & spark : sparks){
		delete spark;
	}
	sparks.clear();
}

Flame::~Flame()
{
	//dtor
}
