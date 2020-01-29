#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0x195c2787, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x396a0e1f, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xbaaf0a39, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x6362158b, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0xd3ceec67, __VMLINUX_SYMBOL_STR(cdev_alloc) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0x403f9529, __VMLINUX_SYMBOL_STR(gpio_request_one) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0x53a1da6e, __VMLINUX_SYMBOL_STR(kthread_stop) },
	{ 0x1245b308, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0x64a887e4, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xdf2df03d, __VMLINUX_SYMBOL_STR(gpiod_get_raw_value) },
	{ 0xec7ccae3, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0x4b9fab3e, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0xafb574ea, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x79063479, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0xb3f7646e, __VMLINUX_SYMBOL_STR(kthread_should_stop) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "DD06ABA42AFA09C76852397");
