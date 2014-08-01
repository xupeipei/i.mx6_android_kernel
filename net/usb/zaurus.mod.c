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
	{ 0xf540910d, "usbnet_resume" },
	{ 0x12993c12, "usbnet_suspend" },
	{ 0xe0b5fa87, "usbnet_disconnect" },
	{ 0xfca25979, "usbnet_probe" },
	{ 0x7504fda2, "usbnet_cdc_unbind" },
	{ 0xa03d0ff3, "usb_register_driver" },
	{ 0xc0aeed87, "usb_deregister" },
	{ 0x31a46eb4, "usbnet_generic_cdc_bind" },
	{ 0x5491c753, "skb_put" },
	{ 0xa34f1ef5, "crc32_le" },
	{ 0x97708908, "dev_kfree_skb_any" },
	{ 0x912fc4f, "skb_copy_expand" },
	{ 0xc045c476, "_dev_info" },
	{ 0x71c90087, "memcmp" },
	{ 0xf8fb82a2, "usbnet_get_endpoints" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v04DDp8004d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v04DDp8005d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v04DDp8006d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v04DDp8007d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v04DDp9031d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v04DDp9032d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v04DDp9050d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v22B8p6027d*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v22B8p6425d*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v07B4p0F02d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v046DpC11Fd*dc*dsc*dp*ic02isc0Aip00*");
