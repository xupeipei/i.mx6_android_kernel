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

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xadc92799, "module_layout" },
	{ 0x76314e81, "platform_driver_register" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x9d669763, "memcpy" },
	{ 0x996bdb64, "_kstrtoul" },
	{ 0x97255bdf, "strlen" },
	{ 0x71c90087, "memcmp" },
	{ 0xf998eb2b, "mxs_reset_block" },
	{ 0x572b94ec, "clk_enable" },
	{ 0x63ec13e7, "sysfs_create_group" },
	{ 0x68ef6bed, "clk_get" },
	{ 0x40a6f522, "__arm_ioremap" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x15754b4d, "platform_get_resource" },
	{ 0x37a0cba, "kfree" },
	{ 0x2e1ca751, "clk_put" },
	{ 0xde4cdbbb, "clk_disable" },
	{ 0xf1bddad6, "dev_set_drvdata" },
	{ 0x31d362c3, "sysfs_remove_group" },
	{ 0x6af67000, "dev_get_drvdata" },
	{ 0x4be5e728, "platform_driver_unregister" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

