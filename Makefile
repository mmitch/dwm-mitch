VERSION = 3.3git
DWM_VERSION = 4.7
DMENU_VERSION = 3.4
PREFIX = /usr/local

DWM = dwm-$(DWM_VERSION)
DWM_TAR = $(DWM).tar.gz
DMENU = dmenu-$(DMENU_VERSION)
DMENU_TAR = $(DMENU).tar.gz

SUBDIRS = $(DWM) $(DMENU)

export PATCHVERSION = mitch-${VERSION}

all:	fetch expand patch build install

.PHONY:	all fetch expand patch build install uninstall dist clean workspace-patch workspace-personal workspace personal

fetch:	stamp-fetched

expand:	stamp-expanded

patch:	stamp-patched

build:	stamp-built

stamp-fetched:
	wget -q -O$(DWM_TAR)   http://www.suckless.org/download/$(DWM_TAR)
	wget -q -O$(DMENU_TAR) http://www.suckless.org/download/$(DMENU_TAR)
	touch $@

stamp-expanded:	stamp-fetched
	tar xzf $(DWM_TAR)
	tar xzf $(DMENU_TAR)
	touch $@

stamp-patched:	stamp-expanded
	@for PATCH in ??_patch_dwm_*.diff ; do \
		echo applying $$PATCH ; \
		patch -p1 -d $(DWM) < $$PATCH || exit 1 ; \
	done
	@if [ "$$DWM_PERSOCON" ] ; then \
		echo applying personal patch from $$DWM_PERSOCON ; \
		patch -p1 -d $(DWM) < $$DWM_PERSOCON || exit 1 ; \
	fi
	@for PATCH in ??_patch_dmenu_*.diff ; do \
		echo applying $$PATCH ; \
		patch -p1 -d $(DMENU) < $$PATCH || exit 1 ; \
	done
	@if [ "$$DMENU_PERSOCON" ] ; then \
		echo applying personal patch from $$DMENU_PERSOCON ; \
		patch -p1 -d $(DMENU) < $$DMENU_PERSOCON || exit 1 ; \
	fi
	touch $@

stamp-built:	stamp-patched
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

uninstall:	stamp-expanded
	$(MAKE) -C $(DWM) uninstall
	$(MAKE) -C $(DMENU) uninstall
	rm -f ${DESTDIR}${PREFIX}/bin/dwm-mitch
	rm -f ${DESTDIR}${PREFIX}/bin/dwm-choose
	rm -f /usr/share/xsessions/dwm-mitch.desktop

clean:
	rm -rf $(DWM)/ $(DMENU)/
	rm -rf dwm-mitch-*
	rm -f $(DWM_TAR) $(DMENU_TAR)
	rm -f *~
	rm -f stamp-*

dist:	clean
	mkdir dwm-mitch-$(VERSION)
	-cp * dwm-mitch-$(VERSION)/
	tar -czvf dwm-mitch-$(VERSION).tar.gz dwm-mitch-$(VERSION)/
	rm -rf dwm-mitch-$(VERSION)/

workspace-patch:
	@(\
	set +e; \
	LAST=$(DWM); \
	for PATCH in ??_patch_dwm_*.diff; do \
		THISLAST=$$LAST; \
		PATCHDIR=`echo $${PATCH%%.diff} | sed "s/patch_dwm_[^_]*_/dwm-$(DWM_VERSION)_/"`; \
		LAST=$$PATCHDIR; \
		[ -d $$PATCHDIR ] && echo skipping $$PATCH && continue; \
		echo processing $$PATCH... ; \
		mkdir -p $$PATCHDIR; \
		cp $$THISLAST/* $$PATCHDIR/; \
		patch -N -p1 -d $$PATCHDIR < $$PATCH || exit 1; \
	done \
	)
	@( \
	set +e; \
	LAST=$(DMENU); \
	for PATCH in ??_patch_dmenu_*.diff; do \
		THISLAST=$$LAST; \
		PATCHDIR=`echo $${PATCH%%.diff} | sed "s/patch_dmenu_[^_]*_/dmenu-$(DMENU_VERSION)_/"`; \
		LAST=$$PATCHDIR; \
		[ -d $$PATCHDIR ] && echo skipping $$PATCH && continue; \
		echo processing $$PATCH... ; \
		mkdir -p $$PATCHDIR; \
		cp $$THISLAST/* $$PATCHDIR/; \
		patch -N -p1 -d $$PATCHDIR < $$PATCH || exit 1; \
	done \
	)

workspace-personal:
	@for PERSONAL in dwm_personal_configuration.*.diff; do \
		mkdir -p `echo $${PERSONAL%%.diff} | sed "s/^dwm/99_dwm-$(DWM_VERSION)/"`; \
	done
	@for PERSONAL in dmenu_personal_configuration.*.diff; do \
		mkdir -p `echo $${PERSONAL%%.diff} | sed "s/^dmenu/99_dmenu-$(DMENU_VERSION)/"`; \
	done

workspace:	clean expand workspace-patch

personal:	clean workspace-personal
	DWM_PERSOCON= DMENU_PERSOCON= make patch
	@for PERSONAL in dwm_personal_configuration.*.diff; do \
		echo expanding $$PERSONAL:; \
		TARGET=`echo $${PERSONAL%%.diff} | sed "s/^dwm/99_dwm-$(DWM_VERSION)/"`; \
		cp $(DWM)/* $$TARGET/; \
		patch -p1 -d $$TARGET < $$PERSONAL || exit 1; \
	done
	@for PERSONAL in dmenu_personal_configuration.*.diff; do \
		echo expanding $$PERSONAL:; \
		TARGET=`echo $${PERSONAL%%.diff} | sed "s/^dmenu/99_dmenu-$(DMENU_VERSION)/"`; \
		cp $(DMENU)/* $$TARGET/; \
		patch -p1 -d $$TARGET < $$PERSONAL || exit 1; \
	done
