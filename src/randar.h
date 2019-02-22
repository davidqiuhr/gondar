
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
  int getRandomNum(int lower, int higher); 
 private:
  RandomManager();
  unsigned int getSeed();
  unsigned int seed;
  std::mt19937 gen;
 public:
  // singleton buddies
  // see https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
  RandomManager(RandomManager const&) = delete;
  void operator=(RandomManager const&) = delete;

};

}

#endif // SRC_RANDAR_H
