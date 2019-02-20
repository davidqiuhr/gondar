#! /usr/bin/env python3

import array
import datetime
import os

if __name__ == '__main__':
  instr = 'the quick brown fox jumped over the lazy dog'
  inbytes = array.array('B', bytes(instr, 'utf-8'))
  randbytes = array.array('B', (bytes(os.urandom(len(inbytes)))))
  derivedbytes = array.array('B')
  for inbyte, randbyte in zip(inbytes, randbytes):
    derivedbytes.append(inbyte^randbyte)
    
  print(inbytes)
  print(randbytes)
  print(derivedbytes)
  outbytes = array.array('B')
  for randbyte, derivedbyte in zip(randbytes, derivedbytes):
    outbytes.append(randbyte^derivedbyte)
  print(outbytes.tostring())
