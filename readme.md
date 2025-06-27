#1 Loadable kernal

## Makefile

    ```bash
    # Khai báo tên module object
    obj-m := mailbox_module.o

    # Biên dịch module kernel (module.ko) bằng kernel headers
    sudo make -C /lib/modules/$(uname -r)/build M=$PWD modules

    # Nạp module vào kernel
    sudo insmod ./mailbox_module.ko

    # Gỡ module ra khỏi kernel
    sudo rmmod mailbox_module

    # Theo dõi log hệ thống để xem thông báo từ module
    sudo tail -f /var/log/syslog

    ```

## 1.1. mailbox_module.c loadable module

    ```bash

    # Được gọi khi module được nạp vào kernel (insmod)
    int mailbox_module_init(void){
        printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
        return 0;
    }

    # Được gọi khi module bị xóa khỏi kernel (rmmod)
    void mailbox_module_exit(void){
        printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
    }

    module_init(mailbox_module_init);
    module_exit(mailbox_module_exit);
    ```

## từ khóa **init, **initdata

    - __init và __initdata là các macro đặc biệt trong lập trình kernel module dùng để tối ưu bộ nhớ.
    - Khi một hàm được khai báo với __init, kernel biết rằng hàm này chỉ được sử dụng khi module được load (insmod).

## module param

    ```bash
    #include <linux/moduleparam.h>

    module_param(biến, kiểu, quyền);
    function(){}


    #truyền tham số
    sudo ismod ./module.ko biến=value
    ```

#2. Character device driver

```bash
#Register character device driver
register_chrdev(Major number, "Name Dev", File operation);
#Unregister character device driver
unregister_chrdev(Major number, "Name Dev");
```

```bash
    #Đường dẫn truy cập thư viện được import vào module
    /lib/modules/$(uname -r)/build/include/linux/name.h

    #file_operation linux/fs.h
```

#3 Hướng dẫn cách chạy chương trình

##1. clone repository
##2. Mở termial tại foder mailbox

### - Chạy lệnh dưới để biên dịch module kernel

    ```bash
    sudo make -C /lib/modules/$(uname -r)/build M=$PWD modules
    ```

### - Nạp module vào kernel

    ```bash
    sudo insmod ./mailbox_module.ko
    ```

### - Cấp quyền cho device

    ```bash
    sudo mknod -m 666 /dev/mailbox_device c 240 0
    ```

### Mở log để theo dõi trạng thái

    ```bash
    sudo tail -f /var/log/syslog
    ```

##3. Mở terminal mới để test chương trình
`bash
    ./test.sh
    `
##4. xử lý nếu lỗi
```bash
#Nếu lỗi ở test thì thử build lại
gcc test_rw.c -o test
