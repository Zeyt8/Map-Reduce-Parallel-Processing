#include <fstream>
#include <tgmath.h>
#include <limits>
#include <algorithm>
#include "tema1.h"

void *map(void *arg)
{
    mapArg mArg = *static_cast<mapArg*>(arg);

    while(!mArg.files->empty())
    {
        pthread_mutex_lock(mArg.mutex);
        if(mArg.files->empty())
        {
            pthread_mutex_unlock(mArg.mutex);
            break;
        }
        std::ifstream file(mArg.files->front()); 
        mArg.files->pop();
        pthread_mutex_unlock(mArg.mutex);

        std::unordered_set<int> powerLists[mArg.maxPower - 1];
        uint32_t no;
        file >> no;

        for(uint32_t i = 0; i < no; i++)
        {
            uint32_t c;
            file >> c;

            if(c == 1)
            {
                for(uint32_t j = 0; j < mArg.maxPower - 1; j++)
                {
                    powerLists[j].insert(c);
                }
            }
            else
            {
                checkPow(c, mArg.maxPower, powerLists);
            }
        }
        file.close();

        for(uint32_t i = 0; i < mArg.maxPower - 1; i++)
        {
            mArg.powerLists[i].insert(powerLists[i].begin(), powerLists[i].end());
        }
    }

    pthread_barrier_wait(mArg.barrier);

    pthread_exit(NULL);
}

void *reduce(void *arg)
{
    reduceArg rArg = *static_cast<reduceArg*>(arg);
    std::unordered_set<int> myPowerArray;
    pthread_barrier_wait(rArg.barrier);

    for(uint32_t i = 0; i < rArg.mappers; i++)
    {
        myPowerArray.insert(
            rArg.powerLists[i * rArg.reducers + rArg.id].cbegin(),
            rArg.powerLists[i * rArg.reducers + rArg.id].cend()
            );
    }

    std::ofstream file("out" + std::to_string(rArg.id + 2) + ".txt");
    file << myPowerArray.size();

    pthread_exit(NULL);
}

void checkPow(uint32_t number, uint32_t maxPower, std::unordered_set<int> *powerLists)
{
    maxPower = fmin(maxPower, (uint32_t)ceil(log2(number)));
    uint32_t sqrtNumber = (uint32_t)ceil(sqrt(number));
    for (uint32_t exp = 2; exp <= maxPower; exp++)
    {
        uint32_t left = 1;
        uint32_t right = sqrtNumber;
        while (left <= right)
        {
            int64_t base = left + (right - left) / 2;
            int64_t p = fast_pow(base, exp);
            if (p == number)
            {
                powerLists[exp - 2].insert(number);
                break;
            }
            else if (p < number)
            {
                left = base + 1;
            }
            else
            {
                right = base - 1;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    // read cli args
    int mappers = std::stoi(argv[1]);
    int reducers = std::stoi(argv[2]);
    char* filePath = argv[3];

    // threads, args for threads and other shared data
    pthread_t threads[mappers + reducers];
    std::unordered_set<int> powerLists[mappers][reducers];
    mapArg *mapArgs = new mapArg[mappers];
    reduceArg *reduceArgs = new reduceArg[reducers];

    // read main file
    std::ifstream file(filePath);
    int numberOfFiles;
    file >> numberOfFiles;
    std::queue<std::string> files;
    for(int i = 0; i < numberOfFiles; i++)
    {
        std::string f;
        file >> f;
        files.push(f);
    }
    file.close();

    // init barrier
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, mappers + reducers);

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    // start threads
    for(int i = 0, j = 0; i < mappers || j < reducers;)
    {
        if(i < mappers)
        {
            mapArgs[i] = mapArg(
                &files, powerLists[i], reducers + 1,
                &barrier, &mutex
                );
            pthread_create(&threads[i + j], NULL, map, &mapArgs[i]);
            i++;
        }
        if(j < reducers)
        {
            reduceArgs[j] = reduceArg(j, &powerLists[0][0], mappers, reducers, &barrier);
            pthread_create(&threads[i + j], NULL, reduce, &reduceArgs[j]);
            j++;
        }
    }

    // join threads
    for(int i = 0; i < mappers + reducers; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // clean up
    pthread_barrier_destroy(&barrier);
    pthread_mutex_destroy(&mutex);

    free(mapArgs);
    free(reduceArgs);
}