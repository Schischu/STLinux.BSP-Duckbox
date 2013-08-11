# -*-makefile-*-
#
# Copyright (C) 2013 by schpuntik <schpuntik@freenet.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DEVINIT) += devinit

DEVINIT_VERSION	:= 0.1

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/devinit.targetinstall:
	@$(call targetinfo)
#
# TODO: To build your own package, if this step requires one
	@$(call install_init, devinit)
	@$(call install_fixup,devinit,PRIORITY,optional)
	@$(call install_fixup,devinit,SECTION,base)
	@$(call install_fixup,devinit,AUTHOR,"schpuntik <schpuntik@freenet.de>")
	@$(call install_fixup,devinit,DESCRIPTION,missing)
#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish

	@$(call install_alternative, devinit, 0, 0, 0755, /bin/devinit)

	@$(call install_finish,devinit)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/devinit.clean:
	@$(call targetinfo)
	@$(call clean_pkg, DEVINIT)

##############################################################################

PACKAGES-$(PTXCONF_VDSTANDBY) += vdstanby

VDSTANDBY_VERSION	:= 0.1


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/vdstanby.targetinstall:
	@$(call targetinfo)
#
# TODO: To build your own package, if this step requires one
	@$(call install_init, vdstanby)
	@$(call install_fixup,vdstanby,PRIORITY,optional)
	@$(call install_fixup,vdstanby,SECTION,base)
	@$(call install_fixup,vdstanby,AUTHOR,"schpuntik <schpuntik@freenet.de>")
	@$(call install_fixup,vdstanby,DESCRIPTION,missing)
#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish

	@$(call install_alternative, vdstanby, 0, 0, 0755, /bin/vdstandby)

	@$(call install_finish,vdstanby)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/vdstanby.clean:
	@$(call targetinfo)
	@$(call clean_pkg, VDSTANDBY)


##############################################################################


PACKAGES-$(PTXCONF_MOTD) += motd

MOTD_VERSION	:= 0.1


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/motd.targetinstall:
	@$(call targetinfo)
#
# TODO: To build your own package, if this step requires one
	@$(call install_init, motd)
	@$(call install_fixup,motd,PRIORITY,optional)
	@$(call install_fixup,motd,SECTION,base)
	@$(call install_fixup,motd,AUTHOR,"schpuntik <schpuntik@freenet.de>")
	@$(call install_fixup,motd,DESCRIPTION,missing)
#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish

	@$(call install_alternative, motd, 0, 0, 0644, /etc/motd)

	@$(call install_finish,motd)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/motd.clean:
	@$(call targetinfo)
	@$(call clean_pkg, MOTD)


# vim: syntax=make
