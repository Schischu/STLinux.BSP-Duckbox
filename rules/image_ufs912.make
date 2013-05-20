# -*-makefile-*-
#
# Copyright (C) 2003-2010 by the ptxdist project <ptxdist@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

SEL_ROOTFS-$(PTXCONF_IMAGE_UFS912)	+= $(IMAGEDIR)/ufs912.software.V1.00.B00.data

ifdef PTXCONF_IMAGE_UFS912

$(STATEDIR)/image_working_dir_prepared:
	@echo -n "Preparing... "
	cd $(image/work_dir);	\
	((								\
		echo -n "mkdir -p $(image/work_dir)/map ";	\
		echo -n "         $(image/work_dir)/map/fw ";	\
		echo -n "         $(image/work_dir)/map/root; ";	\
		echo -n 'for dir in `ls $(image/work_dir)/`; do cp -a $$(dir) $(image/work_dir)/map/root/; done; '; \
		echo -n "rm -rf $(image/work_dir)/map/root/boot/*; ";	\
		echo -n "cp -a $(image/work_dir)/boot/*.elf $(image/work_dir)/map/fw/; ";	\
		echo -n "cp -a $(image/work_dir)/boot/*.mvi $(image/work_dir)/map/fw/; ";	\
		echo -n "echo \"/dev/mtdblock3	/boot	jffs2	defaults	0	0\" >> $(image/work_dir)/map/root/etc/fstab" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) -s $(STATEDIR)/image_working_dir_prepared --
	
	@echo "done."
	@$(call touch)


# $MKFSJFFS2 -qnUfv -p0x800000 -e0x20000 -r $TMPFWDIR -o $CURDIR/mtd_fw.bin
$(IMAGEDIR)/mtd_fw.bin: $(STATEDIR)/image_working_dir_prepared $(STATEDIR)/host-mtd-utils.install.post
	@echo -n "Creating mtd_fw.bin from working dir... "
	@cd $(image/work_dir);								\
	(awk -F: $(DOPERMISSIONS) $(image/permissions) &&			\
	(								\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/mkfs.jffs2 ";	\
		echo -n "-d $(image/work_dir)/map/fw ";			\
		echo -n "-qnUf -p0x800000 -e0x20000 ";\
		echo  "-o $@" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

# $SUMTOOL -v -p -e 0x20000 -i $CURDIR/mtd_fw.bin -o $CURDIR/mtd_fw.sum.bin
$(IMAGEDIR)/mtd_fw.sum.bin: $(IMAGEDIR)/mtd_fw.bin
	@echo -n "Creating mtd_fw.sum.bin with summary... "
	@cd $(image/work_dir);						\
	((								\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/sumtool ";	\
		echo -n "-i $< ";					\
		echo -n "-p -e 0x20000 ";	\
		echo "-o $@" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

# $MKFSJFFS2 -qUfv -p0x4000000 -e0x20000 -r $TMPROOTDIR -o $CURDIR/mtd_root.bin
$(IMAGEDIR)/mtd_root.bin: $(STATEDIR)/image_working_dir_prepared $(STATEDIR)/host-mtd-utils.install.post
	@echo -n "Creating mtd_root.bin from working dir... "
	@cd $(image/work_dir);								\
	(awk -F: $(DOPERMISSIONS) $(image/permissions) &&			\
	(								\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/mkfs.jffs2 ";	\
		echo -n "-d $(image/work_dir)/map/root ";			\
		echo -n "-qnUf -p0x4000000 -e0x20000 ";\
		echo  "-o $@" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

# $SUMTOOL -v -p -e 0x20000 -i $CURDIR/mtd_root.bin -o $CURDIR/mtd_root.sum.bin
$(IMAGEDIR)/mtd_root.sum.bin: $(IMAGEDIR)/mtd_root.bin
	@echo -n "Creating mtd_root.sum.jffs2 with summary... "
	@cd $(image/work_dir);						\
	((								\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/sumtool ";	\
		echo -n "-i $< ";					\
		echo -n "-p -e 0x20000 ";	\
		echo "-o $@" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

$(IMAGEDIR)/uImage.bin: $(IMAGEDIR)/linuximage
	@echo -n "Creating uImage.bin... "
	@cd $(image/work_dir);						\
	((								\
		echo "cp $(IMAGEDIR)/linuximage $(IMAGEDIR)/uImage.bin" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) --
	@echo "done."

$(IMAGEDIR)/updatescript.sh:
	@echo -n "Creating updatescript.sh... "
	@cd $(image/work_dir);						\
	((								\
		echo "cp $(PTXDIST_WORKSPACE)/local_src/extra/ufs912/flash/updatescript.sh $(IMAGEDIR)/updatescript.sh" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) --
	@echo "done."


#$(image/work_dir)
$(IMAGEDIR)/ufs912.software.V1.00.B00.data: $(STATEDIR)/image_working_dir \
                                            $(IMAGEDIR)/uImage.bin        \
                                            $(IMAGEDIR)/mtd_root.sum.bin  \
                                            $(IMAGEDIR)/mtd_fw.sum.bin    \
                                            $(IMAGEDIR)/updatescript.sh   \
                                            $(STATEDIR)/host-mup.install.post
	@echo -n "Creating ufs912.software.V1.00.B00.data... "
	cd $(IMAGEDIR);						\
	echo -e "2\n0x00400000, 0x800000, 3, foo\n0x00C00000, 0x4000000, 3, foo\n0x00000000, 0x0, 1, uImage.bin\n0x00400000, 0x0, 1, mtd_fw.sum.bin\n0x00C00000, 0x0, 1, mtd_root.sum.bin\n;\n" > $@.cfg;	\
	((								\
		echo "$(PTXCONF_SYSROOT_HOST)/bin/mup c $@ < $@.cfg" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) --
	@echo -n "Cleaning image work_dir and files... "
	@cd $(IMAGEDIR);						\
	((								\
		echo -n "rm -rf  ";	\
		echo -n "       $(IMAGEDIR)/linuximage ";	\
		echo -n "       $(IMAGEDIR)/mtd_fw.bin ";	\
		echo -n "       $(IMAGEDIR)/mtd_root.bin ";	\
		echo -n "       $(STATEDIR)/image_working_dir_prepared ";	\
		echo "          $@.cfg" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) --
	@echo "done."
	
	@echo "-----------------------------------------------------------------------"
	@echo "To flash the created image copy the "
	@echo "updatescript.sh `basename $@` "
	@echo "file to your usb drive in the subfolder /kathrein/ufs912/"

endif

# vim600:set foldmethod=marker:
# vim600:set syntax=make:
