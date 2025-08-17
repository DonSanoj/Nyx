
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

// Function to explain the concept
static void explain_memory_layout(void)
{
    pr_info("\n=== Memory Layout Explanation ===\n");
    pr_info("1. task_struct is a large structure (9792 bytes) containing process info\n");
    pr_info("2. The 'cred' field is a POINTER located at offset 2960 within task_struct\n");
    pr_info("3. This pointer POINTS TO a separate cred structure (184 bytes)\n");
    pr_info("4. So we have:\n");
    pr_info("   - task_struct address: where the process structure starts\n");
    pr_info("   - cred pointer location: task_struct + 2960 bytes\n");
    pr_info("   - cred structure address: where the pointer points to\n");
    pr_info("5. task->cred gives us the VALUE of the pointer (cred structure address)\n");
    pr_info("6. &(task->cred) would give us the ADDRESS of the pointer itself\n");
}

// Function to escalate privileges by modifying credentials
static void escalate_privileges(void)
{
    struct cred *new_cred;
    
    pr_info("\n=== Privilege Escalation ===\n");
    pr_info("Target process: %s (PID: %d)\n", current->comm, current->pid);
    pr_info("Original credentials:\n");
    pr_info("  UID: %d, GID: %d\n", 
            current->cred->uid.val, current->cred->gid.val);
    pr_info("  EUID: %d, EGID: %d\n", 
            current->cred->euid.val, current->cred->egid.val);
    
    // Check if already root
    if (current->cred->uid.val == 0) {
        pr_info("Process is already running as root - escalation not needed\n");
        pr_info("This is normal when using 'sudo insmod'\n");
        return;
    }
    
    // Prepare new credentials
    new_cred = prepare_creds();
    if (!new_cred) {
        pr_err("Failed to prepare credentials\n");
        return;
    }
    
    pr_info("Modifying credentials to root (UID=0, GID=0)...\n");
    
    // Set all UIDs and GIDs to 0 (root)
    new_cred->uid.val = 0;
    new_cred->gid.val = 0;
    new_cred->euid.val = 0;
    new_cred->egid.val = 0;
    new_cred->suid.val = 0;
    new_cred->sgid.val = 0;
    new_cred->fsuid.val = 0;
    new_cred->fsgid.val = 0;
    
    // Commit the new credentials
    commit_creds(new_cred);
    
    pr_info("New credentials:\n");
    pr_info("  UID: %d, GID: %d\n", 
            current->cred->uid.val, current->cred->gid.val);
    pr_info("  EUID: %d, EGID: %d\n", 
            current->cred->euid.val, current->cred->egid.val);
    pr_info("Privilege escalation completed!\n");
}

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
    pr_info("  Manual calculation (pointer storage location):\n");
    pr_info("    task_struct(%p) + offset(%lu) = %p\n",
            task, offsetof(struct task_struct, cred),
            (void*)task + offsetof(struct task_struct, cred));
    pr_info("  Actual cred structure address: %p\n", cred_ptr);
    pr_info("  Verification: *(task_struct + offset) should equal cred address\n");
    
    // Demonstrate pointer dereferencing
    pr_info("Method 4 - Pointer dereferencing demonstration:\n");
    pr_info("  Address where cred pointer is stored: %p\n", 
            (void*)task + offsetof(struct task_struct, cred));
    pr_info("  Value stored at that address (cred pointer): %p\n", 
            *(const struct cred **)((void*)task + offsetof(struct task_struct, cred)));
    pr_info("  Direct access result: %p\n", task->cred);
    pr_info("  All three should be equal: %s\n",
            (cred_ptr == *(const struct cred **)((void*)task + offsetof(struct task_struct, cred)) &&
             cred_ptr == task->cred) ? "YES" : "NO");
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
    
    // Add explanation
    explain_memory_layout();
    
    // Escalate privileges
    escalate_privileges();
    
    return 0;
}

static void __exit nyx_exit(void)
{
    pr_info("Nyx module unloaded\n");
}

module_init(nyx_init);
module_exit(nyx_exit);