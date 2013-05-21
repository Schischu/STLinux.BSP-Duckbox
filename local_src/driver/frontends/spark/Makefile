ifdef SPARK7162
CCFLAGSY += -I$(TREE_ROOT)/spark7162
CCFLAGSY += -I$(TREE_ROOT)/spark7162/stv0367
CCFLAGSY += -I$(TREE_ROOT)/spark7162/stv0367/stv0367qam
CCFLAGSY += -I$(TREE_ROOT)/spark7162/stv0367/stv0367ter
CCFLAGSY += -I$(TREE_ROOT)/spark7162/6158
CCFLAGSY += -I$(TREE_ROOT)/spark7162/tuner
CCFLAGSY += -I$(TREE_ROOT)/spark
CCFLAGSY += -I$(TREE_ROOT)/include
CCFLAGSY += -I$(TREE_ROOT)/
endif

ifdef SPARK
CCFLAGSY += -I$(TREE_ROOT)/spark
CCFLAGSY += -I$(TREE_ROOT)/include
CCFLAGSY += -I$(TREE_ROOT)/
endif

CCFLAGSY += -Wall -DCONFIG_DVB_DUMMY_FE #-Werror
EXTRA_CFLAGS += $(CCFLAGSY) $(EXTRA_CFLAGSS)

ifdef SPARK
stv090x-objs +=	spark/stv090x.o
stv090x-objs +=	spark/stv6110x.o
stv090x-objs +=	spark/ix7306.o
stv090x-objs +=	spark/core.o
stv090x-objs += vz7903.o
obj-m += stv090x.o
endif

ifdef SPARK7162
spark7162-objs +=        spark7162/spark7162.o

spark7162-objs +=        ioarch.o

spark7162-objs +=        spark7162/D3501.o

spark7162-objs +=        spark/ix7306.o
spark7162-objs +=        vz7903.o
spark7162-objs +=        dvb_dummy_fe.o
spark7162-objs +=        spark7162/sharp7803.o
spark7162-objs +=        spark7162/sharp6465.o
spark7162-objs +=        spark7162/sharp5469c.o

spark7162-objs +=        spark7162/stv0367/D0367.o
spark7162-objs +=        spark7162/stv0367/D0367ter.o
spark7162-objs +=        spark7162/stv0367/D0367cab.o

spark7162-objs +=        spark7162/stv0367/stv0367ter/chip_0367ter.o
spark7162-objs +=        spark7162/stv0367/stv0367ter/stv0367ofdm_drv.o
spark7162-objs +=        spark7162/stv0367/stv0367ter/d0367_ter.o

spark7162-objs +=        spark7162/stv0367/stv0367qam/chip_0367qam.o
spark7162-objs +=        spark7162/stv0367/stv0367qam/stv0367qam_drv.o
spark7162-objs +=        spark7162/stv0367/stv0367qam/d0367_qam.o

spark7162-objs +=        spark7162/6158/d6158.o
spark7162-objs +=        spark7162/6158/MN88472_autoctrl.o
spark7162-objs +=        spark7162/6158/MN88472_register.o
spark7162-objs +=        spark7162/6158/MN88472_variable.o

spark7162-objs +=        spark7162/tuner/mxl301rf.o
spark7162-objs +=        spark7162/tuner/mxl_common.o
spark7162-objs +=        spark7162/tuner/mxl_spur_table.o
spark7162-objs +=        spark7162/tuner/mxL_user_define.o

obj-m += spark7162.o
endif
