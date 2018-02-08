# Examples are built for 32 or 64 bit based on the bitness of the OS.
# Uncomment the line starting with "# flags" to always build for 32 bit.

nilibs=-lnidaqmxbase
cc=gcc
# flags= -O2 -m32

ifeq ($(shell uname -m),x86_64)
   flags?= -O2 -m64
else
   flags?= -O2 -m32
endif

ai_examples = DAQ const_DAQ

all : $(ai_examples)

% : %.c
	$(cc) $(flags) $< $(nilibs) -o $@

clean :
	rm -f $(ai_examples)
