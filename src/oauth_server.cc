// Copyright 2019 Neverware
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "oauth_server.h"

#include <QString>

#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "rand_util.h"
#include "util.h"

#if defined(_MSC_VER) && _MSC_VER + 0 <= 1800
/* Substitution is OK while return value is not used */
#define snprintf _snprintf
#endif

#define GET 0

const int MIN_PORT = 4000;
const int MAX_PORT = 5000;

// as discussed in OVER-8763
const char* gondarpage =
    "<html><head><title>Signed in to CloudReady USB "
    "Maker</title></head><body>You are now signed in with Google.<br>Close "
    "this window and "
    "return to the CloudReady USB Maker to continue.</body></html>";

const char* errorpage =
    "<html><head><title>Error signing into CloudReady USB Maker with "
    "Google</title></head><body>There was an error signing you in.<br>Please "
    "close this "
    "window and try again, or contact <a "
    "href='https://neverware.com/support'>Neverware Support</a></body></html>";

struct connection_info_struct {
  int connectiontype;
};

static int send_page(struct MHD_Connection* connection, const char* page) {
  int ret;
  struct MHD_Response* response;

  response = MHD_create_response_from_buffer(strlen(page), (void*)page,
                                             MHD_RESPMEM_PERSISTENT);
  if (!response)
    return MHD_NO;

  ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);

  return ret;
}

static void request_completed(void* cls,
                              struct MHD_Connection* connection,
                              void** con_cls,
                              enum MHD_RequestTerminationCode toe) {
  struct connection_info_struct* con_info = (connection_info_struct*)*con_cls;
  (void)cls;        /* Unused. Silence compiler warning. */
  (void)connection; /* Unused. Silence compiler warning. */
  (void)toe;        /* Unused. Silence compiler warning. */

  // so far all con_info is used for is nothing, it just gets cleaned up here
  if (nullptr == con_info)
    return;

  free(con_info);
  *con_cls = nullptr;
}

static int answer_to_connection(void* cls,
                                struct MHD_Connection* connection,
                                const char* url,
                                const char* method,
                                const char* version,
                                const char* upload_data,
                                size_t* upload_data_size,
                                void** con_cls) {
  (void)version;          /* Unused. Silent compiler warning. */
  (void)upload_data;      /* Unused. Silent compiler warning. */
  (void)upload_data_size; /* Unused. Silent compiler warning. */
  OauthServer* server_ptr = (OauthServer*)cls;
  // if this is the first contact with the server, create connection
  // info for it and return
  if (nullptr == *con_cls) {
    struct connection_info_struct* con_info;

    con_info = new struct connection_info_struct;
    if (nullptr == con_info)
      return MHD_NO;

    con_info->connectiontype = GET;

    *con_cls = (void*)con_info;

    return MHD_YES;
  }
  // conversely, if it's not a first request, we look at it if it's a GET
  if (0 == strcmp(method, "GET")) {
    auto urlstr = std::string(url);
    // gate on '/' to make sure we're not looking up the favicon or similar
    if (urlstr == "/") {
      QString state = MHD_lookup_connection_value(
          connection, MHD_GET_ARGUMENT_KIND, "state");
      QString code = MHD_lookup_connection_value(connection,
                                                 MHD_GET_ARGUMENT_KIND, "code");
      QString error = MHD_lookup_connection_value(
          connection, MHD_GET_ARGUMENT_KIND, "error");
      // state should equal the state we sent
      // code should be populated
      if (!state.isEmpty() && !code.isEmpty()) {
        emit server_ptr->callbackReceived(state, code);
        return send_page(connection, gondarpage);
      } else if (!error.isEmpty()) {
        // then it's a known error during auth process
        emit server_ptr->authError(error);
        return send_page(connection, errorpage);
      } else {
        LOG_WARNING << "HTTPSERVER UNKNOWN CASE: this should never be reached";
        // TODO(kendall): make separate pages for each error
        return send_page(connection, errorpage);
      }
    } else {
      // only root should be hit
      return send_page(connection, errorpage);
    }
  } else {
    // a non-get request
    // TODO(kendall): make separate pages for each error
    return send_page(connection, errorpage);
  }
}

OauthServer::OauthServer(QObject* parent) : QObject(parent) {
  port = gondar::getRandomNum(MIN_PORT, MAX_PORT);
}

void OauthServer::start() {
  if (daemon == nullptr) {
    auto client_filter_func = nullptr;  // null means all clients allowed
    auto filter_args = nullptr;         // args to above
    // auto did not work for this var; had to explicitly make it a voidptr
    void* request_completed_args = nullptr;
    daemon = MHD_start_daemon(
        MHD_USE_SELECT_INTERNALLY, port, client_filter_func, filter_args,
        &answer_to_connection, this, MHD_OPTION_NOTIFY_COMPLETED,
        request_completed, request_completed_args, MHD_OPTION_END);
  }
}

void OauthServer::stop() {
  if (daemon != nullptr) {
    MHD_stop_daemon(daemon);
  }
}

int OauthServer::getPort() {
  return port;
}
