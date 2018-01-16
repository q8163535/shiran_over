/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :uart.c
* Author :none
* Date :2016-02-03
* Description : 串口相关属性设置
* Others :
*******************************************************************************
* History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
// 者及修改内容简述
*------------------------------------------------------------------------------
* //2016-02-03: 版本：V 0.1 作者: shiran
* Modification :
*------------------------------------------------------------------------------
******************************************************************************/
#include <termios.h>            /* tcgetattr, tcsetattr */
#include <stdio.h>              /* perror, printf, puts, fprintf, fputs */
#include <unistd.h>             /* read, write, close */
#include <fcntl.h>              /* open */
#include <sys/signal.h>
#include <sys/types.h>
#include <string.h>             /* bzero, memcpy */
#include <limits.h>             /* CHAR_MAX */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>

#include <signal.h>


#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>

#include <unistd.h>

#include <stdlib.h>

#include <fcntl.h>

#include <linux/rtc.h>

#include <sys/time.h>

#include <sys/ioctl.h>


#define eth0    (int32_t)0
#define eth1    (int32_t)1


int com1;
int com2;
int com3;
int com4;
int com5;
int com6;
int com7;
int com8;
int com9;

int32_t efd;
int32_t sfd;







/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :uart.c
* Author :shiran
* Date :2016-02-03
* Description : 初始化协议，创建索引表
* Others :
*******************************************************************************
* History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
// 者及修改内容简述
*------------------------------------------------------------------------------
* //2016-02-03: 版本：V 0.1 作者: shiran
* Modification :
*------------------------------------------------------------------------------
******************************************************************************/
#include <termios.h>            /* tcgetattr, tcsetattr */
#include <stdio.h>              /* perror, printf, puts, fprintf, fputs */
#include <unistd.h>             /* read, write, close */
#include <fcntl.h>              /* open */
#include <sys/signal.h>
// #include <sys/types.h>
#include <string.h>             /* bzero, memcpy */
#include <limits.h>             /* CHAR_MAX */



static struct termios termios_old, termios_new;
static void     set_baudrate ( int32_t );
// static int32_t    get_baudrate ( int fd );
static void     set_data_bit ( int32_t databit );
static int32_t    baudrate2Bxx ( int32_t baudrate );
// static int32_t    Bxx2baudrate ( int32_t _baudrate );
static int32_t    set_port_attr (
    int32_t baudrate, 		//
    int32_t databit,
    const char *stopbit,
    char parity,
    int fd );
static void     set_stopbit ( const char *stopbit );
static void     set_parity ( char parity );


/* Open serial port ComPort at baudrate baud rate. */
int32_t OpenComPort ( int32_t ComPort,
                      int32_t baudrate,
                      int32_t databit,
                      const char *stopbit,
                      char parity )
{
    char *pComPort;
    int32_t retval;
    int fd;

    switch ( ComPort )
    {
    case 0:
        pComPort = "/dev/ttyO0";//"/dev/ttyUSB0"
        break;
    case 1:
        pComPort = "/dev/ttyO1";//"/dev/ttyS0"
        break;
    case 2:
        pComPort = "/dev/ttyO2";// dsp
        break;
    case 3:
        pComPort = "/dev/ttyO3";
        break;
    case 4:
        pComPort = "/dev/ttyO4";//pc
        break;
    case 5:
        pComPort = "/dev/ttyO5";//lcd
        break;
    case 6:
        pComPort = "/dev/ttyXRUSB0";//lcd
        break;
    case 7:
        pComPort = "/dev/ttyXRUSB1";//lcd
        break;
    case 8:
        pComPort = "/dev/ttyXRUSB2";//lcd
        break;
    case 9:
        pComPort = "/dev/ttyXRUSB3";//lcd
        break;
    default:
        pComPort = "/dev/ttyO1";
        break;
    }

    fd = open ( pComPort, O_RDWR | O_NOCTTY | O_NDELAY ); //| O_NDELAY);
    if ( -1 == fd )
    {
        fprintf ( stderr, "cannot open port %s\n", pComPort );
        return ( -1 );
    }

    printf ( "comport fd = %d\n", fd );

    tcgetattr ( fd, &termios_old );     /* save old termios value */
    /* 0 on success, -1 on failure */
    retval = set_port_attr ( baudrate, databit, stopbit, parity, fd );
    if ( -1 == retval )
    {
        fprintf ( stderr, "\nport %s cannot set baudrate at %d\n", pComPort,baudrate );
        return ( -1 );
    }
    return ( fd );
}

