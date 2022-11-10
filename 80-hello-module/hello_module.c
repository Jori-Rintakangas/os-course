#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>

static struct proc_dir_entry *proc_dir;
char greeting_msg[13] = "Hello World!\n";

static ssize_t read_proc_file(struct file *file, char __user *buffer, size_t size, loff_t *offset)
{
	return simple_read_from_buffer(buffer, size, offset, greeting_msg, 13);
}

static struct proc_ops proc_file_operations = {
        .proc_read = read_proc_file,
};

static int __init hello_init(void)
{
	proc_dir = proc_mkdir("hello", NULL);
	
	if (proc_dir == NULL)
	{
		printk(KERN_ERR "Creating proc entry failed\n");
		return -1;
	}

	proc_create("greeting", 0444, proc_dir, &proc_file_operations);
	printk(KERN_INFO "proc entry: /hello/greeting created\n");
	return 0;
}

static void __exit hello_exit(void)
{
	proc_remove(proc_dir);
	printk(KERN_INFO "proc entry: /hello/greeting removed\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A module that creates an entry to /proc and greets world when entry is read");
MODULE_AUTHOR("Jori Rintakangas <jori.rintakangas@tuni.fi>");
MODULE_VERSION("1.0");
