#define LINUX
#define PROCFS_MAX_SIZE 1024
#define PROCFS_NAME "kmlab"
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

struct processes {
   struct list_head list;
   int pid;
   unsigned long CPUTime;
};

LIST_HEAD(head);


static ssize_t procfile_write(struct file *file, const char __user *buff, size_t len, loff_t *off)
{
   pr_info("proc write\n");
   memset(&proc_buffer[0], 0, sizeof(proc_buffer));

   proc_buffer_size = len;
   if(proc_buffer_size > PROCFS_MAX_SIZE)
   {
      proc_buffer_size = PROCFS_MAX_SIZE;
   }

   if(copy_from_user(proc_buffer, buff, proc_buffer_size))
      pr_info("Copy from user failed.\n");
      return -EFAULT;
   
   proc_buffer[proc_buffer_size & (PROCFS_MAX_SIZE - 1)] = '\0';
   *off += proc_buffer_size;
   pr_info("Wrote %s to proc.\n", proc_buffer);

   pid_t pid;
   unsigned long cputime;
   if(kstrtoint(proc_buffer, 10, &pid) != 0)
   {
      pr_info("kstrtoint failed.\n");
      return -EINVAL;
   }
   else
   {
      get_cpu_use(pid, &cputime);
      struct processes *new_entry = kmalloc(sizeof(struct processes), GFP_KERNEL);
      pr_info("PID: %d, CPU Time: %d", pid, cputime);
      if(new_entry)
      {
         new_entry->pid = pid;
         new_entry->CPUTime = cputime;
         list_add_tail(&new_entry->list, &head);
         pr_info("Added Process With PID %s to List.\n", proc_buffer);
      }
   }

   return proc_buffer_size;
}

static ssize_t procfile_read(struct file *file_pointer, char __user *buffer, size_t buffer_length, loff_t *offset)
{
   int len = sizeof(proc_buffer);
   ssize_t ret = len;

   if(*offset >= len)
   {
      return 0;
   }
   if(copy_to_user(buffer, proc_buffer, len))
   {
      pr_info("Failed to copy to user.\n");
      ret = 0;
   }
   else
   {




      pr_info("procfile read /proc/%s\n", PROCFS_NAME);
      *offset = len;
   }

   return ret;
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
   proc_kmlab_dir = proc_mkdir(PROCFS_NAME, NULL);
   proc_kmlab_status = proc_create("status", 0666, proc_kmlab_dir, &proc_file_fops);

   if(NULL == proc_kmlab_dir || NULL == proc_kmlab_status)
   {
      pr_alert("Initialization Error: %s Generation Failed.", (NULL == proc_kmlab_dir) ? PROCFS_NAME : "proc/kmlab/status");
      return -ENOMEM;
   }
   pr_info("/proc/%s created\n", PROCFS_NAME);
   pr_info("/proc/%s/%s created\n", PROCFS_NAME, "status");
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
