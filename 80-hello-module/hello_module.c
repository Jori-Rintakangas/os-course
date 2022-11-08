#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>

static int __init hello_init(void)
{
	printk(KERN_INFO "Hello, world\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodbye, world\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A module that creates an entry to /proc and greets when entry is read");
MODULE_AUTHOR("Jori Rintakangas <jori.rintakangas@tuni.fi>");
MODULE_VERSION("1.0");
