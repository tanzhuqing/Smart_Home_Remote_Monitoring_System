#!/bin/sh
clear
echo " "
#declare name="ds"
echo "请输入用户名:"
read user_name
echo "输入密码"
read passwd
case $user_name in
	"ds")
		echo "欢迎,$user_name."
			case $passwd in
			    "123456")
				echo "******************"
				insmod ./drivers/demo.o
				insmod ./drivers/dc-motor.o
				insmod ./drivers/s3c2410-adc.o
				./gprs_4.0/gprs
				;;
			     *)
				echo "*******************"
				echo "密码错误"
				;;
			esac
			;;
	*)
		 echo "输入用户名有误"
		  ;;
esac



