cmake_minimum_required(VERSION 3.5)

add_library(cppevent_base
  src/cppevent_base/async_signal.cpp
  src/cppevent_base/event_bus.cpp
  src/cppevent_base/event_callback.cpp
  src/cppevent_base/event_loop.cpp
  src/cppevent_base/io_listener.cpp
  src/cppevent_base/io_service.cpp
  src/cppevent_base/timer.cpp
  src/cppevent_base/util.cpp
)

target_include_directories(cppevent_base PUBLIC src)
target_link_libraries(cppevent_base PRIVATE uring)

add_subdirectory(test)
