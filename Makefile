DISTVERSION =
PREFIX = /usr/local

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
	mkdir -p /usr/share/xsessions
	cp -f dwm-mitch.desktop /usr/share/xsessions/
	chmod 644 /usr/share/xsessions/dwm-mitch.desktop
# icon as pixmap for plain .xsession
	mkdir -p /usr/share/pixmaps
	cp -f dwm-mitch.png /usr/share/pixmaps/
	chmod 644 /usr/share/pixmaps/dwm-mitch.png
# icon for LightDM/Debian Jessie lightdm-gtk-greeter - strange filenames this one wants
	mkdir -p /usr/share/icons/hicolor/16x16/apps/
	cp -f dwm-mitch_badge-symbolic.png /usr/share/icons/hicolor/16x16/apps/
	chmod 644 /usr/share/icons/hicolor/16x16/apps/dwm-mitch_badge-symbolic.png
# icon for LightDM/Ubuntu Unity greeter - this one also needs strange names, but ok, it needs a round icon
	mkdir -p /usr/share/unity-greeter
	cp -f custom_dwm-mitch_badge.png /usr/share/unity-greeter/
	chmod 644 /usr/share/unity-greeter/custom_dwm-mitch_badge.png

uninstall:
	$(MAKE) -C $(DWM) uninstall
	$(MAKE) -C $(DMENU) uninstall
	rm -f ${DESTDIR}${PREFIX}/bin/dwm-mitch
	rm -f ${DESTDIR}${PREFIX}/bin/dwm-choose
# xsession
	rm -f /usr/share/xsessions/dwm-mitch.desktop
	-rmdir -p /usr/share/xsessions
# icon as pixmap for plain .xsession
	rm -f /usr/share/pixmaps/dwm-mitch.png
	-rmdir -p /usr/share/pixmaps
# icon for LightDM/Debian Jessie lightdm-gtk-greeter - strange filenames this one wants
	rm -f /usr/share/icons/hicolor/16x16/apps/dwm-mitch_badge-symbolic.png
	-rmdir -p /usr/share/icons/hicolor/16x16/apps
# icon for LightDM/Ubuntu Unity greeter - this one also needs strange names, but ok, it needs a round icon
	rm -f /usr/share/unity-greeter/custom_dwm-mitch_png.png
	-rmdir -p /usr/share/unity-greeter

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
