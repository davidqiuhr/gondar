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
#include "../gdisk/parttypes.h"

class PalData : public GPTData {
 public:
  PalData();
  WhichToUse UseWhichPartitions(void) override;
  void ClearDisk();
};

PalData::PalData() : GPTData() {}

WhichToUse PalData::UseWhichPartitions(void) {
  // The disk may be in a weird state, but we are about to reformat it.
  // Just always use a new partition table.
  return use_new;
}

//TODO: rename
void PalData::ClearDisk() {
  int newPartNum = 0;
  uint64_t startSector = FindFirstInLargest();
  Align(&startSector);
  uint64_t endSector = FindLastInFree(startSector);
  MakeProtectiveMBR();
  CreatePartition(newPartNum, startSector, endSector);
  // set partition type to FAT-32
  if (!ChangePartType(newPartNum, GUIDData("EBD0A0A2-B9E5-4433-87C0-68B6B72699C7"))) {
    printf("We couldn't change the partition type!\n");
  } else {
    printf("We change the partition type!\n");
  }
  // arg is 'quiet'
  if (!SaveGPTData(true)) {
    printf("Error Saving GPT Data!  Have a nice day!\n");
  } else {
    printf("Successfully Saved GPT Data!  Have a nice day!\n");
  }
  DisplayGPTData();
}

bool clearMbrGpt(const char* physical_path) {
  std::string physical_path_str(physical_path);
  PalData gptdata;
  // set the physical path for this GPT object to act on
  gptdata.LoadPartitions(std::string(physical_path));
  int quiet = true;
  // attempt to fix any gpt/mbr problems by setting to a sane, empty state
  gptdata.SaveGPTData(quiet);
  int problems = gptdata.Verify();
  if (problems > 0) {
    return false;
  } else {
    return true;
  }
  return true;
}

bool makeEmptyPartition(const char* physical_path) {
  char* newPartInfo;
  PalData gptdata;
  gptdata.LoadPartitions(std::string(physical_path));
  //gptdata.DestroyGPT();
  //gptdata.DestroyMBR();
  gptdata.ClearDisk();
  return true;

  int problems = gptdata.Verify();
  free(newPartInfo);
  if (problems > 0) {
    return true;
  } else {
    return false;
  }
}
