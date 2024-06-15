#!/usr/bin/make -f
# Makefile for control-button-board.lv2 #
# ----------------------- #
# Created by falkTX
#

include Makefile.mk

# --------------------------------------------------------------

PREFIX  ?= /usr/local
DESTDIR ?=

# --------------------------------------------------------------
# Default target is to build all plugins

all: build

# --------------------------------------------------------------
# control-button-board build rules

build: control-button-board.lv2/control-button-board$(LIB_EXT) control-button-board.lv2/manifest.ttl

control-button-board.lv2/control-button-board$(LIB_EXT): control-button-board.c
	$(CC) $^ $(BUILD_C_FLAGS) $(LINK_FLAGS) -lm $(SHARED) -o $@

control-button-board.lv2/manifest.ttl: control-button-board.lv2/manifest.ttl.in
	sed -e "s|@LIB_EXT@|$(LIB_EXT)|" $< > $@

# --------------------------------------------------------------

clean:
	rm -f control-button-board.lv2/control-button-board$(LIB_EXT) control-button-board.lv2/manifest.ttl

# --------------------------------------------------------------

install: build
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/control-button-board.lv2
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/control-button-board.lv2/modgui
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/control-button-board.lv2/modgui/control-button-board_sprite_sheets
	install -m 644 control-button-board.lv2/*.so  $(DESTDIR)$(PREFIX)/lib/lv2/control-button-board.lv2/
	install -m 644 control-button-board.lv2/*.ttl $(DESTDIR)$(PREFIX)/lib/lv2/control-button-board.lv2/
	install -m 644 control-button-board.lv2/modgui/*.png $(DESTDIR)$(PREFIX)/lib/lv2/control-button-board.lv2/modgui
	install -m 644 control-button-board.lv2/modgui/*.html $(DESTDIR)$(PREFIX)/lib/lv2/control-button-board.lv2/modgui
	install -m 644 control-button-board.lv2/modgui/*.css $(DESTDIR)$(PREFIX)/lib/lv2/control-button-board.lv2/modgui
	install -m 644 control-button-board.lv2/modgui/control-button-board_sprite_sheets/*.png $(DESTDIR)$(PREFIX)/lib/lv2/control-button-board.lv2/modgui/control-button-board_sprite_sheets

# --------------------------------------------------------------
