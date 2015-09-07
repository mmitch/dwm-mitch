DISTVERSION =
PREFIX = /usr/local
# for some files (eg. sessions and session images), /usr/local is not checked
PREFIXSHARE = /usr/share

# get version from git if available, otherwise take version from distribution tarball
VERSION = $(shell if [ -d .git ]; then git describe; else echo $(DISTVERSION); fi )

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
# xsession
	mkdir -p ${DESTDIR}${PREFIXSHARE}/xsessions
	cp -f dwm-mitch.desktop ${DESTDIR}${PREFIXSHARE}/xsessions/
	chmod 644 ${DESTDIR}${PREFIXSHARE}/xsessions/dwm-mitch.desktop
# icon as pixmap for plain .xsession
	mkdir -p ${DESTDIR}${PREFIXSHARE}/pixmaps
	cp -f dwm-mitch.png ${DESTDIR}${PREFIXSHARE}/pixmaps/
	chmod 644 ${DESTDIR}${PREFIXSHARE}/pixmaps/dwm-mitch.png
# icon for LightDM/Debian Jessie lightdm-gtk-greeter - strange filenames this one wants
# try to just rename the 32x32 icon
	mkdir -p ${DESTDIR}${PREFIXSHARE}/icons/hicolor/16x16/apps/
	cp -f dwm-mitch.png ${DESTDIR}${PREFIXSHARE}/icons/hicolor/16x16/apps/dwm-mitch_badge-symbolic.png
	chmod 644 ${DESTDIR}${PREFIXSHARE}/icons/hicolor/16x16/apps/dwm-mitch_badge-symbolic.png
# icon for LightDM/Ubuntu Unity greeter - this one also needs strange names, but ok, it needs a round icon
	mkdir -p ${DESTDIR}${PREFIXSHARE}/unity-greeter
	cp -f custom_dwm-mitch_badge.png ${DESTDIR}${PREFIXSHARE}/unity-greeter/
	chmod 644 ${DESTDIR}${PREFIXSHARE}/unity-greeter/custom_dwm-mitch_badge.png

uninstall:
	$(MAKE) -C $(DWM) uninstall
	$(MAKE) -C $(DMENU) uninstall
	rm -f ${DESTDIR}${PREFIX}/bin/dwm-mitch
	rm -f ${DESTDIR}${PREFIX}/bin/dwm-choose
	-rmdir -p ${DESTDIR}${PREFIX}/bin
	-rmdir -p ${DESTDIR}${PREFIX}/share/man/man1
# xsession
	rm -f ${DESTDIR}${PREFIXSHARE}/xsessions/dwm-mitch.desktop
	-rmdir -p ${DESTDIR}${PREFIXSHARE}/xsessions
# icon as pixmap for plain .xsession
	rm -f ${DESTDIR}${PREFIXSHARE}/pixmaps/dwm-mitch.png
	-rmdir -p ${DESTDIR}${PREFIXSHARE}/pixmaps
# icon for LightDM/Debian Jessie lightdm-gtk-greeter - strange filenames this one wants
	rm -f ${DESTDIR}${PREFIXSHARE}/icons/hicolor/16x16/apps/dwm-mitch_badge-symbolic.png
	-rmdir -p ${DESTDIR}${PREFIXSHARE}/icons/hicolor/16x16/apps
# icon for LightDM/Ubuntu Unity greeter - this one also needs strange names, but ok, it needs a round icon
	rm -f ${DESTDIR}${PREFIXSHARE}/unity-greeter/custom_dwm-mitch_badge.png
	-rmdir -p ${DESTDIR}${PREFIXSHARE}/unity-greeter

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
	sed -e 's/^DISTVERSION.*/DISTVERSION = $(VERSION)/' < Makefile > dwm-mitch-$(VERSION)/Makefile
	-cp -R dwm/ dmenu/ dwm-mitch-$(VERSION)/
	tar -czvf dwm-mitch-$(VERSION).tar.gz dwm-mitch-$(VERSION)/
	rm -rf dwm-mitch-$(VERSION)/
