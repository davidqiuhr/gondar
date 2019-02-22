
#ifndef SRC_RANDAR_H
#define SRC_RANDAR_H

#include <random>

namespace gondar {

class RandomManager {
 public:
  static RandomManager& getInstance() {
    static RandomManager instance;
    return instance;
  }
  std::mt19937 gen;

 private:
  RandomManager();
  unsigned int getSeed();
  unsigned int seed;

 public:
  // singleton buddies
  // see https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
  RandomManager(RandomManager const&) = delete;
  void operator=(RandomManager const&) = delete;
};

int getRandomNum(int lower, int higher);
}

#endif  // SRC_RANDAR_H
