OBJ = basic_driver.o
KDIR = /usr/src/linux-source-3.13.0/linux-source-3.13.0/

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install

.PHONY: all clean
clean:
	-rf -rm $(OBJ)

