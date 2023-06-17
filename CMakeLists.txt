add_library(cppevent-base STATIC
  src/cppevent_base/event_loop.cpp
)

target_include_directories(cppevent-base PUBLIC src)

add_subdirectory(examples/timer)
