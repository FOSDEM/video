#! /usr/bin/python
 
from __future__ import print_function
from slugify import slugify
import json
import os
import shutil
import subprocess
import sys
import wget
import xml.etree.ElementTree as ET 

penta_url='https://archive.fosdem.org/2016/schedule/xml'
outdir ='/tmp/jsonfiles'

print("Checking if we have the schedule...")
if os.path.isfile(os.path.basename(penta_url)):
    penta = os.path.basename(penta_url)
else:
    print("Getting the schedule...")
    penta = wget.download(penta_url)

print("Parsing the schedule file...")
pentaparse = ET.parse(penta).getroot()

print("Finding the talks to process into json files...")
talks = pentaparse.findall(".//event")

for talk in talks:
    print("Creating relevant file names for this talk...")
    title = talk.find('title').text
    track = talk.find('track').text
    slug_track = slugify(track)
    slug_title = slugify(title)
    videobasename = slug_title
    track_dir = outdir + "/"+ slug_track
    jsonfilename = slug_title+ '.json'

    print("Creating track_dir...")
    try:
            os.makedirs(track_dir, 0o755)
    except:
            pass

    output = '''{
        "event_id": "'''+str(talk.get('id'))+'''",
        "needs_video_team_intervention": "",
        "url": "",
        "start": "",
        "end": ""
}'''

    f = open(track_dir+ '/'+ jsonfilename, 'w')
    f.write(output)
    f.close()
