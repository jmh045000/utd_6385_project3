
#include "utils.h"

using boost::mt19937;
using boost::uniform_int;
using boost::uniform_01;
using boost::variate_generator;

extern "C"
{
#   include <fcntl.h>
}

Random::Random(uint32_t n)
{
    int randomSeed;
    int randomFd = open("/dev/urandom", O_RDONLY);
    read(randomFd, &randomSeed, sizeof(randomSeed));
    close(randomFd);

    intRng_ = new mt19937(randomSeed);
    intDist_ = new uniform_int<>(0, n-1);
    intGen_ = new variate_generator < mt19937&, uniform_int<> >(*intRng_, *intDist_);

    zeroOneRng_ = new mt19937(randomSeed);
    zeroOneGen_ = new uniform_01<mt19937>(*zeroOneRng_);
}

Random::~Random()
{
    delete intRng_;
    delete intDist_;
    delete intGen_;

    delete zeroOneRng_;
    delete zeroOneGen_;
}

uint32_t Random::getRandomInt()
{
    return (uint32_t)(*intGen_)();
}

double Random::getRandom01()
{
    return (*zeroOneGen_)();
}
