# 1. mailbox_module.c
File mã nguồn chính của kernel module bạn viết.

Chứa các hàm như: init_module, cleanup_module hoặc module_init(), module_exit(), và các thao tác như open, read, write, ioctl, v.v.

Đây là file bạn viết và sửa đổi.
# 2. Makefile

File dùng để build kernel module.

Chứa dòng lệnh kiểu:

obj-m += mailbox_module.o

Khi bạn chạy make -C /lib/modules/$(uname -r)/build M=$(PWD) modules, hệ thống sẽ dùng file này để biên dịch ra .ko (kernel object).
# 3. mailbox_module.ko

Đây là file kernel object – module đã được biên dịch và có thể chèn vào kernel bằng lệnh:

sudo insmod mailbox_module.ko

.ko là viết tắt của "kernel object".
# 4. mailbox_module.mod.c

File này được tự động sinh ra trong quá trình build module.

Nó chứa thông tin mô tả module để kernel sử dụng, ví dụ: thông tin license, dependencies, v.v.

Bạn không cần sửa file này.
# 5. mailbox_module.mod.o
# 6. mailbox_module.o

Các file object (.o) là sản phẩm trung gian của quá trình biên dịch từ .c.

mailbox_module.o là file object biên dịch trực tiếp từ mailbox_module.c

mailbox_module.mod.o là object chứa metadata của module.
# 7. Module.symvers

Chứa danh sách các symbol được export (ví dụ hàm, biến toàn cục) để dùng giữa các module khác.

Nếu bạn export symbol bằng EXPORT_SYMBOL(), file này sẽ lưu thông tin đó.

Hữu ích nếu bạn chia thành nhiều module có phụ thuộc lẫn nhau.
# 8. modules.order

Danh sách thứ tự build và nạp các module.

Kernel dùng file này khi build hoặc cài đặt module.

Không cần chỉnh sửa.

# 9. test.out

Đây có vẻ là một chương trình test user-space bạn build từ một file .c.

Dùng để giao tiếp với module (thông qua /dev/ device), gọi open(), read(), ioctl(),…
# 10. initramfs, initramfs.cpio.gz, Dockerfile
initramfs/ và initramfs.cpio.gz

Đây là thư mục chứa file hệ thống gốc (root filesystem) dùng cho initramfs.

File .cpio.gz là version nén của initramfs/ – dùng để boot kernel trong môi trường ảo hóa (QEMU, Docker,…).

##Dockerfile

File cấu hình để tạo image Docker.

Có thể dùng để tạo môi trường build hoặc test kernel module một cách tự động, cách ly.
