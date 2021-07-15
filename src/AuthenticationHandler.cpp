#include "AuthenticationHandler.h"

AuthenticationHandler::AuthenticationHandler(const String &username,
											 const String &password, int TIMEOUT_MS)
	: username(username), password(password), timeout(TIMEOUT_MS) {}

AuthenticationHandler::AuthenticationHandler(const char *username,
											 const char *password, int TIMEOUT_MS)
	: username(username), password(password), timeout(TIMEOUT_MS) {}

bool AuthenticationHandler::authenticate(Stream &client)
{
	ActionMap authentication = ActionMap::fromStream(client, timeout);

	if (authentication.has("username") &&
		authentication.has("password") &&
		(*authentication.get("username")) == this->username && (*authentication.get("password")) == this->password)
	{
		Response::successResponse().write(client);

		return true;
	}

	Response::errorResponse().write(client);
	return false;
}