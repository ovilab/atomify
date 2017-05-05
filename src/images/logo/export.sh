#!/bin/bash
for x in 40 48 72 76 80 96 120 144 152 180 192 512 1024 ; do inkscape --export-png atomify_logo_${x}.png -w ${x} atomify_logo.svg ; done
