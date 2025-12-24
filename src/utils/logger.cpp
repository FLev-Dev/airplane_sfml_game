#include "logger.hpp"

#include <filesystem>
#include <cassert>

#include <quill/Frontend.h>
#include <quill/sinks/ConsoleSink.h>
#include <quill/sinks/RotatingFileSink.h>

#include <SFML/System/String.hpp>

FLEV_NODISCARD Logger_ptr create_or_get_logger(const std::string& logger_name)
{
    if (auto logger = quill::Frontend::get_logger(logger_name))
    {
        assert(logger->is_valid_logger());
        return logger;
    }

    auto logger_path = std::filesystem::current_path() / "Logs" / logger_name;
    logger_path = logger_path.lexically_normal();
    logger_path.replace_extension(".log");

    auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1");
    auto rotating_file_sink = quill::Frontend::create_or_get_sink<quill::RotatingFileSink>(
        logger_path.string(),
        []() {
            quill::RotatingFileSinkConfig cfg;
            cfg.set_open_mode('a');
            cfg.set_max_backup_files(5);
            cfg.set_rotation_max_file_size(1024ull * 10 * 10);
            return cfg;
        }()
            );

    auto logger = quill::Frontend::create_or_get_logger(
        logger_name,
        { std::move(console_sink), std::move(rotating_file_sink) },
        []() {
            quill::PatternFormatterOptions format_options;
            format_options.format_pattern =
                "%(time) [%(thread_id)] %(short_source_location:<20) "
                "%(log_level:<10) %(message)";
            format_options.timestamp_pattern = ("[%d.%m] %H:%M:%S.%Qns");
            format_options.timestamp_timezone = quill::Timezone::LocalTime;
            format_options.add_metadata_to_multi_line_logs = false;
            return format_options;
        }()
            );
    assert(logger);
    assert(logger->is_valid_logger());
#ifndef NDEBUG
    // In debug mode, force immediate flushing of log messages
    // This blocks the caller thread until log entry is written to disk
    // Simulates synchronous logging behavior for easier debugging
    // Note: This can impact performance and should only be used during development
    logger->set_immediate_flush(1u);
#endif
    logger->set_log_level(quill::LogLevel::TraceL3);

    if (!quill::Backend::is_running())
    {
        quill::BackendOptions options{
            // Disable check symbols
            .check_printable_char = [](char c) { return true; }
        };
        quill::Backend::start(options);
        LOG_DEBUG(logger, "Logger backend thread id is {}", quill::Backend::get_thread_id());
    }
    return logger;
}//!create_or_get_logger
//---------------------------------------------------------------------------------------

FLEV_NODISCARD Logger_ptr get_global_logger()
{
    static Logger_ptr global_logger = create_or_get_logger("global");
    return global_logger;
}//!get_global_logger
//---------------------------------------------------------------------------------------

FLEV_NODISCARD std::string_view log_u8string(const std::u8string u8str)
{
    return std::string_view{reinterpret_cast<const char*>(u8str.data()), u8str.size()};
}//!log_u8string
//---------------------------------------------------------------------------------------