add_executable(cppevent-base-test EXCLUDE_FROM_ALL
    main.cpp
    byte_buffer_test.cpp
    awaiter_test.cpp
    event_bus_test.cpp
    util_test.cpp
)

target_link_libraries(cppevent-base-test PRIVATE
    cppevent-base
    doctest
)
