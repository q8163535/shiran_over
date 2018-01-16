#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <net/route.h>
#include <unistd.h>
#include <stdio.h>

int set_gateway ( unsigned long gw )
{
    int skfd;
    struct rtentry rt;
    int err;

    skfd = socket ( PF_INET, SOCK_DGRAM, 0 );
    if ( skfd < 0 )
        return -1;

    /* Delete existing defalt gateway */
    memset ( &rt, 0, sizeof ( rt ) );

    rt.rt_dst.sa_family = AF_INET;
    ( ( struct sockaddr_in * ) &rt.rt_dst )->sin_addr.s_addr = 0;

    rt.rt_genmask.sa_family = AF_INET;
    ( ( struct sockaddr_in * ) &rt.rt_genmask )->sin_addr.s_addr = 0;

    rt.rt_flags = RTF_UP;

    err = ioctl ( skfd, SIOCDELRT, &rt );

    if ( ( err == 0 || errno == ESRCH ) && gw )
    {
        /* Set default gateway */
        memset ( &rt, 0, sizeof ( rt ) );

        rt.rt_dst.sa_family = AF_INET;
        ( ( struct sockaddr_in * ) &rt.rt_dst )->sin_addr.s_addr = 0;

        rt.rt_gateway.sa_family = AF_INET;
        ( ( struct sockaddr_in * ) &rt.rt_gateway )->sin_addr.s_addr = gw;

        rt.rt_genmask.sa_family = AF_INET;
        ( ( struct sockaddr_in * ) &rt.rt_genmask )->sin_addr.s_addr = 0;

        rt.rt_flags = RTF_UP | RTF_GATEWAY;

        err = ioctl ( skfd, SIOCADDRT, &rt );
    }

    close ( skfd );

    return err;
}
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int get_gateway ( unsigned long *p )
{
    FILE *fp;
    char buf[256]; // 128 is enough for linux
    char iface[16];
    unsigned long dest_addr, gate_addr;
    *p = INADDR_NONE;
    fp = fopen ( "/proc/net/route", "r" );
    if ( fp == NULL )
        return -1;
    /* Skip title line */
    fgets ( buf, sizeof ( buf ), fp );
    while ( fgets ( buf, sizeof ( buf ), fp ) )
    {
        if ( sscanf ( buf, "%s\t%lX\t%lX", iface,       &dest_addr, &gate_addr ) != 3 ||
                dest_addr != 0 )
            continue;
        *p = gate_addr;
        break;
    }

    fclose ( fp );
    return 0;
}






#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <error.h>
#include <net/route.h>




int SetIfAddr(char *gateway)
{
    int fd;
    int rc;
    struct ifreq ifr; 
    struct sockaddr_in *sin;
    struct rtentry  rt;

   
    //网关
    memset(&rt, 0, sizeof(struct rtentry));
    memset(sin, 0, sizeof(struct sockaddr_in));
    sin->sin_family = AF_INET;
    sin->sin_port = 0;
    if(inet_aton(gateway, &sin->sin_addr)<0)
    {
       printf ( "inet_aton error\n" );
    }
    memcpy ( &rt.rt_gateway, sin, sizeof(struct sockaddr_in));
    ((struct sockaddr_in *)&rt.rt_dst)->sin_family=AF_INET;
    ((struct sockaddr_in *)&rt.rt_genmask)->sin_family=AF_INET;
    rt.rt_flags = RTF_GATEWAY;
    if (ioctl(fd, SIOCADDRT, &rt)<0)
    {
        printf( "ioctl(SIOCADDRT) error in set_default_route\n");
        close(fd);
        return -1;
    }
    close(fd);
    return rc;
}

#if 0
int  main()
{
    struct in_addr ip_route;
    ulong net_gate_way = 0;

    get_gateway ( &net_gate_way );

    memcpy ( &ip_route, &net_gate_way, 4 );

    printf ( "ip_route:%s \n", inet_ntoa ( ip_route ) ); //注意这一句的运行结果


}
#endif


#include <arpa/inet.h>  //for in_addr   
#include <linux/rtnetlink.h>    //for rtnetlink   
#include <net/if.h> //for IF_NAMESIZ, route_info   
#include <stdlib.h> //for malloc(), free()   
#include <string.h> //for strstr(), memset()   

