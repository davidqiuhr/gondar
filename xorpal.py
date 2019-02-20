#! /usr/bin/env python3

import array
import datetime
import os

def get_rand(instr):
  inbytes = array.array('B', bytes(instr, 'utf-8'))
  randbytes = array.array('B', (bytes(os.urandom(len(inbytes)))))
  return randbytes

def get_derived(instr, randbytes):
  inbytes = array.array('B', bytes(instr, 'utf-8'))
  derivedbytes = array.array('B')
  for inbyte, randbyte in zip(inbytes, randbytes):
    derivedbytes.append(inbyte^randbyte)
  return derivedbytes

def get_original(randbytes, derivedbytes):
  outbytes = array.array('B')
  for randbyte, derivedbyte in zip(randbytes, derivedbytes):
    outbytes.append(randbyte^derivedbyte)
  return outbytes.tostring()

if __name__ == '__main__':
  instr = 'the quick brown fox jumped over the lazy dog'
  randbytes = get_rand(instr) 
  derivedbytes = get_derived(instr, randbytes)
  outstring = get_original(randbytes, derivedbytes)
  print(instr)
  print(randbytes)
  print(derivedbytes)
  print(outstring)
