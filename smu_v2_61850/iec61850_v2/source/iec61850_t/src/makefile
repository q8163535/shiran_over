#PCC = arm-none-linux-gnueabi-
PCC = arm-linux-
CC = gcc
WFLAGS = -Wall
GFLAGS = #-g
CFLAGS = #-std=c99
PFLAGS=-D_REENTRANT
LDFLAGS=-lpthread -lxml2
IOH = ../../io.h
INCPATH ?= ../inc

INCIO ?= ../../
# XMLINCPATH ?= /home/sr/xml-analysis/xml-worklib/include/libxml2
#  XMLLIBPATH= -L/home/sr/xml-analysis/xml-worklib/lib
 
 XMLINCPATH ?=  /home/maxm/Downloads/xml-worklib/include/libxml2
XMLLIBPATH =-L/home/maxm/Downloads/xml-worklib/lib


#callback_function_process.o comReceive.o data_process.o debugSwitch.o file_operate.o init.o Modbus_s.o net.o protocol_cfg.o protocol_operate.o queue.o socket_comm.o timer_process.o uart.o xml_csv.o xml_point_list_analysis.o 


 OBJS ?= callback_function_process.o comReceive.o data_process.o debugSwitch.o file_operate.o init.o Modbus_s.o net.o protocol_cfg.o protocol_operate.o queue.o socket_comm.o timer_process.o uart.o xml_csv.o xml_point_list_analysis.o debug_config.o
ModbusSlave: $(OBJS)
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -o ModbusSlave $(OBJS) $(LDFLAGS)  $(XMLLIBPATH)
uart.o: uart.c   
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)   -c  -o $@ $^
comReceive.o: comReceive.c  
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)   -c  -o $@ $^
data_process.o: data_process.c  
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS)  -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c -o $@  $^
protocol_interface.o: protocol_interface.c  
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^
debugSwitch.o: debugSwitch.c   
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^	
net.o: net.c  
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^
file_operate.o: file_operate.c   
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^
init.o: init.c   
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^
Modbus_s.o:Modbus_s.c
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^
socket_comm.o: socket_comm.c  
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^	
io.o: ../../io.c $(IOH)
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^
protocol_cfg.o: protocol_cfg.c  
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^
protocol_operate.o: protocol_operate.c  
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^
queue.o: queue.c  
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^
timer_process.o:timer_process.c  
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^
callback_function_process.o:callback_function_process.c
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^
xml_csv.o:xml_csv.c
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^
xml_point_list_analysis.o:xml_point_list_analysis.c
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^
debug_config.o:debug_config.c
	$(PCC)$(CC) $(CFLAGS) $(GFLAGS) $(WFLAGS) -I$(INCPATH) -I$(INCIO) -I$(XMLINCPATH)  -c  -o $@ $^

.PHONY: clean
clean :
	rm -f  ModbusSlave  *.o  *~  ../inc/*.gch    ../../*.gch
	
other:
	rm -f   *.o  *~  ../inc/*.gch    ../../*.gch
cp:
	cp -f ModbusSlave   /mnt/hgfs/share-2
