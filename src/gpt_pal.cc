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

#include <winioctl.h>       // for MEDIA_TYPE

// FIXME: temp
#include <stdio.h>

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
  printf("start:%d\nend:%d\n", startSector, endSector);
  int ret = CreatePartition(newPartNum, startSector, endSector);
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
  int success = gptdata.WriteProtectiveMBR();
  printf("mbr clear success = %d\n", success);
  success = gptdata.ClearGPTData();
  printf("gpt clear success = %d\n", success);

  int quiet = true;
  // attempt to fix any gpt/mbr problems by setting to a sane, empty state
  gptdata.SaveGPTData(quiet);
  int problems = gptdata.Verify();
  printf("cleared mbr/gpt\n");
  if (problems > 0) {
    return false;
  } else {
    return true;
  }
}

void kewlcallback() {
  printf("made it to the callback!\n");
}

// FIXME: this should make a fat32 partition
// right now it does not really do anything
bool makeEmptyPartition(const char* physical_path) {
  char* newPartInfo;
  PalData gptdata;
  gptdata.LoadPartitions(std::string(physical_path));
  // make an unformatted partition with label for fat32
  gptdata.ClearDisk();
  // now make the volume
  //pfFormatEx(wVolumeName, DRIVE_REMOVABLE, &wFSType[index], wLabel,
  //  IsChecked(IDC_QUICK_FORMAT), ulClusterSize, FormatExCallback);
  // 0x0b is RemovableMedia according to https://msdn.microsoft.com/en-us/library/windows/desktop/aa365231(v=vs.85).aspx
  // TODO: lettuce use the actual value
  pfFormatEx(physical_path, MEDIA_TYPE.RemovableMedia, L"FAT32", L"", /*quick*/true, /*clustersize*/512, kewlcallback);
  int problems = gptdata.Verify();
  free(newPartInfo);
  // TODO: unclear if we care about problems in this regard
  if (problems > 0) {
    printf("there were problems\n");
    return true;
  } else {
    printf("there were no problems\n");
    return false;
  }
}
