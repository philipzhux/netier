#pragma once
#include "server.h"
#include "context.h"
#include "reactor.h"
#include "threadpool.h"

enum class ER {
  UNDEFINED,
  SUCCESS,
  SOCKET_ERROR,
  POLLER_ERROR,
  CONNECTION_ERROR,
  ACCEPTOR_ERROR,
  UNIMPLEMENTED,
  FILE_ERROR
};