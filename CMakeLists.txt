add_library(cppevent-base STATIC
  src/cppevent_base/async_signal.cpp
  src/cppevent_base/event_bus.cpp
  src/cppevent_base/event_loop.cpp
  src/cppevent_base/io_listener.cpp
  src/cppevent_base/signal_listener.cpp
  src/cppevent_base/timer.cpp
  src/cppevent_base/util.cpp
)

target_include_directories(cppevent-base PUBLIC src)

add_subdirectory(test)
