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
	{ 0xf1bddad6, "dev_set_drvdata" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x79aa04a2, "get_random_bytes" },
	{ 0x6c6d712c, "input_event" },
	{ 0x41e92619, "__init_waitqueue_head" },
	{ 0xb5c63e4c, "dev_err" },
	{ 0xc045c476, "_dev_info" },
	{ 0x22ec0043, "hid_connect" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0x32f80ea9, "prepare_to_wait" },
	{ 0xe25f8de3, "__hid_register_driver" },
	{ 0xf83178bd, "finish_wait" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x676bbc0f, "_set_bit" },
	{ 0x49ebacbd, "_clear_bit" },
	{ 0x23d430a0, "hid_unregister_driver" },
	{ 0x6af67000, "dev_get_drvdata" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("hid:b0003v0000046Dp0000C513");
MODULE_ALIAS("hid:b0003v0000046Dp0000C50C");
MODULE_ALIAS("hid:b0003v0000046Dp0000C517");
MODULE_ALIAS("hid:b0003v0000046Dp0000C101");
MODULE_ALIAS("hid:b0003v0000046Dp0000C704");
MODULE_ALIAS("hid:b0003v0000046Dp0000C714");
MODULE_ALIAS("hid:b0003v0000046Dp0000C71F");
MODULE_ALIAS("hid:b0003v0000046Dp0000C30A");
MODULE_ALIAS("hid:b0003v0000046Dp0000C512");
MODULE_ALIAS("hid:b0003v0000046Dp0000C215");
MODULE_ALIAS("hid:b0003v0000046Dp0000C294");
MODULE_ALIAS("hid:b0003v0000046Dp0000C20A");
MODULE_ALIAS("hid:b0003v0000046Dp0000C211");
MODULE_ALIAS("hid:b0003v0000046Dp0000C219");
MODULE_ALIAS("hid:b0003v0000046Dp0000C283");
MODULE_ALIAS("hid:b0003v0000046Dp0000C286");
MODULE_ALIAS("hid:b0003v0000046Dp0000C295");
MODULE_ALIAS("hid:b0003v0000046Dp0000CA03");
MODULE_ALIAS("hid:b0003v0000046Dp0000C299");
MODULE_ALIAS("hid:b0003v0000046Dp0000C29B");
MODULE_ALIAS("hid:b0003v0000046Dp0000C298");
MODULE_ALIAS("hid:b0003v0000046Dp0000C29C");
MODULE_ALIAS("hid:b0003v0000046Dp0000C293");
MODULE_ALIAS("hid:b0003v0000046Dp0000C218");
MODULE_ALIAS("hid:b0003v0000046Dp0000C287");
MODULE_ALIAS("hid:b0003v0000046Dp0000C626");
MODULE_ALIAS("hid:b0003v0000046Dp0000C623");
