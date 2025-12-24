#pragma once
#include "defines.hpp"

#include <quill/Backend.h>
#include <quill/Logger.h>
#include <quill/LogMacros.h>
#include <quill/DeferredFormatCodec.h>

// Alias for convenience
using Logger_ptr = quill::Logger*;

/** 
 * @brief Create or get a logger with the specified name.
 * 
 * @param logger_name[in] The name of the logger to create or get.
 * 
 * @return Logger_ptr - A pointer to the created or retrieved logger.
 * 
 * @note Path for log files is set to "./Logs/{logger_name}.log".
 * @see quill::Frontend::create_or_get_logger
 */
FLEV_NODISCARD Logger_ptr create_or_get_logger(const std::string& logger_name);

/** @brief Get the global logger instance. */
FLEV_NODISCARD Logger_ptr get_global_logger();

//  +-----------------------------------+
//  |   Utility functions for logging   |
//  +-----------------------------------+

/** @brief Convert a std::u8string to std::string_view for logging purposes. */
FLEV_NODISCARD std::string_view log_u8string(const std::u8string u8str);

//  +---------------------------------------------------+
//  |   Specializations for fmtquill and quill codecs   |
//  +---------------------------------------------------+

// Specialization for std::thread::id to format it as a size_t hash value
template <>
struct fmtquill::formatter<std::thread::id>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    auto format(::std::thread::id const& tid, format_context& ctx) const
    {
        return fmtquill::format_to(ctx.out(), "{}", std::hash<std::thread::id>{}(tid));
    }
};
template <>
struct quill::Codec<std::thread::id> : quill::DeferredFormatCodec<std::thread::id> {};
