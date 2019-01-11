#ifndef EVOLUTION_HPP
#define EVOLUTION_HPP
#include "Brain.hpp"

class Evolution {
    private:
        int population;
        int genomeSize;
        Brain **brains;
    public:
        Evolution(int population, int inputs, int outputs, int hiddenLayers, int neuronsPerLayer);
        ~Evolution();
        float evolve(int podio, float sumFactor, float mulFactor, float replaceFactor, float sumsize, float mulsize);
        Brain *getBrain(int index);
        int getBestBrainIndex();
        void setResult(int index, float result);
        float *results;
};

#endif