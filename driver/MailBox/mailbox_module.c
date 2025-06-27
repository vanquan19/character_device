#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h> //for the character divice support
#include <linux/uaccess.h> 
#include <linux/mutex.h> 
#include <linux/wait.h> 

MODULE_LICENSE("GLP");
MODULE_AUTHOR("QUAN");

/* Declare global varibale*/
#define BUFFER_SIZE 1024

static char buffer[BUFFER_SIZE];
static int head = 0, tail = 0, count = 0; /*head: read ponter, tail: write ponter, cout: number of current bytes in buffer*/


/* Two separate mutexes to protect parallel write and read operations */
static DEFINE_MUTEX(read_lock);
static DEFINE_MUTEX(write_lock);

/* Queue to block reading when buffer is empty */
static DECLARE_WAIT_QUEUE_HEAD(read_queue);

/* Ensures only one process has read or write access at a time */
static int reader_opened = 0;
static int writer_opened = 0;



ssize_t mailbox_read(struct file *pfile, char __user *ubuf, size_t length, loff_t *offset){
	size_t i = 0;
	char ch;

	printk(KERN_INFO "[mailbox] PID %d yêu cầu đọc %zu byte\n", current->pid, length);

	if (wait_event_interruptible(read_queue, count >= length)) {
		printk(KERN_INFO "[mailbox] PID %d bị gián đoạn khi chờ dữ liệu\n", current->pid);
		return -ERESTARTSYS;
	}

	mutex_lock(&read_lock);
	while (i < length && count > 0) {
		ch = buffer[head];
		head = (head + 1) % BUFFER_SIZE;
		count--;

		if (copy_to_user(&ubuf[i], &ch, 1)) {
		    	mutex_unlock(&read_lock);
			printk(KERN_ERR "[mailbox] Lỗi copy_to_user khi đọc\n");
		    	return -EFAULT;
		}
		i++;
	}
	mutex_unlock(&read_lock);
	printk(KERN_INFO "[mailbox] PID %d đã đọc được %zu byte\n", current->pid, i);
	return i;
}


ssize_t mailbox_write (struct file *pfile, const char __user *ubuf, size_t length, loff_t *offset){
	size_t i;
	char ch;
	printk(KERN_INFO "[mailbox] PID %d yêu cầu ghi %zu byte\n", current->pid, length);
	mutex_lock(&write_lock);
	for (i = 0; i < length; i++) {
		if (count >= BUFFER_SIZE)
			break;

		if (copy_from_user(&ch, &ubuf[i], 1)){
			printk(KERN_ERR "[mailbox] Lỗi copy_from_user khi ghi\n");
			break;
		}

		buffer[tail] = ch;
		tail = (tail + 1) % BUFFER_SIZE;
		count++;
	}
	mutex_unlock(&write_lock);
	
	wake_up_interruptible(&read_queue);

	printk(KERN_INFO "[mailbox] PID %d đã ghi thành công %zu byte\n", current->pid, i);
	return i;
}


int mailbox_open (struct inode *pnode, struct file *pfile){
	if ((pfile->f_mode & FMODE_READ)) {
		if (reader_opened) {
			printk(KERN_WARNING "[mailbox] PID %d mở đầu đọc nhưng đã có tiến trình khác đang mở\n", current->pid);
			return -EBUSY;
		}
		reader_opened = 1;
		printk(KERN_INFO "[mailbox] PID %d đã mở đầu đọc\n", current->pid);
	}

	if ((pfile->f_mode & FMODE_WRITE)) {
		if (writer_opened){ 
			printk(KERN_WARNING "[mailbox] PID %d mở đầu ghi nhưng đã có tiến trình khác đang mở\n", current->pid);
			return -EBUSY;
		}
		writer_opened = 1;
	}

	return 0;
}


int mailbox_close (struct inode *pnode, struct file *pfile){
	if (pfile->f_mode & FMODE_READ){
		reader_opened = 0;
		printk(KERN_INFO "[mailbox] PID %d đã đóng đầu đọc\n", current->pid);
	}
	if (pfile->f_mode & FMODE_WRITE){
		writer_opened = 0;
		printk(KERN_INFO "[mailbox] PID %d đã đóng đầu ghi\n", current->pid);
	}
	return 0;
}


/* To hole the file operations performed on this device*/
struct file_operations mailbox_file_operations = {
	.owner   = THIS_MODULE,
	.open    = mailbox_open,
	.read    = mailbox_read,
	.write   = mailbox_write,
	.release = mailbox_close,
};


static int __init mailbox_module_init(void){
	//register with kernal and indicate that we are registering a char device driver
	int result = register_chrdev(240, "mailbox device", &mailbox_file_operations);
	if (result < 0) {
		printk(KERN_ALERT "[mailbox] Không thể đăng ký thiết bị\n");
		return result;
	}

	printk(KERN_INFO "[mailbox] Module được nạp thành công (major = 240)\n");
	return 0;
}

void mailbox_module_exit(void){
	//Unregister the char device driver
	unregister_chrdev(240, "mailbox device");
	printk(KERN_INFO "[mailbox] Module đã được gỡ bỏ\n");
}

module_init(mailbox_module_init);
module_exit(mailbox_module_exit);
