# 解决linux无法开机，报错VFS: Unable to Mount Root FS on Unknown-Block(0,0)
## 起因
由于电脑装了双系统，经常遇到需要切换的情景，前天由于实验室要检查设备，应用要切换到Windows才能使用，我忘记了当时正在升级软件，就把电脑重启了。没想到平回来后就无法正常开机了。


## 报错：
VFS: Unable to Mount Root FS on Unknown-Block(0,0)


## 错误解析：
root fs是Linux文件系统的开始，错误信息是无法挂载，这样自然也就无法打开Linux
简单来讲，就是上面在软件更新时候把fs引导系统损坏了

**Linux内核丢失了Initramfs文件，这个文件可以把临时根文件加载到内存中**

对root fs的详细解析可以参考这一篇文章：
https://blog.csdn.net/LEON1741/article/details/78159754

## 解决方案：
第一步需要解决的是开机问题，现在我们的状态应该是卡住在报错的黑屏位置，鼠标键盘都无法使用，只能选择强制关机重启。

下一步，在重启时选择进入ubuntu高级选项，就是开机的时候弹出来的“Advanced options for Ubuntu”，会弹出来几个选项，选择进入一个老版本的（recovery mode）自己尝试几次看那个能进去，进去就好了。

高级选项可以对系统进行一些修复，清理复制，我们按照下面的操作进行，依次选择并回车即可：

    1. clean  
    2. dpkg   
    3. grub  
    4. fsck  
    5. resume


fsck可能会报不成功，但是不影响
开机后就可以更改挂载了。

更改挂载：（$符号表示是命令行，不要复制）

1. 查看挂载


    $ sudo fdisk -l


我的输出为：（会输出很多磁盘扇区信息，主要看这一部分）



设备             起点       末尾       扇区   大小 类型

/dev/sda1          34      32767      32734    16M Microsoft 保留

/dev/sda2       32768 2858450943 2858418176   1.3T Microsoft 基本数据

/dev/sda3  2858450944 3058450431  199999488  95.4G Linux 文件系统

/dev/sda4  3058450432 3858450431  800000000 381.5G Linux 文件系统


要搞清楚自己的root挂载到了那个sda，一般自己做双系统的时候小的那个是root，我的是sda3

1. 设置挂载


注意第一个命令，更改为你的sda号
$ sudo mount /dev/sda3 /mnt
$ sudo mount --bind /dev /mnt/dev
$ sudo mount --bind /dev/pts /mnt/dev/pts
$ sudo mount --bind /proc /mnt/proc
$ sudo mount --bind /sys /mnt/sys
$ sudo chroot /mnt

2. 更新initramfs


首先查看你的最新版本的内核，你需要更新的是这个内核的fs，使用命令查看：

$ dpkg --list | grep linux-image

我的输出为：

linux-image-5.15.0-52-generic
           
linux-image-unsigned-5.15.0-53-generic  

只需要记住版本号：5.15.0-53-generic

更新命令：

$ update-initramfs -u -k 5.15.0-53-generic

1. 最后，更新grub

注意，上面sudo chroot /mnt会进入root状态，有时候会报错找不到某某文件，可以选择exit退出后再使用sudo+命令尝试

$ update-grub

1. 再重启，应该就可以正常开机了

解决挂载问题的方案来自于下面的连接，我对其中一些地方做了解释：


https://www.geekswarrior.com/2019/07/solved-how-to-fix-kernel-panic-on-linux.html


作者参考的解决方案来自于Ubuntu论坛：


https://askubuntu.com/questions/41930/kernel-panic-not-syncing-vfs-unable-to-mount-root-fs-on-unknown-block0-0