#include "Spark.h"

Spark::Spark()
{
    //ctor
}

void Spark::setup(ofVec3f *_pos, float *_radFactor){ //, ofVec3f *_vel){
    radFactor = *_radFactor;
    pos = *_pos;
    pos.x += ofRandomf()*radFactor*2;
    pos.y += ofRandomf()*radFactor*2;
    //vel = *_vel;
    vel = ofVec3f(ofRandomf(),ofRandomf());
    upwardForce = ofRandomuf()*0.5*-1;


    rad = ofRandomuf()*radFactor;

	alphaFade = ofRandomuf()*0.02;

    color.r = 0.9F + ofRandomuf()*0.1F;
    color.g = 0.8F + ofRandomuf()*0.2F;
    color.b = 0.7F + ofRandomuf()*0.1F;
    color.a = 0.9F + ofRandomuf()*0.1F;
    //color = ofFloatColor(r,g,b);
}

void Spark::update(){

    vel.y += upwardForce;
    pos += vel;

    //float gmult = ofMap();
    //float alphaFade = 0.01;
    color.r -= alphaFade;
    color.g *= 0.95;
    color.b *= 0.9;
    color.a -= alphaFade;


}

void Spark::draw(){
    ofSetColor(color);
    ofCircle(pos,rad);
}

Spark::~Spark()
{
    //dtor
}
