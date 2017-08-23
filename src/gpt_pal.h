
#ifndef GPT_PAL_H
#define GPT_PAL_H

// We use gdisk to clean up the GPT such that Windows is happy writing to
// the disk

// This function is kept in a separate file to avoid an issue with pragmas
// within gdisk affecting rufus-based functionality (getting disk extents)
bool clearMbrGpt(char * physical_path);

#endif
