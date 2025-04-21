include ../config.mk

TARGETS := $(wildcard *.c)
TARGETS := $(basename $(TARGETS))
VARIABLE_IR := ./ir/

all: link_all

asm:
	$(foreach TARGET,$(TARGETS), \
	    $(CC) $(CFLAGS) $(TARGET).c -o $(VARIABLE_IR)$(TARGET).0.ll; \
		$(OPT) $(VENUSOPTFLAGS) $(VARIABLE_IR)$(TARGET).0.ll -o $(VARIABLE_IR)$(TARGET).split.ll; \
	    $(LLC) $(VENUSLLCFLAGS) $(VARIABLE_IR)$(TARGET).split.ll -o $(TARGET).s; \
	)

OBJECTS=$(addsuffix .s,$(TARGETS))

link_all: asm $(OBJECTS)
	$(CC) -v $(VENUSARCH) -o variable $(OBJECTS) -ffreestanding -nostdlib --std=gnu99 -Wl,-Bstatic,-T,variable.ld,-Map,variable.map,--strip-debug -lgcc 
	$(CPY) -O binary variable variable.bin 
	$(DUMP) $(VENUSDUMPFLAGS) variable > variable.txt 

map_variable:
	$(PYTHON) map_variable.py
	
clean:
	-$(RM) *.c  *.s *.map *.o *.bin *_asm.txt *.txt variable 
	$(RM) -rf $(VARIABLE_IR)