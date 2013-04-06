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
"depends=ksound,stm_monitor,stmdvb,p2div64,pseudocard";

#ifdef CONFIG_LKM_ELF_HASH
static unsigned long __symtab_hash[]
__used
__attribute__((section(".undef.hash"))) = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
};
#endif
#ifdef CONFIG_LKM_ELF_HASH

#include <linux/types.h>
static uint32_t htable__ksymtab[]
__used
__attribute__((section("__ksymtab.htable"))) = {
	3, /* bucket lenght*/
	7, /* chain lenght */
	/* the buckets */
	   0,    4,    1, 
	/* the chains */
	   9,    2,    3,    7,    5,    6,    8, 
};
#endif
