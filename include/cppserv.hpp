/*
 * Modified on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */
#pragma once

enum class ER
{
  UNDEFINED,
  SUCCESS,
  SOCKET_ERROR,
  POLLER_ERROR,
  CONNECTION_ERROR,
  ACCEPTOR_ERROR,
  UNIMPLEMENTED,
  FILE_ERROR
};