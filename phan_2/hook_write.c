#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/unistd.h>
#include<linux/fs.h>

void **syscall_table_addr = NULL;

char* fd_to_pathname(int fd)
{
	struct file *file;
	int fput_needed;
	char *file_name = NULL;
	mm_segment_t segment;
	
	file = fget_light(fd, &fput_needed);
	if (file)
	{
		segment = get_fs();
		set_fs(get_ds());
		file_name file->f_path.dentry->d_iname;		
		set_fs(segment);
	}
	return file_name;
}

asmlinkage int (*custom_syscall)(char *name);

asmlinkage ssize_t hook_write(int fd, const void *buf, size_t cnt)
{
	printk(KERN_INFO"This is my hook_write()\n");

	char *pathname = fd_to_pathname(fd);

	printk(KERN_INFO "Written file: %s\n", pathname);

	int written_bytes = custom_syscall(fd, buf, cnt);

	printk(KERN_INFO"Number of written bytes: %d\n);

	return written_bytes;
}

static int __int init_mysyscall(void)
{
	printk(KERN_INFO "loaded mysyscall hook\n");
	
	syscall_table_addr = (void*)0xffffffff820001e0;

	custom_syscall = syscall_table_addr[__NR_write];

	make_rw((unsigned long)syscall_table_addr);

	syscall_table_addr[__NR_write] = hook_write;

	return 0;
}


static int __exit exit_mysyscall(void)
{
	printk(KERN_INFO "removed mysyscall hook\n");

	syscall_table_addr[__NR_write] = custom_syscall;

	make_ro((unsigned long)syscall_table_addr);

	return 0;
}


module_init(init_mysyscall);
module_exit(exit_mysyscall);