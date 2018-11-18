#ifndef _SOCKET_COMM_H
#define _SOCKET_COMM_H


#include "io.h"

#define MAXLINE 1024

int socket_hand(void);
int32_t socket_rev_process(uint8_t *inbuf, uint16_t inbuf_len);
int socket_send_comm_state(uint16_t dev_code, uint16_t index, uint16_t comm_state);
int socket_send_signal(uint16_t com, uint16_t comm_signal);
void   socket_communicate_process ( const  int32_t *p_conn_socket_fd );


  extern   int sever_sock_communicate_fd ;
#endif
