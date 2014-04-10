TARGETS=csvcut samplecsv
CFLAGS=-Wall -Wextra

all: $(TARGETS)

csvcut: csvcut.o

samplecsv: samplecsv.c

.PHONY: clean distclean touch

clean:
		$(RM) *.o

distclean:
		$(RM) *.o
		$(RM) $(TARGETS)
		$(RM) *.obj
		$(RM) *.lnk
		$(RM) *.exe

touch:
		touch -c *.c
		touch -c *.h
