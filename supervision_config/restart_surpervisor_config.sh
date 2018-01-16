#!/bin/bash
# author shiran
# time 2017-06-15
# program : 判断进程是否存在，并重新启动

# 检测标记
var_record=0
var_doube_record=0

#检测 相关函数 是否 online，如果一个不再就执行 S99 
check_program_exist()
{
      
	count=`ps -ef |grep ModbusSlave |grep -v "grep" |wc -l`
      #echo $count
      if [ 0 -eq $count ];then
         echo "ModbusSlave  end _ restart"
         var_record=1
      fi
	  
	count=`ps -ef |grep ModbusMaster |grep -v "grep" |wc -l`
      #echo $count
      if [ 0 -eq $count ];then
         echo "ModbusMaster  end _ restart"
         var_record=1
      fi
	  
	count=`ps -ef |grep DataProcess |grep -v "grep" |wc -l`
      #echo $count
      if [ 0 -eq $count ];then
         echo "DataProcess  end _ restart"
         var_record=1
      fi
      	count=`ps -ef |grep IEC104Slave |grep -v "grep" |wc -l`
      #echo $count
      if [ 0 -eq $count ];then
         echo "IEC104Slave  end _ restart"
         var_record=1
      fi
            	count=`ps -ef |grep mqtt_convert |grep -v "grep" |wc -l`
      #echo $count
      if [ 0 -eq $count ];then
         echo "mqtt_convert  end _ restart"
         var_record=1
      fi

      if [ $var_record -eq 1 ];then
	var_doube_record=`expr $var_doube_record + 1`
	echo "var_doube_record" $var_doube_record
	var_record=0	
      fi 
 
       if [ $var_doube_record -gt 1 ];then
       	echo "reboot_program"
	sh /etc/init.d/S99supervisord stop
	
	ps -ef | grep ModbusSlave | grep -v grep |cut -c 1-5 | xargs kill -9

	ps -ef | grep IEC104Slave | grep -v grep |cut -c 1-5 | xargs kill -9

	ps -ef | grep DataProcess | grep -v grep |cut -c 1-5 | xargs kill -9

	ps -ef | grep ModbusMaster | grep -v grep |cut -c 1-5 | xargs kill -9

	ps -ef | grep mqtt_convert | grep -v grep |cut -c 1-5 | xargs kill -9
	
	sh /etc/init.d/S99supervisord start
      fi  
}

count_total=0	
while true
do


#等待10s  每次间隔时间 ，第一次等待10s 等待 S99 所有的程序 都启动运行 确定检测
sleep 10

# 调用函数check  判断是all online
check_program_exist

#累加计数
if [ $var_doube_record -gt 1 ];then
	count_total=`expr $count_total + 1`
	echo "count_total" $count_total
	var_doube_record=0
fi


#累计重启超过20次 退出重启
if [ $count_total -gt 20 ];then

	echo  "return  break "$count_total
	echo  "累计重启超过20次 退出重启"
	reboot
	break;
fi


done


