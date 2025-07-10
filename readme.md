# 1 Loadable kernal

## Makefile

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
## 1.1. mailbox_module.c loadable module

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

## từ khóa **init, **initdata

    - __init và __initdata là các macro đặc biệt trong lập trình kernel module dùng để tối ưu bộ nhớ.
    - Khi một hàm được khai báo với __init, kernel biết rằng hàm này chỉ được sử dụng khi module được load (insmod).

## module param

    #include <linux/moduleparam.h>

    module_param(biến, kiểu, quyền);
    function(){}


    #truyền tham số
    sudo ismod ./module.ko biến=value
    
# 2. Character device driver

    #Register character device driver
    register_chrdev(Major number, "Name Dev", File operation);
    #Unregister character device driver
    unregister_chrdev(Major number, "Name Dev");

    #Đường dẫn truy cập thư viện được import vào module
    /lib/modules/$(uname -r)/build/include/linux/name.h

    #file_operation linux/fs.h

# 3 Hướng dẫn cách chạy chương trình

## 1. clone repository
## 2. Mở termial tại foder mailbox

### - Chạy lệnh dưới để biên dịch module kernel

    sudo make -C /lib/modules/$(uname -r)/build M=$PWD modules

### - Nạp module vào kernel

    sudo insmod ./mailbox_module.ko

### - Cấp quyền cho device

    sudo mknod -m 666 /dev/mailbox_device c 240 0

### Mở log để theo dõi trạng thái

    sudo tail -f /var/log/syslog

## 3. Mở terminal mới để test chương trình

    ./test.sh
    
## 4. xử lý nếu lỗi

# Nếu lỗi ở test thì thử build lại
    gcc test_rw.c -o test

# Luồng hoạt động
## Ta đi từ thư mục MailBoxTest
### test_rw.c
- file này nhận tham số argc là số lượng tham số, argv là mảng chuỗi chứa từng đồi số của dòng lệnh
- argc < 3 là kiểm tra lệnh gọi trong bash đã lớn hơn 3 đối số chưa(bao gồm cả tên file) nếu chưa thì không thể thực hiện tiếp vì ta cần ít nhất là 3 đối số tên file, mode(read, write), và data(nếu ghi), số byte cần đọc(nếu đọc)
- vì argv là mảng các đối số mà bash gọi là
  ./test write ABC
  ./test read 5
  -> mode sẽ là argv[1] nên dòng char *mode = argv[1] sẽ là kiểu đọc hay ghi
- nếu mode = read => vào if, write vào else if
- strcmp() là hàm so sánh chuỗi
- trong hàm if tức là hàm đọc
    if (strcmp(mode, "read") == 0) {
        int n = atoi(argv[2]); => chuyển chuỗi thành số vì mặc định argv mà mảng chuỗi
        char buf[1024] = {0}; => vùng nhớ tạm để lưu dữ liệu đọc được

        fd = open("/dev/mailbox_device", O_RDONLY); //mở thiết bị chỉ có quyền đọc trả về số nguyên -1 nếu lỗi, >=0 nếu thành công hệ thống sẽ tìm chỉ số nhỏ nhất còn trống trong bảng file descriptor của tiến trình hiện tại để đại diện cho thiết bị được mở
        if (fd < 0) {
            perror("Mở /dev/mailbox_device (read) thất bại");
            return 1;
        }

        int r = read(fd, buf, n); đọc thiết bị với số đại diện thiết bị đó fd, ghi bào vùng nhớ buff với số byte cần
        if (r < 0) {
            perror("Đọc thất bại");
            close(fd);
            return 1;
        }

        printf("Đã đọc: %.*s\n", r, buf); in ra r ký tự từ buff
        close(fd);
    }
- trong hàm else if tức là hàm ghi
     else if (strcmp(mode, "write") == 0) {
        char *data = argv[2];

        fd = open("/dev/mailbox_device", O_WRONLY); => mở thiết bị chỉ để ghi
        if (fd < 0) {
            perror("Mở /dev/mailbox_device (write) thất bại");
            return 1;
        }

        int w = write(fd, data, strlen(data)); //ghi vào thiết bị định danh ghi fd, data cần ghi và độ dài của data
        if (w < 0) {
            perror("Ghi thất bại");
            close(fd);
            return 1;
        }

        printf("Đã ghi: %s\n", data); //in data đã ghi 
        close(fd);
    }
  ### test là file được biên dịch từ test_rw.c với lệnh gcc ở trên
  ### test.sh là file bash gọi từng dòng lệnh để thực thi
  - Ghi ABC
  - đợi 0.5s
  - Ghi XY
  - đợi 0.5s
  - đọc 2 byte
  - đợi 1s
  - đọc 4 byte
  - đợi 3s
  - Ghi Z
    #### các bước đợi đơn giản chỉ để luồng hoạt động theo đúng đề bài vào số s đợi để dễ quan sát thêm luồng hoạt động như nào trên log
## Phần kernal
- tại sao lại có /dev/mailbox_device sử dụng trong phần test trong khi register trong module là mailbox device?
  => vì khi khởi tạo mailbox device có major là 240 sau đó tạo thiết bị thủ công với lệnh **sudo mknod -m 666 /dev/mailbox_device c 240 0** lệnh này chatgpt là ra
- các bước chạy kernel và test đã ghi ở trên
## trong file mailbox_module.c
- khi ismod thì file hàm **static int __init mailbox_module_init** sẽ chạy  trong đó có đăng ký thiết bị **int result = register_chrdev(240, "mailbox device", &mailbox_file_operations);** 240 là major, tên thiết bị và thuộc tính của nó được khai báo bên trên
  /* To hole the file operations performed on this device*/
struct file_operations mailbox_file_operations = {
	.owner   = THIS_MODULE,
	.open    = mailbox_open,
	.read    = mailbox_read,
	.write   = mailbox_write,
	.release = mailbox_close,
};
- các thuộc tính này gắn với cáo hàm đã viết
- tại sao khi gọi đọc hoặc ghi trong test_rw.c lại truyền như thế? => vì mấy hàm được viết bên mail_module.c yêu cầu đối số đó

  ## Chi tiết các chức năng trong mấy hàm thuộc tính là gì:
  ### 1. mailbox_read
  ### 2. mailbox_write
  ### 3. mailbox_open
  ### 4. mailbox_close
  ### 5. Giải thích các cờ, biến trong file
