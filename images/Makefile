thisdir=$(dir $(lastword $(MAKEFILE_LIST)))

IMAGES := \
    6.bitmap 7.bitmap \
    6.reversed.bitmap 7.reversed.bitmap \
    cardsprite_top.bitmap cardsprite_bottom.bitmap cardsprite_bg.bitmap

IMAGES := $(addprefix $(thisdir), $(IMAGES))

%.mono: %.bmp
	convert $^ MONO:$@

%.reversed.mono: %.bmp
	convert -flip -flop $^ MONO:$@

%.bitmap: %.mono
	$(thisdir)bitreverse.py < $< > $@

CLEANFILES += \
    $(thisdir)*.bitmap \
    $(thisdir)*.reversed.bitmap \
    $(thisdir)*.mono \
    $(thisdir)*.reversed.mono
