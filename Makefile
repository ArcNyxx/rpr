# rpr - rich presence daemon
# Copyright (C) 2022 ArcNyxx
# see LICENCE file for licensing information

.POSIX:

include config.mk

SRC = rpserv.c rpclnt.c util.c
HEAD = util.h
OBJ = $(SRC:.c=.o)

SRCSERV = rpserv.c util.c
SRCCLNT = rpclnt.c util.c

OBJSERV = $(SRCSERV:.c=.o)
OBJCLNT = $(SRCCLNT:.c=.o)

MAN = rpserv.1 rpclnt.1 rpr.1

all: rpserv rpclnt

$(OBJSERV): config.mk $(HEAD)
$(OBJCLNT): config.mk $(HEAD)

util.h:
	sed 's|SOCKET|"$(SOCKET)"|g' < util.def.h > util.h

.c.o:
	$(CC) -c $(CFLAGS) $<

rpserv: $(OBJSERV)
	$(CC) $(OBJSERV) -o $@ $(LDFLAGS)

rpclnt: $(OBJCLNT)
	$(CC) $(OBJCLNT) -o $@ $(LDFLAGS)

clean:
	rm -f rpserv rpclnt $(OBJ) util.h rpr-$(VERSION).tar.gz

dist: clean
	mkdir -p rpr-$(VERSION)
	cp -R README LICENCE Makefile config.mk $(SRC) $(MAN) rpr.sh \
		rpr-$(VERSION)
	tar -cf rpr-$(VERSION).tar rpr-$(VERSION)
	gzip rpr-$(VERSION).tar
	rm -rf rpr-$(VERSION)

install: all
	mkdir -p $(PREFIX)/bin $(MANPREFIX)/man1
	cp -f rpserv rpclnt $(PREFIX)/bin
	sed 's|SOCKET|$(SOCKET)|g' < rpr.sh > $(PREFIX)/bin/rpr
	chmod 755 $(PREFIX)/bin/rpserv $(PREFIX)/bin/rpr
	sed 's|SOCKET|$(SOCKET)|g;s|VERSION|$(VERSION)|g' < rpserv.1 \
		> $(MANPREFIX)/man1/rpserv.1
	sed 's|SOCKET|$(SOCKET)|g;s|VERSION|$(VERSION)|g' < rpclnt.1 > \
		$(MANPREFIX)/man1/rpclnt.1
	sed 's|VERSION|$(VERSION)|g' < rpr.1 > $(MANPREFIX)/man1/rpr.1
	chmod 644 $(MANPREFIX)/man1/rpserv.1 $(MANPREFIX)/man1/rpclnt.1 \
		$(MANPREFIX)/man1/rpr.1

uninstall: all
	rm -f $(PREFIX)/bin/rpserv $(PREFIX)/bin/rpclnt $(PREFIX)/bin/rpr \
		$(MANPREFIX)/man1/rpserv.1 $(MANPREFIX)/man1/rpclnt.1 \
		$(MANPREFIX)/man1/rpr.1

.PHONY: all clean dist install uninstall
