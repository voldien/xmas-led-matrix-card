#!/bin/bash

# Version
MAJOR := 0
MINOR := 0
PATCH := 9
STATE := a
VERSION := $(MAJOR).$(MINOR)$(STATE)$(PATCH)
# Utilitys
RM := rm -f
CP := cp
MKDIR := mkdir -p
CHMOD := chmod
# Destination
DESTDIR ?=
PREFIX ?= /usr
INSTALL_LOCATION=$(DESTDIR)$(PREFIX)
# Compiler
CC ?= cc

TARGET := christmascard

all :
	$(MAKE) -C firmware install

install : $(TARGET)
	$(MAKE) -C firmware install

distribution : $(TARGET)
	$(RM) -r $(TARGET)-$(VERSION)
	$(MKDIR) $(TARGET)-$(VERSION)
	$(CP) -r Makefile README.md  $(TARGET)-$(VERSION)
	tar cf - $(TARGET)-$(VERSION) | gzip -c > $(TARGET)-$(VERSION).tar.gz
	$(RM) -r $(TARGET)-$(VERSION)

clean :
	$(MAKE) -C firmware clean
	$(MAKE) -C driver/bin clean
	$(MAKE) -C driver/lib clean

.PHONY : $(TARGET) all install distribution clean
