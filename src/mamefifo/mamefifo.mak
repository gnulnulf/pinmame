# only MAMEFIFO specific output files and rules
#

OBJDIRS += $(OBJ)/mamefifo

# add precompiler defines for MAMEFIFO80
DEFS += -DMAME_FIFO

# add objects for MAMEFIFO
MAMEFIFOOBJS = \
 $(OBJ)/mamefifo/mamefifo.o 

# MAMEFIFO functions
$(OBJ)/mamefifo/%.o: src/mamefifo/%.c
	@echo Compiling $<...
	$(CC) $(CDEFS) $(CCFLAGS) -c $< -o $@
