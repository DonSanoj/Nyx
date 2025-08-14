
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ADS");
MODULE_DESCRIPTION("Nyx Kernel Module");
MODULE_VERSION("1.0");

static int __init nyx_init(void) 
{

}

static void __exit nyx_exit(void)
{
    pr_info("Nyx module unloaded\n");
}

module_init(nyx_init);
module_exit(nyx_exit);