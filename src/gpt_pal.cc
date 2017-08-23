// Copyright 2017 Neverware
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

#include "gpt_pal.h"

// We use gdisk to clean up the GPT such that Windows is happy writing to
// the disk
#include "../gdisk/gpt.h"

bool clearMbrGpt(char* physical_path) {
  std::string physical_path_str(physical_path);
  GPTData gptdata(physical_path_str);
  // let's get some before/after data on the state of the gpt
  gptdata.ClearGPTData();
  gptdata.MakeProtectiveMBR();
  int quiet = true;
  gptdata.SaveGPTData(quiet);
  int problems = gptdata.Verify();
  if (problems > 0) {
    return false;
  } else {
    return true;
  }
}
