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

#include <winioctl.h>       // for MEDIA_TYPE


HMODULE  OpenedLibraryHandle;
static __inline HMODULE GetLibraryHandle() {
  return OpenedLibraryHandle;
}

// TODO: further simplify
#define PF_INIT(proc, name)         if (pf##proc == NULL) pf##proc = \
  (proc##_t) GetProcAddress(GetLibraryHandle(), #proc)

/* Callback command types (some errorcode were filled from HPUSBFW V2.2.3 and their
   designation from msdn.microsoft.com/en-us/library/windows/desktop/aa819439.aspx */
typedef enum {
  FCC_PROGRESS,
  FCC_DONE_WITH_STRUCTURE,
  FCC_UNKNOWN2,
  FCC_INCOMPATIBLE_FILE_SYSTEM,
  FCC_UNKNOWN4,
  FCC_UNKNOWN5,
  FCC_ACCESS_DENIED,
  FCC_MEDIA_WRITE_PROTECTED,
  FCC_VOLUME_IN_USE,
  FCC_CANT_QUICK_FORMAT,
  FCC_UNKNOWNA,
  FCC_DONE,
  FCC_BAD_LABEL,
  FCC_UNKNOWND,
  FCC_OUTPUT,
  FCC_STRUCTURE_PROGRESS,
  FCC_CLUSTER_SIZE_TOO_SMALL,
  FCC_CLUSTER_SIZE_TOO_BIG,
  FCC_VOLUME_TOO_SMALL,
  FCC_VOLUME_TOO_BIG,
  FCC_NO_MEDIA_IN_DRIVE,
  FCC_UNKNOWN15,
  FCC_UNKNOWN16,
  FCC_UNKNOWN17,
  FCC_DEVICE_NOT_READY,
  FCC_CHECKDISK_PROGRESS,
  FCC_UNKNOWN1A,
  FCC_UNKNOWN1B,
  FCC_UNKNOWN1C,
  FCC_UNKNOWN1D,
  FCC_UNKNOWN1E,
  FCC_UNKNOWN1F,
  FCC_READ_ONLY_MODE,
} FILE_SYSTEM_CALLBACK_COMMAND;

typedef BOOLEAN (__stdcall *FILE_SYSTEM_CALLBACK)(
  FILE_SYSTEM_CALLBACK_COMMAND Command,
  ULONG                        Action,
  PVOID                        pData
);

/* Parameter names aligned to
   http://msdn.microsoft.com/en-us/library/windows/desktop/aa819439.aspx */
typedef VOID (WINAPI *FormatEx_t)(
  WCHAR*               DriveRoot,
  MEDIA_TYPE           MediaType,   // See WinIoCtl.h
  WCHAR*               FileSystemTypeName,
  WCHAR*               Label,
  BOOL                 QuickFormat,
  ULONG                DesiredUnitAllocationSize,
  FILE_SYSTEM_CALLBACK Callback
);

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

/*
 * FormatEx callback. Return FALSE to halt operations
 */
static BOOLEAN __stdcall FormatExCallback(FILE_SYSTEM_CALLBACK_COMMAND Command, DWORD, PVOID)
{
  //LOG(INFO) << "made it to the callback!";
  switch(Command) {
  case FCC_PROGRESS:
    //LOG(INFO) << "progress case";
    break;
  case FCC_DONE:
    //LOG(INFO) << "done case";
    // we don't need our library anymore
    FreeLibrary(OpenedLibraryHandle);
    break;
  default:
    //LOG(INFO) << "some other case";
    break;
  }
  return true;
}


// FIXME: this should make a fat32 partition
// right now it does not really do anything
bool makeEmptyPartition(const char* physical_path) {
  PalData gptdata;
  gptdata.LoadPartitions(std::string(physical_path));
  // make an unformatted partition with label for fat32
  gptdata.ClearDisk();
  // now make the volume

  FormatEx_t pfFormatEx = NULL;
  // LoadLibrary("fmifs.dll") appears to changes the locale, which can lead to
  // problems with tolower(). Make sure we restore the locale. For more details,
  // see http://comments.gmane.org/gmane.comp.gnu.mingw.user/39300
  char* locale = setlocale(LC_ALL, NULL);
  PF_INIT(FormatEx, Fmifs);
  //PF_INIT(EnableVolumeCompression, Fmifs);
  setlocale(LC_ALL, locale);

  // 0x0b is RemovableMedia according to https://msdn.microsoft.com/en-us/library/windows/desktop/aa365231(v=vs.85).aspx
  // TODO: physical_path needs to be wchar.
  // TODO: a real version of this please
  wchar_t wc[50];
  mbstowcs(&wc[0], physical_path, 50);
  wchar_t* wcp = (wchar_t*)&wc;
  // TODO: uncomment
  pfFormatEx(wcp, RemovableMedia, L"FAT32", L"", /*quick*/true, /*clustersize*/512, FormatExCallback);

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
