#include "request_handler.h"

#include <functional>
#include <iostream>

#include "sqlite3.h"

const std::string kQueryCreateA =
    "CREATE TABLE A (id INTEGER PRIMARY KEY, name VARCHAR(16) NOT NULL);";
const std::string kQueryCreateB =
    "CREATE TABLE B (id INTEGER PRIMARY KEY, name VARCHAR(16) NOT NULL);";
const std::string kInsert = "INSERT";
const std::string kTrunc = "TRUNCATE";
const std::string kIntrSec = "INTERSECTION";
const std::string kSymDif = "SYMMETRIC_DIFFERENCE";

int WriteAnswer(void *answer, int columns, char **data, char **names) {
  std::string *res = static_cast<std::string *>(answer);
  for (int i = 0; i < columns; ++i) {
    if (i != 0) {
      res->append(",");
    }
    res->append((data[i] ? data[i] : ""));
  }
  res->append("\n");
  return 0;
};

RequestHandler::RequestHandler()
    : _handle(nullptr) {
  const char *db_name = "sqlite_db.sqlite";
  // const char *db_name = ":memory:"; // <--- без файла, БД размещается в
  // памяти

  if (sqlite3_open(db_name, &_handle)) {
    std::cerr << "Can't open database: " << sqlite3_errmsg(_handle)
              << std::endl;
    sqlite3_close(_handle);
  }
  std::cerr << db_name << " database opened successfully!" << std::endl;
  CreateTaible();
}

RequestHandler::~RequestHandler() {
  sqlite3_close(_handle);
}

std::string RequestHandler::exec_query(std::string query) {
  //  std::cout << " get " << query << std::endl;
  char *errmsg;
  std::optional<std::string> true_qwery = ParseQuery(std::move(query));
  if (!true_qwery) {
    return "Bad query\n";
  }
  std::string answer;
  int rc = sqlite3_exec(_handle, true_qwery.value().data(), WriteAnswer,
                        &answer, &errmsg);
  if (rc != SQLITE_OK) {
    return ErrorHandler(errmsg);
  }
  answer.append("OK\n");
  //  std::cout << " send " << answer << std::endl;
  return answer;
}

void RequestHandler::CreateTaible() {
  char *errmsg;
  std::array<std::string_view, 2> arr = {kQueryCreateA, kQueryCreateB};
  for (auto str_v : arr) {
    int rc = sqlite3_exec(_handle, str_v.data(), 0, 0, &errmsg);
    if (rc != SQLITE_OK) {
      ErrorHandler(errmsg);
    }
  }
}

std::optional<std::string> RequestHandler::ParseQuery(std::string query) {
  std::string table;
  std::string true_qwery;
  if (query.substr(0, kInsert.size()) == kInsert) {
    std::string id;
    std::string name;
    int pos_beg = kInsert.size() + 1;  // пробел
    int pos_end = query.find_first_of(' ', pos_beg);
    table = query.substr(pos_beg, pos_end - pos_beg);

    pos_beg = pos_end + 1;  // пробел
    pos_end = query.find_first_of(' ', pos_beg);
    id = query.substr(pos_beg, pos_end - pos_beg);

    pos_beg = pos_end + 1;  // пробел
    pos_end = query.find_first_of(' ', pos_beg);
    name = query.substr(pos_beg, pos_end - pos_beg);
    true_qwery =
        kInsert + " INTO " + table + " VALUES(" + id + ", '" + name + "');";
  } else if (query.substr(0, kTrunc.size()) == kTrunc) {
    int pos_beg = query.find_first_of(' ', 0) + 1;  // пробел
    int pos_end = query.find_first_of(' ', pos_beg);
    table = query.substr(pos_beg, pos_end - pos_beg);
    true_qwery = "DELETE FROM " + table + ";";
  } else if (query.substr(0, kIntrSec.size()) == kIntrSec) {
    true_qwery = "SELECT A.id, A.name, B.name FROM A, B WHERE A.id = B.id;";
  } else if (query.substr(0, kSymDif.size()) == kSymDif) {
    true_qwery =
        "SELECT * FROM A WHERE id NOT IN (SELECT id FROM B) UNION "
        "SELECT * FROM B WHERE id NOT IN (SELECT id FROM A);";
  } else {
    return std::nullopt;
  }
  return true_qwery;
}

std::string RequestHandler::ErrorHandler(char *errmsg) {
  std::string result;
  result = "ERR ";
  result.append(errmsg);
  result.append("\n");
  std::cerr << result << std::endl;
  sqlite3_free(errmsg);  // <--- обратите внимание не C-style работу
  return result;
}

//  while (true) {
//    char *errmsg;
//    std::string query;
//    std::getline(std::cin, query);
//    int rc = sqlite3_exec(_handle, query.data(), print_results, 0, &errmsg);
//    if (rc != SQLITE_OK) {
//      std::cout << "error " << errmsg << std::endl;
//    }
//  }
