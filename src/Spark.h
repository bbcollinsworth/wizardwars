#ifndef SPARK_H
#define SPARK_H

#include "ofMain.h"

class Spark
{
    public:
        Spark();

        void setup(ofVec3f *_pos, float *_radFactor);//, ofVec3f *_vel);
        void update();
        void draw();

        float rad, upwardForce, radFactor, alphaFade;

        ofVec3f pos, vel;
        ofFloatColor color;

        virtual ~Spark();
    protected:
    private:
};

#endif // SPARK_H
