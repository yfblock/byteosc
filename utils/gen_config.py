#!/bin/env python

import tomllib
import argparse
import pprint

parser = argparse.ArgumentParser(
    prog='gen_config.py',
    description='generate config through the given config file',
    epilog='example usage: gen_config.py test.toml')

parser.add_argument('file', help="the file will be load as input config file.")
parser.add_argument('output', help="the .h file will be generated")

args = parser.parse_args()

# Get the config from the given path
# Exit it if error occurs
def read_config(path):
    try:
        f = open(path, "rb")
        return tomllib.load(f)
    except OSError as e:
        print(e)
        exit(-1)

config = read_config(args.file)
print(config)
pprint.pp(config)
