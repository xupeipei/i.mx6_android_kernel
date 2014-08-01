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
	{ 0x6ef5ded3, "register_netdevice" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xf9a482f9, "msleep" },
	{ 0xd4fab16, "complete_and_exit" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x311b7963, "_raw_spin_unlock" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x3ec8886f, "param_ops_int" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x97255bdf, "strlen" },
	{ 0x89177421, "wake_lock_destroy" },
	{ 0xf1bddad6, "dev_set_drvdata" },
	{ 0xa98a4f44, "find_vpid" },
	{ 0x47939e0d, "__tasklet_hi_schedule" },
	{ 0x8dadf55a, "netif_carrier_on" },
	{ 0x8bd94317, "_raw_spin_lock_bh" },
	{ 0x6af82156, "skb_clone" },
	{ 0x14329ff2, "dev_get_by_name" },
	{ 0x20000329, "simple_strtoul" },
	{ 0xf7802486, "__aeabi_uidivmod" },
	{ 0xe776900a, "skb_copy" },
	{ 0x8949858b, "schedule_work" },
	{ 0x1c3f30f, "down_interruptible" },
	{ 0x2a3aa678, "_test_and_clear_bit" },
	{ 0x52645594, "netif_carrier_off" },
	{ 0x6c49917d, "usb_kill_urb" },
	{ 0x4d761412, "remove_proc_entry" },
	{ 0xdda9e423, "wake_lock" },
	{ 0xeac96010, "filp_close" },
	{ 0x4e830a3e, "strnicmp" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0xb78c61e8, "param_ops_bool" },
	{ 0x7426f76c, "init_timer_key" },
	{ 0x2659bf9b, "mutex_unlock" },
	{ 0xf554e1f, "usb_autopm_get_interface" },
	{ 0x85df9b6c, "strsep" },
	{ 0x58ea1ea6, "usb_enable_autosuspend" },
	{ 0x999e8297, "vfree" },
	{ 0xdbf0f34f, "usb_disable_autosuspend" },
	{ 0x55b74b46, "wake_unlock" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x7d11c268, "jiffies" },
	{ 0xb5ae54fc, "skb_trim" },
	{ 0xe2d5255a, "strcmp" },
	{ 0x9b131f55, "__netdev_alloc_skb" },
	{ 0xd684d0ae, "netif_rx" },
	{ 0xfc07caf6, "__pskb_pull_tail" },
	{ 0x41e92619, "__init_waitqueue_head" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0x72aa82c6, "param_ops_charp" },
	{ 0xfa2a45e, "__memzero" },
	{ 0xa4476cec, "del_timer_sync" },
	{ 0x8f4641c7, "register_early_suspend" },
	{ 0x7f37d714, "skb_queue_purge" },
	{ 0x5f754e5a, "memset" },
	{ 0xd1016640, "dev_alloc_skb" },
	{ 0x74c97f9c, "_raw_spin_unlock_irqrestore" },
	{ 0x37befc70, "jiffies_to_msecs" },
	{ 0xc0aeed87, "usb_deregister" },
	{ 0xb795af0a, "__mutex_init" },
	{ 0x27e1a049, "printk" },
	{ 0x42224298, "sscanf" },
	{ 0x71c90087, "memcmp" },
	{ 0x58797273, "free_netdev" },
	{ 0xfaef0ed, "__tasklet_schedule" },
	{ 0x328a05f1, "strncpy" },
	{ 0x8f1731e8, "register_netdev" },
	{ 0xa99a9343, "wireless_send_event" },
	{ 0x230d66e5, "usb_control_msg" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0xf0498a82, "skb_push" },
	{ 0xc4ab8022, "mutex_lock" },
	{ 0x9545af6d, "tasklet_init" },
	{ 0x53985936, "mod_timer" },
	{ 0x2469810f, "__rcu_read_unlock" },
	{ 0x9abce797, "kill_pid" },
	{ 0xa1c0b3aa, "skb_pull" },
	{ 0xf5677525, "device_init_wakeup" },
	{ 0x6cdf4fdc, "init_net" },
	{ 0x2196324, "__aeabi_idiv" },
	{ 0x97708908, "dev_kfree_skb_any" },
	{ 0xd79b5a02, "allow_signal" },
	{ 0x59e5070d, "__do_div64" },
	{ 0xa2413a3e, "flush_signals" },
	{ 0xa1fce2b0, "skb_queue_tail" },
	{ 0x6103c716, "netif_device_attach" },
	{ 0x9a6ad14a, "usb_submit_urb" },
	{ 0x4d9d3517, "usb_get_dev" },
	{ 0xb368ec89, "_raw_spin_unlock_bh" },
	{ 0x84596adf, "usb_reset_device" },
	{ 0x3bd1b1f6, "msecs_to_jiffies" },
	{ 0x6b62598, "usb_put_dev" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0x999fd442, "eth_type_trans" },
	{ 0x779d1cb4, "create_proc_entry" },
	{ 0xc2161e33, "_raw_spin_lock" },
	{ 0x341dbfa3, "__per_cpu_offset" },
	{ 0xbd7083bc, "_raw_spin_lock_irqsave" },
	{ 0x56ba7be3, "unregister_netdevice_queue" },
	{ 0x44da5d0f, "__csum_ipv6_magic" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0xde5d1761, "wake_lock_init" },
	{ 0xbf8eee02, "dev_alloc_name" },
	{ 0x7a728ef4, "up" },
	{ 0xa03d0ff3, "usb_register_driver" },
	{ 0x7e9ebb05, "kernel_thread" },
	{ 0x47b3dc0, "unregister_early_suspend" },
	{ 0xf74a2bbe, "skb_dequeue" },
	{ 0xc9b0bd27, "unregister_netdev" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x676bbc0f, "_set_bit" },
	{ 0x4f7f2d1b, "complete" },
	{ 0x701d0ebd, "snprintf" },
	{ 0xab1aa42d, "__netif_schedule" },
	{ 0xca54fee, "_test_and_set_bit" },
	{ 0x99bb8806, "memmove" },
	{ 0xe113bbbc, "csum_partial" },
	{ 0x85670f1d, "rtnl_is_locked" },
	{ 0x8d522714, "__rcu_read_lock" },
	{ 0x49ebacbd, "_clear_bit" },
	{ 0x5491c753, "skb_put" },
	{ 0x48d316aa, "wait_for_completion_timeout" },
	{ 0xc3fe87c8, "param_ops_uint" },
	{ 0x85caced0, "skb_copy_bits" },
	{ 0x6af67000, "dev_get_drvdata" },
	{ 0xcf376034, "usb_free_urb" },
	{ 0x9e7d6bd0, "__udelay" },
	{ 0x747e89, "usb_autopm_put_interface" },
	{ 0xdc43a9c8, "daemonize" },
	{ 0xacfc751b, "usb_alloc_urb" },
	{ 0xbb04b75f, "filp_open" },
	{ 0xdfbef99d, "alloc_etherdev_mqs" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v0BDAp8191d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8176d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8170d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp817Ed*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp817Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp817Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp817Dd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8754d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp817Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp818Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp018Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8177d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8178d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp817Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019pED17d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0052d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v7392p7811d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07B8p8189d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0EB0p9071d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v06F8pE033d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v103Cp1629d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2001p3308d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp1102d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019pAB2Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v20F4p648Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v4855p0090d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3357d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p005Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp5088d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v4856p0091d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019p4902d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019pAB2Ed*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp317Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3359d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3358d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04F2pAFF7d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04F2pAFF9d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04F2pAFFAd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04F2pAFF8d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04F2pAFFBd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04F2pAFFCd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019p1201d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2001p3307d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2001p330Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2001p3309d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0586p341Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v7392p7822d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019pAB2Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07B8p8178d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AAp0056d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v4855p0091d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2001p3307d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp2102d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp2103d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v20F4p624Dd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0061d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0B05p17ABd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0846p9021d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E66p0019d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8186d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "657E53E8FE213B566819C54");
