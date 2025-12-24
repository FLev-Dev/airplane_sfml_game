#pragma once
#include "defines.hpp"
#include "logger.hpp"
#include <sqlite3.h>
#include <variant>

class Database
{
public:
	// Type aliases
	using Cols_array = std::vector<std::string>;
	using Row_array  = std::vector<std::string>;
	using Rows_array = std::vector<Row_array>;

	/** 
	 * @brief Constructor.
	 * 
	 * @param db_name[in][opt] - Database file name. [Default: "game_database.db"]
	 */
	Database(const std::string& db_name = "game_database.db");

	/** 
	 * @brief Constructor with flags.
	 * 
	 * @param db_name[in] - Database file name.
	 * @param flags[in]   - SQLite open flags.
	 * 
	 * @see https://www.sqlite.org/c3ref/open.html for flags details.
	 */
	Database(const std::string& db_name, int32_t flags);

	/** @return true if database is open, false otherwise. */
	FLEV_NODISCARD bool is_open() const { return db_ != nullptr; }
	operator bool() const { return is_open(); }

	/** @brief Destructor. */
	~Database();

	// Non-copyable
	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;

	/** 
	 * @brief Execute SQL query.
	 * 
	 * @param query[in] - SQL query string.
	 * 
	 * @return true if query executed successfully, false otherwise.
	 */
	FLEV_NODISCARD bool execute_query(const std::string& query);

	/**
	 * @brief Execute a parameterized SQL query (e.g., INSERT, UPDATE, DELETE).
	 *
	 * Example: execute_prepared("INSERT INTO t (a, b) VALUES ({}, {})", {42, "hello"});
	 *
	 * @param sql[in]    - SQL query with {} placeholders.
	 * @param params[in] - Parameters to bind (int, std::string).
	 *
	 * @return true on success, false on error.
	 */
	FLEV_NODISCARD bool execute_prepared(
		const std::string& sql,
		const std::vector<std::variant<int32_t, std::string>>& params
	);

	/** @return Query result columns. */
	FLEV_NODISCARD Cols_array get_cols();

	/** @return Query result rows. */
	FLEV_NODISCARD Rows_array get_rows();

private/*methods*/:

	/** 
	 * @brief SQLite query callback function.
	 * 
	 * @param self_ptr[in]	- Pointer to this instance.
	 * @param argc[in]		- Number of columns.
	 * @param argv[in]		- Column values.
	 * @param col_names[in] - Column names.
	 * 
	 * @return Always returns 0.
	 * 
	 * @see https://www.sqlite.org/c3ref/exec.html
	 */
	static int32_t query_callback(void* self_ptr, int32_t argc, char** argv, char** col_names);

private/*vars*/:

	Logger_ptr logger = nullptr; ///< Logger instance.
	sqlite3* db_ = nullptr;		 ///< Database handle.

	// Query execution
	std::mutex M_exec_;			 ///< Mutex for query execution.
	Rows_array rows_;			 ///< Query result rows.
	Cols_array cols_;			 ///< Query result columns.
};