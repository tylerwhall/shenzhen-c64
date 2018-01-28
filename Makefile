export CC65_HOME = /usr/share/cc65
CC65_TARGET = c64


SOURCES = test.o kernal.o
PROGRAM = test

ifdef CC65_TARGET
CC      = cl65
ASFLAGS = -t $(CC65_TARGET)
CFLAGS  = -t $(CC65_TARGET) --create-dep $(<:.c=.d) -Oirs -g -l $(PROGRAM).lst
LDFLAGS = -t $(CC65_TARGET) -m $(PROGRAM).map -vm -Ln $(PROGRAM).lbl
else
CC      = gcc
CFLAGS  = -MMD -MP -O
LDFLAGS = -Wl,-Map,$(PROGRAM).map
endif

########################################

.SUFFIXES:
.PHONY: all clean dis run
all: $(PROGRAM)

ifneq ($(MAKECMDGOALS),clean)
-include $(SOURCES:.o=.d)
endif

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.s
	$(CC) -c $(ASFLAGS) -o $@ $<

$(PROGRAM): $(SOURCES)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(SOURCES) $(SOURCES:.o=.d) $(PROGRAM) $(PROGRAM).map *.lst *.lbl

dis: $(PROGRAM)
	da65 $(PROGRAM)

run: $(PROGRAM)
	x64 -moncommands cmds.txt test