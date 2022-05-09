# rpr - rich presence
# Copyright (C) 2022 ArcNyxx
# see LICENCE file for licensing information

.POSIX:

include config.mk

SRC = rpserv.c
OBJ = $(SRC:.c=.o)

all: rpserv

$(OBJ): config.mk

.c.o:
	$(CC) -c $(CFLAGS) $<

rpserv: $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -f rpserv $(OBJ) rpr-$(VERSION).tar.gz

dist: clean
	mkdir -p rpr-$(VERSION)
	cp -R README LICENCE Makefile config.mk $(SRC) $(MAN) rpr.sh \
		rpr-$(VERSION)
	tar -cf rpr-$(VERSION).tar rpr-$(VERSION)
	gzip rpr-$(VERSION).tar
	rm -rf rpr-$(VERSION)

install: all
	mkdir -p $(PREFIX)/bin $(MANPREFIX)/man1
	cp -f rpserv $(PREFIX)/bin
	cp -f rpr.sh $(PREFIX)/bin/rpr
	chmod 755 $(PREFIX)/bin/rpserv $(PREFIX)/bin/rpr

uninstall: all
	rm -f $(PREFIX)/bin/rpserv $(PREFIX)/bin/rpr

.PHONY: all clean dist install uninstall
