/* This loadable kernel module creates a directory /hello under /proc directory
and hello contains a file greeting which has "Hello World!" as its content.
Once the module is loaded, the file greeting can be read from the user space
applications.
The module has been tested by reading the file greeting with cat, head, tail,
more, and less commands and also with c application that opens and creates FILE
pointer to the greeting file and uses fgets() to read bytes from the FILE pointer. */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>

#define MSG_SIZE 13
char greeting_msg[MSG_SIZE] = "Hello World!\n";

static struct proc_dir_entry *proc_dir;


/* This function will be invoked when we read the greeting file from the user space. */
static ssize_t read_proc_file(struct file *file, char __user *buffer, size_t size, loff_t *offset)
{
	/* simple_read_from_buffer() uses copy_to_user() to copy data from the kernel space to the
	user space. buffer is the user space array where the data will be written to, size is the
	amount of bytes requested from the user space, offset is the file position, greeting_msg
	buffer contains the data to write to the user buffer and MSG_SIZE is the size of that buffer. */
	ssize_t bytes_read = simple_read_from_buffer(buffer, size, offset, greeting_msg, MSG_SIZE);
	if (bytes_read < 0)
	{
		printk(KERN_ERR "Reading from greeting file failed.\n");
		return -1;
	}
	return bytes_read;
}

/* We create a file that can only be read so only reading operation is needed.
In addition, we also set seek operation to be not allowed so that a separate seek function
implementation is not needed. This struct will map the readings of the greeting file into
the read_proc_file() function. */
static struct proc_ops proc_file_operations = {
        .proc_read = read_proc_file,
		.proc_lseek = no_llseek,
};

/* This will be invoked when the module is loaded. */
static int __init hello_init(void)
{
	/* Creating a directory named hello under /proc */
	proc_dir = proc_mkdir("hello", NULL);
	
	if (proc_dir == NULL)
	{
		printk(KERN_ERR "Creating proc entry failed.\n");
		return -1;
	}

	/* Creating a file named greeting inside the /proc/hello directory.
	We set only reading permissions and specify file operations with
	proc_file_operations struct */
	proc_create("greeting", 0444, proc_dir, &proc_file_operations);
	printk(KERN_INFO "proc entry: /hello/greeting created.\n");
	return 0;
}

/* This will be invoked when the module is unloaded. */
static void __exit hello_exit(void)
{
	proc_remove(proc_dir);
	printk(KERN_INFO "proc entry: /hello/greeting removed.\n");
}

/* Registering module init and exit functions. */
module_init(hello_init);
module_exit(hello_exit);

/* This module metadata can be dispalyed with modinfo command. */
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A module that creates an entry to /proc and greets world when entry is read");
MODULE_AUTHOR("Jori Rintakangas <jori.rintakangas@tuni.fi>");
MODULE_VERSION("1.0");
