#include <utility>

#if __cplusplus >= 202302L
# define MARK_UNREACHABLE() std::unreachable()
#elif defined(__GNUC__) || defined(__clang__)
# if defined __has_builtin
#  if __has_builtin(__builtin_unreachable)
#   define MARK_UNREACHABLE() __builtin_unreachable()
#  endif
# else
#  define MARK_UNREACHABLE()
# endif
#elif defined(_MSC_VER) && !defined(__clang__)
# define MARK_UNREACHABLE() __assume(0)
#endif