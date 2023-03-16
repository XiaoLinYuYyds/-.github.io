  ::----------------------------------------------------------------
  :: create by james at 2011-09-01
  ::----------------------------------------------------------------

    @echo off
    ::----------------------------------------------------------------
    :: update ramfs
    ::----------------------------------------------------------------
    @copy .\..\rootfs\drv\sdmmc2_for_phoenix.drv  .\ramfs\drv\sdmmc2.drv
    @copy .\..\ramfs\drv\matrixkey.drv  .\ramfs\drv\key.drv
    
    @copy .\..\ramfs\drv\twi.drv        .\ramfs\drv\twi.drv
    @copy .\..\ramfs\drv\rtc.drv        .\ramfs\drv\rtc.drv
    @copy .\..\ramfs\drv\power2.drv      .\ramfs\drv\power.drv
    @copy .\..\ramfs\drv\spinor.drv     .\ramfs\drv\spinor.drv
    @copy .\..\ramfs\drv\spi.drv        .\ramfs\drv\spi.drv
    @copy .\..\ramfs\drv\power_cfg.ini  .\ramfs\drv\power_cfg.ini
    @copy .\..\ramfs\mod\slib.mod       .\ramfs\mod\slib.mod

    ::----------------------------------------------------------------
    :: update rootfs
    ::----------------------------------------------------------------
    @copy .\..\rootfs\pwm_cfg.ini        .\rootfs\pwm_cfg.ini
    @copy .\..\ramfs\drv\ir.drv         .\rootfs\drv\ir.drv
    @copy .\..\ramfs\drv\matrixkey.drv  .\rootfs\drv\key.drv
    @copy .\..\rootfs\epos.img          .\rootfs\epos.img
    @copy .\..\rootfs\drv\audio_aec.drv     .\rootfs\drv\audio.drv
    @copy .\..\rootfs\drv\display.drv   .\rootfs\drv\display.drv
    @copy .\..\rootfs\drv\fm.drv        .\rootfs\drv\fm.drv
    @copy .\..\rootfs\drv\sdmmc.drv     .\rootfs\drv\sdmmc.drv
    @copy .\..\rootfs\drv\usbd_msc.drv  .\rootfs\drv\usbd_msc.drv
    @copy .\..\rootfs\drv\usb_host0.drv .\rootfs\drv\usb_host0.drv
    @copy .\..\rootfs\drv\monitor.drv        .\rootfs\drv\monitor.drv

    
    ::----------------------------------------------------------------
    :: update rootfs modules
    ::----------------------------------------------------------------
    @copy .\..\rootfs\mod\orange.mod    .\rootfs\mod\orange.mod
    @copy .\..\rootfs\mod\charset.mod   .\rootfs\mod\charset.mod
    ::@copy .\..\rootfs\mod\charset.bin   .\rootfs\mod\charset.bin
    @copy .\..\rootfs\mod\update.mod 	.\rootfs\mod\update.mod
    ::----------------------------------------------------------------
    :: update livedesk
    ::----------------------------------------------------------------
    @copy .\..\beetles\ramfs\startup.esh       .\ramfs\startup.esh
    @copy .\..\beetles\ramfs\shell.zgj       .\ramfs\shell.zgj

    @copy .\..\beetles\rootfs\mod\desktop.mod        .\rootfs\mod\desktop.mod
    @xcopy .\..\beetles\rootfs\apps        .\rootfs\apps  /s/i/yz
    @xcopy .\..\beetles\rootfs\res        .\rootfs\res  /s/i/yz


    pause