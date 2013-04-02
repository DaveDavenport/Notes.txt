PREFIX=$(HOME)/.local/


INCLUDE_FILES=$(wildcard includes/*.inc)
PROGRAM_FILE=notes.sh
CONFIG_FILE=config.inc


INCLUDE_DEST=$(foreach FL, $(INCLUDE_FILES), $(PREFIX)/share/notes/$(FL))
PROGRAM_FILE_DEST=$(PREFIX)/bin/$(PROGRAM_FILE)
CONFIG_DEST=$(HOME)/.notes.config

all:

$(PREFIX)/share/notes/includes/%.inc: includes/%.inc
	install -C -D $^ $@	

$(PROGRAM_FILE_DEST): $(PROGRAM_FILE)
	install -C $^ $@

$(CONFIG_DEST): $(CONFIG_FILE)
	install -c $^ $@
	sed -i "s|^PREFIX=.*$$|PREFIX=${PREFIX}|g" $@

install: $(INCLUDE_DEST) $(PROGRAM_FILE_DEST) $(CONFIG_DEST)


uninstall:
	rm -f $(INCLUDE_DEST) $(PROGRAM_FILE_DEST)

