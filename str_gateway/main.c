
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

/*******************************************************************************
 * Function       : get_element_from_str
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : 使用逗号分隔符解析字符串,
 * Calls              :
 * Input             : char *buf,字符串,char separator 分隔符
 * Output          :char *element 返回字段
 * Return           :字符串位置
 ********************************************************************************/
static uint16_t get_element_from_str ( char *buf,char *element,char separator )
{
    uint32_t i = 0;
    for ( i = 0; ( buf[i]!= separator ) && ( buf[i]!= '\0' ) && ( i< 127 ); ++i )
    {
        element[i] = buf[i];
    }
    element[i] = '\0';


    if ( element[i-1] == '\r' )
    {
        element[i-1] = '\0';
    }
    return ( i+1 );
//     return i;
}



  int32_t data_gateway_1 = 0;
  int32_t data_gateway_2 = 0; 
  int32_t data_gateway_3 = 0;
  int32_t data_gateway_4 = 0;

static void get_gateway_element ( char *tmp_element, int32_t *p_net_gate_way1,int32_t *p_net_gate_way2,int32_t *p_net_gate_way3,int32_t *p_net_gate_way4 )
{
    int32_t  section_len  = 0;
    char tmp_gateway1[30] = { 0 };
    char tmp_gateway2[30] = { 0 };
    char tmp_gateway3[30] = { 0 };
    char tmp_gateway4[30] = { 0 };
    
    int32_t gate_tmp_1 = 0;
    int32_t gate_tmp_2 = 0;
    int32_t gate_tmp_3 = 0;
    int32_t gate_tmp_4 = 0;
       
    section_len +=   get_element_from_str ( & ( tmp_element[section_len] ),tmp_gateway1,'.' );
    section_len +=   get_element_from_str ( & ( tmp_element[section_len] ),tmp_gateway2,'.' );
    section_len +=   get_element_from_str ( & ( tmp_element[section_len] ),tmp_gateway3,'.' );
    section_len +=   get_element_from_str ( & ( tmp_element[section_len] ),tmp_gateway4,'.' );

    
    gate_tmp_1 =  strtoul ( tmp_gateway1 ,NULL, 0 );
    gate_tmp_2 =  strtoul ( tmp_gateway2 ,NULL, 0 );
    gate_tmp_3 =  strtoul ( tmp_gateway3 ,NULL, 0 );
    gate_tmp_4 =  strtoul ( tmp_gateway4 ,NULL, 0 );
    
  pthread_mutex_lock ( &mutex_gateway );
    *p_net_gate_way1 =  gate_tmp_1;
    *p_net_gate_way2  = gate_tmp_2;
    *p_net_gate_way3 = gate_tmp_3;
    *p_net_gate_way4 =  gate_tmp_4;
  
    pthread_mutex_unlock ( &mutex_gateway );
}


  


static int reset_system_gateway ( int32_t gate_way_name)
{

 char set_del_route_str[128] = { 0 };
 char set_add_route_str[128] = { 0 };
 
sprintf ( set_del_route_str,"route del  default gw %d.%d.%d.%d eth0",    data_gateway_1 ,   data_gateway_2 ,   data_gateway_3 ,   data_gateway_4 );
sprintf ( set_add_route_str,"route add  default gw  %d.%d.%d.%d eth0",    data_gateway_1 ,   data_gateway_2 ,   data_gateway_3 ,   data_gateway_4  );
	system ( set_del_route_str);
	system ( set_add_route_str);
	
    
    return 0;
}


static int reset_lcd_set_gateway ( int32_t gate_way_name)
{

 char set_del_route_str[128] = { 0 };
 char set_add_route_str[128] = { 0 };
 
sprintf ( set_del_route_str,"route del  default gw %d.%d.%d.%d eth0",    data_gateway_1 ,   data_gateway_2 ,   data_gateway_3 ,   data_gateway_4 );


    sprintf ( set_add_route_str,"route add default gw %d.%d.%d.%d",
              gdata[indexid][GATEWAY1].i_var,gdata[indexid][GATEWAY2].i_var,gdata[indexid][GATEWAY3].i_var,gdata[indexid][GATEWAY4].i_var );
    
	system ( set_del_route_str);
	system ( set_add_route_str);
	
    
    return 0;
}

  
static int get_gateway ( void )
{

  
    char gate_way_info[40] = { 0 };

    struct in_addr ip_route;

    long  unsigned int   net_gate_way = 0;
    FILE *fp_route;
    char buf[256]; // 128 is enough for linux
    char iface[16];
    unsigned long dest_addr = 0 ;
    unsigned long  gate_addr = 0 ;

    net_gate_way = INADDR_NONE;

    fp_route = fopen ( "/proc/net/route", "r" );
    if ( fp_route == NULL )
    {
        fclose ( fp_route );
        return -1;
    }
    fgets ( buf, sizeof ( buf ), fp_route );
       printf ( "fp_route:%s  \n", fp_route ); //注意这一句的运行结果
    while ( fgets ( buf, sizeof ( buf ), fp_route ) )
    {
        if ( ( sscanf ( buf, "%s\t%lX\t%lX", iface,       &dest_addr, &gate_addr ) != 3 ) || ( dest_addr == 0 ) )
        {
            net_gate_way = gate_addr;
            break;
        }
    }

    if ( net_gate_way != INADDR_NONE )
    {
        memcpy ( &ip_route, &net_gate_way, 4 );

        strcpy ( gate_way_info, inet_ntoa ( ip_route ) );

        printf ( "ip_route:%s  \n", gate_way_info ); //注意这一句的运行结果

        get_gateway_element ( gate_way_info, &data_gateway_1,&data_gateway_2,&data_gateway_3,&data_gateway_4 );

    }

    return 0;
}
int main(int argc, char **argv) {
   
  get_gateway();
  
    return 0;
}
