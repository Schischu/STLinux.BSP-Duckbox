#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

#ifdef CONFIG_LKM_ELF_HASH
static unsigned long __symtab_hash[]
__used
__attribute__((section(".undef.hash"))) = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
};
#endif
#ifdef CONFIG_LKM_ELF_HASH

#include <linux/types.h>
static uint32_t htable__ksymtab[]
__used
__attribute__((section("__ksymtab.htable"))) = {
	17, /* bucket lenght*/
	27, /* chain lenght */
	/* the buckets */
	   4,    2,   28,    5,    1,    7,   23,    3,    8,   29,   -1,   -1, 
	   0,    9,   12,   26,   11, 
	/* the chains */
	  10,    6,   13,   -1,   -1,   15,   22,   -1,   18,   19,   14,   17, 
	  -1,   -1,   16,   21,   20,   25,   -1,   24,   27,   -1,   -1,   -1, 
	  -1,   -1,   30, 
};
#endif