/* close serial port by use of file descriptor fd */
void CloseComPort ( int fd )
{
    /* flush output data before close and restore old attribute */
    tcsetattr ( fd, TCSADRAIN, &termios_old );
    close ( fd );
}

#if 0
/* get serial port baudrate */
static int32_t get_baudrate ( int fd )
{
    return ( Bxx2baudrate ( cfgetospeed ( &termios_new ) ) );
}
#endif

/* set serial port baudrate by use of file descriptor fd */
static void set_baudrate ( int32_t baudrate )
{
    termios_new.c_cflag = baudrate2Bxx ( baudrate ); /* set baudrate */
}

static void set_data_bit ( int32_t databit )
{
    termios_new.c_cflag &= ~CSIZE;
    switch ( databit )
    {
    case 8:
        termios_new.c_cflag |= CS8;
        break;
    case 7:
        termios_new.c_cflag |= CS7;
        break;
    case 6:
        termios_new.c_cflag |= CS6;
        break;
    case 5:
        termios_new.c_cflag |= CS5;
        break;
    default:
        termios_new.c_cflag |= CS8;
        break;
    }
}

static void set_stopbit ( const char *stopbit )
{
    if ( 0 == strcmp ( stopbit, "1" ) )
    {
        termios_new.c_cflag &= ~CSTOPB; /* 1 stop bit */
    }
    else if ( 0 == strcmp ( stopbit, "1.5" ) )
    {
        termios_new.c_cflag &= ~CSTOPB; /* 1.5 stop bits */
    }
    else if ( 0 == strcmp ( stopbit, "2" ) )
    {
        termios_new.c_cflag |= CSTOPB;  /* 2 stop bits */
    }
    else
    {
        termios_new.c_cflag &= ~CSTOPB; /* 1 stop bit */
    }
}
#if 0
static void set_parity ( char parity )
{
    switch ( parity )
    {
    case 'N':                  /* no parity check */
        termios_new.c_cflag &= ~PARENB;
        break;
    case 'E':                  /* even */
        termios_new.c_cflag |= PARENB;
        termios_new.c_cflag &= ~PARODD;
        break;
    case 'O':                  /* odd */
        termios_new.c_cflag |= PARENB;
        termios_new.c_cflag |= ~PARODD;
        break;
    default:                   /* no parity check */
        termios_new.c_cflag &= ~PARENB;
        break;
    }
}
#endif

static void set_parity ( char parity )
{
    switch ( parity )
    {
    case 'N':                  /* no parity check */
    case 'n':                  /* no parity check */
        termios_new.c_cflag &= ~PARENB;
        termios_new.c_iflag &= ~INPCK;     /* Enable parity checking */
        break;
    case 'E':                  /* even */
    case 'e':                  /* even */
        termios_new.c_cflag |= PARENB;
        termios_new.c_cflag &= ~PARODD;
        termios_new.c_iflag |= INPCK;       /* Disnable parity checking */
        break;
    case 'O':                  /* odd */
    case 'o':                  /* odd */
//         termios_new.c_cflag |= PARENB;
//         termios_new.c_cflag |= ~PARODD;
        termios_new.c_cflag |= ( PARODD | PARENB ); /* 设置为奇效验*/
        termios_new.c_iflag |= INPCK;
        break;
    default:                   /* no parity check */
        termios_new.c_cflag &= ~PARENB;
        break;
    }
}



