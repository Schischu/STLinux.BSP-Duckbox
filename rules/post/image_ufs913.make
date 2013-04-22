# -*-makefile-*-
#
# Copyright (C) 2003-2010 by the ptxdist project <ptxdist@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

SEL_ROOTFS-$(PTXCONF_IMAGE_UFS913)	+= $(IMAGEDIR)/ufs913.software.V1.00.B00.data


$(STATEDIR)/image_working_dir_prepared:
	@echo -n "Preparing... "
	cd $(image/work_dir);	\
	((								\
		echo -n "mkdir -p $(image/work_dir)/map ";	\
		echo -n "         $(image/work_dir)/map/fw ";	\
		echo -n "         $(image/work_dir)/map/root; ";	\
		echo -n 'for dir in `ls $(image/work_dir)/`; do cp -a $$(dir) $(image/work_dir)/map/root/; done; '; \
		echo -n "rm -rf $(image/work_dir)/map/root/boot/*.elf; ";	\
		echo -n "cp -a $(image/work_dir)/boot/*.elf $(image/work_dir)/map/fw/; ";	\
		echo -n "echo \"/dev/mtdblock8	/boot	jffs2	defaults	0	0\" >> $(image/work_dir)/map/root/etc/fstab" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) -s $(IMAGEDIR)/image_working_dir_prepared.state --
	
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
	) | $(FAKEROOT) -i $(IMAGEDIR)/image_working_dir_prepared.state --
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
	) | $(FAKEROOT) -i $(IMAGEDIR)/image_working_dir_prepared.state --
	@echo "done."

