ifdef SPARK7162
CCFLAGSY += -I$(TREE_ROOT)/spark7162
CCFLAGSY += -I$(TREE_ROOT)/spark7162/stv0367
CCFLAGSY += -I$(TREE_ROOT)/spark7162/stv0367/stv0367qam
CCFLAGSY += -I$(TREE_ROOT)/spark7162/stv0367/stv0367ter
CCFLAGSY += -I$(TREE_ROOT)/spark7162/6158
CCFLAGSY += -I$(TREE_ROOT)/spark7162/tuner
CCFLAGSY += -I$(TREE_ROOT)/base
CCFLAGSY += -I$(TREE_ROOT)/spark

CCFLAGSY += -Wall -DCONFIG_DVB_DUMMY_FE #-Werror
endif

EXTRA_CFLAGS += $(CCFLAGSY) $(EXTRA_CFLAGSS)
# >= 2.6.30
ifdef HL101
fe-core-objs +=        hl101/stb0899_algo.o
fe-core-objs +=        hl101/stb0899_drv.o
fe-core-objs +=        hl101/stb6100.o
fe-core-objs +=        hl101/stv090x.o
fe-core-objs +=        hl101/stv6110x.o
fe-core-objs +=        hl101/ix7306.o
fe-core-objs +=        hl101/core.o
obj-m += fe-core.o
endif

ifdef VIP1_V2
fe-core-objs +=        vip2/stv090x.o
fe-core-objs +=        vip2/stv6110x.o
fe-core-objs +=        vip2/ix7306.o
fe-core-objs +=        vip2/zl10353.o
fe-core-objs +=        vip2/core.o
obj-m += fe-core.o
endif

ifdef VIP2_V1
fe-core-objs +=        vip2/stv090x.o
fe-core-objs +=        vip2/stv6110x.o
fe-core-objs +=        vip2/ix7306.o
fe-core-objs +=        vip2/zl10353.o
fe-core-objs +=        vip2/core.o
obj-m += fe-core.o
endif

ifdef SPARK
stv090x-objs +=	spark/stv090x.o
stv090x-objs +=	spark/stv6110x.o
stv090x-objs +=	spark/ix7306.o
stv090x-objs +=	spark/core.o
stv090x-objs += spark/vz7903.o
obj-m += stv090x.o
endif

ifdef SPARK7162
spark7162-objs +=        spark7162/spark7162.o

spark7162-objs +=        spark/ioarch.o

spark7162-objs +=        spark7162/D3501.o

spark7162-objs +=        spark/ix7306.o
spark7162-objs +=        spark/vz7903.o
spark7162-objs +=        spark/dvb_dummy_fe.o
spark7162-objs +=        base/sharp7803.o
spark7162-objs +=        base/sharp6465.o
spark7162-objs +=        base/sharp5469c.o

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

ifdef HS7110
stv090x-objs +=	stv090x/stv090x.o
stv090x-objs +=	stv090x/stv6110x.o
stv090x-objs +=	stv090x/ix7306.o
stv090x-objs +=	stv090x/core.o
obj-m += stv090x.o
obj-y += lnb/
endif

ifdef WHITEBOX
stv090x-objs +=	stv090x/stv090x.o
stv090x-objs +=	stv090x/stv6110x.o
stv090x-objs +=	stv090x/ix7306.o
stv090x-objs +=	stv090x/core.o
obj-m += stv090x.o
obj-y += lnb/
endif

ifdef HS7810A
stv090x-objs +=	stv090x/stv090x.o
stv090x-objs +=	stv090x/stv6110x.o
stv090x-objs +=	stv090x/ix7306.o
stv090x-objs +=	stv090x/core.o
obj-m += stv090x.o
obj-y += lnb/
endif

ifdef FORTIS_HDBOX
stv090x-objs +=	stv090x/stv090x.o
stv090x-objs +=	stv090x/stv6110x.o
stv090x-objs +=	stv090x/ix7306.o
stv090x-objs +=	stv090x/core.o
obj-m += stv090x.o
endif

