#ifndef BERNOULLI_H
#define BERNOULLI_H

#include <random>

/*!
    \class Bernoulli
    \brief The Bernoulli class calculates numbers in the range 1 to 3
    given 3 probabilities

    Bernoulli uses the standard library distribution of the same
    name to generate 2 sets of distributions, in which both of them
    overlap to generate 3 possible outputs.

*/

class Bernoulli{
public:

    float p1_, p2_, p3_;
    std::bernoulli_distribution d1, d2;

    /*! Creates new Bernoulli distribution
        \param p1 The probability for processing type
        \param p1 The probability for reading type
        \param p1 The probability for writing type
    */

    Bernoulli(float p1, float p2, float p3){
        setProbability(p1, p2, p3);
        recalculateDistributions();
    }

    /*! Recalculates the distribution with given parameters
    */

    void recalculateDistributions(){

        d1 = std::bernoulli_distribution(p1_);
        d2 = std::bernoulli_distribution(p2_/(1-p1_)); //Bayes formula
    }

    /*! Sets the parameter for the distribution
        \param p1 The probability for processing type
        \param p1 The probability for reading type
        \param p1 The probability for writing type
    */

    void setProbability(float p1, float p2, float p3){
        if(p1+p2 > 1){
            p2=1-p1;
        }
        if(p3 > 1-p1-p2){
            p3 = 1-p1-p2;
        }
        p1_ = p1;
        p2_ = p2;
        p3_ = p3;
        recalculateDistributions();
    }

    /*! Calculates a class in the bernoulli distribution
        \return Class
    */

    size_t calculate(){
        // Generates the new seed for the distribution
        std::random_device rd;
        std::mt19937 gen(rd());
        bool A = d1(gen);
        bool B = d2(gen);

        return 1*(A)+(!A)*(2*B+3*(!B)); //lookup into the cases
    }
};

#endif // BERNOULLI_H