static int32_t set_port_attr (
    int32_t baudrate,       // 1200 2400 4800 9600 .. 115200
    int32_t databit,        // 5, 6, 7, 8
    const char *stopbit,    //  "1", "1.5", "2"
    char parity,             // N(o), O(dd), E(ven)
    int fd )
{
    bzero ( &termios_new, sizeof ( termios_new ) );
    cfmakeraw ( &termios_new );

    set_baudrate ( baudrate );
    termios_new.c_cflag |= CLOCAL | CREAD;      /* | CRTSCTS */
    set_data_bit ( databit );
    set_parity ( parity );
    set_stopbit ( stopbit );
    termios_new.c_oflag 			= 0;
    termios_new.c_lflag 			|= 0;
    termios_new.c_oflag 			&= ~OPOST;
    termios_new.c_cc[VTIME] 	= 1;        /* unit: 1/10 second. */
    termios_new.c_cc[VMIN] 		= 255; /* minimal characters for reading */
    tcflush ( fd, TCIFLUSH );

    return ( tcsetattr ( fd, TCSANOW, &termios_new ) );
}




/**
 * baudrate xxx to Bxxx
 *
 * @@param baudrate xxx
 *
 * @@return
 */
static int32_t baudrate2Bxx ( int32_t baudrate )
{
    switch ( baudrate )
    {
    case 0:
        return ( B0 );
    case 50:
        return ( B50 );
    case 75:
        return ( B75 );
    case 110:
        return ( B110 );
    case 134:
        return ( B134 );
    case 150:
        return ( B150 );
    case 200:
        return ( B200 );
    case 300:
        return ( B300 );
    case 600:
        return ( B600 );
    case 1200:
        return ( B1200 );
    case 2400:
        return ( B2400 );
    case 4800:
        return ( B4800 );
    case 9600:
        return ( B9600 );
    case 19200:
        return ( B19200 );
    case 38400:
        return ( B38400 );
    case 57600:
        return ( B57600 );
    case 115200:
        return ( B115200 );
    default:
        return ( B9600 );
    }
}

















int  epoll_fd;


void insertpoll ( int fd )
{
  int s;
  int fdall;
  struct epoll_event  event;
  fdall=fd;
  // insert_comm ( fd,name ); //注册一个协议处理的链表实例
  event.data.fd = fdall;
  event.events = EPOLLIN | EPOLLET;   //读入,边缘触发方式
  //  event.events = EPOLLIN ;   //读入,边缘触发方式

  s = epoll_ctl ( epoll_fd, EPOLL_CTL_ADD, fdall, &event );

}

/*********************************************************************************/
int32_t  StateEventsProcessOprate ( int fd )
{

  int32_t done = 0;
  int32_t count;
  uint8_t inbuf[1024*4+1];
  uint8_t sendbuf[1024*4+1];
  int32_t sendptr = 0;
  int32_t readptr = 0;
  int32_t sendnum = 0;

  while ( 1 )
    {

      count = read ( fd, &inbuf[readptr], sizeof ( inbuf ) );
      if ( count <= 0 )
        {
          break;
        }
      readptr += count;

    }

#if 0
  int i;
  printf ( "\n receive: " );
  for ( i = 0 ; i<readptr  ; i++ )
    {
      printf ( "0x%0x ",inbuf[i] );
    }

#endif


  for ( sendptr = 0; sendptr < readptr; )
    {
      count = write ( fd, &inbuf[sendptr], readptr );

      if ( count == -1 )
        {
          /* If errno == EAGAIN, that means send buffer is full. So send later. */
          if ( errno == EAGAIN )
            {
              continue;
            }
          else
            {
              perror ( "write err" );
              done = 1;
              break;
            }
        }
      sendptr += count;
    }
#if 0

  printf ( "\n send:sendptr %d \n",sendptr );
  for ( i = 0 ; i<sendptr  ; i++ )
    {
      printf ( "0x%0x ",sendbuf[i] );
    }

#endif



}


#define MAXREADCOMN (int32_t) 1024
int comRecPcArray[MAXREADCOMN]  = {0};

int  com_send_data ( int  comfd );