ifdef UFS922
cx21143-objs +=	cx24116/cx24116.o
obj-m += cx21143.o

avl2108-objs +=        avl2108/avl2108.o
avl2108-objs +=        avl2108/stv6110a.o
avl2108-objs +=        avl2108/stv6306.o
avl2108-objs +=        avl2108/lnb_pio.o
avl2108-objs +=        avl2108/lnbh221.o
obj-m += avl2108.o

avl2108_platform-objs += avl2108/avl2108_platform.o
obj-m += avl2108_platform.o
endif

ifdef UFS910
cx24116-objs += cx24116_dvbapi5/cx24116.o
obj-m += cx24116.o
endif

ifdef TF7700
cx24116-objs += cx24116_dvbapi5/cx24116.o
obj-m += cx24116.o
endif

ifdef OCTAGON1008
avl2108-objs +=        avl2108/avl2108.o
avl2108-objs +=        avl2108/stv6110a.o
avl2108-objs +=        avl2108/stv6306.o
avl2108-objs +=        avl2108/lnb_pio.o
avl2108-objs +=        avl2108/lnbh221.o
obj-m += avl2108.o

avl2108_platform-objs += avl2108/avl2108_platform.o
obj-m += avl2108_platform.o
endif

ifdef ATEVIO7500
obj-y	+= multituner/
endif

ifdef IPBOX9900
fe-core-objs +=        ipbox/stv090x.o 
fe-core-objs +=        ipbox/stv6110x.o
fe-core-objs +=        ipbox/zl10353.o
fe-core-objs +=        ipbox/mxl5007t.o
fe-core-objs +=        ipbox/tda10023.o
fe-core-objs +=        ipbox/core.o
obj-m += fe-core.o
endif

ifdef IPBOX99
fe-core-objs +=        ipbox/stv090x.o 
fe-core-objs +=        ipbox/stv6110x.o
fe-core-objs +=        ipbox/zl10353.o
fe-core-objs +=        ipbox/mxl5007t.o
fe-core-objs +=        ipbox/tda10023.o
fe-core-objs +=        ipbox/core.o
obj-m += fe-core.o
endif

ifdef IPBOX55
fe-core-objs +=        ipbox/stv090x.o 
fe-core-objs +=        ipbox/stv6110x.o
fe-core-objs +=        ipbox/core.o
obj-m += fe-core.o
endif

ifdef CUBEREVO
obj-y	+= multituner/
endif

ifdef CUBEREVO_MINI
obj-y	+= multituner/
endif

ifdef CUBEREVO_MINI2
obj-y	+= multituner/
endif

ifdef CUBEREVO_250HD
obj-y	+= multituner/
endif

ifdef CUBEREVO_9500HD
obj-y	+= multituner/
endif

ifdef CUBEREVO_2000HD
obj-y	+= multituner/
endif

ifdef CUBEREVO_MINI_FTA
obj-y	+= multituner/
endif

ifdef ADB_BOX
# Single ----------------------------------

# stb0899

fe-core-objs +=        adb/stb0899_algo.o
fe-core-objs +=        adb/stb0899_drv.o
fe-core-objs +=        adb/stb6100.o
fe-core-objs +=        adb/core.o
obj-m += fe-core.o

# Dual -------------------------------------

# stb0899


fe-core-twin1-objs +=        adb_dual/stb0899_algo.o
fe-core-twin1-objs +=        adb_dual/stb0899_drv.o
fe-core-twin1-objs +=        adb_dual/stb6100.o
fe-core-twin1-objs +=        adb_dual/core.o
obj-m += fe-core-twin1.o 

# stv0900

fe-core-twin2-objs +=	adb_stv900/stv090x.o
fe-core-twin2-objs +=	adb_stv900/stb6100.o
fe-core-twin2-objs +=	adb_stv900/core.o
obj-m += fe-core-twin2.o

#------------------------------------------------- 

endif
