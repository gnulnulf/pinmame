# only MAMESOCKET specific output files and rules
#


# add precompiler defines for MAMESOCKET
DEFS += -DMAME_SOCKET

MY_CFLAGS += -DMAME_SOCKET

# add objects for MAMESOCKET
MAMESOCKETOBJS = \
 $(OBJ)/mamesocket/mamesocket.o 

OBJDIRS += $(OBJ)/mamesocket
OBJS += $(MAMESOCKETOBJS)

# MAMEFIFO functions
$(OBJ)/mamesocket/%.o: src/mamesocket/%.c
	@echo Compiling $<...
	$(CC) $(CDEFS) $(CCFLAGS) -c $< -o $@


# $(OBJ)/mamesocket/mamefifo.o: src/unix/mamefifo.c
#	$(CC_COMMENT) @echo '[MAMEFIFO] Compiling $< ...'
#	$(CC_COMPILE) $(CC) $(MY_CFLAGS) -o $@ -c $<
