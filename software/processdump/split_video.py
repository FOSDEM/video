#! /usr/bin/python
 
from __future__ import print_function
from PIL import Image
from PIL import ImageFont
from PIL import ImageDraw
from slugify import slugify
import argparse
import json
import os
import shutil
import subprocess
import sys
import wget
import xml.etree.ElementTree as ET 

penta_url='https://archive.fosdem.org/2016/schedule/xml'
tmpdir='/tmp/split_video_tmp/'
base_outdir ='/tmp/processedvideos'

parser = argparse.ArgumentParser()
parser.add_argument("-e", "--event", help="a json formatted file that contains what we need to process the video of a presentation at FOSDEM", required=True)
args = parser.parse_args()

with open(args.event) as event_file:
    data = json.load(event_file)

if data['needs_video_team_intervention']:
    # FIXME. Do something
    sys.exit(1)

print("Checking if we have the schedule...")
if os.path.isfile(os.path.basename(penta_url)):
    penta = os.path.basename(penta_url)
else:
    print("Getting the schedule...")
    penta = wget.download(penta_url)

print("Parsing the schedule file...")
pentaparse = ET.parse(penta).getroot()
# FIXME Either keep the downloaded penta xml file and make sure it doesn't get downloaded again, or throw it out and always get a fresh one.

print("Finding the talk to process...")
talk = pentaparse.find(".//event[@id='"+data['event_id']+"']")

print("Creating relevant file names for this talk...")
title = talk.find('title').text
track = talk.find('track').text
slug_track = slugify(track)
outdir = base_outdir+ '/'+slug_track+'/'
slug_title = slugify(title)
videobasename = slug_title
nakedvideomp4name = tmpdir+videobasename+ '.mp4'
nakedvideotsname = tmpdir+videobasename+ '.ts'
prerollbasename = tmpdir+'preroll'+ slug_title
prerollimgname = prerollbasename+ '.jpg'
prerolltsname = prerollbasename+ '.ts'
#postrollbasename = tmpdir+'postroll'
postrollbasename = 'postroll'
postrollimgname = postrollbasename+ '.jpg'
postrolltsname = postrollbasename+ '.ts'
finalcut = outdir+ videobasename+ '.mp4'

print("Creating tmpdir")
try:
	os.makedirs(tmpdir, 0o755)
except:
	pass

print("Creating outdir...")
try:
	os.makedirs(outdir, 0o755)
except:
	pass

print("Getting list of speakers for this talk...")
persons = [p.text for p in talk.find('persons')]
personsline = ''
for p in persons:
    personsline += p + '   '

print("Grabbing the part of the video we need...")
url = data['url'] + "?start=" + data['start'] + "&end=" + data['end']
filename =  wget.download(url,out=nakedvideomp4name)

print("Creating custom preroll image from talk metadata...")
img = Image.open("preroll.jpg")
draw = ImageDraw.Draw(img)
font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 64)
draw.text((0, 0),title,(0,0,0),font=font)
draw.text((0, 100),personsline,(0,0,0),font=font)
img.save(prerollimgname)

print("Processing preroll and postroll images into video...") 
subprocess.check_call(['ffmpeg', '-y', '-loop', '1' , '-i', prerollimgname, '-c:v', 'libx264', '-r', '25', '-frames:v',  '125', prerolltsname])
subprocess.check_call(['ffmpeg', '-y', '-loop', '1' , '-i', "postroll.jpg", '-c:v', 'libx264', '-r', '25', '-frames:v',  '125', postrolltsname])

print("Concatenating preroll, video and postroll...")
subprocess.check_call(['ffmpeg', '-y', '-i', nakedvideomp4name, '-c', 'copy', nakedvideotsname])
concatcommand = 'ffmpeg -y -f mpegts -i "concat:'+prerolltsname+ '|'+ nakedvideotsname+ '|'+ postrolltsname+'" -c copy -bsf:a aac_adtstoasc '+ finalcut
subprocess.check_call(concatcommand, shell=True)

print("Cleaning up temporary directory...")
shutil.rmtree(tmpdir)
