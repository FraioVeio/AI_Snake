#ifndef BRAIN_HPP
#define BRAIN_HPP


typedef struct genome_s {
    int size;
    double *genome;
} genome_t;

class Brain {
    private:
        int inputSize, outputSize;
        int hiddenLayers;
        int neuronsPerLayer;
        double *inputWeights;   //Array dimensioni inputSize x neuronsPerLayer
        double *outputWeights;  //Array dimensioni neuronsPerLayer x outputSize
        double *layerWeights;   //Array dimensioni hiddenLayers-1 x neuronsPerLayer x neuronsPerLayer
    public:
        Brain(int inputSize, int outputSize, int hiddenLayers, int neuronsPerLayer);
        ~Brain();
        genome_t getGenome();
        void setGenome(genome_t genome);
        void compute(double *inputs, double *outputs);
};

#endif