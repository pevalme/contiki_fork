  /**
   * \file
   *         A network application that listens on UDP port 50000 and echoes.
   * \author
   *         Clément Burin des Roziers <clement.burin-des-roziers@inrialpes.fr>
   */

 #include "contiki.h"
 #include "contiki-net.h"
 //#include <syslog.h>
 #include <signal.h>

 #include "dev/leds.h"

 #include <stdio.h>
 #include <string.h>


 #define UDP_DATA_LEN 120
 #define UDP_HDR ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])

 char input_file_name[1024];

 static struct uip_udp_conn *udpconn;
 static uint8_t udpdata[UDP_DATA_LEN] = "rx=";


 static void udphandler(process_event_t ev, process_data_t data)
 {
     if (uip_newdata())
     {
         /* Set the last byte of the received data as 0 in order to print it. */
         int len = uip_datalen();
         printf("uip_datalen() returns %d\n",len);
         ((char *)uip_appdata)[len] = 0;
         printf("Received from %u.%u.%u.%u:%u: '%s'\n", uip_ipaddr_to_quad(&UDP_HDR->srcipaddr), htons(UDP_HDR->srcport), (char*)uip_appdata);
         raise(SIGINT);
//         openlog("SERVER-TESTED-BY-AFL", LOG_PID|LOG_CONS, LOG_USER);
//         syslog(LOG_INFO, "A different kind of Hello world ... %s\n",(char*)uip_appdata);
//         closelog();
//         /* Prepare the response datagram in a local buffer */
//         memcpy(udpdata, "rx=", 3);
//         memcpy(udpdata+3, uip_appdata, len);
//
//         /* Copy the information about the sender to the udpconn in order to reply */
//         uip_ipaddr_copy(&udpconn->ripaddr , &UDP_HDR->srcipaddr); // ip address
//         udpconn->rport = UDP_HDR->srcport; // UDP port
//
//         /* Send the reply datagram */
//         printf("sending back\n");
//         uip_udp_packet_send(udpconn, udpdata, uip_datalen()+3);
//
//         /* Restore the udpconn to previous setting in order to receive other packets */
//         uip_ipaddr_copy(&udpconn->ripaddr , &uip_all_zeroes_addr);
//         udpconn->rport = 0;
     }
 }

 /*---------------------------------------------------------------------------*/
 /*
  * We declare the process.
  */
 PROCESS(example_udp_server_process, "Example UDP server");
 AUTOSTART_PROCESSES(&example_udp_server_process);


 /*---------------------------------------------------------------------------*/

 /*
  * The definition of the process.
  */
 PROCESS_THREAD(example_udp_server_process, ev, data)
 {

     uip_ipaddr_t ipaddr;

     PROCESS_BEGIN();
     printf("UDP Echo Server test\n");

     /* Create a UDP 'connection' with IP 0.0.0.0 and port 0 as remote host.
      * This means the stack will accepts UDP datagrams from any node. */
     udpconn = udp_new(NULL, htons(0), NULL);

     /* Bind the UDP 'connection' to the port 50000. That's the port we're listening on. */
     udp_bind(udpconn, htons(50000));

     printf("listening on UDP port %u\n", htons(udpconn->lport));

     while(1) {
         /* Wait until we have an event caused by tcpip interaction */
         PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
         /* Handle it */
         udphandler(ev, data);
     }

     PROCESS_END();
 }
 /*---------------------------------------------------------------------------*/

