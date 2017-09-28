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
#include "../gdisk/crc32.h"
#include <stdio.h>

class PalData : public GPTData {
public:
  PalData(string filename);
  WhichToUse UseWhichPartitions(void) override;
};

PalData::PalData(string filename) {
    // if we want to call our own copy of UseWhichPartitions()
   blockSize = SECTOR_SIZE; // set a default
   diskSize = 0;
   partitions = NULL;
   state = gpt_invalid;
   device = "";
   justLooking = 0;
   mainCrcOk = 0;
   secondCrcOk = 0;
   mainPartsCrcOk = 0;
   secondPartsCrcOk = 0;
   apmFound = 0;
   bsdFound = 0;
   sectorAlignment = MIN_AF_ALIGNMENT; // Align partitions on 4096-byte boundaries by default
   beQuiet = 0;
   whichWasUsed = use_new;
   mainHeader.numParts = 0;
   numParts = 0;
   // Initialize CRC functions...
   chksum_crc32gentab();
   if (!LoadPartitions(filename))
      exit(2);
}

WhichToUse PalData::UseWhichPartitions(void) {
  // The disk may be in a weird state, but we are about to reformat it.
  // Just always use a new partition table.
  return use_gpt;
}

bool clearMbrGpt(const char* physical_path) {
  std::string physical_path_str(physical_path);
  PalData gptdata(physical_path_str);
  // attempt to fix any gpt/mbr problems by setting to a sane, empty state
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
