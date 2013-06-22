# -*-makefile-*-
#
# Copyright (C) 2012 by fabricega
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIRC) += lirc

#
# Paths and names
#
LIRC_VERSION	:= 0.9.0
LIRC_MD5	:= b232aef26f23fe33ea8305d276637086
LIRC		:= lirc-$(LIRC_VERSION)
LIRC_SUFFIX	:= tar.bz2
LIRC_URL	:= $(call ptx/mirror, SF, lirc/$(LIRC).$(LIRC_SUFFIX))
LIRC_SOURCE	:= $(SRCDIR)/$(LIRC).$(LIRC_SUFFIX)
LIRC_DIR	:= $(BUILDDIR)/$(LIRC)
LIRC_LICENSE	:= GPLv2

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
LIRC_PATH := PATH=$(CROSS_PATH)

LIRC_ENV	:= \
	$(CROSS_ENV) \
	CFLAGS="-Os -D__KERNEL_STRICT_NAMES" \
	ARCH=$(PTXCONF_KERNEL_ARCH_STRING) \
	MAKEFLAGS=-j1

#
# autoconf
#
LIRC_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-shared \
	--disable-static \
	--without-x \
	--with-devdir=/dev \
	--with-moduledir=/lib/modules \
	--with-major=61 \
	--with-driver=userspace \
	--with-syslog=LOG_DAEMON \
	--enable-debug \
	--with-kerneldir=$(KERNEL_DIR)


$(STATEDIR)/lirc.prepare:
	@$(call targetinfo)
	@$(call clean, $(LIRC_DIR)/config.cache)
	cd $(LIRC_DIR) && \
		$(LIRC_PATH) $(LIRC_ENV) \
		autoreconf -i
	cd $(LIRC_DIR) && \
		$(LIRC_PATH) $(LIRC_ENV) \
		./configure $(LIRC_CONF_OPT)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/lirc.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIRC)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/lirc.install:
#	@$(call targetinfo)
#	@$(call world/install, LIRC)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/lirc.targetinstall:
	@$(call targetinfo)

	@$(call install_init, lirc)
	@$(call install_fixup, lirc,PRIORITY,optional)
	@$(call install_fixup, lirc,SECTION,base)
	@$(call install_fixup, lirc,AUTHOR,"fabricega")
	@$(call install_fixup, lirc,DESCRIPTION,missing)

	@$(call install_copy, lirc, 0, 0, 0755, -, /usr/sbin/lircd)

	@$(call install_lib, lirc, 0, 0, 0644, liblirc_client)

ifdef PTXCONF_LIRC_IRCAT
	@$(call install_copy, lirc, 0, 0, 0755, -, /usr/bin/ircat)
endif
ifdef PTXCONF_LIRC_IREXEC
	@$(call install_copy, lirc, 0, 0, 0755, -, /usr/bin/irexec)
endif
ifdef PTXCONF_LIRC_IRPTY
	@$(call install_copy, lirc, 0, 0, 0755, -, /usr/bin/irpty)
endif
ifdef PTXCONF_LIRC_IRRECORD
	@$(call install_copy, lirc, 0, 0, 0755, -, /usr/bin/irrecord)
endif
ifdef PTXCONF_LIRC_IRSEND
	@$(call install_copy, lirc, 0, 0, 0755, -, /usr/bin/irsend)
endif
ifdef PTXCONF_LIRC_IRW
	@$(call install_copy, lirc, 0, 0, 0755, -, /usr/bin/irw)
endif
ifdef PTXCONF_LIRC_LIRCRCD
	@$(call install_copy, lirc, 0, 0, 0755, -, /usr/bin/lircrcd)
endif
ifdef PTXCONF_LIRC_MODE2
	@$(call install_copy, lirc, 0, 0, 0755, -, /usr/bin/mode2)
endif
ifdef PTXCONF_LIRC_PRONTO2LIRC
	@$(call install_copy, lirc, 0, 0, 0755, -, /usr/bin/pronto2lirc)
endif
ifdef PTXCONF_LIRC_LIRCMD
	@$(call install_copy, lirc, 0, 0, 0755, -, /usr/sbin/lircmd)
endif
ifdef PTXCONF_LIRC_ETC_LIRCD_CONF
	@$(call install_alternative, lirc, 0, 0, 0755, /etc/lircd.conf)
endif
ifdef PTXCONF_LIRC_ETC_HARDW_CONF
	@$(call install_alternative, lirc, 0, 0, 0755, /etc/lirc/hardware.conf)
endif
ifdef PTXCONF_LIRC_UDEV_RULE
	@$(call install_alternative, lirc, 0, 0, 0755, /lib/udev/rules.d/10-irremote.rules)
endif

#	# install init script
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_LIRC_STARTSCRIPT
	@$(call install_alternative, lirc, 0, 0, 0755, /etc/init.d/lirc)

ifneq ($(call remove_quotes,$(PTXCONF_LIRC_BBINIT_LINK)),)
	@$(call install_link, lirc, \
		../init.d/lirc, \
		/etc/rc.d/$(PTXCONF_LIRC_BBINIT_LINK))
endif
endif
endif

	@$(call install_finish, lirc)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/lirc.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, LIRC)

# vim: syntax=make