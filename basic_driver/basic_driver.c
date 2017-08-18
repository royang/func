#include <linux/module.h>
#include <linux/init.h>

static int __init module_init_func(void)
{
	printk("module init\n");
	return 0;
}

static void __exit module_exit_func(void)
{
	printk("module exit\n");
}

module_init(module_init_func);
module_exit(module_exit_func);
MODULE_LIENCES("GPL");
