# rpr - rich presence
# Copyright (C) 2022 ArcNyxx
# see LICENCE file for licensing information

.POSIX:

include config.mk

SRCS = rpserv.c util.c
OBJS = $(SRCS:.c=.o)

SRCC = rpclnt.c util.c
OBJC = $(SRCC:.c=.o)

HEAD = util.h

all: rpserv rpclnt

$(OBJS): config.mk $(HEAD)
$(OBJC): config.mk $(HEAD)

.c.o:
	$(CC) -c $(CFLAGS) $<

rpserv: $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

rpclnt: $(OBJC)
	$(CC) $(OBJC) -o $@ $(LDFLAGS)

clean:
	rm -f rpserv rpclnt $(OBJS) $(OBJC) rpr-$(VERSION).tar.gz

dist: clean
	mkdir -p rpr-$(VERSION)
	cp -R README LICENCE Makefile config.mk $(SRC1) $(SRC2) $(MAN) rpr.sh \
		rpr-$(VERSION)
	tar -cf rpr-$(VERSION).tar rpr-$(VERSION)
	gzip rpr-$(VERSION).tar
	rm -rf rpr-$(VERSION)

install: all
	mkdir -p $(PREFIX)/bin $(MANPREFIX)/man1
	cp -f rpserv rpclnt $(PREFIX)/bin
	cp -f rpr.sh $(PREFIX)/bin/rpr
	chmod 755 $(PREFIX)/bin/rpserv $(PREFIX)/bin/rpr

uninstall: all
	rm -f $(PREFIX)/bin/rpserv $(PREFIX)/bin/rpclnt $(PREFIX)/bin/rpr

.PHONY: all clean dist install uninstall
