thisdir=$(dir $(lastword $(MAKEFILE_LIST)))

IMAGES := \
    1.bitmap \
    2.bitmap \
    3.bitmap \
    4.bitmap \
    5.bitmap \
    6.bitmap \
    7.bitmap \
    8.bitmap \
    9.bitmap \
    1.reversed.bitmap \
    2.reversed.bitmap \
    3.reversed.bitmap \
    4.reversed.bitmap \
    5.reversed.bitmap \
    6.reversed.bitmap \
    7.reversed.bitmap \
    8.reversed.bitmap \
    9.reversed.bitmap \
    cardsprite_top.bitmap cardsprite_bottom.bitmap cardsprite_bg.bitmap \
    mouse_sprite.bitmap

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