int  com_send_data ( int  comfd )
{

  char send_data_buf[100] = {0x01, 0x02 , 0x03 , 0x04 , 0x05 , 0x06 ,0x07, 0x08,0x09, 0x10,0x11, 0x12 , 0x13 , 0x14 , 0x15 , 0x16 ,0x17, 0x18,'\0'};
  printf("send data: 0x01, 0x02 , 0x03 , 0x04 , 0x05 , 0x06 ,0x07, 0x08,0x09, 0x10,0x11, 0x12 , 0x13 , 0x14 , 0x15 , 0x16 ,0x17, 0x18    \n");
  int k =0;
  int count = 0;
  int send_len  = 18;

  printf ( "send_len %d \n",send_len );
  for ( k = 0; k < send_len; )
    {
      count = write ( comfd, &send_data_buf[k], 18 );

      if ( count == -1 )
        {
          /* If errno == EAGAIN, that means send buffer is full. So send later. */
          if ( errno == EAGAIN )
            {

              printf ( "  errno == EAGAIN, that means send buffer is full. So send later \n   " );
              perror ( " errno == EAGAIN" );
              continue;
            }
          else
            {
              printf ( "write err" );
              perror ( "write err" );

              break;
            }
        }
      k += count;
    }
  printf ( "count %d \n",count );
}

/*******************************************************************************
 * Function       : EpollAddEvent
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  :设置event方式加入到epoll句柄中
 * Calls           : create_and_bind  make_socket_non_blocking  listen
 * Input          :  int32_t epoll_fd  句柄集,int32_t  fd 网络设备句柄,u_int32_t  EventType 读入边缘方式（预留）
 * Output       :
 * Return        :
 ********************************************************************************/
void EpollAddEvent ( int32_t epoll_fd,int32_t fd,u_int32_t  EventType )
{

  int32_t s;
  struct epoll_event event;

  // insert_comm ( fd,name ); //注册一个协议处理的链表实例
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET;   //读入,边缘触发方式
  s = epoll_ctl ( epoll_fd, EPOLL_CTL_ADD, fd, &event );
  if ( s == -1 )
    {
      perror ( "epoll_ctl err" );
      abort ();
    }
}




/*******************************************************************************
 * Function       : InsertNetEpoll
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  : 创建，监听socket句柄
 * Calls           : create_and_bind  make_socket_non_blocking  listen
 * Input          :  CommConnObj  *  insertComm_reg     //端口链接对象结构体指针
 * Output       :
 * Return        :
 ********************************************************************************/





/*******************************************************************************
 * Function       : ErrorAbort
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  : 错误处理
 * Calls           : perror  abort
 * Input          :  int32_t ret_s,  char * errorShow
 * Output       :
 * Return        :
 ********************************************************************************/
void ErrorAbort ( int ret_s,  char * errorShow )
{

  printf ( "s = %d\n", ret_s );
  if ( ret_s == -1 )
    {
      perror ( errorShow );
      abort ();
    }


}

/*******************************************************************************
 * Function       : socketAcceptLink
 * Author          : shiran
 * Date              : 2016.02.01
 * Description  : socket客户端连接
 * Calls           : accept
 * Input          :  int32_t sfd  socket创建句柄
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t   socketAcceptLink ( int32_t  sfd )
{
  char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

  struct sockaddr in_addr;
  socklen_t in_len;
  int32_t  retfd = 0;
  int32_t  s;
//     char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

  in_len = sizeof in_addr;
  retfd = accept ( sfd, &in_addr, &in_len );
//     printf ( "%d infd \n",retfd );
  if ( retfd == -1 )
    {
      if ( ( errno == EAGAIN ) ||
           ( errno == EWOULDBLOCK ) )
        {
          /* We have processed all incoming
             connections. */
          //   break; //处理完 lisen队列里所有的连接
          return retfd;
        }
      else
        {
          perror ( "accept err" );
          //   break;
          return  retfd;
        }
    }
  //将地址转化为主机名或者服务名
  s = getnameinfo ( &in_addr, in_len,
                    hbuf, sizeof hbuf,
                    sbuf, sizeof sbuf,
                    NI_NUMERICHOST | NI_NUMERICSERV ); //flag参数:以数字名返回
  //主机地址和服务地址

  if ( s == 0 )
    {
      printf ( "Accepted connection on descriptor %d (host=%s, port=%s)\n", retfd, hbuf, sbuf );
    }

  return retfd;
}


