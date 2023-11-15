#define LINUX
#define PROCFS_MAX_SIZE 1024
#define PROCFS_NAME "kmlab"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/seq_file.h>
#include <linux/spinlock.h>

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
static spinlock_t lock;
static struct timer_list timer;
static struct workqueue_struct *kmlab_workqueue;
static struct work_struct *kmlab_work;
struct processes {
   struct list_head list;
   int pid;
   unsigned long CPUTime;
};

LIST_HEAD(head);


static void update_list(struct timer_list *t)
{
   pr_info("Timer Woke Up. Updating List...");
   schedule_work(&kmlab_work);
   mod_timer(&timer, jiffies + msecs_to_jiffies(5000));
}

static void work_function(struct work_struct *work)
{
   
}

static ssize_t procfile_write(struct file *file, const char __user *buff, size_t len, loff_t *off)
{
   struct processes *new_entry;
   char *kernel_buf;
   unsigned long flags;
   int result = 0;

   new_entry = kmalloc(sizeof(struct processes), GFP_KERNEL);
   if(!new_entry)
   {
      return -ENOMEM;
   }
   INIT_LIST_HEAD(&new_entry->list);
   kernel_buf = kmalloc(len + 1, GFP_KERNEL);
   if(!kernel_buf)
   {
      kfree(new_entry);
      return -ENOMEM;
   }

   if(copy_from_user(kernel_buf, buff, len))
   {
      result = -EFAULT;
   }
   else{
      kernel_buf[len] = '\0';
      if(sscanf(kernel_buf, "%u", &new_entry->pid) != 1)
      {
         result = -EINVAL;
      }
      else{
         new_entry->CPUTime = 0;
         spin_lock_irqsave(&lock, flags);
         list_add(&new_entry->list, &head);
         spin_unlock_irqrestore(&lock, flags);
         pr_info("Added a process to list.");
      }
   }

   kfree(kernel_buf);
   if(result)
   {
      kfree(new_entry);
      return result;
   }

   return len;
}

static ssize_t print_process_data(struct file *file_pointer, char __user *buffer, size_t buffer_length, loff_t *offset)
{
   pr_info("Inside read");
   char *buf;
   unsigned long flags;
   size_t buf_size = 0;
   int error = 0;
   struct processes *entry;

   buf = kmalloc(buffer_length, GFP_KERNEL);
   if(buf == NULL)
   {
      pr_info("Line 113 Error");
      return -ENOMEM;
   }
   if(*offset > 0)
   {
      pr_info("Line 118 Error");
      return 0;
   }

   spin_lock_irqsave(&lock, flags);
   list_for_each_entry(entry, &head, list){
      int len = 0;
      if(buf_size >= buffer_length)
      {
         break;
      }
      len += snprintf(buf + buf_size, buffer_length - buf_size, "%u: CPU Time: %lu\n", entry->pid, entry->CPUTime);
      
      buf_size += len;
   }
   spin_unlock_irqrestore(&lock, flags);

   if(!error)
   {
      pr_info("Buffer: %s", buf);
      
      if(copy_to_user(buffer, buf, buf_size))
      {
         pr_info("Line 142");
         error = -EFAULT;
      }
      else
      {
         pr_info("Success: copied %d bytes.", buf_size);
         *offset += buf_size;
      }
   }
   pr_info("Successfully Displayed List data.");
   kfree(buf);
   return error ? error : buf_size;
}

static const struct proc_ops proc_file_fops = {
   .proc_read = print_process_data,
   .proc_write = procfile_write,
};




// kmlab_init - Called when module is loaded
int __init kmlab_init(void)
{
   #ifdef DEBUG
   pr_info("KMLAB MODULE LOADING\n");
   #endif
   

   proc_kmlab_dir = proc_mkdir(PROCFS_NAME, NULL);
   proc_kmlab_status = proc_create("status", 0666, proc_kmlab_dir, &proc_file_fops);

   if(NULL == proc_kmlab_dir || NULL == proc_kmlab_status)
   {
      pr_alert("Initialization Error: %s Generation Failed.", (NULL == proc_kmlab_dir) ? PROCFS_NAME : "proc/kmlab/status");
      return -ENOMEM;
   }
   pr_info("/proc/%s created\n", PROCFS_NAME);
   pr_info("/proc/%s/%s created\n", PROCFS_NAME, "status");


   spin_lock_init(&lock);
   timer_setup(&timer, update_list, 0);
   mod_timer(&timer, jiffies + msecs_to_jiffies(5000));


   kmlab_workqueue = create_workqueue("kmlab_workqueue");
   if(!kmlab_workqueue)
   {
      pr_err("Failed to create workqueue.\n");
   }

   kmlab_work = kmalloc(sizeof(*kmlab_work), GFP_KERNEL);
   if(!kmlab_work)
   {
      destroy_workqueue(kmlab_workqueue);
   }
   INIT_WORK(kmlab_work, work_function);

  
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
   del_timer(&timer);
   flush_workqueue(kmlab_workqueue);
   destroy_workqueue(kmlab_workqueue);

   pr_info("KMLAB MODULE UNLOADED\n");
}

// Register init and exit funtions
module_init(kmlab_init);
module_exit(kmlab_exit);
