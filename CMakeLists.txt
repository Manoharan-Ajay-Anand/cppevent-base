add_library(cppevent-base STATIC
  src/cppevent_base/event_loop.cpp
  src/cppevent_base/event_listener.cpp
)

target_include_directories(cppevent-base PUBLIC src)

add_subdirectory(examples/timer)
