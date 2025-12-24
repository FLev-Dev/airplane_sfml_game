#include "database_api.hpp"

Database::Database(const std::string& db_name) 
	: Database(db_name, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE) {}

Database::Database(const std::string& db_name, int32_t flags)
{
	logger = create_or_get_logger("Database");

	const auto result = sqlite3_open_v2(db_name.c_str(), &db_, flags, nullptr);
	if (result != SQLITE_OK)
	{
		LOG_ERROR(
			logger,
			"Failed to open database {}. Error: [{}] {}",
			db_name,
			result,
			sqlite3_errmsg(db_)
		);
		db_ = nullptr;
	}
	else
	{
		LOG_INFO(logger, "Database {} open successfully.", db_name);
	}
}//!Database
//---------------------------------------------------------------------------------------

Database::~Database()
{
	if (!db_) return;
	
	const auto result = sqlite3_close(db_);
	if (result != SQLITE_OK)
	{
		LOG_ERROR(
			logger,
			"Failed to close database. Error: [{}] {}",
			result,
			sqlite3_errmsg(db_)
		);
	}
	else
	{
		LOG_INFO(logger, "Database closed successfully.");
	}
	db_ = nullptr;
}//!~Database
//---------------------------------------------------------------------------------------

FLEV_NODISCARD bool Database::execute_query(const std::string& query)
{
	if (!db_)
	{
		LOG_ERROR(logger, "Database handle is null. Cannot execute query.");
		return false;
	}
	if (query.empty())
	{
		LOG_ERROR(logger, "Cannot execute empty query.");
		throw std::invalid_argument("Query string is empty.");
	}

	std::lock_guard lock(M_exec_);

	rows_.clear();
	cols_.clear();

	char* err_msg = nullptr;
	const auto rc = sqlite3_exec(db_, query.c_str(), query_callback, this, &err_msg);

	if (rc != SQLITE_OK)
	{
		LOG_ERROR(
			logger,
			"SQL error while executing query: '{}'. Error: [{}] {}",
			query,
			rc,
			err_msg ? err_msg : "Unknown"
		);
		sqlite3_free(err_msg);
		return false;
	}

	return true;
}//!execute_query
//---------------------------------------------------------------------------------------

FLEV_NODISCARD bool Database::execute_prepared(
	const std::string& sql_with_braces,
	const std::vector<std::variant<int32_t, std::string>>& params
)
{
	if (!db_) return false;
	if (sql_with_braces.empty())
	{
		LOG_ERROR(logger, "Cannot execute empty query.");
		return false;
	}
	if(params.empty())
	{
		LOG_WARNING(logger, "No parameters provided for prepared statement. Fallback to execute_querry");
		return execute_query(sql_with_braces);
	}

	// Replace {} with ? for SQLite
	std::string sql = sql_with_braces;
	size_t pos = 0;
	int32_t brace_count = 0;
	while ((pos = sql.find("{}", pos)) != std::string::npos)
	{
		sql.replace(pos, 2, "?");
		pos += 1; // Skip '?'
		brace_count++;
	}

	if (brace_count != static_cast<int32_t>(params.size()))
	{
		LOG_ERROR(
			logger, 
			"Parameter count mismatch: {} placeholders, {} params",
			brace_count, 
			params.size()
		);
		return false;
	}

	sqlite3_stmt* stmt = nullptr;
	const char* tail = nullptr;

	// 1. Prepare statement
	auto rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, &tail);
	if (rc != SQLITE_OK)
	{
		LOG_ERROR(
			logger,
			"Failed to prepare statement: {}. Error: [{}] {}",
			sql,
			rc,
			sqlite3_errmsg(db_)
		);
		return false;
	}

	// 2. Bind parameters
	for (size_t i = 0; i < params.size(); i++)
	{
		auto idx = static_cast<int32_t>(i + 1); // SQLite: 1-based!

		if (std::holds_alternative<int32_t>(params[i]))
		{
			auto value = std::get<int32_t>(params[i]);
			rc = sqlite3_bind_int(stmt, idx, value);
		}
		else if (std::holds_alternative<std::string>(params[i]))
		{
			const std::string& value = std::get<std::string>(params[i]);
			// SQLITE_TRANSIENT — SQLite makes its own private copy of the data
			rc = sqlite3_bind_text(stmt, idx, value.c_str(), -1, SQLITE_TRANSIENT);
		}
		else
		{
			rc = SQLITE_MISUSE;
		}

		if (rc != SQLITE_OK)
		{
			LOG_ERROR(
				logger,
				"Failed to bind parameter at index {}. Error: [{}] {}",
				idx,
				rc,
				sqlite3_errmsg(db_)
			);
			sqlite3_finalize(stmt);
			return false;
		}
	}

	// 3. Execute statement
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		LOG_ERROR(
			logger,
			"Failed to execute prepared statement: {}. Error: [{}] {}",
			sql,
			rc,
			sqlite3_errmsg(db_)
		);
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_finalize(stmt);
	return true;
}//!execute_prepared
//---------------------------------------------------------------------------------------

FLEV_NODISCARD Database::Cols_array Database::get_cols()
{
	std::lock_guard lock(M_exec_);
	return cols_;
}//!get_cols
//---------------------------------------------------------------------------------------

FLEV_NODISCARD Database::Rows_array Database::get_rows()
{
	std::lock_guard lock(M_exec_);
	return rows_;
}//!get_rows
//---------------------------------------------------------------------------------------

int32_t Database::query_callback(void* self_ptr, int32_t argc, char** argv, char** col_names)
{
	auto self = static_cast<Database*>(self_ptr);
	if (!self) return 1;

	// Get columns only at first call per query
	if (self->rows_.empty())
	{
		self->cols_.clear();
		for (int32_t i = 0; i < argc; ++i)
		{
			self->cols_.emplace_back(col_names[i]);
		}
	}

	Row_array row;
	for (int32_t i = 0; i < argc; ++i)
	{
		row.push_back(argv[i] ? argv[i] : "NULL");
	}
	self->rows_.push_back(std::move(row));

	return 0;
}//!query_callback
//---------------------------------------------------------------------------------------