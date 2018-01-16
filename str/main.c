







#if 0





























void getPoint ( char * PPort )
{
    char *PPname = PPort;
    while ( ( PPname = strchr ( PPname+1, ',' ) ) != NULL )
    {
        PPname= PPname+1;
        printf ( "PPname %s \n",PPname );
    }

}

int array[10];


unsigned int  xmlChar_change_u32 ( unsigned char * data_Attr )
{

    unsigned char  tmp_data[32] = { 0 };

    unsigned char var_data[32] = { 0 };

    strcpy ( tmp_data ,data_Attr );

    unsigned int ret = 0;


    if ( ( ( tmp_data[0] == '0' ) && ( tmp_data[1] == 'x' ) ) || ( ( tmp_data[0] == '0' ) && ( tmp_data[1] == 'X' ) ) )
    {
        strcpy ( var_data, &tmp_data[2] );
        ret  = strtoul ( ( char * ) var_data, NULL,    16 ) ;

        printf ( "0x%0X \n",ret );

    }
    else
    {
        ret  = strtoul ( ( char * ) tmp_data, NULL,    10 ) ;
    }

    return ret;

}


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
void get_addr_eth()
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset ( &hints, 0, sizeof hints );
//   --- 必须先清零再使用
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP address ------- 此情况下，第一个参数可以为null，则返回所有IP，相当于bind（INADDR_ANY）。

    if ( ( rv = getaddrinfo ( NULL, "3490", &hints, &servinfo ) ) != 0 )
    {
        fprintf ( stderr, "getaddrinfo: %s\n", gai_strerror ( rv ) );
        exit ( 1 );
    }

// loop through all the results and bind to the first we can
    for ( p = servinfo; p != NULL; p = p->ai_next )
    {
        //   ---------------------------------使用实例：可以参考gsoap的tcp_connect，soap_bind等函数。

        printf ( " p->ai_family %d p->ai_socktype %d     p->ai_protocol \n", p->ai_family , p->ai_socktype, p->ai_protocol );
        if ( ( sockfd = socket ( p->ai_family, p->ai_socktype,
                                 p->ai_protocol ) ) == -1 )
        {
            perror ( "socket" );
            continue;
        }

        if ( bind ( sockfd, p->ai_addr, p->ai_addrlen ) == -1 )
        {
            // ------ 尽管返回多个ip，但有些可能会失败，所以需要尝试
            close ( sockfd );
            perror ( "bind" );
            continue;
        }

        break; // if we get here, we must have connected successfully
    }

    if ( p == NULL )
    {
        // looped off the end of the list with no successful bind
        fprintf ( stderr, "failed to bind socket\n" );
        exit ( 2 );
    }

    freeaddrinfo ( servinfo ); // all done with this structure ------- 必须调用，否则内存泄露
}


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>
#include <arpa/inet.h>

#include <string.h>


// ********************************************************************************/
uint16_t get_element_from_str ( char *buf,char *element,char separator )
{
    uint32_t i = 0;
    for ( i = 0; ( buf[i]!= separator ) && ( buf[i]!= '\0' ) && ( i< 127 ); ++i )
    {
        element[i] = buf[i];
    }
    element[i] = '\0';
//     return i;
//     printf ( "element %s  i %d \n",element,i );
    if ( element[i-1] == '\r' )
    {
        element[i-1] = '\0';
    }
    return ( i+1 );

}


