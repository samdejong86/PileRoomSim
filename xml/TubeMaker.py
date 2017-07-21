#!/usr/bin/python

import os
import sys
from lxml import etree as ET
from xml.dom import minidom
from subprocess import call
import argparse

scriptPath=os.path.dirname(os.path.realpath(sys.argv[0]))

parser = argparse.ArgumentParser(description='Create a helium-3 tube description')
parser.add_argument('-f','--filename', help='The output filename. New tubes are appended to existing files', default='DEFAULT.xml', required=True)


defaultTree = ET.parse(scriptPath+"/HE3TUBE-default.xml")

defaultRoot = defaultTree.getroot()

for active in defaultRoot:
     for param in active:
          parser.add_argument('--'+param.tag, default=str(param.text).strip(), help = "(default: %(default)s)")

args = parser.parse_args()


fname = args.filename
del args.filename

root = ET.Element("Active")

for arg in vars(args):
     ET.SubElement(root, arg).text = '\n    '+str(getattr(args, arg))+'\n  '

fileExists =  os.path.isfile(fname)

if fileExists:
    call(["sed","-i","-e","s/<\\/xml>//g",fname])

f = open(str(fname), 'a+')

if fileExists==False:
    f.write("<xml>\n")

f.write(ET.tostring(root, pretty_print=True))
f.write("</xml>")




