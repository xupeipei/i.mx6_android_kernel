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
	{ 0xe25f8de3, "__hid_register_driver" },
	{ 0x9c0ba064, "malloc_sizes" },
	{ 0x22ec0043, "hid_connect" },
	{ 0xb5c63e4c, "dev_err" },
	{ 0xf1bddad6, "dev_set_drvdata" },
	{ 0x66439ba5, "kmem_cache_alloc" },
	{ 0x37a0cba, "kfree" },
	{ 0x860f3836, "hid_disconnect" },
	{ 0x6c6d712c, "input_event" },
	{ 0x676bbc0f, "_set_bit" },
	{ 0x6af67000, "dev_get_drvdata" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x23d430a0, "hid_unregister_driver" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("hid:b0003v000009DAp00000006");
MODULE_ALIAS("hid:b0003v000009DAp0000000A");
MODULE_ALIAS("hid:b0003v000009DAp0000001A");
