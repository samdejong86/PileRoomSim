#!/usr/bin/python

import os
from lxml import etree as ET
from xml.dom import minidom
from subprocess import call
import argparse

parser = argparse.ArgumentParser(description='Create a helium-3 tube description')
parser.add_argument('-f','--filename', help='The output filename. New tubes are appended to existing files', default='DEFAULT.xml', required=True)

parser.add_argument('--Material', default='G4_STAINLESS-STEEL', help = "(default: %(default)s)")
parser.add_argument('--x_pos', default=-100, help = "(default: %(default)s)")
parser.add_argument('--y_pos', default=0, help = "(default: %(default)s)")
parser.add_argument('--z_pos', default=0, help = "(default: %(default)s)")
parser.add_argument('--AngleX', default=90, help = "(default: %(default)s)")
parser.add_argument('--AngleY', default=0, help = "(default: %(default)s)")
parser.add_argument('--AngleZ', default=0, help = "(default: %(default)s)")
parser.add_argument('--tube_innerRadius', default=2.069, help = "(default: %(default)s)")
parser.add_argument('--tube_outerRadius', default=2.54, help = "(default: %(default)s)")
parser.add_argument('--tube_hz', default=10.19, help = "(default: %(default)s)")
parser.add_argument('--endcap_hz', default=0.47, help = "(default: %(default)s)")
parser.add_argument('--gas_outerRadius', default=2.0, help = "(default: %(default)s)")
parser.add_argument('--gas_hz', default=7.62, help = "(default: %(default)s)")
parser.add_argument('--sensitive_volume_offset', default=0.6096, help = "(default: %(default)s)")
parser.add_argument('--Colour', default='#2B2B2B', help = "(default: %(default)s)")



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



