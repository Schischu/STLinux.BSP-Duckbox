# -*-makefile-*-
#
# Copyright (C) 2003-2010 by the ptxdist project <ptxdist@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

SEL_ROOTFS-$(PTXCONF_IMAGE_ATEVIO7500)	+= $(IMAGEDIR)/update.ird

ifdef PTXCONF_IMAGE_ATEVIO7500

$(STATEDIR)/image_working_dir_prepared:
	@echo -n "Preparing... "
	cd $(image/work_dir);																	\
	((																						\
		echo -n "mkdir -p $(image/work_dir)/map ";											\
		echo -n "         $(image/work_dir)/map/fw ";										\
		echo -n "         $(image/work_dir)/map/root; ";									\
		echo -n 'rsync -a $(image/work_dir)/* $(image/work_dir)/map/root/ --exclude map; ';	\
		echo -n "rm -rf $(image/work_dir)/map/root/boot/*; ";								\
		echo -n "cp -a $(image/work_dir)/boot/*.elf $(image/work_dir)/map/fw/; ";			\
		echo -n "cp -a $(image/work_dir)/boot/*.mvi $(image/work_dir)/map/fw/; ";			\
		echo -n "echo \"/dev/mtdblock2	/boot	jffs2	defaults	0	0\" >> $(image/work_dir)/map/root/etc/fstab" ) | tee -a "$(PTX_LOGFILE)"		\
	) | $(FAKEROOT) -s $(STATEDIR)/image_working_dir_prepared --
	
	@echo "done."
	@$(call touch)

#
#
############################## UIMAGE ##############################################################