int wirteReadEeprom ( void )
{
  int fd;
  //int fd1;
  int ret;
  unsigned char u8_wr_buf[256]= {0};
  unsigned char u8_rd_buf[256]= {0};
  unsigned long block;
  unsigned int u32_wr_cnt = 0;
  int count = 0;

  fd = open ( "/sys/class/spi_master/spi2/spi2.0/eeprom",O_RDWR );
  if ( fd < 0 )
    {
      printf ( "open device error\n" );
      return 0;
    }
  else
    {
      printf ( "open file ok\n" );
    }

  int i;
  for ( i = 0; i<250; i++ )
    {
      u8_wr_buf[i]=i;
      //   printf ( "%d  ",u8_wr_buf[i] );
    }
  ret = write ( fd,u8_wr_buf,250 );
  if ( ret != 250 )
    {
      printf ( "ERROR write\n" );
    }
  else
    {

      printf ( "\r\n write OK \r\n" );
    }



//     for ( i = 0; i<250; i++ )
//     {
//         u8_wr_buf[i]=55;
//         //   printf ( "%d  ",u8_wr_buf[i] );
//
//     }
//    printf ( "\r\n" );
  lseek ( fd,SEEK_SET,0 );
  ret = read ( fd,u8_rd_buf,250 );
  //  printf ( "ret=%d\r\n",ret );
  if ( ret != 250 )
    {
      printf ( "ERROR read\n" );
    }
  else
    {


      if ( 0== memcmp ( u8_wr_buf,u8_rd_buf,250 ) )
        {
          printf ( "\r\n eeprom read OK \r\n" );
        }
      else
        {
          printf ( "ERROR eeprom\n" );
        }
    }

//     for ( i = 0; i<250; i++ )
//     {
//         printf ( "%d  ",u8_wr_buf[i] );
//     }
//     printf ( "\r\n" );



  close ( fd );

}


/*******************************************************************************
 * Function       : DirExist
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : 文件是否存在,
 * Calls              :access 测试文件存在与否
 * Input             : char* configfilePATH xml文件路径
 * Output          :
 * Return           :存在返回真  不存在返回假
 ********************************************************************************/
int DirExist ( char *FilePath )
{
// printf("---%d --- ", access ( FilePath,F_OK ) ==0);
  return ( access ( FilePath,F_OK ) ==0 );
}


int usbOperate1()
{

  unsigned char Buff_Temp[1025]= {0};

  char tempbufr[1024]= {0};

  time_t now;
  struct tm *timenow;
  time_t end;
  struct tm *timeend;
  char usbTofile[1024]= {0};
  int fd;

  int filelen= 0;

//     if ( DirExist ( "/dev/sda1" ) )
//     {
//
//     }
//     else
//     {
//         printf ( "usb device error\n" );
//         return ;
//     }
  fd = open ( "usb_local.txt",O_RDWR|O_CREAT|O_TRUNC );
  if ( fd < 0 )
    {
      //  printf ( "open device error\n" );
      return 0;
    }
  printf ( "22222222222222222\n" );
  int i;
  int ret;
  char tempbufw[1024]= {1,2,3,4,5,6,7,8,9,10,'\0'};
  filelen =strlen ( tempbufw ) ;
  printf ( "\r\n" );
  lseek ( fd,SEEK_SET,0 );
  ret = write ( fd,tempbufw,filelen );
  //  printf ( "write ret=%d:",ret );
//     for ( i = 0; i<filelen; i++ )
//     {
//         printf ( "%d  ",tempbufw[i] );
//     }
//     printf ( "\r\n" );

  close ( fd );


  system ( "cp -rf usb_local.txt /media" );

  time ( &now );
  timenow=localtime ( &now );
  sprintf ( Buff_Temp,"cp -rf   usb_local.txt /media/usb_%02d%02d%02d.txt",timenow->tm_hour,timenow->tm_min,timenow->tm_sec );
  system ( Buff_Temp );

  sprintf ( usbTofile,"usb_%02d%02d%02d.txt",timenow->tm_hour,timenow->tm_min,timenow->tm_sec );
  memset ( Buff_Temp,0,sizeof ( Buff_Temp ) );

  sprintf ( Buff_Temp,"cp -rf   /media/%s  ./ ",usbTofile );
  system ( Buff_Temp );

  sprintf ( Buff_Temp,"rm -rf   ./%s  ",usbTofile );

  fd = open ( usbTofile,O_RDWR );
  if ( fd < 0 )
    {
      printf ( "open device error\n" );
      return 0;
    }
  else
    {

      //   printf ( "open file ok\n" );
    }




  // printf ( "\r\n" );
  lseek ( fd,SEEK_SET,0 );
  ret = read ( fd,tempbufr,250 );
  //  printf ( "read  ret=%d:",ret );


  if ( 0 == memcmp ( tempbufw,tempbufr,filelen ) )
    {

      system ( Buff_Temp );
      printf ( "usb operate  OK \r\n" );
    }
  else
    {
      system ( Buff_Temp );
      printf ( "usb operate ERROR \n" );

    }


  close ( fd );


}


