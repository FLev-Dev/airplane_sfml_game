#pragma once

/** GENERAL DEFINES BLOCK */

#ifndef FLEV_NODISCARD
#  define FLEV_NODISCARD [[nodiscard]]
#  define FLEV_NODISCARD_MSG(message) [[nodiscard(message)]]
#endif // !FLEV_NODISCARD

#ifndef FLEV_DO_NOTHING
#  define FLEV_DO_NOTHING ((void)0)
#endif // !DO_NOTHING

#ifndef FLEV_AS_STR
#  define FLEV_AS_STR(x) #x
#  define FLEV_STRINGIFY(x) FLEV_AS_STR(x)
#endif // !FLEV_AS_STR

#ifndef FLEV_FALSE_ASSERT
#  define FLEV_FALSE_ASSERT(t) (sizeof(t) == 0)
#endif // !FLEV_FALSE_ASSERT

/** FILE INFO BLOCK */

#ifndef FLEV_FUNCTION_NAME
#  if defined(_MSC_VER)
#    define FLEV_FUNCTION_NAME __FUNCSIG__
#  elif defined(__clang__) || defined(__GNUC__) || defined(__INTEL_COMPILER)
#    define FLEV_FUNCTION_NAME __PRETTY_FUNCTION__
#  else
#    define FLEV_FUNCTION_NAME __FUNCTION__
#  endif
#endif // !FLEV_FUNCTION_NAME

#ifndef FLEV_FILE_NAME
#  define FLEV_FILE_NAME __FILE__
#endif // !FLEV_FILE_NAME

#ifndef FLEV_LINE_NO
#  define FLEV_LINE_NO __LINE__
#endif // !FLEV_LINE_NO

#ifndef FLEV_FILE_INFO
#  define FLEV_FILE_INFO FLEV_FILE_NAME ":" FLEV_STRINGIFY(FLEV_LINE_NO)
#endif // !FLEV_FILE_INFO


/** NAMESPACES BLOCK */

#ifndef FLEV_NAMESPACE_BEGIN
#  define FLEV_NAMESPACE_BEGIN namespace flev {
#  define FLEV_NAMESPACE_END }
#endif // !FLEV_NAMESPACE_BEGIN

#ifndef FLEV_DEBUG_NAMESPACE_BEGIN
#  define FLEV_DEBUG_NAMESPACE_BEGIN                                \
     FLEV_NAMESPACE_BEGIN                                           \
     namespace debug {
#  define FLEV_DEBUG_NAMESPACE_END                                  \
    } FLEV_NAMESPACE_END                                              
#endif // !FLEV_DEBUG_NAMESPACE_BEGIN


/** DEBUG ONLY BLOCK */
#ifndef NDEBUG

#ifndef TODO
#  ifdef _MSC_VER
#    define TODO(msg)                                                       \
          __pragma(message(FLEV_FILE_NAME "(" FLEV_AS_STR(FLEV_LINE_NO)     \
              ") : warning: TODO " msg))
#    define TODO_LOG TODO("Need to push log here")
#  else
#    define TODO(msg) _Pragma("GCC TODO warning \"" msg "\"")
#  endif
#endif // !TODO




#endif //! NDEBUG 


#ifndef TODO
#  define TODO(msg)
#endif