$(IMAGEDIR)/uImage.bin: $(IMAGEDIR)/linuximage
	@echo -n "Creating uImage... "
	@cd $(image/work_dir);										\
	((															\
		echo "cp $(IMAGEDIR)/linuximage $(IMAGEDIR)/uImage.bin";	\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) --
	@echo "done."

#
#
############################## MTD_FW_SUM_PAD_SIGNED ###############################################

# $MKFSJFFS2 -qUfv -p0x6E0000 -e0x20000 -r $TMPFWDIR -o $CURDIR/mtd_fw.bin > /dev/null
$(IMAGEDIR)/mtd_fw.bin: $(STATEDIR)/image_working_dir_prepared $(STATEDIR)/host-mtd-utils.install.post
	@echo -n "Creating $@... "
	@cd $(image/work_dir);									\
	(awk -F: $(DOPERMISSIONS) $(image/permissions) &&		\
	(														\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/mkfs.jffs2 ";	\
		echo -n "-d $(image/work_dir)/map/fw ";				\
		echo -n "-qUfv -p0x6E0000 -e0x20000 ";				\
		echo    "-o $@";										\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	
	@echo "done."

# $SUMTOOL -v -p -e 0x20000 -i $CURDIR/mtd_fw.bin -o $CURDIR/mtd_fw.sum.bin > /dev/null
$(IMAGEDIR)/mtd_fw.sum.bin: $(IMAGEDIR)/mtd_fw.bin
	@echo -n "Creating $@... "
	@cd $(image/work_dir);									\
	((														\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/sumtool ";	\
		echo -n "-i $< ";									\
		echo -n "-p -e 0x20000 ";							\
		echo    "-o $@";										\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

# $PAD 0x6E0000 $CURDIR/mtd_fw.sum.bin $CURDIR/mtd_fw.sum.pad.bin
$(IMAGEDIR)/mtd_fw.sum.pad.bin: $(IMAGEDIR)/mtd_fw.sum.bin $(STATEDIR)/host-pad.install.post
	@echo -n "Creating $@... "
	@cd $(image/work_dir);								\
	((													\
		echo -n "$(PTXCONF_SYSROOT_HOST)/bin/pad ";	\
		echo -n "0x6E0000 ";							\
		echo -n "$< ";									\
		echo    "$@";									\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

# cat $CURDIR/dummy.squash.signed.padded > $CURDIR/mtd_fw.sum.pad.signed.bin
# cat $CURDIR/mtd_fw.sum.pad.bin >> $CURDIR/mtd_fw.sum.pad.signed.bin
$(IMAGEDIR)/mtd_fw.sum.pad.signed.bin: $(IMAGEDIR)/mtd_fw.sum.pad.bin
	@echo -n "Creating $@... "
	@cd $(image/work_dir);																				\
	((																									\
		echo -n "cat ";																					\
		echo -n "$(PTXDIST_WORKSPACE)/local_src/extra/atevio7500/flash/dummy.squash.signed.padded ";	\
		echo    "> $@";																					\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	
	@cd $(image/work_dir);	\
	((						\
		echo -n "cat ";		\
		echo -n "$< ";		\
		echo    ">> $@";	\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

#
#
############################## MTD_ROOT_SUM_PAD_SIGNED ###############################################

# $MKFSJFFS2 -qUfv -p0x2C60000 -e0x20000 -r $TMPROOTDIR -o $CURDIR/mtd_root.bin > /dev/null
$(IMAGEDIR)/mtd_root.bin: $(STATEDIR)/image_working_dir_prepared $(STATEDIR)/host-mtd-utils.install.post
	@echo -n "Creating $@... "
	@cd $(image/work_dir);									\
	(awk -F: $(DOPERMISSIONS) $(image/permissions) &&		\
	(														\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/mkfs.jffs2 ";	\
		echo -n "-d $(image/work_dir)/map/root ";			\
		echo -n "-qUfv -p0x2C60000 -e0x20000 ";				\
		echo    "-o $@";										\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

# $SUMTOOL -v -p -e 0x20000 -i $CURDIR/mtd_root.bin -o $CURDIR/mtd_root.sum.bin > /dev/null
$(IMAGEDIR)/mtd_root.sum.bin: $(IMAGEDIR)/mtd_root.bin
	@echo -n "Creating $@... "
	@cd $(image/work_dir);									\
	((														\
		echo -n "$(PTXCONF_SYSROOT_HOST)/sbin/sumtool ";	\
		echo -n "-i $< ";									\
		echo -n "-p -e 0x20000 ";							\
		echo "-o $@" ) | tee -a "$(PTX_LOGFILE)"			\
	) | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

# $PAD 0x2C60000 $CURDIR/mtd_root.sum.bin $CURDIR/mtd_root.sum.pad.bin
$(IMAGEDIR)/mtd_root.sum.pad.bin: $(IMAGEDIR)/mtd_root.sum.bin $(STATEDIR)/host-pad.install.post
	@echo -n "Creating $@... "
	@cd $(image/work_dir);								\
	((													\
		echo -n "$(PTXCONF_SYSROOT_HOST)/bin/pad ";	\
		echo -n "0x2C60000 ";							\
		echo -n "$< ";									\
		echo    "$@";									\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

#
#
############################## MTD_ROOT_SUM_PAD_R_SIGNED ############################################

# dd if=$CURDIR/mtd_root.sum.pad.bin of=$CURDIR/mtd_root.sum.pad.r.bin bs=65536 skip=0 count=189
$(IMAGEDIR)/mtd_root.sum.pad.r.bin: $(IMAGEDIR)/mtd_root.sum.pad.bin
	@echo -n "Creating $@... "
	@cd $(image/work_dir);						\
	((											\
		echo -n "dd ";							\
		echo -n "if=$< ";						\
		echo -n "of=$@ ";						\
		echo    "bs=65536 skip=0 count=189";	\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

# cat $CURDIR/dummy.squash.signed.padded > $CURDIR/mtd_root.sum.pad.r.signed.bin
# cat $CURDIR/mtd_root.sum.pad.r.bin >> $CURDIR/mtd_root.sum.pad.r.signed.bin
$(IMAGEDIR)/mtd_root.sum.pad.r.signed.bin: $(IMAGEDIR)/mtd_root.sum.pad.r.bin
	@echo -n "Creating $@... "
	@cd $(image/work_dir);																				\
	((																									\
		echo -n "cat ";																					\
		echo -n "$(PTXDIST_WORKSPACE)/local_src/extra/atevio7500/flash/dummy.squash.signed.padded ";	\
		echo    "> $@";																					\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	
	@cd $(image/work_dir);	\
	((						\
		echo -n "cat ";		\
		echo -n "$< ";		\
		echo    ">> $@";	\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

#
#
############################## MTD_ROOT_SUM_PAD_D_SIGNED ############################################

# dd if=$CURDIR/mtd_root.sum.pad.bin of=$CURDIR/mtd_root.sum.pad.d.bin bs=65536 skip=189 count=45
$(IMAGEDIR)/mtd_root.sum.pad.d.bin: $(IMAGEDIR)/mtd_root.sum.pad.bin
	@echo -n "Creating $@... "
	@cd $(image/work_dir);						\
	((											\
		echo -n "dd ";							\
		echo -n "if=$< ";						\
		echo -n "of=$@ ";						\
		echo    "bs=65536 skip=189 count=45";	\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

# cat $CURDIR/dummy.squash.signed.padded > $CURDIR/mtd_root.sum.pad.d.signed.bin
# cat $CURDIR/mtd_root.sum.pad.d.bin >> $CURDIR/mtd_root.sum.pad.d.signed.bin
$(IMAGEDIR)/mtd_root.sum.pad.d.signed.bin: $(IMAGEDIR)/mtd_root.sum.pad.d.bin
	@echo -n "Creating $@... "
	@cd $(image/work_dir);																				\
	((																									\
		echo -n "cat ";																					\
		echo -n "$(PTXDIST_WORKSPACE)/local_src/extra/atevio7500/flash/dummy.squash.signed.padded ";	\
		echo    "> $@";																					\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	
	@cd $(image/work_dir);	\
	((						\
		echo -n "cat ";		\
		echo -n "$< ";		\
		echo    ">> $@";	\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

#
#
############################## MTD_ROOT_SUM_PAD_CX #################################################

# dd if=$CURDIR/mtd_root.sum.pad.bin of=$CURDIR/mtd_root.sum.pad.c0.bin bs=65536 skip=234 count=4
$(IMAGEDIR)/mtd_root.sum.pad.c0.bin: $(IMAGEDIR)/mtd_root.sum.pad.bin
	@echo -n "Creating $@... "
	@cd $(image/work_dir);						\
	((											\
		echo -n "dd ";							\
		echo -n "if=$< ";						\
		echo -n "of=$@ ";						\
		echo    "bs=65536 skip=234 count=4";	\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

# dd if=$CURDIR/mtd_root.sum.pad.bin of=$CURDIR/mtd_root.sum.pad.c4.bin bs=65536 skip=238 count=4
$(IMAGEDIR)/mtd_root.sum.pad.c4.bin: $(IMAGEDIR)/mtd_root.sum.pad.bin
	@echo -n "Creating $@... "
	@cd $(image/work_dir);						\
	((											\
		echo -n "dd ";							\
		echo -n "if=$< ";						\
		echo -n "of=$@ ";						\
		echo    "bs=65536 skip=238 count=4";	\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

# dd if=$CURDIR/mtd_root.sum.pad.bin of=$CURDIR/mtd_root.sum.pad.c8.bin bs=65536 skip=242 count=2
$(IMAGEDIR)/mtd_root.sum.pad.c8.bin: $(IMAGEDIR)/mtd_root.sum.pad.bin
	@echo -n "Creating $@... "
	@cd $(image/work_dir);						\
	((											\
		echo -n "dd ";							\
		echo -n "if=$< ";						\
		echo -n "of=$@ ";						\
		echo    "bs=65536 skip=242 count=2";	\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

# dd if=$CURDIR/mtd_root.sum.pad.bin of=$CURDIR/mtd_root.sum.pad.ca.bin bs=65536 skip=244 count=2
$(IMAGEDIR)/mtd_root.sum.pad.ca.bin: $(IMAGEDIR)/mtd_root.sum.pad.bin
	@echo -n "Creating $@... "
	@cd $(image/work_dir);						\
	((											\
		echo -n "dd ";							\
		echo -n "if=$< ";						\
		echo -n "of=$@ ";						\
		echo    "bs=65536 skip=244 count=2";	\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

#
#
############################## MTD_ROOT_SUM_PAD_U ##################################################

# dd if=$CURDIR/mtd_root.sum.pad.bin of=$CURDIR/mtd_root.sum.pad.u.bin bs=65536 skip=246 count=464
$(IMAGEDIR)/mtd_root.sum.pad.u.bin: $(IMAGEDIR)/mtd_root.sum.pad.bin
	@echo -n "Creating $@... "
	@cd $(image/work_dir);						\
	((											\
		echo -n "dd ";							\
		echo -n "if=$< ";						\
		echo -n "of=$@ ";						\
		echo    "bs=65536 skip=246 count=464";	\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) -i $(STATEDIR)/image_working_dir_prepared --
	@echo "done."

#
#
############################## UPDATE_IRD ##########################################################


# $FUP -c $OUTFILE -k $CURDIR/uImage -a $CURDIR/mtd_fw.sum.pad.signed.bin -r $CURDIR/mtd_root.sum.pad.r.signed.bin -d $CURDIR/mtd_root.sum.pad.d.signed.bin -c0 $CURDIR/mtd_root.sum.pad.c0.bin -c4 $CURDIR/mtd_root.sum.pad.c4.bin -c8 $CURDIR/mtd_root.sum.pad.c8.bin -ca $CURDIR/mtd_root.sum.pad.ca.bin -u $CURDIR/mtd_root.sum.pad.u.bin
$(IMAGEDIR)/update.ird: $(STATEDIR)/image_working_dir               \
                        $(IMAGEDIR)/uImage.bin                     \
                        $(IMAGEDIR)/mtd_fw.sum.pad.signed.bin       \
                        $(IMAGEDIR)/mtd_root.sum.pad.r.signed.bin   \
                        $(IMAGEDIR)/mtd_root.sum.pad.d.signed.bin   \
                        $(IMAGEDIR)/mtd_root.sum.pad.c0.bin  \
                        $(IMAGEDIR)/mtd_root.sum.pad.c4.bin  \
                        $(IMAGEDIR)/mtd_root.sum.pad.c8.bin  \
                        $(IMAGEDIR)/mtd_root.sum.pad.ca.bin  \
                        $(IMAGEDIR)/mtd_root.sum.pad.u.bin   \
                        $(STATEDIR)/host-fup.install.post
	@echo -n "Creating $@... "
	@cd $(IMAGEDIR);												\
	((																\
		echo -n "$(PTXCONF_SYSROOT_HOST)/bin/fup ";					\
		echo -n "-c $@ ";											\
		echo -n "-k $(IMAGEDIR)/uImage.bin ";						\
		echo -n "-a $(IMAGEDIR)/mtd_fw.sum.pad.signed.bin ";		\
		echo -n "-r $(IMAGEDIR)/mtd_root.sum.pad.r.signed.bin ";	\
		echo -n "-d $(IMAGEDIR)/mtd_root.sum.pad.d.signed.bin ";	\
		echo -n "-c0 $(IMAGEDIR)/mtd_root.sum.pad.c0.bin ";			\
		echo -n "-c4 $(IMAGEDIR)/mtd_root.sum.pad.c4.bin ";			\
		echo -n "-c8 $(IMAGEDIR)/mtd_root.sum.pad.c8.bin ";			\
		echo -n "-ca $(IMAGEDIR)/mtd_root.sum.pad.ca.bin ";			\
		echo    "-u $(IMAGEDIR)/mtd_root.sum.pad.u.bin ";			\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) --
	@echo "done."
	
	@echo -n "Cleaning image work_dir and files... "
	@cd $(IMAGEDIR);												\
	((																\
		echo -n "rm -rf ";											\
		echo -n "       $(IMAGEDIR)/linuximage ";					\
		echo -n "       $(IMAGEDIR)/mtd_fw.bin ";					\
		echo -n "       $(IMAGEDIR)/mtd_root.bin ";					\
		echo -n "       $(IMAGEDIR)/*.pad*.bin ";					\
		echo    "       $(STATEDIR)/image_working_dir_prepared";	\
	) | tee -a "$(PTX_LOGFILE)") | $(FAKEROOT) --
	@echo "done."
	
	@echo "-----------------------------------------------------------------------"
	@echo "To flash the created image copy the "
	@echo "`basename $@` "
	@echo "file to your usb drive"

endif

# vim600:set foldmethod=marker:
# vim600:set syntax=make:
