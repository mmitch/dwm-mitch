VERSION = 3.7
PREFIX = /usr/local

DWM = dwm
DMENU = dmenu

SUBDIRS = $(DWM) $(DMENU)

export PATCHVERSION = mitch-${VERSION}

all:	build install

.PHONY:	all build install uninstall dist clean

build:	stamp-built

stamp-built:
	$(MAKE) -C $(DWM)
	$(MAKE) -C $(DMENU)
	touch $@

install:	stamp-built
	$(MAKE) -C $(DWM) install
	$(MAKE) -C $(DMENU) install
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f dwm-mitch ${DESTDIR}${PREFIX}/bin
	cp -f dwm-choose ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/dwm-mitch
	chmod 755 ${DESTDIR}${PREFIX}/bin/dwm-choose
	-[ -d /usr/share/xsessions ] && cp -f dwm-mitch.desktop /usr/share/xsessions/ && chmod 644 /usr/share/xsessions/dwm-mitch.desktop

uninstall:
	$(MAKE) -C $(DWM) uninstall
	$(MAKE) -C $(DMENU) uninstall
	rm -f ${DESTDIR}${PREFIX}/bin/dwm-mitch
	rm -f ${DESTDIR}${PREFIX}/bin/dwm-choose
	rm -f /usr/share/xsessions/dwm-mitch.desktop

clean:
	$(MAKE) -C $(DWM) clean
	$(MAKE) -C $(DMENU) clean
	rm -rf dwm-mitch-*
	rm -f *~
	rm -f $(DWM)/*~ $(DWM)/config.h
	rm -f $(DMENU)/*~
	rm -f stamp-*

dist:	clean
	mkdir dwm-mitch-$(VERSION)
	-cp * dwm-mitch-$(VERSION)/
	tar -czvf dwm-mitch-$(VERSION).tar.gz dwm-mitch-$(VERSION)/
	rm -rf dwm-mitch-$(VERSION)/
