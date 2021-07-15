#ifndef AUTHENTICATION_HANDLER_H
#define AUTHENTICATION_HANDLER_H

#include <memory>
#include <Arduino.h>
#include "SerialMap.h"
#include "Response.h"
#include "Common.h"

class AuthenticationHandler
{
public:
  AuthenticationHandler() = delete;
  AuthenticationHandler(const String &username,
                        const String &password, int TIMEOUT_MS);
  AuthenticationHandler(const char *username, const char *password, int TIMEOUT_MS);
  bool authenticate(Stream &client);

private:
  String username, password;
  int timeout;
  char buffer[1024];
};

#endif
