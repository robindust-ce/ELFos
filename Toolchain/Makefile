# Makefile zum Toolchain bauen
# ============================
#
# Alles bauen (Toolchain (binutils, GCC) + QEMU + GDB + UBOOT (image, mkimage)):
#	make all
#
# Einzelne Dinge bauen:
# 	make toolchain
# 	make qemu
# 	make gdb
# 	make uboot
# 	make mkimage
#
# Aufräumen:
# 	make clean		-- Build-Verzeichnisse löschen
# 	make mrproper		-- wie clean, zusätzlich entpackte Quellen löschen
# 	make distclean		-- wie mrproper, zusätzlich die heruntergeladenen Archive löschen
#
# Um die Toolchain woanders zu installieren (anstatt $HOME/arm), muss die
# Umgebungsvariable PREFIX entsprechend gesetzt sein. Beispielsweise:
# 	PREFIX=/arm make all
#
# Um sich die ganzen Kommandos anzuschauen (quasi das HOWTO in Kompaktform),
# funktioniert z. B. sowas wie:
# 	 make -n -B all | grep -v "^touch"
#

# Standardinstallationspunkt der Toolchain
PREFIX ?= $(HOME)/arm

# Falls das Kompilieren fehlschlägt, weil capstone.h nicht gefunden wird
# kann dieser Fix aktiviert werden indem die flag auf "true" gesetzt wird
# Stellt sicher, dass die capstone library vorhanden ist
QEMU_CAPSTONE_FIX ?= false

# Kann auf false gesetzt werden um den qemu patch zum ausgeben der LED's
# zu deaktivieren.
QEMU_LED_PATCH ?= true

# Default-Target
all:

#
# Bestandteile der Toolchain
#

FETCH_FROM = $(X-$*)_URL

BINUTILS = binutils-2.35
BINUTILS_ARCHIVE = $(BINUTILS).tar.bz2
BINUTILS_URL = http://ftp.gnu.org/gnu/binutils
BINUTILS_CONFIGURE = ../$(BINUTILS)/configure --target=arm-none-eabi --prefix=$(PREFIX) --enable-interwork --disable-nls

GCC_VERSION = 10.2.0
GCC = gcc-$(GCC_VERSION)
GCC_ARCHIVE = $(GCC).tar.gz
GCC_URL = ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/$(GCC)
#GCC_URL = http://ftp.gnu.org/gnu/gcc/$(GCC)
GCC_CONFIGURE = ln -snf ../$(GMP) gmp && ln -snf ../$(MPFR) mpfr && ln -snf ../$(MPC) mpc && ../$(GCC)/configure --target=arm-none-eabi --prefix=$(PREFIX) --enable-interwork --enable-languages="c" --with-newlib --without-headers --disable-nls

GMP = gmp-6.2.0
GMP_ARCHIVE = $(GMP).tar.bz2
GMP_URL = https://gmplib.org/download/gmp/

MPFR = mpfr-4.1.0
MPFR_ARCHIVE = $(MPFR).tar.bz2
MPFR_URL = http://www.mpfr.org/$(MPFR)

MPC = mpc-1.1.0
MPC_ARCHIVE = $(MPC).tar.gz
MPC_URL = https://ftp.gnu.org/gnu/mpc/

NEWLIB = newlib-3.3.0
NEWLIB_ARCHIVE = $(NEWLIB).tar.gz
NEWLIB_URL = ftp://sourceware.org/pub/newlib
NEWLIB_CONFIGURE = ../$(NEWLIB)/configure --target=arm-none-eabi --prefix=$(PREFIX) --enable-interwork --disable-nls

UBOOT = u-boot-2020.10
UBOOT_ARCHIVE = $(UBOOT).tar.bz2
UBOOT_URL = ftp://ftp.denx.de/pub/u-boot

QEMU = qemu-5.1.0
QEMU_ARCHIVE = $(QEMU).tar.xz
QEMU_URL = http://download.qemu.org/
QEMU_CONFIGURE = ../$(QEMU)/configure --target-list="arm-softmmu" --prefix=$(PREFIX)
qemu-pool: QEMU_CONFIGURE += --disable-sdl --disable-bluez --disable-vnc --disable-gtk

GDB = gdb-9.2
GDB_ARCHIVE = $(GDB).tar.gz
GDB_URL = http://ftp.gnu.org/gnu/gdb
GDB_CONFIGURE = ../$(GDB)/configure --target=arm-none-eabi --prefix=$(PREFIX) --enable-interwork --disable-nls

# Einzelteile zusammensammeln
PACKAGES = $(PACKAGES_BUILD) $(PACKAGES_EXTRA)
PACKAGES_BUILD = BINUTILS GCC NEWLIB UBOOT QEMU GDB
PACKAGES_EXTRA = GMP MPFR MPC

ARCHIVES = $(foreach target,$(PACKAGES),$($(target)_ARCHIVE))

# ermöglicht Reverse-Lookup (so dass z. B. $(X-gdb-7.6) nach "GDB" umgesetzt wird)
$(foreach target,$(PACKAGES),$(eval X-$($(target)) = $(target)))


#
# Globale Einstellungen
#

THREADS := $(shell grep "^processor" /proc/cpuinfo | wc -l)
export PATH := $(PREFIX)/bin:$(PATH)