#define BUFSIZE 8192   

int get_gateway_1(char *gateway)   ;
    
struct route_info{   
 u_int dstAddr;   
 u_int srcAddr;   
 u_int gateWay;   
 char ifName[IF_NAMESIZE];   
};   
int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)   
{   
  struct nlmsghdr *nlHdr;   
  int readLen = 0, msgLen = 0;   
  do{   
    //收到内核的应答   
    if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0)   
    {   
      perror("SOCK READ: ");   
      return -1;   
    }   
      
    nlHdr = (struct nlmsghdr *)bufPtr;   
    //检查header是否有效   
    if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))   
    {   
      perror("Error in recieved packet");   
      return -1;   
    }   
      
       
    if(nlHdr->nlmsg_type == NLMSG_DONE)    
    {   
      break;   
    }   
    else   
    {   
         
      bufPtr += readLen;   
      msgLen += readLen;   
    }   
      
       
    if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)    
    {   
         
     break;   
    }   
  } while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));   
  return msgLen;   
}   
//分析返回的路由信息   
void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway)   
{   
  struct rtmsg *rtMsg;   
  struct rtattr *rtAttr;   
  int rtLen;   
  char *tempBuf = NULL;   
  struct in_addr dst;   
  struct in_addr gate;   
     
  tempBuf = (char *)malloc(100);   
  rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);   
  // If the route is not for AF_INET or does not belong to main routing table   
  //then return.    
  if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))   
  return;   
     
  rtAttr = (struct rtattr *)RTM_RTA(rtMsg);   
  rtLen = RTM_PAYLOAD(nlHdr);   
  for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen)){   
   switch(rtAttr->rta_type) {   
   case RTA_OIF:   
    if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);   
    break;   
   case RTA_GATEWAY:   
    rtInfo->gateWay = *(u_int *)RTA_DATA(rtAttr);   
    break;   
   case RTA_PREFSRC:   
    rtInfo->srcAddr = *(u_int *)RTA_DATA(rtAttr);   
    break;   
   case RTA_DST:   
    rtInfo->dstAddr = *(u_int *)RTA_DATA(rtAttr);   
    break;   
   }   
  }   
  dst.s_addr = rtInfo->dstAddr;   
  if (strstr((char *)inet_ntoa(dst), "0.0.0.0"))   
  {   
    printf("oif:%s",rtInfo->ifName);   
    gate.s_addr = rtInfo->gateWay;   
    sprintf(gateway, (char *)inet_ntoa(gate));   
    printf("%s\n",gateway);   
    gate.s_addr = rtInfo->srcAddr;   
    printf("src:%s\n",(char *)inet_ntoa(gate));   
    gate.s_addr = rtInfo->dstAddr;   
    printf("dst:%s\n",(char *)inet_ntoa(gate));    
  }   
  free(tempBuf);   
  return;   
}   
   
int get_gateway_1(char *gateway)   
{   
 struct nlmsghdr *nlMsg;   
 struct rtmsg *rtMsg;   
 struct route_info *rtInfo;   
 char msgBuf[BUFSIZE];   
    
 int sock, len, msgSeq = 0;   
   
 if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)   
 {   
  perror("Socket Creation: ");   
  return -1;   
 }   
    
    
 memset(msgBuf, 0, BUFSIZE);   
    
    
 nlMsg = (struct nlmsghdr *)msgBuf;   
 rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);   
    
    
 nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.   
 nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .   
    
 nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.   
 nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.   
 nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.   
    
    
 if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0){   
  printf("Write To Socket Failed…\n");   
  return -1;   
 }   
    
    
 if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0) {   
  printf("Read From Socket Failed…\n");   
  return -1;   
 }   
    
 rtInfo = (struct route_info *)malloc(sizeof(struct route_info));   
 for(;NLMSG_OK(nlMsg,len);nlMsg = NLMSG_NEXT(nlMsg,len)){   
  memset(rtInfo, 0, sizeof(struct route_info));   
  parseRoutes(nlMsg, rtInfo,gateway);   
 }   
 free(rtInfo);   
 close(sock);   
 return 0;   
}   
   
int main()   
{   
    char buff[256];   
    get_gateway(buff);   
    return 0;   
}  
