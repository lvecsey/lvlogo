#!/bin/bash

def_annotatestr='LV'

if [ "$1" != "" ]; then
    annotate_str="$1"
else
    annotate_str="$def_annotatestr"
fi

CMR10_TTF=/usr/share/fonts/truetype/lyx/cmr10.ttf

outpng_fn='flatlogo.png'

dd if=/dev/zero of='/tmp/blank_960x960.rgb' count=55296 bs=100

echo Creating flat logo with string: "$annotate_str"

convert -size 960x960 /tmp/blank_960x960.rgb -fill white -font "$CMR10_TTF" -pointsize 576 -gravity Center -annotate 0 "$annotate_str" -depth 32 "$outpng_fn"

convert "$outpng_fn" -depth 16 flatlogo_960x960.rgb

exit 0
