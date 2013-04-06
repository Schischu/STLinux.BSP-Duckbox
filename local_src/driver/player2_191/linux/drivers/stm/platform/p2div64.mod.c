#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .arch = MODULE_ARCH_INIT,
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "ED20C9304693B33430DC5D2");
#ifdef CONFIG_LKM_ELF_HASH
static unsigned long __symtab_hash[]
__used
__attribute__((section(".undef.hash"))) = {
	0, 0, 0, 0, 
};
#endif
#ifdef CONFIG_LKM_ELF_HASH

#include <linux/types.h>
static uint32_t htable__ksymtab[]
__used
__attribute__((section("__ksymtab.htable"))) = {
	3, /* bucket lenght*/
	1, /* chain lenght */
	/* the buckets */
	   0,    1,   -1, 
	/* the chains */
	   2, 
};
#endif