int usbOperate()
{

  unsigned char Buff_Temp[1025]= {0};

  char tempbufr[1024]= {0};

  time_t now;
  struct tm *timenow;
  time_t end;
  struct tm *timeend;
  char usbTofile[1024]= {0};
  int fd;

  int filelen= 0;

//      char dirname[]="/proc/scsi/usb-storage";

  if ( DirExist ( "/dev/sda1" ) &&DirExist ( "/proc/scsi/usb-storage" ) )
    {
      printf ( "RRRRR---usb operate  OK \r\n" );
    }
  else
    {
      printf ( "XXXXX---usb operate ERROR \n" );
    }

  return ;



}

void printWriteCom ( void )
{


  time_t now;
  struct tm *timenow;
  time_t end;
  struct tm *timeend;

  while ( 1 )
    {
      time ( &now );
      timenow=localtime ( &now );

      printf ( "%02d:%02d:%02d:%02d:%02d:%02d\n",timenow->tm_year+1900  ,  timenow->tm_mon  ,timenow->tm_mday  ,timenow->tm_hour,timenow->tm_min,timenow->tm_sec );
      sleep ( 10*1 );
    }
  return ;
}








int xopen_rtc()
{

  int fd;

  struct rtc_time rtc;

  system ( "date 10:11" );

  system ( "hwclock -w " );

  fd= open ( "/dev/rtc0", O_RDONLY );
  if ( fd <=  0 )
    {

      printf ( "RTC error\n" );

      exit ( EXIT_FAILURE );

    }

  ioctl ( fd,RTC_RD_TIME,&rtc );



  if ( ( rtc.tm_hour == 10 ) && ( rtc.tm_min == 11 ) )
    {
      printf ( "RTC OK\n" );
    }
  else
    {
      printf ( "RTC error\n" );
    }

  // printf ( "\n\nCurrentRTC data/time is %d-%d-%d, %02d:%02d:%02d.\n", rtc.tm_mday, rtc.tm_mon +1,rtc.tm_year + 1900, rtc.tm_hour, rtc.tm_min, rtc.tm_sec );


  close ( fd );



  exit ( EXIT_SUCCESS );

}
// return xopen(rtcname, flags);



#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <asm/types.h>
#include <linux/watchdog.h>
#include <sys/stat.h>
#include <signal.h>



