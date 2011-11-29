
#ifndef _UTILS_H
#define _UTILS_H

extern "C"
{
#   include <stdint.h>
}

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/variate_generator.hpp>

class Random
{
private:

    /* Integer Generation */
    boost::mt19937 *intRng_;
    boost::uniform_int<> *intDist_;
    boost::variate_generator< boost::mt19937&, boost::uniform_int<> > *intGen_;
    
    /* 0-1 Generation */
    boost::mt19937 *zeroOneRng_;
    boost::uniform_01<boost::mt19937> *zeroOneGen_;

public:
    Random(uint32_t n);
    ~Random();
    uint32_t getRandomInt();
    double getRandom01();
};

#endif /* _UTILS_H */