#
# Allgemeine Regeln
#

.PHONY: all all-pool toolchain gdb mkimage qemu qemu-pool
all: toolchain gdb qemu uboot mkimage
all-pool: toolchain gdb mkimage qemu-pool
toolchain: $(GCC)-build/.installed
gdb: $(GDB)-build/.installed
uboot: $(UBOOT)-build/.installed
mkimage: $(UBOOT)-build/.mki_installed
qemu: $(QEMU)-build/.installed
qemu-pool: qemu

.PHONY: clean mrproper distclean
clean:
	rm -Rf $(foreach target,$(PACKAGES_BUILD),$($(target))-build)
mrproper: clean
	rm -Rf $(foreach target,$(PACKAGES),$($(target)))
distclean: mrproper
	rm -Rf $(ARCHIVES)


#
# Generelle Regeln
#

.SECONDARY:

%-build/.installed: %-build/.built
	make -C $*-build install
	touch $@

%-build/.built: %-build/.configured
	make -C $*-build -j $(THREADS)
	touch $@

%-build/.configured: %/.unpacked
	mkdir -p $*-build
	cd $*-build && $($(X-$*)_CONFIGURE)
	touch $@

%/.unpacked: %.tar.gz
	tar xzf $<
	touch $@

%/.unpacked: %.tar.bz2
	tar xjf $<
	touch $@

%.patch: %.patch.gz
	gunzip -c $< > $@

%/.unpacked: %.tar.xz
	tar xJf $<
	touch $@

$(filter %.tar.bz2,$(ARCHIVES)): %.tar.bz2:
	wget -c --no-check-certificate $($(FETCH_FROM))/$($(X-$*)_ARCHIVE) -O $@.part
	mv $@.part $@

$(filter %.tar.gz,$(ARCHIVES)): %.tar.gz:
	wget -c --no-check-certificate $($(FETCH_FROM))/$($(X-$*)_ARCHIVE) -O $@.part
	mv $@.part $@

$(filter %.tar.xz,$(ARCHIVES)): %.tar.xz:
	wget -c --no-check-certificate $($(FETCH_FROM))/$($(X-$*)_ARCHIVE) -O $@.part
	mv $@.part $@

$(filter %.patch.gz,$(ARCHIVES)): %.gz:
	wget -c --no-check-certificate $($(FETCH_FROM))/$($(X-$*)_ARCHIVE) -O $@.part
	mv $@.part $@

#
# Spezielle Regeln
#

# GCC braucht Binutils und besteht aus mehreren Quellpaketen
$(GCC)-build/.configured: $(BINUTILS)-build/.installed $(GMP)/.unpacked $(MPFR)/.unpacked $(MPC)/.unpacked

# minimale GCC (wird benötigt, um Newlib bauen zu können, um wiederum GCC fertig zu bauen)
$(GCC)-build/.built-gcc: $(GCC)-build/.configured
	make -C $(GCC)-build -j $(THREADS) all-gcc
	touch $@

$(GCC)-build/.installed-gcc: $(GCC)-build/.built-gcc
	make -C $(GCC)-build install-gcc
	touch $@

# Newlib braucht minimale GCC
$(NEWLIB)-build/.configured: $(GCC)-build/.installed-gcc

# vollständige GCC braucht Newlib
$(GCC)-build/.built: $(NEWLIB)-build/.installed

# uboot/mkimage hat kein "normales" Makefile und teilen sich das archive
$(UBOOT)-build/.mki_installed: $(UBOOT)-build/.mki_built
	cp $(UBOOT)-build/tools/mkimage $(PREFIX)/bin
	touch $@

$(UBOOT)-build/.installed: $(UBOOT)-build/.built
	cp $(UBOOT)-build/u-boot.bin ./
	touch $@

$(UBOOT)-build/.configured: $(UBOOT)/.unpacked toolchain
	cp -r $(UBOOT) $(UBOOT)-build
	CROSS_COMPILE=$(PREFIX)/bin/arm-none-eabi- make -C $(UBOOT)-build rpi_2_defconfig
	touch $@

$(UBOOT)-build/.mki_built: $(UBOOT)-build/.configured
	CROSS_COMPILE=$(PREFIX)/bin/arm-none-eabi- make -C $(UBOOT)-build -j $(THREADS) tools-only
	touch $@

$(UBOOT)-build/.built: $(UBOOT)-build/.configured
	CROSS_COMPILE=$(PREFIX)/bin/arm-none-eabi- make -C $(UBOOT)-build -j $(THREADS) u-boot.bin
	touch $@

# Spezielles build target für Qemu wegen speziellen Konditionen
$(QEMU)-build/.built: $(QEMU)-build/.configured
ifeq ($(QEMU_CAPSTONE_FIX),true)
	echo "Applying capstone fix"
	find ./$(QEMU) -type f -exec sed -i 's/#include <capstone.h>/#include <capstone\/capstone.h>/g' {} +
endif
ifeq ($(QEMU_LED_PATCH),true)
	echo "Applaying qemu led patch"
	-patch -d $(QEMU) -p0 -N -r - < qemu-led.patch
endif
	make -C $(QEMU)-build -j $(THREADS)
	touch $@
