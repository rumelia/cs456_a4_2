/*
 * Kernel Module cs456_pagefault
 * creates a new file in the procfs filesystem
 * named /proc/num_pagefaults that produces the
 * page fault count (since restart) when it is read.
 *
 * Most of the code is referenced from tldp.ord/LDP/lkmpg/2.6/html/x710.html
 * Modified by: Maisha Rumelia Rahman
 * For CS456 Assignment 4: Kernel Hacking
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

#define procfs_name "num_pagefaults"

struct proc_dir_entry *Proc_File;

int procfile_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
	int ret;

	printk(KERN_INFO "procfile_read (/proc/%s) called\n", procfs_name);

	// calculate total page fault #
	struct task_struct *tsk;
	unsigned long num_pagefault;

	tsk = current;
	num_pagefault = tsk->maj_flt + tsk->min_flt;

	if (offset > 0) {
		ret = 0;
	} else {
		ret = sprintf(buffer, "Total number of page faults: %lu\n", num_pagefault);
	}
	return ret;
}

int init_module()
{
	Proc_File = create_proc_entry(procfs_name, 0644, NULL);

	if (Proc_File == NULL) {
		remove_proc_entry(procfs_name, NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n", procfs_name);
		return -ENOMEM;
	}

	Proc_File->read_proc = procfile_read;
	Proc_File->mode      = S_IFREG | S_IRUGO;
	Proc_File->uid       = 0;
	Proc_File->gid       = 0;
	Proc_File->size      = 37;

	printk(KERN_INFO "/proc/%s created\n", procfs_name);
	return 0;
}

void cleanup_module()
{
	remove_proc_entry(procfs_name, NULL);
	printk(KERN_INFO "/proc/%s removed\n", procfs_name);
}
