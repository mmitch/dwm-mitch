DISTVERSION =
PREFIX = /usr/local
# for some files (eg. sessions and session images), /usr/local is not checked
PREFIXSHARE = /usr/share

# get version from git if available, otherwise take version from distribution tarball
VERSION = $(shell if [ -d .git ]; then git describe; else echo $(DISTVERSION); fi )

DWM = dwm
DMENU = dmenu
BINARIES = dwm-mitch dwm-choose

SUBDIRS = $(DWM) $(DMENU)

export PATCHVERSION = mitch-${VERSION}

all:	build install

.PHONY:	all build install uninstall dist clean chown

build:	stamp-built

stamp-built:
	$(MAKE) -C $(DWM)
	$(MAKE) -C $(DMENU)
	touch $@

install:	stamp-built
	$(MAKE) -C $(DWM) install
	$(MAKE) -C $(DMENU) install
	mkdir -p ${DESTDIR}${PREFIX}/bin
	for BINARY in $(BINARIES); do \
		cp -f $$BINARY ${DESTDIR}${PREFIX}/bin ; \
		chmod 755 ${DESTDIR}${PREFIX}/bin/$$BINARY ; \
	done
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
	for BINARY in $(BINARIES); do \
		rm -f ${DESTDIR}${PREFIX}/bin/$$BINARY ; \
	done
	-rmdir -p ${DESTDIR}${PREFIX}/bin
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

OWNER = mitch:mitch
export OWNER

chown:
	$(MAKE) -C $(DWM) chown
	$(MAKE) -C $(DMENU) chown
	chown $(OWNER) ${DESTDIR}${PREFIX}/bin/dwm-mitch
	chown $(OWNER) ${DESTDIR}${PREFIX}/bin/dwm-choose
	chown $(OWNER) ${DESTDIR}${PREFIXSHARE}/xsessions/dwm-mitch.desktop
	chown $(OWNER) ${DESTDIR}${PREFIXSHARE}/pixmaps/dwm-mitch.png
	chown $(OWNER) ${DESTDIR}${PREFIXSHARE}/icons/hicolor/16x16/apps/dwm-mitch_badge-symbolic.png
	chown $(OWNER) ${DESTDIR}${PREFIXSHARE}/unity-greeter/custom_dwm-mitch_badge.png
