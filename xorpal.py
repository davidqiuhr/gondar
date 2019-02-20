#! /usr/bin/env python3

import array
import datetime
import os
import argparse

def get_rand(instr):
  inbytes = array.array('B', bytes(instr, 'utf-8'))
  randbytes = array.array('B', (bytes(os.urandom(len(inbytes)))))
  return randbytes

def str_to_bytes(instr):
  return array.array('B', bytes(instr, 'utf-8'))

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
  return str(outbytes, 'utf-8')

def handle_rand(args):
  randbytes = get_rand(args.string) 
  print(bytes(randbytes).hex())

def handle_derived(args):
  derivedbytes = get_derived(args.string, bytearray.fromhex(args.rand))
  print(bytes(derivedbytes).hex())

def handle_reverse(args):
  outstring = get_original(str_to_bytes(args.rand), str_to_bytes(args.derived))
  print(outstring)

def parse_args():
  parser = argparse.ArgumentParser()
  subparsers = parser.add_subparsers(dest='mode')
  subparsers.required = True
  rand_parser = subparsers.add_parser('rand')
  rand_parser.add_argument('string')
  derived_parser = subparsers.add_parser('derived')
  derived_parser.add_argument('string')
  derived_parser.add_argument('rand')
  reverse_parser = subparsers.add_parser('reverse')
  reverse_parser.add_argument('rand')
  reverse_parser.add_argument('derived')
  return parser.parse_args()

def main():
  args = parse_args()
  if args.mode == 'rand':
    handle_rand(args)
  elif args.mode == 'derived':
    handle_derived(args)
  elif args.mode == 'reverse':
    handle_reverse(args)
  #args = parse_args()
  #instr = args.string
  # the original string seems to be working as intended
  """
  instr = 'abc'
  rand = get_rand(instr)
  derived = get_derived(instr, rand)
  reverse = get_original(rand, derived)
  print('{}, {}, {}, {}'.format(instr, rand, derived, reverse))
  """

if __name__ == '__main__':
  main()