#define  sleep_time (int)3
int fd;
void catch_int ( int signum )
{
  signal ( SIGINT, catch_int );

  //  printf ( "In signal handler\n" );
  if ( 0 != close ( fd ) )
    printf ( "Close failed in signal handler\n" );
  else
    printf ( "Close succeeded in signal handler\n" );
}
int watchReboot ( void )
{

//  int sleep_time = atoi ( argv[1] );
//     int fd;
  int data = 0;
  int ret_val;
  // daemon ( 0,0 );
  signal ( SIGINT, catch_int );

  fd=open ( "/dev/watchdog",O_WRONLY );

  if ( fd <  0 )
    {
      printf ( "error-Watchdog\n" );
      return 1;
    }

  ret_val = ioctl ( fd, WDIOC_GETTIMEOUT, &data );
  if ( ret_val )
    {
      // printf ( "\nWatchdog Timer : WDIOC_GETTIMEOUT failed" );
    }
  else
    {
      //  printf ( "\nCurrent timeout value is : %d seconds\n", data );
    }

  data = 1;

  ret_val = ioctl ( fd, WDIOC_SETTIMEOUT, &data );
  if ( ret_val )
    {
      printf ( "\nWatchdog Timer : WDIOC_SETTIMEOUT failed" );
    }
  else
    {
      printf ( "\nNew timeout value is : %d seconds", data );
    }

  ret_val = ioctl ( fd, WDIOC_GETTIMEOUT, &data );
  if ( ret_val )
    {
      printf ( "\nWatchdog Timer : WDIOC_GETTIMEOUT failed" );
    }
  else
    {
      printf ( "\nCurrent timeout value is : %d seconds\n", data );
    }

  while ( 1 )
    {
      if ( 1 != write ( fd, "\0", 1 ) )
        {
          printf ( "Write failed\n" );
          break;
        }
      // else
      //   printf ( "Write succeeded\n" );
      sleep ( sleep_time );
    }

  if ( 0 != close ( fd ) )
    printf ( "Close failed\n" );
  else
    printf ( "Close succeeded\n" );

}


int modbus_type_right ( char modbus_buf[], int len );
int main ( int argc, char **argv )
{



  int  com_id = 0;
  int  com_rate = 0;
  int com_date_bit = 0;
  
  if( argc < 5 )
  {
      printf("./comtest  4 9600 8 \"1\" \'N\' \n");
      return ;
  }
  else 
  {
      printf("./comtest  4 9600 8 \"1\" \'N\' \n");
  }

  com_id =  atoi ( argv[1] );

  com_rate=atoi ( argv[2] );
  
  com_date_bit=atoi ( argv[3] );
  
  char jiaoyan=argv[5][0];
  




  int com_fd  = 0;

  switch ( com_id )
    {
    case 1:
      com1  =   OpenComPort ( 1,  9600, 8,"1",'N' );
      com_fd = com1;
      break;
    case 2:
      com2  =   OpenComPort ( 2,  9600, 8,"1",'N' );
      com_fd = com2;
      break;
    case 3:
      com3  =   OpenComPort ( 3,  9600, 8,"1",'N' );
      com_fd = com3;
      break;
    case 4:
      printf("4 , com_rate %d com_date_bit %d argv[4] %s jiaoyan %c  \n", com_rate, com_date_bit,argv[4],jiaoyan);
      com4  =   OpenComPort ( 4 , com_rate, com_date_bit,argv[4],jiaoyan );
      com_fd = com4;
      break;
    case 5:
      com5 =   OpenComPort ( 5,  115200, 8,"1",'N' );
      com_fd = com5;
      break;
    case 6:
      com6 =   OpenComPort ( 6,  9600, 8,"1",'N' );
      com_fd = com6;
      break;
    case 7:
      com7 =   OpenComPort ( 7,  9600, 8,"1",'N' );
      com_fd = com7;
      break;
    case 8:
      com8 =   OpenComPort ( 8,  9600, 8,"1",'N' );
      com_fd = com8;
      break;
    case 9:
      com9 =   OpenComPort ( 9,  9600, 8,"1",'N' );
      com_fd = com9;
      break;

    default:
      printf ( "--------------------error com------------------\n" );
      break;
    }



  int testnum = 0;
  uint8_t comRecPcArray[1024]  = {0};
  uint8_t readbuf[1024]= {0};

  int32_t    i;
  int32_t            retval = 0;
  int32_t bytes_read_count;
  int32_t readlen;

  tcflush ( com_fd, TCIOFLUSH );

//     ioctl ( comfd,FIOSETOPTIONS,OPT_RAW );
//     ioctl ( comfd,FIOFLUSH,0 );

  fd_set fdread;
  fd_set fdreadShunt;
  int32_t maxfd = com_fd+1;
  static struct timeval tv_timeout= {0,5000};

  int32_t id;
  int32_t cmd;
  int32_t len;
  int32_t cmdlen = 8;
  int32_t   ret = 0;


  com_send_data ( com_fd );


  memset ( readbuf,0,sizeof ( readbuf ) );

  bytes_read_count = 0;

  while ( bytes_read_count<MAXREADCOMN )
    {
      tv_timeout.tv_sec = 1;
      tv_timeout.tv_usec=0;

      FD_ZERO ( &fdread );
      FD_SET ( com_fd, &fdread );

      retval = select ( maxfd, &fdread, NULL, NULL, &tv_timeout );
      if ( retval >0 )
        {
          while ( 1 )
            {
              readlen = read ( com_fd, ( readbuf+bytes_read_count ), MAXREADCOMN );
              if ( readlen <= 0 )
                {
                  break;
                }
              bytes_read_count += readlen;
            }

        }
      else if ( retval == 0 )
        {

          if ( bytes_read_count > 0 )
            {

              printf ( "comfd %d \n",com_fd );
              for ( i = 0 ; i<bytes_read_count  ; i++ )
                {
                  printf ( "0x%0x ",readbuf[i] );
                }
              printf ( "\n " );


              int flag = 0;
              flag =        modbus_type_right ( readbuf, bytes_read_count );

              if ( flag  == 0 )
                {
                  printf ( "*******************ERROR**********************\n" );
                }
              else
                {
                  printf ( "------------------------------OK------------------------\n" );
                }
              return 0;

              bytes_read_count = 0;

            }
          else
            {
              printf ( "*******************ERROR**********************\n" );
              return 0;

            }

        }
      else if ( ret == -1 )
        {
          printf ( "*******************ERROR**********************\n" );
          return 0;
        }
    }


    
    CloseComPort(com_fd);
}



