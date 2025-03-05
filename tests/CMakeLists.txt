find_package(GTest REQUIRED)

add_executable(blang_type_tests
    blang_type_tests.cpp
)

target_include_directories(blang_type_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(blang_type_tests
    PRIVATE
        BLang
        GTest::GTest
        GTest::Main
        Threads::Threads
        fmt::fmt
)

add_test(NAME blang_type_tests COMMAND blang_type_tests)

add_executable(blang_type_formatter_tests
    blang_type_formatter_tests.cpp
)

target_include_directories(blang_type_formatter_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(blang_type_formatter_tests
    PRIVATE
        BLang
        GTest::GTest
        GTest::Main
        Threads::Threads
        fmt::fmt
)

add_test(NAME blang_type_formatter_tests COMMAND blang_type_formatter_tests)

add_executable(blang_expression_tests
blang_expression_tests.cpp
)
target_include_directories(blang_expression_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(blang_expression_tests
    PRIVATE
        BLang
        GTest::GTest
        GTest::Main
        Threads::Threads
        fmt::fmt
)
add_test(NAME blang_expression_tests COMMAND blang_expression_tests)

add_executable(blang_expression_formatter_tests
    blang_expression_formatter_tests.cpp
)
target_include_directories(blang_expression_formatter_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(blang_expression_formatter_tests
    PRIVATE
        BLang
        GTest::GTest
        GTest::Main
        Threads::Threads
        fmt::fmt
)
add_test(NAME blang_expression_formatter_tests COMMAND blang_expression_formatter_tests)

add_executable(blang_predicate_tests
blang_predicate_tests.cpp
)
target_include_directories(blang_predicate_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(blang_predicate_tests
    PRIVATE
        BLang
        GTest::GTest
        GTest::Main
        Threads::Threads
        fmt::fmt
)
add_test(NAME blang_predicate_tests COMMAND blang_predicate_tests)

add_executable(blang_predicate_formatter_tests
    blang_predicate_formatter_tests.cpp
)
target_include_directories(blang_predicate_formatter_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(blang_predicate_formatter_tests
    PRIVATE
        BLang
        GTest::GTest
        GTest::Main
        Threads::Threads
        fmt::fmt
)
add_test(NAME blang_predicate_formatter_tests COMMAND blang_predicate_formatter_tests)

add_executable(blang_tinyxml2_reader_tests
    blang_tinyxml2_reader_tests.cpp
)

target_include_directories(blang_tinyxml2_reader_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(blang_tinyxml2_reader_tests
    PRIVATE
        BLang
        GTest::GTest
        GTest::Main
        Threads::Threads
        fmt::fmt
)
add_test(NAME blang_tinyxml2_reader_tests COMMAND blang_tinyxml2_reader_tests)

add_executable(blang_xml_writer_tests
    blang_xml_writer_tests.cpp
)
target_include_directories(blang_xml_writer_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(blang_xml_writer_tests
    PRIVATE
        BLang
        GTest::GTest
        GTest::Main
        Threads::Threads
        fmt::fmt
)
add_test(NAME blang_xml_writer_tests COMMAND blang_xml_writer_tests)

