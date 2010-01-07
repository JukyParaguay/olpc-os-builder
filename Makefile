export VERSION=$(shell grep "^VERSION=" osbuilder.py | sed 's/^VERSION="\(.*\)"/\1/')
export docdir=$(DESTDIR)/usr/share/doc/olpc-os-builder-$(VERSION)

all: bin_

bin: bin_

bin_:
	$(MAKE) -C bin

install: all
	install -D -m 0755 osbuilder.py $(DESTDIR)/usr/sbin/olpc-os-builder
	sed -i -e 's/^INSTALLED=0/INSTALLED=1/' $(DESTDIR)/usr/sbin/olpc-os-builder
	install -d $(docdir)
	install -m 0644 -t $(docdir) COPYING doc/README doc/README.devel
	$(MAKE) -C bin install
	$(MAKE) -C lib install
	$(MAKE) -C examples install
	$(MAKE) -C modules install
.PHONY: install

dist:
	git archive --prefix=olpc-os-builder-$(VERSION)/ HEAD | bzip2 > olpc-os-builder-$(VERSION).tar.bz2
.PHONY: dist

clean:
	make -C bin clean
.PHONY: clean
