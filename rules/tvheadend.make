# -*-makefile-*-
#
# Copyright (C) 2003 by Robert Schwebel <r.schwebel@pengutronix.de>
#                       Pengutronix <info@pengutronix.de>, Germany
#               2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_TVHEADEND) += tvheadend

#
# Paths and names
#
TVHEADEND_VERSION	:= 6c9bff1179ef0dcdbb995e0d26c008fd1b6aeb06
TVHEADEND		:= tvheadend-$(TVHEADEND_VERSION)
TVHEADEND_URL	:= git://github.com/tvheadend/tvheadend
TVHEADEND_GIT_BRANCH	:= master
TVHEADEND_GIT_HEAD	:= $(TVHEADEND_VERSION)
TVHEADEND_SOURCE	:= $(SRCDIR)/tvheadend.git
TVHEADEND_DIR	:= $(BUILDDIR)/$(TVHEADEND)
TVHEADEND_LICENSE	:= GPLv3

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
TVHEADEND_CFLAGS	:= $(CROSS_CFLAGS)
TVHEADEND_CFLAGS	+= $(CROSS_CPPFLAGS)
TVHEADEND_ENV		:= $(CROSS_ENV) CFLAGS="$(TVHEADEND_CFLAGS)" LDFLAGS="$(CROSS_LDFLAGS)"

#
# autoconf
#
TVHEADEND_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--cc=$(CROSS_CC) \
	--enable-bundle \
	--platform=linux

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tvheadend.targetinstall:
	@$(call targetinfo)

	@$(call install_init, tvheadend)
	@$(call install_fixup, tvheadend,PRIORITY,optional)
	@$(call install_fixup, tvheadend,SECTION,base)
	@$(call install_fixup, tvheadend,AUTHOR,"fabricega")
	@$(call install_fixup, tvheadend,DESCRIPTION,missing)

	@$(call install_copy, tvheadend, 0, 0, 0755, -, /usr/bin/tvheadend)

	@cd $(TVHEADEND_PKGDIR)/usr/share/tvheadend/data && \
	find . -type f | while read file; do \
		$(call install_copy, tvheadend, 0, 0, 644, \
		$(TVHEADEND_PKGDIR)/usr/share/tvheadend/data/$$file, \
			/usr/share/tvheadend/data/$$file); \
	done

	@cd $(TVHEADEND_PKGDIR)/usr/share/tvheadend/docs && \
	find . -type f | while read file; do \
		$(call install_copy, tvheadend, 0, 0, 644, \
		$(TVHEADEND_PKGDIR)/usr/share/tvheadend/docs/$$file, \
			/usr/share/tvheadend/docs/$$file); \
	done

	@cd $(TVHEADEND_PKGDIR)/usr/share/tvheadend/src && \
	find . -type f | while read file; do \
		$(call install_copy, tvheadend, 0, 0, 644, \
		$(TVHEADEND_PKGDIR)/usr/share/tvheadend/src/$$file, \
			/usr/share/tvheadend/src/$$file); \
	done

ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_TVHEADEND_STARTSCRIPT
	@$(call install_alternative, tvheadend, 0, 0, 0755, /etc/init.d/tvheadend)

ifneq ($(call remove_quotes,$(PTXCONF_TVHEADEND_BBINIT_STARTLINK)),)
	@$(call install_link, tvheadend, \
		../init.d/tvheadend, \
		/etc/rc.d/$(PTXCONF_TVHEADEND_BBINIT_STARTLINK))
endif
ifneq ($(call remove_quotes,$(PTXCONF_TVHEADEND_BBINIT_STOPLINK)),)
	@$(call install_link, tvheadend, \
		../init.d/tvheadend, \
		/etc/rc.d/$(PTXCONF_TVHEADEND_BBINIT_STOPLINK))
endif
endif
endif

	@$(call install_finish, tvheadend)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/tvheadend.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, TVHEADEND)

# vim: syntax=make