/*******************************************************************************
 * Function       : getCRC16
 * Author         : Xu Shun'an
 * Date           : 2011.09.13
 * Description    : CRC
 * Calls          : None
 * Input          : volatile uint8_t *ptr, uint16_t len
 * Output         : None
 * Return         : CRC
 ********************************************************************************/

uint16_t getCRC16 ( volatile uint8_t *ptr, uint16_t len )
{
  uint8_t i;
  uint16_t crc=0xFFFF;
  if ( len==0 )
    {
      len=1;
    }
  while ( len-- )
    {
      crc ^= *ptr;
      for ( i=0; i<8; i++ )
        {
          if ( crc&1 )
            {
              crc >>= 1;
              crc ^= 0xA001;
            }
          else
            {
              crc >>= 1;
            }
        }
      ptr++;
    }
  return ( crc );
}
/*************************************************************************
 * Function       : crccheck
 * Author         : linfei
 * Date           : 2011.10.31
 * Description    : CRC(CRC16/CRC32)
 * Calls          : getCRC16
 * Input          : pbuf: 待校验数据指针
 *                  len: 待校验数据（包含crc）数据字节长度
 *                  crctype: CRC类型,=0 ?CRC16,=1 ?CRC32
 * Output         : None
 * Return         : 0: crc校验错误;1:crc校验成功
**************************************************************************/
uint8_t crccheck ( uint8_t *pbuf, uint16_t buflen, uint8_t crctype )
{
  if ( buflen < 3 )
    {
      return 0;
    }
  uint32_t crcresult = 0;
  uint8_t tmp[4] = {0};

  if ( crctype == 0 )
    {
      crcresult = getCRC16 ( pbuf, buflen - 2 );
      tmp[0] = crcresult & 0xff;
      tmp[1] = ( crcresult >> 8 ) & 0xff;


      //    printf("%0x:tmp[0] ) 0x%0x   %0x :tmp[1 0x%0x]  \n",pbuf[buflen-2],tmp[0] , pbuf[buflen-1],tmp[1] );
      if ( ( pbuf[buflen-2] == tmp[0] ) && ( pbuf[buflen-1] == tmp[1] ) )
        {
          return 1;
        }

    }


  return 0;
}




int modbus_type_right ( char modbus_buf[], int len )
{
  if ( crccheck ( modbus_buf, len, 0 ) )
    {
      return 1;
    }
  else
    {
      return 0;
    }

}






