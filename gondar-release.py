#! /usr/bin/env python
"""
This program will propagate the desired release candidate to the prod stable
release folder.  This is predicated on having a ~/.aws/config file set up in
the following style

[default]
region=us-east-1

[profile dev]
aws_access_key_id = (key)
aws_secret_access_key = (secret)

[profile prod]
aws_access_key_id = (key)
aws_secret_access_key = (secret)
"""

import argparse
import boto3
import botocore
import os
import shutil

WORKDIR = 'gondar-release-workdir'
SOURCE_BUCKET = 'neverware-gondar-release-candidates'
SOURCE_FILENAME = 'cloudready-usb-maker.exe'
DEST_BUCKET = 'neverware-cloudready-usb-releases'

BEEROVER_UPLOAD_NAME = 'cloudready-free'
CHROMEOVER_UPLOAD_NAME = 'cloudready'

STABLE_KEY_PREFIX = 'stable'
DEST_FILENAME = 'cloudready-usb-maker.exe'

def clean_workdir(makedir=True):
  """
  Empty our local working directory used to stash the downloaded file.
  Optionally remake the empty working directory.
  """
  if os.path.exists(WORKDIR):
    shutil.rmtree(WORKDIR)
  if makedir:
    os.makedirs(WORKDIR)

def get_clients():
  dev_session = boto3.Session(profile_name='dev')
  dev_client = dev_session.client('s3')
  prod_session = boto3.Session(profile_name='prod')
  prod_client = prod_session.client('s3')
  return dev_client, prod_client

def download_file(dev_client, local_path, candidate, beerover):
  if beerover:
    product = 'beerover'
  else:
    product = 'chromeover'
  key = '/'.join([candidate, product, SOURCE_FILENAME])
  print("downloading from key={}".format(key))
  try:
    dev_client.download_file(SOURCE_BUCKET, key, local_path)
    return True
  except botocore.exceptions.ClientError as e:
    if e.response['Error']['Code'] == '404':
      # invalid release candidate file
      return False
    else:
      raise

def set_public_read(client, bucket, key):
  client.put_object_acl(ACL='public-read',
                        Bucket=bucket,
                        Key=key)

def upload_file(prod_client, local_path, candidate, beerover):
  # then we upload it to the prod bucket
  if beerover:
    product_upload_name = BEEROVER_UPLOAD_NAME
  else:
    product_upload_name = CHROMEOVER_UPLOAD_NAME
  stable_key_path = '/'.join([STABLE_KEY_PREFIX,
                              product_upload_name,
                              DEST_FILENAME])
  candidate_key_path = '/'.join([candidate,
                                 product_upload_name,
                                 DEST_FILENAME])
  print("uploading to key={}".format(stable_key_path))
  print("archiving release to key={}".format(candidate_key_path))
  for key in (stable_key_path, candidate_key_path):
    prod_client.upload_file(local_path, DEST_BUCKET, key)
    # make the files public
    set_public_read(prod_client, DEST_BUCKET, key)

def parse_args():
  parser = argparse.ArgumentParser()
  parser.add_argument('candidate', help='version number of release candidate (for example, 20)')
  return parser.parse_args()

def release(dev_client, prod_client, candidate, beerover):
  local_path = os.path.join(WORKDIR, DEST_FILENAME)
  # empty work directory of contents
  clean_workdir()
  # first we download the gondar RC to a local file
  if not download_file(dev_client, local_path, candidate, beerover):
    print("Release candidate not found")
    return
  # then we upload it to the appropriate prod bucket and set public readable
  upload_file(prod_client, local_path, candidate, beerover)

def main():
  args = parse_args()
  dev_client, prod_client = get_clients()
  # release beerover
  release(dev_client, prod_client, args.candidate, beerover=True)
  # release chromeover
  release(dev_client, prod_client, args.candidate, beerover=False)
  # when we are finished, delete the directory
  clean_workdir(makedir=False)

if __name__ == "__main__":
  main()
