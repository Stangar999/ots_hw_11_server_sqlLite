#pragma once
#include <optional>
#include <string>
class sqlite3;

class RequestHandler {
public:
  RequestHandler();
  ~RequestHandler();

  std::string exec_query(std::string query);

private:
  static int PrintResults(void *result, int columns, char **data, char **names);
  void CreateTaible();
  std::optional<std::string> ParseQuery(std::string query);
  std::string ErrorHandler(char *errmsg);
  sqlite3 *_handle;
};
