#define LINUX
#define PROCFS_MAX_SIZE 1024
#define PROCFS_NAME "procbuf"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/seq_file.h>

#include "kmlab_given.h"
// Include headers as needed ...


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bergdahl"); // Change with your lastname
MODULE_DESCRIPTION("CPTS360 Lab 4");

#define DEBUG 1

// Global variables as needed ...
static struct proc_dir_entry *proc_kmlab_dir;
static struct proc_dir_entry *proc_kmlab_status;
static char proc_buffer[PROCFS_MAX_SIZE];
static unsigned long proc_buffer_size = 0;



static ssize_t procfile_write(struct file *file, const char __user *buff, size_t len, loff_t *off)
{
   return 0;
}

static ssize_t procfile_read(struct file *file_pointer, char __user *buffer, size_t buffer_length, loff_t *offset)
{

   return 0;
}

static const struct proc_ops proc_file_fops = {
   .proc_read = procfile_read,
   .proc_write = procfile_write,
};




// kmlab_init - Called when module is loaded
int __init kmlab_init(void)
{
   #ifdef DEBUG
   pr_info("KMLAB MODULE LOADING\n");
   #endif
   // Insert your code here ...
   proc_kmlab_dir = proc_mkdir("kmlab", NULL);
   proc_kmlab_status = proc_create("status", 0666, proc_kmlab_dir, &proc_file_fops);

   if(NULL == proc_kmlab_dir || NULL == proc_kmlab_status)
   {
      pr_alert("Initialization Error: %s Generation Failed.", (NULL == proc_kmlab_dir) ? "proc/kmlab" : "proc/kmlab/status");
      return -ENOMEM;
   }
   pr_info("/proc/%s created\n", "kmlab");
   pr_info("/proc/%s created\n", "kmlab/status");
   pr_info("KMLAB MODULE LOADED\n");

   return 0;   
}


// kmlab_exit - Called when module is unloaded
void __exit kmlab_exit(void)
{
   #ifdef DEBUG
   pr_info("KMLAB MODULE UNLOADING\n");
   #endif
   // Insert your code here ...
   
   remove_proc_entry("status", proc_kmlab_dir);
   remove_proc_entry("kmlab", NULL);

   pr_info("KMLAB MODULE UNLOADED\n");
}

// Register init and exit funtions
module_init(kmlab_init);
module_exit(kmlab_exit);
