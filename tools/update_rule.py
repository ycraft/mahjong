#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import subprocess
import sys
import re

import google.protobuf.text_format

# adding import path for auto-generated protobuf library
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../src-gen/py/'))

import mahjong_rule_pb2

ROOT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), '../')
RAW_BASE_DIR = os.path.join(ROOT_DIR, 'res/raw')

RULE_FILE_PATH = os.path.join(ROOT_DIR, 'data/rule.pb.txt')

EXPORT_FILE_PATH = os.path.join(RAW_BASE_DIR, 'rule.pb')

def write_to_raw_dir(data):
  outfile = open(EXPORT_FILE_PATH, 'w')
  outfile.write(data)
  outfile.close()
  print 'Exported to ' + EXPORT_FILE_PATH
  
def load_rule_file():
  f = open(RULE_FILE_PATH)
  rule = f.readlines()
  f.close()
  return ''.join(rule)

def main():
  rule = mahjong_rule_pb2.Rule()
  google.protobuf.text_format.Merge(load_rule_file(), rule)
  google.protobuf.text_format.PrintMessage(rule, sys.stdout, as_utf8=True)
  write_to_raw_dir(rule.SerializeToString())

if __name__ == '__main__':
  main()
