moddir=$(DESTDIR)/usr/share/olpc-os-builder/modules.d
moddocdir=$(docdir)/modules
modules=$(filter-out Makefile,$(wildcard *))

all:

$(moddir)/%: $(moddocdir)
	install -d $@
	install -m 0644 -t $@ $(filter-out %/README,$(wildcard $(notdir $@)/*))
	install -m 0644 -T $(notdir $@)/README $(moddocdir)/$(notdir $@).txt

$(moddocdir):
	install -d $(moddocdir)

install: all $(addprefix $(moddir)/,$(modules))

