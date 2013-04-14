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


$(STATEDIR)/image_working_dir_prepared: $(STATEDIR)/image_working_dir
	@echo -n "Preparing... "
	@cd $(image/work_dir);	\
	@mkdir -p $(image/work_dir)/map	\
	@mkdir -p $(image/work_dir)/map/fw	\
	@mkdir -p $(image/work_dir)/map/root	\
	@cp -a `ls $(image/work_dir)/ | grep -v map` $(image/work_dir)/map/root/	\
	@rm -rf $(image/work_dir)/map/root/boot/*.elf	\
	@cp -a ls $(image/work_dir)/boot/*.elf $(image/work_dir)/map/fw/	\
	echo "/dev/mtdblock8	/boot	jffs2	defaults	0	0" >> $(image/work_dir)/map/root/etc/fstab
	
	@echo "done."
	@$(call touch)


# $MKFSJFFS2 -qnUfv -p0x800000 -e0x20000 -r $TMPFWDIR -o $CURDIR/mtd_fw.bin
$(IMAGEDIR)/mtd_fw.jffs2: $(STATEDIR)/image_working_dir_prepared $(STATEDIR)/host-mtd-utils.install.post
	@echo -n "Creating root.jffs2 from working dir... "
	@cd $(image/work_dir);								\
	(awk -F: $(DOPERMISSIONS) $(image/permissions) &&			\
	(								\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/mkfs.jffs2 ";	\
		echo -n "-d $(image/work_dir/map/fw) ";			\
		echo -n "-qnUfv -p0x800000 -e0x20000 ";\
		echo  "-o $@" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) --
	@echo "done."

# $SUMTOOL -v -p -e 0x20000 -i $CURDIR/mtd_fw.bin -o $CURDIR/mtd_fw.sum.bin
$(IMAGEDIR)/mtd_fw.sum.jffs2: $(IMAGEDIR)/mtd_fw.jffs2
	@echo -n "Creating root.sum.jffs2 with summary... "
	@cd $(image/work_dir);						\
	((								\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/sumtool ";	\
		echo -n "-i $< ";					\
		echo -n "-v -p -e 0x20000 ";	\
		echo "-o $@" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) --
	@echo "done."

# $MKFSJFFS2 -qnUfv -p0x7800000 -e0x20000 -r $TMPROOTDIR -o $CURDIR/mtd_root.bin
$(IMAGEDIR)/mtd_root.jffs2: $(STATEDIR)/image_working_dir_prepared $(STATEDIR)/host-mtd-utils.install.post
	@echo -n "Creating root.jffs2 from working dir... "
	@cd $(image/work_dir);								\
	(awk -F: $(DOPERMISSIONS) $(image/permissions) &&			\
	(								\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/mkfs.jffs2 ";	\
		echo -n "-d $(image/work_dir/map/root) ";			\
		echo -n "-qnUfv -p0x7800000 -e0x20000 ";\
		echo  "-o $@" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) --
	@echo "done."

# $SUMTOOL -v -p -e 0x20000 -i $CURDIR/mtd_root.bin -o $CURDIR/mtd_root.sum.bin
$(IMAGEDIR)/mtd_root.sum.jffs2: $(IMAGEDIR)/mtd_root.jffs2
	@echo -n "Creating root.sum.jffs2 with summary... "
	@cd $(image/work_dir);						\
	((								\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/sumtool ";	\
		echo -n "-i $< ";					\
		echo -n "-v -p -e 0x20000 ";	\
		echo "-o $@" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) --
	@echo "done."

$(IMAGEDIR)/uImage.bin: $(IMAGEDIR)/linuximage
	@mv $(IMAGEDIR)/linuximage $(IMAGEDIR)/uImage.bin

$(STATEDIR)/image_working_dir_prepared2: $(STATEDIR)/image_working_dir
	@echo -n "Preparing... "
	@cd $(image/work_dir);	\
	@mkdir -p $(image/work_dir)/map	\
	@mkdir -p $(image/work_dir)/map/tiny	\
	tar -C $(image/work_dir)/map/tiny -xf $(PTXDIST_WORKSPACE)/local_src/extra/ufs913/flash/nor_root.tar.gz 2> /dev/null	\
		\
	rm -rf $(image/work_dir)/map/tiny/app	\
	rm -rf $(image/work_dir)/map/tiny/config	\
	rm -rf $(image/work_dir)/map/tiny/data	\
	rm -rf $(image/work_dir)/map/tiny/softwares	\
		\
	sudo rm -rf $(image/work_dir)/map/tiny/etc/gtk-2.0	\
	rm -rf $(image/work_dir)/map/tiny/etc/rcS.d	\
		\
	rm -rf $(image/work_dir)/map/tiny/etc/directfbrc	\
	rm -rf $(image/work_dir)/map/tiny/etc/fb.modes	\
	rm -rf $(image/work_dir)/map/tiny/etc/gdk-pixbuf.loaders	\
	rm -rf $(image/work_dir)/map/tiny/etc/profile	\
		\
	rm -rf $(image/work_dir)/map/tiny/etc/init.d/*	\
	\
	cp $(PTXDIST_WORKSPACE)/local_src/extra/ufs913/flash/profile $(image/work_dir)/map/tiny/etc/	\
	cp $(PTXDIST_WORKSPACE)/local_src/extra/ufs913/flash/rcS $(image/work_dir)/map/tiny/etc/init.d/	\
	chmod 755 $(image/work_dir)/map/tiny/etc/init.d/rcS	\
	cp $(PTXDIST_WORKSPACE)/local_src/extra/ufs913/flash/update $(image/work_dir)/map/tiny/bin	\
	chmod 755 $(image/work_dir)/map/tiny/bin/update	\

	@echo "done."
	@$(call touch)

# "$MKCRAMFS $TMPROOTDIR $CURDIR/mtd_tiny.bin"
$(IMAGEDIR)/mtd_tiny.bin: $(STATEDIR)/image_working_dir_prepared2 $(STATEDIR)/host-mtd-utils.install.post
	@echo -n "Creating root.jffs2 from working dir... "
	@cd $(image/work_dir/map/root);								\
	(awk -F: $(DOPERMISSIONS) $(image/permissions) &&			\
	(								\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/mkfs.cramfs ";	\
		echo  "-o $@" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) --
	@echo "done."

$(IMAGEDIR)/uImage_tiny.bin:
	@cp $(PTXDIST_WORKSPACE)/local_src/extra/ufs913/flash/uImage_rootmtd7 $(IMAGEDIR)/uImage_tiny.bin

$(IMAGEDIR)/ufs913.software.V1.00.B00.data: $(IMAGEDIR)/mtd_fw.sum.bin   \
                                            $(IMAGEDIR)/mtd_root.sum.bin \
                                            $(IMAGEDIR)/uimage.bin       \
                                            $(IMAGEDIR)/mtd_tiny.bin     \
                                            $(IMAGEDIR)/uimage_tiny.bin
	@echo -n "Creating ufs913.software.V1.00.B00.data... "
	@cd $(image/work_dir/tiny);						\
	((								\
		echo "$(PTXCONF_SYSROOT_HOST)/sbin/mup c $@ << EOF"	\
		echo "3"	\
		echo "0x00400000, 0x0, 0, uImage_tiny.bin"	\
		echo "0x00660000, 0x0, 0, mtd_tiny.bin"	\
		echo ";"	\
		echo "EOF" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) --
	@echo "done."


# vim600:set foldmethod=marker:
# vim600:set syntax=make:
