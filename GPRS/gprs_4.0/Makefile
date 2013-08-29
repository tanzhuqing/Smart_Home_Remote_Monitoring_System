TOPDIR = ../
include $(TOPDIR)Rules.mak

EXTRA_LIBS += -lpthread

EXEC =  gprs ./gprs
OBJS =  main.o tty.o gprs.o ds1820.o keyshell.o $(TOPDIR)/keyboard/keyboard.o $(TOPDIR)/keyboard/get_key.o

all: $(EXEC)


$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)  $(EXTRA_LIBS) -liconv

install:
	$(EXP_INSTALL) $(EXEC) $(INSTALL_DIR)

clean:
	-rm -f $(EXEC) *.elf *.elf2flt *.gdb *.o
	-rm -f $(TOPDIR)/keyboard/*.o
