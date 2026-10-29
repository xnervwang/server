/* Copyright (c) 2026, MariaDB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1335  USA */

#ifndef DISCONNECT_WATCHER_INCLUDED
#define DISCONNECT_WATCHER_INCLUDED

/*
  Notices client sockets that failed (reset etc), while the server is busy
  executing a statement and does not read from or write to the socket.
  The connection is then killed with KILL_DISCONNECT, unless it is idle.
  Only the events the OS reports on its own are used: EPOLLERR and EPOLLHUP
  on Linux, FD_CLOSE on Windows. The read side is not polled for EOF.

  A connection is watched from the end of login until THD::disconnect().
*/

#include "violite.h"

#if !defined(EMBEDDED_LIBRARY) && (defined(__linux__) || defined(_WIN32))
bool disconnect_watcher_start();
void disconnect_watcher_stop();
#ifdef __linux__
/* The fd to poll in the main loop, and what to call when it is readable */
int disconnect_watcher_fd();
void disconnect_watcher_process();
#endif
/*
  Both are called by the thread that owns the connection, with no THD locks
  held. add() returns an opaque handle, NULL if the connection is not watched.
  remove() must be called before the socket is closed, and resets the handle.
  The watcher refers to a connection only by its id.
*/
void *disconnect_watcher_add(ulonglong thread_id, Vio *vio);
void disconnect_watcher_remove(void **handle, Vio *vio);
#else
static inline bool disconnect_watcher_start() { return false; }
static inline void disconnect_watcher_stop() {}
static inline int disconnect_watcher_fd() { return -1; }
static inline void disconnect_watcher_process() {}
static inline void *disconnect_watcher_add(ulonglong, Vio *) { return NULL; }
static inline void disconnect_watcher_remove(void **, Vio *) {}
#endif

#endif