void DispNetInfo ( const char* szDevName )
{
    int s = socket ( AF_INET, SOCK_DGRAM, 0 );
    if ( s < 0 )
    {
        fprintf ( stderr, "Create socket failed!errno=%d", errno );
        return;
    }
    struct ifreq ifr;
    unsigned char mac[6];
    unsigned long nIP, nNetmask, nBroadIP;
    printf ( "%s:\n", szDevName );
    strcpy ( ifr.ifr_name, szDevName );
    if ( ioctl ( s, SIOCGIFHWADDR, &ifr ) < 0 )
    {
        return;
    }
    memcpy ( mac, ifr.ifr_hwaddr.sa_data, sizeof ( mac ) );
    printf ( "\tMAC: %02x-%02x-%02x-%02x-%02x-%02x\n",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
    strcpy ( ifr.ifr_name, szDevName );
    if ( ioctl ( s, SIOCGIFADDR, &ifr ) < 0 )
    {
        nIP = 0;
    }
    else
    {
        nIP = * ( unsigned long* ) &ifr.ifr_broadaddr.sa_data[2];
    }



    struct sockaddr_in *myaddr;
    myaddr = ( struct sockaddr_in* ) & ( ifr.ifr_addr );
    printf ( "IP: %s\n", inet_ntoa ( myaddr->sin_addr ) );


    printf ( "\tIP: %s\n", inet_ntoa ( * ( struct in_addr* ) &nIP ) );
    strcpy ( ifr.ifr_name, szDevName );
    if ( ioctl ( s, SIOCGIFBRDADDR, &ifr ) < 0 )
    {
        nBroadIP = 0;
    }
    else
    {
        nBroadIP = * ( unsigned long* ) &ifr.ifr_broadaddr.sa_data[2];
    }
    printf ( "\tBroadIP: %s\n", inet_ntoa ( * ( struct in_addr* ) &nBroadIP ) );
    strcpy ( ifr.ifr_name, szDevName );
    if ( ioctl ( s, SIOCGIFNETMASK, &ifr ) < 0 )
    {
        nNetmask = 0;
    }
    else
    {
        nNetmask = * ( unsigned long* ) &ifr.ifr_netmask.sa_data[2];
    }
    printf ( "\tNetmask: %s\n", inet_ntoa ( * ( struct  in_addr* ) &nNetmask ) );
    close ( s );
}



int32_t get_net_ip_info ( const char* comm_name ,char *ip1,char *ip2,char *ip3,char *ip4 )
{
    int32_t ret  = 0;
    int32_t len = 0;
    int32_t  socket_fd = socket ( AF_INET, SOCK_DGRAM, 0 );
    if ( socket_fd < 0 )
    {
        fprintf ( stderr, "Create socket failed!errno=%d", errno );
        return;
    }
    struct ifreq ifr ;
    char ip_info[30] = {0};

    char tmp_ip_info[10];

    strcpy ( ifr.ifr_name, comm_name );
    if ( ioctl ( socket_fd, SIOCGIFADDR, &ifr ) < 0 )
    {
        ret = -1;
    }
    else
    {

        struct sockaddr_in *myaddr;
        myaddr = ( struct sockaddr_in* ) & ( ifr.ifr_addr );
        strcpy ( ip_info , inet_ntoa ( myaddr->sin_addr ) );

        len +=   get_element_from_str ( & ip_info[len],ip1,'.' );
        printf ( "IP: %s: tmp_ip_info %s \n", ip_info,ip1 );

        len +=   get_element_from_str ( &ip_info[len],ip2,'.' );
        printf ( "IP: %s: tmp_ip_info %s \n", ip_info,ip2 );

        len +=   get_element_from_str ( & ip_info[len],ip3,'.' );
        printf ( "IP: %s: tmp_ip_info %s \n", ip_info,ip3 );

        len +=   get_element_from_str ( & ip_info[len],ip4,'.' );
        printf ( "IP: %s: tmp_ip_info %s \n", ip_info,ip4 );


    }
    close ( socket_fd );
    return ret;
}






int32_t get_net_mask_info ( const char* comm_name,char *mask1,char *mask2,char *mask3,char *mask4 )
{
    int32_t ret  = 0;
    struct sockaddr_in *myaddr;
    char net_mask_info[30] = { 0 };
    int32_t len = 0;
    int32_t  socket_fd = socket ( AF_INET, SOCK_DGRAM, 0 );
    if ( socket_fd < 0 )
    {
        fprintf ( stderr, "Create socket failed!errno=%d", errno );
        return;
    }
    struct ifreq ifr ;


    strcpy ( ifr.ifr_name, comm_name );
    if ( ioctl ( socket_fd, SIOCGIFNETMASK, &ifr ) < 0 )
    {
        ret = -1 ;
    }
    else
    {
        myaddr = ( struct sockaddr_in* ) & ( ifr.ifr_netmask );
        printf ( "\tNetmask: %s\n",  inet_ntoa ( myaddr->sin_addr ) );
        strcpy ( net_mask_info , inet_ntoa ( myaddr->sin_addr ) );

        len +=   get_element_from_str ( & net_mask_info[len],mask1,'.' );
        printf ( "IP: %s: tmp_ip_info %s \n", net_mask_info,mask1 );

        len +=   get_element_from_str ( &net_mask_info[len],mask2,'.' );
        printf ( "IP: %s: tmp_ip_info %s \n", net_mask_info,mask2 );

        len +=   get_element_from_str ( & net_mask_info[len],mask3,'.' );
        printf ( "IP: %s: tmp_ip_info %s \n", net_mask_info,mask3 );

        len +=   get_element_from_str ( & net_mask_info[len],mask4,'.' );
        printf ( "IP: %s: tmp_ip_info %s \n", net_mask_info,mask4 );


    }
    close ( socket_fd );
    return ret;
}





int32_t  get_dns ( char *dns1, char *dns2 )
{

    FILE*fp=NULL;
    char *line = NULL;
    int32_t  section_len  = 0;

    char tmp_dns[30] = { 0 };

    fp=fopen ( "/etc/resolv.conf","r" );
    if ( NULL==fp )
    {
        return 0;
    }
    fseek ( fp, 0, SEEK_SET );

    size_t len = 0;
    ssize_t read;
    int32_t name_server_num = 0;

    while ( ( read = getline ( &line, &len, fp ) ) != -1 )
    {

//         printf ( "line : %s len :%d \n",line, len );

        if ( strstr ( line,"nameserver" ) )
        {
            name_server_num ++;

            if ( name_server_num == 1 )
            {
                strcpy ( dns1 ,&line[10] );
            }
            else if ( name_server_num == 2 )
            {
                strcpy ( dns2 ,&line[10] );
            }

        }


    }

    free ( line );


    printf ( "dns1 : %s dns2 :%s \n", dns1,dns2 );

// return ;
    if ( strlen ( dns1 ) >= 7 )
    {
        section_len +=   get_element_from_str ( & dns1[section_len],tmp_dns,'.' );
        printf ( "dns1: tmp_ip_info %s \n", tmp_dns );

        section_len +=   get_element_from_str ( &dns1[section_len],tmp_dns,'.' );
        printf ( "dns1: tmp_ip_info %s \n", tmp_dns );

        section_len +=   get_element_from_str ( & dns1[section_len],tmp_dns,'.' );
        printf ( "dns1: tmp_ip_info %s \n", tmp_dns );

        section_len +=   get_element_from_str ( & dns1[section_len],tmp_dns,'.' );
        printf ( "dns1:tmp_ip_info %s \n", tmp_dns );

    }

    section_len = 0;
    if ( strlen ( dns2 ) >= 7 )
    {
        section_len +=   get_element_from_str ( &dns2[section_len],tmp_dns,'.' );
        printf ( "dns2: tmp_ip_info %s \n", dns2,tmp_dns );

        section_len +=   get_element_from_str ( & dns2[section_len],tmp_dns,'.' );
        printf ( "dns2: tmp_ip_info %s \n", dns2,tmp_dns );

        section_len +=   get_element_from_str ( & dns2[section_len],tmp_dns,'.' );
        printf ( "dns2: tmp_ip_info %s \n", dns2,tmp_dns );


        section_len +=   get_element_from_str ( & dns2[section_len],tmp_dns,'.' );
        printf ( "dns2: tmp_ip_info %s \n", dns2,tmp_dns );
    }
    /*
            len +=   get_element_from_str ( & dns1[len],tmp_dns,'.' );
            printf ( "IP: %s: tmp_ip_info %s \n", dns1,tmp_dns );*/

}




int32_t  get_broad_ip_info ( const char* comm_name )
{
    int32_t ret  = 0;
    struct sockaddr_in *myaddr;
    int32_t  socket_fd = socket ( AF_INET, SOCK_DGRAM, 0 );
    if ( socket_fd < 0 )
    {
        fprintf ( stderr, "Create socket failed!errno=%d", errno );
        return;
    }
    struct ifreq ifr ;

    unsigned long nBroadIP = 0;

    strcpy ( ifr.ifr_name, comm_name );

    if ( ioctl ( socket_fd, SIOCGIFBRDADDR, &ifr ) < 0 )
    {
        ret = -1;
    }
    else
    {
        myaddr = ( struct sockaddr_in* ) & ( ifr.ifr_broadaddr );
        printf ( "\tBroadIP: %s\n",  inet_ntoa ( myaddr->sin_addr ) );

    }

    close ( socket_fd );

    return ret ;
}



int32_t get_net_mac_info ( const char* comm_name )
{
    int32_t ret = 0;
    int32_t  socket_fd = socket ( AF_INET, SOCK_DGRAM, 0 );
    if ( socket_fd < 0 )
    {
        fprintf ( stderr, "Create socket failed!errno=%d", errno );
        return;
    }
    struct ifreq ifr;
    unsigned char mac[7] = {0};

    strcpy ( ifr.ifr_name, comm_name );
    if ( ioctl ( socket_fd, SIOCGIFHWADDR, &ifr ) < 0 )
    {
        ret = -1;
    }
    else
    {
        memcpy ( mac, ifr.ifr_hwaddr.sa_data, sizeof ( mac )-1 );
        printf ( "\tMAC: %02x-%02x-%02x-%02x-%02x-%02x\n",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
    }

    close ( socket_fd );
    return ret;
}

#endif

void func()
{
   float data = 0.0;

    unsigned int *rdata = ( unsigned int * ) &data;
    
    unsigned int tmp = 0;
    
    unsigned char c_tmp1=0x00;
    unsigned char  c_tmp2= 0x0;
     unsigned char c_tmp3 = 0xad;
      unsigned char c_tmp4 = 0x42;
    
    rdata = (c_tmp1 <<24 | c_tmp2<<16 | c_tmp3<<8 | c_tmp4 );
    
    printf("%f \n",data);
}


int main ( int argc, char **argv )
{
    int tmp32 = -1;
    
    printf("tmp32 : 0x%0x  \n",     tmp32);

    printf("0x%0x  0x%0x  0x%0x  0x%0x  \n",      ( unsigned char  ) ( ( tmp32>>8 ) &0xFF ), ( unsigned char ) ( tmp32&0xFF ), ( unsigned char ) ( ( tmp32>>24 ) &0xFF ), ( unsigned char ) ( ( tmp32>>16 ) &0xFF ) );


    return 0;
}



