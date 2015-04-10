#ifndef FLAME_H
#define FLAME_H

#include "ofMain.h"
#include "Spark.h"

class Flame

{
    public:
        Flame();

        void setup(ofVec3f *startPos);
        void update();
        void draw();
        void flameMoved(ofVec3f *flamePos);
        void updateThrowForce(ofVec3f *force);
        void intensify(float *_energy);
		void launch();
		void deleteSparks();

		bool flameLaunched;
        float energy;
		int newSparks;


        ofVec3f pos, throwForce;

        vector <Spark *> sparks;

        virtual ~Flame();
    protected:
    private:
};

#endif // FLAME_H
