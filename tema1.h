#include <pthread.h>
#include <string>
#include <queue>
#include <unordered_set>

void *map(void *arg);

void *reduce(void *arg);

void checkPow(uint32_t number, uint32_t max_exp, std::unordered_set<int> *powerLists);

int64_t fast_pow(int64_t x, uint32_t exp)
{
    int64_t result = 1;
    while (exp > 0)
    {
        if ((exp & 1) == 1)
        {
            result = result * x;
        }
        x *= x;
        exp >>= 1;
    }
    return result;
}

struct mapArg
{
    mapArg(){}

    mapArg(
        std::queue<std::string> *files, std::unordered_set<int> *powerLists, uint32_t maxPower,
        pthread_barrier_t *barrier, pthread_mutex_t *mutex
        )
    {
        mapArg::files = files;
        mapArg::powerLists = powerLists;
        mapArg::maxPower = maxPower;
        mapArg::barrier = barrier;
        mapArg::mutex = mutex;
    }

    std::queue<std::string> *files;
    std::unordered_set<int> *powerLists;
    uint32_t maxPower;
    pthread_barrier_t *barrier;
    pthread_mutex_t *mutex;
};

struct reduceArg
{
    reduceArg(){}

    reduceArg(
        uint32_t id, std::unordered_set<int> *powerLists, uint32_t mappers, uint32_t reducers,
        pthread_barrier_t *barrier
        )
    {
        reduceArg::id = id;
        reduceArg::powerLists = powerLists;
        reduceArg::mappers = mappers;
        reduceArg::reducers = reducers;
        reduceArg::barrier = barrier;
    }

    uint32_t id;
    std::unordered_set<int> *powerLists;
    uint32_t mappers;
    uint32_t reducers;
    pthread_barrier_t *barrier;
};