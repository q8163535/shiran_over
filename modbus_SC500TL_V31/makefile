simulate_modbus:simulate_modbus.o  feeddog.o
	arm-linux-gcc -Wall  -o  /mnt/hgfs/share-2/simulate_modbus simulate_modbus.o  feeddog.o

simulate_modbus.o: simulate_modbus.c
	arm-linux-gcc -Wall  -c simulate_modbus.c

feeddog.o: feeddog.c 
	arm-linux-gcc -Wall  -c feeddog.c	

clean:
	/bin/rm   -rf *.o   *~   simulate_modbus
