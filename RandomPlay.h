#include <random>     // mt19937
#include <ctime>      // time()

class RandomPlay // can not belong to GameLabels because it can not be const
{
    public:
        RandomPlay(const GameLabels& labels):
            generator(time(nullptr)),
            distribution(0, labels.count - 1)
        { }
        int Generate()
        {
            return distribution(generator);
        }
    private:
        std::mt19937 generator;
        std::uniform_int_distribution<> distribution;
};
