#ifndef BINOMIAL_H
#define BINOMIAL_H

#include <random>

/*!
    \class Binomial
    \brief The Binomial class calculates a binomial distribution

    Binomial uses the standard library distribution of the same
    name to generate a binomial distribution of numbers in the range
    from 0 to a specified integer n, with an average np, with a specified
    real number p
*/

class Binomial{
public:

    float p_;
    size_t n_;
    std::binomial_distribution<> d;

    /*! Creates new Binomial distribution
        \param n Maximun range for a number
        \param p Number to define the average np
    */

    Binomial(float p, size_t n){
        setProbability(p, n);
        recalculateDistributions();
    }

    /*! Recalculates the binomial distribution
    */

    void recalculateDistributions(){
        d = std::binomial_distribution<>(n_, p_);
    }

    /*! Set the probability for binomial distribution
        \param n Maximun range for a number
        \param p Number to define the average np
    */

    void setProbability(float p, size_t n){
        if(p<=0) p=0.001;
        else if(p>=1) p=0.999;
        p_ =p;
        n_=n;
        recalculateDistributions();
    }

    /*! Calculates a number in the binomial distribution
        \return a number between 0 and n
    */

    size_t calculate(){
        std::random_device rd;
        std::mt19937 gen(rd());
        return d(gen);
    }
};

#endif // BINOMIAL_H
