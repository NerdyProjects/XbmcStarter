/*
     XbmcStarter - a general purpose network triggered startscript
     (C) 2014 by Matthias Larisch

     This file is based on the querystring example of libmicrohttpd
     (C) 2007, 2008 Christian Grothoff (and other contributing authors)

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 2.1 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <microhttpd.h>
//#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

const char *PAGE_INFO  = "<html><body><a href=\"/jsonrpc\">Start XBMC</body></html>";
const char *PAGE_START  = "<html><head><meta http-equiv=\"refresh\" content=\"2;URL='/'\" /></head><body>Reloading <a href="">XBMC Interface</a>... !</body></html>";

sem_t startReq;


int answer_to_connection (void *cls, struct MHD_Connection *connection, 
                          const char *url, 
                          const char *method, const char *version, 
                          const char *upload_data, 
                          size_t *upload_data_size, void **con_cls)
{
  struct MHD_Response *response;

  int ret;
  int state = 0;

  const char *page = PAGE_INFO;

  if(0 == strcmp(url, "/jsonrpc")) {
    printf("received JSONRPC request\n");
    state = 1;
  }

  if(state)
    page = PAGE_START;


  response = MHD_create_response_from_buffer (strlen (page),
                                            (void*) page, MHD_RESPMEM_PERSISTENT);
  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);

  if(state)
    sem_post(&startReq);

  return ret;
}

int main (int argc, char **argv)
{
  struct MHD_Daemon *daemon;
  int port = 0;

  sem_init(&startReq, 0, 0);
  if(argc != 2) {
    fprintf(stderr, "Usage: %s <Port number>\n", argv[0]);
    return 1;
  }

  sscanf(argv[1], "%d", &port);

  if(port < 0 || port > 65535) {
    fprintf(stderr, "Invalid port number: %d, must  be between 1 and 65535\n", port);
    return 1;
  }

  while(1) {
    pid_t pid;
    daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG, port, NULL, NULL, 
        &answer_to_connection, NULL, MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int) 10, MHD_OPTION_END);
    if (NULL == daemon)
      return 1;

    printf("waiting for start request...\n");

    sem_wait(&startReq);
    sleep(1);
    MHD_stop_daemon(daemon);
    printf("starting XBMC!\n");
    system("/usr/bin/xbmc-standalone");
  }

  return 1;
}



