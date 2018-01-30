thisdir=$(dir $(lastword $(MAKEFILE_LIST)))

IMAGES := 6.bitmap 7.bitmap cardsprite_top.bitmap cardsprite_bottom.bitmap cardsprite_bg.bitmap
IMAGES := $(addprefix $(thisdir), $(IMAGES))

%.mono: %.bmp
	convert $^ MONO:$@

%.bitmap: %.mono
	$(thisdir)bitreverse.py < $< > $@

CLEANFILES += $(thisdir)*.bitmap $(thisdir)*.mono
