
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/stddef.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ADS");
MODULE_DESCRIPTION("Nyx Kernel Module - task_struct and cred exploration");
MODULE_VERSION("1.0");

// Function to demonstrate manual cred pointer access
static void explore_task_struct(void)
{
    struct task_struct *task = current;
    const struct cred *cred_ptr;
    
    pr_info("=== Manual task_struct exploration ===\n");
    
    // Method 1: Direct access via current macro
    pr_info("Method 1 - Using 'current' macro:\n");
    pr_info("  current = %p\n", current);
    pr_info("  current->cred = %p\n", current->cred);
    
    // Method 2: Access via task pointer
    pr_info("Method 2 - Via task_struct pointer:\n");
    cred_ptr = task->cred;
    pr_info("  task->cred = %p\n", cred_ptr);
    
    // Method 3: Show memory layout information
    pr_info("Method 3 - Memory layout:\n");
    pr_info("  sizeof(task_struct) = %lu bytes\n", sizeof(struct task_struct));
    pr_info("  sizeof(cred) = %lu bytes\n", sizeof(struct cred));
    pr_info("  cred offset = %lu bytes from task_struct start\n", 
            offsetof(struct task_struct, cred));
    
    // Calculate the address manually (educational purpose)
    pr_info("  Manual calculation: task_struct(%p) + offset(%lu) = %p\n",
            task, offsetof(struct task_struct, cred),
            (void*)task + offsetof(struct task_struct, cred));
    
    // Show what's actually at that memory location
    pr_info("  Value at calculated address: %p\n", 
            *(const struct cred**)((void*)task + offsetof(struct task_struct, cred)));
    pr_info("  This should match cred_ptr: %p\n", cred_ptr);
}

static int __init nyx_init(void) 
{
    struct task_struct *current_task;
    const struct cred *current_cred;
    
    pr_info("Hello from kernel space\n");
    pr_info("=== Task Struct and Cred Exploration ===\n");
    
    // Get the current process's task_struct
    current_task = current;
    
    pr_info("Current Process Information:\n");
    pr_info("  PID: %d\n", current_task->pid);
    pr_info("  Process Name: %s\n", current_task->comm);
    pr_info("  task_struct address: %p\n", current_task);
    
    // Access the cred pointer from task_struct
    current_cred = current_task->cred;
    
    pr_info("Credentials Information:\n");
    pr_info("  cred structure address: %p\n", current_cred);
    pr_info("  Real UID: %d\n", current_cred->uid.val);
    pr_info("  Effective UID: %d\n", current_cred->euid.val);
    pr_info("  Real GID: %d\n", current_cred->gid.val);
    pr_info("  Effective GID: %d\n", current_cred->egid.val);
    
    // Show the offset of cred within task_struct
    pr_info("Offset of cred in task_struct: %lu bytes\n", 
            offsetof(struct task_struct, cred));
    
    // Call our exploration function
    explore_task_struct();
    
    return 0;
}

static void __exit nyx_exit(void)
{
    pr_info("Nyx module unloaded\n");
}

module_init(nyx_init);
module_exit(nyx_exit);