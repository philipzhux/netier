/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#pragma once
#include "server.h"
#include "context.h"
#include "reactor.h"
#include "threadpool.h"
#include <memory>
#include <functional>

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