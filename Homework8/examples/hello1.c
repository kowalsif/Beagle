/* Example Minimal Character Device Driver */
#include <linux/module.h>

static int debug_enable = 0;
/* Added driver parameter */
module_param(debug_enable, int, 0); /* and these 2 lines */
MODULE_PARM_DESC(debug_enable, "Enable module debug mode.");

static int work_enable = 0;
/* Added driver parameter */
module_param(work_enable, int, 0); /* and these 2 lines */
MODULE_PARM_DESC(work_enable, "Enable module work mode.");



static int __init hello_init(void)
{
    printk("Hello Example Init\n");
    printk("debug mode is %s\n", debug_enable ? "enabled" : "disabled");
	printk("work mode is %s\n", work_enable ? "enabled" : "disabled");

    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "Hello Example Exit\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Chris Hallinan, edited by ifk");
MODULE_DESCRIPTION("Hello World Example");
MODULE_LICENSE("GPL");
