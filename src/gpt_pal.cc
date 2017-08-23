
#include "gpt_pal.h"

#include "log.h"
// We use gdisk to clean up the GPT such that Windows is happy writing to
// the disk
#include "../gdisk/gpt.h"

bool clearMbrGpt(char * physical_path) {
  std::string physical_path_str(physical_path);
  GPTData gptdata(physical_path_str);
  // let's get some before/after data on the state of the gpt
  LOG_INFO << "problems with gpt before reformat=" << gptdata.Verify() << std::endl;
  gptdata.ClearGPTData();
  gptdata.MakeProtectiveMBR();
  int quiet = true;
  gptdata.SaveGPTData(quiet);
  int problems = gptdata.Verify();
  LOG_INFO << "problems with gpt after reformat=" << problems << std::endl;
  if (problems > 0) {
    return false;
  } else {
    return true;
  }
}