# $MKFSJFFS2 -qnUfv -p0x7800000 -e0x20000 -r $TMPROOTDIR -o $CURDIR/mtd_root.bin
$(IMAGEDIR)/mtd_root.bin: $(STATEDIR)/image_working_dir_prepared $(STATEDIR)/host-mtd-utils.install.post
	@echo -n "Creating mtd_root.bin from working dir... "
	@cd $(image/work_dir);								\
	(awk -F: $(DOPERMISSIONS) $(image/permissions) &&			\
	(								\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/mkfs.jffs2 ";	\
		echo -n "-d $(image/work_dir)/map/root ";			\
		echo -n "-qnUf -p0x7800000 -e0x20000 ";\
		echo  "-o $@" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) -i $(IMAGEDIR)/image_working_dir_prepared.state --
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
	) | $(FAKEROOT) -i $(IMAGEDIR)/image_working_dir_prepared.state --
	@echo "done."

$(IMAGEDIR)/uImage.bin: $(IMAGEDIR)/linuximage
	@echo -n "Creating uImage.bin... "
	@cd $(image/work_dir);						\
	((								\
		echo "cp $(IMAGEDIR)/linuximage $(IMAGEDIR)/uImage.bin" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) --
	@echo "done."

$(STATEDIR)/image_working_dir_prepared2:
	@echo -n "Preparing... "
	@cd $(image/work_dir);	\
	((								\
		echo -n "mkdir -p $(image/work_dir)/map "; \
		echo -n "         $(image/work_dir)/map/tiny; ";	\
		echo -n "tar -C $(image/work_dir)/map/tiny -xf "; \
		echo -n "        $(PTXDIST_WORKSPACE)/local_src/extra/ufs913/flash/nor_root.tar.gz 2> /dev/null; ";	\
		echo -n "rm -rf $(image/work_dir)/map/tiny/app ";	\
		echo -n "       $(image/work_dir)/map/tiny/config ";	\
		echo -n "       $(image/work_dir)/map/tiny/data ";	\
		echo -n "       $(image/work_dir)/map/tiny/softwares ";	\
		echo -n "       $(image/work_dir)/map/tiny/etc/gtk-2.0 ";	\
		echo -n "       $(image/work_dir)/map/tiny/etc/rcS.d ";	\
		echo -n "       $(image/work_dir)/map/tiny/etc/directfbrc ";	\
		echo -n "       $(image/work_dir)/map/tiny/etc/fb.modes ";	\
		echo -n "       $(image/work_dir)/map/tiny/etc/gdk-pixbuf.loaders ";	\
		echo -n "       $(image/work_dir)/map/tiny/etc/profile ";	\
		echo -n "       $(image/work_dir)/map/tiny/etc/init.d/*; ";	\
		echo -n "cp $(PTXDIST_WORKSPACE)/local_src/extra/ufs913/flash/profile "; \
		echo -n "   $(image/work_dir)/map/tiny/etc/; ";	\
		echo -n "cp $(PTXDIST_WORKSPACE)/local_src/extra/ufs913/flash/rcS "; \
		echo -n "   $(image/work_dir)/map/tiny/etc/init.d/; ";	\
		echo -n "chmod 755 $(image/work_dir)/map/tiny/etc/init.d/rcS; ";	\
		echo -n "cp $(PTXDIST_WORKSPACE)/local_src/extra/ufs913/flash/update $(image/work_dir)/map/tiny/bin; ";	\
		echo -n "chmod 755 $(image/work_dir)/map/tiny/bin/update" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) -s $(IMAGEDIR)/image_working_dir_prepared2.state --
	
	@echo "done."
	@$(call touch)

# "$MKCRAMFS $TMPROOTDIR $CURDIR/mtd_tiny.bin"
$(IMAGEDIR)/mtd_tiny.bin: $(STATEDIR)/image_working_dir_prepared2 $(STATEDIR)/host-cramfs.install.post
	@echo -n "Creating mtd_tiny.bin from working dir... "
	@cd $(image/work_dir)/map/tiny;								\
	((								\
		echo -n "$(PTXCONF_SYSROOT_HOST)/bin/mkcramfs ";	\
		echo -n "$(image/work_dir)/map/tiny ";	\
		echo  "$@" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) -i $(IMAGEDIR)/image_working_dir_prepared2.state --
	@echo "done."

$(IMAGEDIR)/uImage_tiny.bin:
	@echo -n "Creating uImage_tiny.bin... "
	@cd $(image/work_dir);						\
	((								\
		echo "cp $(PTXDIST_WORKSPACE)/local_src/extra/ufs913/flash/uImage_rootmtd7 $(IMAGEDIR)/uImage_tiny.bin" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) --
	@echo "done."

#$(image/work_dir)
$(IMAGEDIR)/ufs913.software.V1.00.B00.data: $(STATEDIR)/image_working_dir \
                                            $(IMAGEDIR)/uImage.bin        \
                                            $(IMAGEDIR)/mtd_root.sum.bin  \
                                            $(IMAGEDIR)/mtd_fw.sum.bin    \
                                            $(IMAGEDIR)/uImage_tiny.bin   \
                                            $(IMAGEDIR)/mtd_tiny.bin      \
                                            $(STATEDIR)/host-mup.install.post
	@echo -n "Creating ufs913.software.V1.00.B00.data... "
	cd $(IMAGEDIR);						\
	echo -e "3\n0x00400000, 0x0, 0, uImage_tiny.bin\n0x00660000, 0x0, 0, mtd_tiny.bin\n;\n" > $@.cfg;	\
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
		echo -n "       $(IMAGEDIR)/mtd_tiny.bin ";	\
		echo -n "       $(IMAGEDIR)/uImage_tiny.bin ";	\
		echo -n "       $(IMAGEDIR)/image_working_dir_prepared.state ";	\
		echo -n "       $(IMAGEDIR)/image_working_dir_prepared2.state ";	\
		echo -n "       permissions ";	\
		echo "          $@.cfg" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) --
	@echo "done."
	
	@echo "-----------------------------------------------------------------------"
	@echo "To flash the created image copy the "
	@echo "uImage.bin mtd_fw.bin mtd_root.bin `basename $@` "
	@echo "file to your usb drive in the subfolder /kathrein/ufs913/"


# vim600:set foldmethod=marker:
# vim600:set syntax=make:
