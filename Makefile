# sdrp - general discord rich presence
# Copyright (C) 2022 ArcNyxx
# see LICENCE file for licensing information

.POSIX:

include config.mk

SRC = setp.c
OBJ = $(SRC:.c=.o)
MAN = setp.1 sdrp.1

all: setp

$(OBJ): config.mk

.c.o:
	$(CC) -c $(CFLAGS) $<

setp: $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -f setp $(OBJ) sdrp-$(VERSION).tar.gz

dist: clean
	mkdir -p sdrp-$(VERSION)
	cp -R README LICENCE Makefile config.mk $(SRC) $(MAN) sdrp.sh \
		sdrp-$(VERSION)
	tar -cf sdrp-$(VERSION).tar sdrp-$(VERSION)
	gzip sdrp-$(VERSION).tar
	rm -rf sdrp-$(VERSION)

install: all
	mkdir -p $(PREFIX)/bin $(MANPREFIX)/man1
	cp -f setp $(PREFIX)/bin
	cp -f sdrp.sh $(PREFIX)/bin/sdrp
	chmod 755 $(PREFIX)/bin/setp $(PREFIX)/bin/sdrp
	cp -f setp.1 sdrp.1 $(MANPREFIX)/man1
	chmod 644 $(MANPREFIX)/man1/setp.1 $(MANPREFIX)/man1/sdrp.1

uninstall: all
	rm -f $(PREFIX)/bin/setp $(PREFIX)/bin/sdrp $(MANPREFIX)/man1/setp.1 \
		$(MANPREFIX)/man1/sdrp.1

.PHONY: all clean dist install uninstall
