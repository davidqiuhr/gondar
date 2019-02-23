// Copyright 2019 Neverware
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SRC_RANDAR_H_
#define SRC_RANDAR_H_

#include <random>

namespace gondar {

class RandomManager {
 public:
  static RandomManager& getInstance() {
    static RandomManager instance;
    return instance;
  }
  unsigned int getRandomByte();

 private:
  RandomManager();
  unsigned int seed;

 public:
  // singleton buddies
  // see https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
  RandomManager(RandomManager const&) = delete;
  void operator=(RandomManager const&) = delete;
};

int getRandomNum(int lower, int higher);

}  // namespace gondar

#endif  // SRC_RANDAR_H_
