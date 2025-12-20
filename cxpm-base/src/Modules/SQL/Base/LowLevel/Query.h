#pragma once

#include <cstddef>
#include <cstring>
#include <format>

/** this is a C++ but C level abstraction just to take note from the form */

typedef struct database_handle_struct {
  const char *priv;
} database_handle;

typedef struct query_handle_struct {
  const char *priv;
} query_handle;

typedef struct element_handle_struct {
  const char *priv;
} element_handle;

inline int database_init(database_handle *handle) { return 0; }

template <typename... ArgumentsTypes>
int database_query(database_handle *handle, query_handle *result,
                   const String::format_string<ArgumentsTypes...>,
                   ArgumentsTypes &&...args) {
  return 0;
}

inline int query_init(query_handle *handle) { return 0; }

inline int query_row(query_handle *handle, size_t index, element_handle *row) {
  return 0;
}
inline int query_column(query_handle *handle, element_handle *row, size_t index,
                        void *value, int size) {
  return 0;
}
inline int query_column_string(query_handle *handle, element_handle *row,
                               size_t index, char *value, int size) {
  return 0;
}
inline int query_column_int(query_handle *handle, element_handle *row,
                            size_t index, int *value) {
  return 0;
}

inline int query_column_double(query_handle *handle, element_handle *row,
                               size_t index, double *value) {
  return 0;
}

inline int query_column_blob(query_handle *handle, element_handle *row,
                             size_t index, unsigned char *value) {
  return 0;
}

inline int query_use_case() {

  database_handle database;

  database_init(&database);

  query_handle query_handle;
  // ... do the query

  const char table_name[] = "users";
  int id = 0;

  query_init(&query_handle);

  // insert into example with variadics. this low level api just rocks, I should
  // design a db with this api some day! haha
  // so simple that every programmer should cry (kidding) =P

  database_query(&database, &query_handle, "INSERT INTO %s ...");

  database_query(&database, &query_handle, "SELECT * FROM %s WHERE id = `%d`",
                 table_name, id);

  element_handle element_handle;
  query_row(&query_handle, 0, &element_handle);

  char name[2048];
  ::memset(name, 0, sizeof(name));

  int age;

  // this is the point
  // how to wrap this construct in C++?
  // I will expose this API
  if (query_column_string(&query_handle, &element_handle, 0, name,
                          sizeof(name)) != 0) {
    return -1;
  }

  if (query_column_int(&query_handle, &element_handle, 1, &age) != 0) {
    return -1;
  }

  // given that, how can I wrap this pattern into C++ using an iterator to
  // iterate over the query handle and extract the correct fields for a
  // structure?

  return 0;
}