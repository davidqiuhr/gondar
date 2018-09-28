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


#include <windows.h>
#include <inttypes.h>

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

// somewhat strangely this also makes a big partition.
// TODO: maybe rename?
void PalData::ClearDisk() {
  //BlankPartitions();
  //SaveGPTData(true);
  // ^ i don't think those are necessary.
  // a cloudready install is like the worst case
  // so if it worked before it should be fine now.
  int newPartNum = 0;
  uint64_t low = FindFirstInLargest();
  Align(&low);
  uint64_t high = FindLastInFree(low);
  uint64_t startSector = low;
  uint64_t endSector = high;
  //LOG(INFO) << "start: " << startSector << "end: " << endSector;
  //int ret = CreatePartition(newPartNum, startSector, endSector);
  CreatePartition(newPartNum, startSector, endSector);
  partitions[0].SetFirstLBA(startSector);
  partitions[0].SetLastLBA(endSector);
  partitions[0].SetType(0x0b00);  // make it fat32
  partitions[0].RandomizeUniqueGUID();
  DisplayGPTData();
  // arg is 'quiet'
  SaveGPTData(true);
}

// regardless this is the 'shared' logic
// this looks good now.
bool clearMbrGpt(const char* physical_path) {
  std::string physical_path_str(physical_path);
  PalData gptdata;
  // set the physical path for this GPT object to act on
  gptdata.LoadPartitions(std::string(physical_path));
  //int success = gptdata.WriteProtectiveMBR();
  gptdata.WriteProtectiveMBR();
  //LOG(INFO) << "mbr clear success = " << success;
  //success = gptdata.ClearGPTData();
  gptdata.ClearGPTData();
  //LOG(INFO) << "gpt clear success = " << success;

  int quiet = true;
  // attempt to fix any gpt/mbr problems by setting to a sane, empty state
  gptdata.SaveGPTData(quiet);
  int problems = gptdata.Verify();
  //LOG(INFO) << "cleared mbr/gpt";
  if (problems > 0) {
    return false;
  } else {
    return true;
  }
}

// FIXME: this should make a fat32 partition
// right now it does not really do anything
bool makeEmptyPartition(const char* physical_path) {
  PalData gptdata;
  gptdata.LoadPartitions(std::string(physical_path));
  // make an unformatted partition with label for fat32
  gptdata.ClearDisk();
  // now make the volume

  int problems = gptdata.Verify();
  // TODO: unclear if we care about problems in this regard
  if (problems > 0) {
    //LOG(INFO) << "there was a problem";
    return true;
  } else {
    //LOG(INFO) << "there were no problems";
    return false;
  }
}
