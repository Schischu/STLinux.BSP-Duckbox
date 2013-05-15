# -*-makefile-*-
#
# Copyright (C) @YEAR@ by @AUTHOR@
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DRIVER_FRONTEND_SPARK) += driver-frontend-spark

#
# Paths and names and versions
#
DRIVER_FRONTEND_SPARK_VERSION	:= 1.0
DRIVER_FRONTEND_SPARK		:= frontend-spark
DRIVER_FRONTEND_SPARK_URL	:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/frontends
DRIVER_FRONTEND_SPARK_DIR	:= $(BUILDDIR)/$(DRIVER_FRONTEND_SPARK)
DRIVER_FRONTEND_SPARK_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_FRONTEND_SPARK
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-frontend-spark.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ifdef PTXCONF_PLATFORM_SPARK
DRIVER_FRONTEND_SPARK_EXTRAS := SPARK=y EXTRA_CFLAGSS=-DSPARK
endif
ifdef PTXCONF_PLATFORM_SPARK7162
DRIVER_FRONTEND_SPARK_EXTRAS := SPARK7162=y EXTRA_CFLAGSS=-DSPARK7162
endif


$(STATEDIR)/driver-frontend-spark.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontend-spark.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTEND_SPARK_DIR) \
		TREE_ROOT=$(DRIVER_FRONTEND_SPARK_DIR) \
		$(DRIVER_FRONTEND_SPARK_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontend-spark.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_FRONTEND_SPARK_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTEND_SPARK_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_FRONTEND_SPARK_PKGDIR) \
		modules_install
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontend-spark.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-frontend-spark)
	@$(call install_fixup, driver-frontend-spark, PRIORITY,optional)
	@$(call install_fixup, driver-frontend-spark, SECTION,base)
	@$(call install_fixup, driver-frontend-spark, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-frontend-spark, DESCRIFRONTEND_SPARKON,missing)

	@cd $(DRIVER_FRONTEND_SPARK_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-frontend-spark, 0, 0, 0644, $(DRIVER_FRONTEND_SPARK_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-frontend-spark)

	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_FRONTEND_SPARK_INIT) += driver-frontend-spark-init

DRIVER_FRONTEND_SPARK_INIT_VERSION	:= head10

$(STATEDIR)/driver-frontend-spark-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,  driver-frontend-spark-init)
	@$(call install_fixup, driver-frontend-spark-init,PRIORITY,optional)
	@$(call install_fixup, driver-frontend-spark-init,SECTION,base)
	@$(call install_fixup, driver-frontend-spark-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-frontend-spark-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-frontend-spark-init, 0, 0, 0755, /etc/init.d/frontend-spark)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_FRONTEND_SPARK_BBINIT_LINK)),)
	@$(call install_link, driver-frontend-spark-init, \
		../init.d/frontend-spark, \
		/etc/rc.d/$(PTXCONF_DRIVER_FRONTEND_SPARK_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, driver-frontend-spark-init)
	
	@$(call touch)

# vim: syntax=make
