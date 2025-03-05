project(BLang_library)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create library target
add_library(BLang
    blang_type.h
    blang_type.cpp
    blang_type_factory.cpp
    blang_expression.h
    blang_expression.cpp
    blang_expression_factory.cpp
    blang_predicate.h
    blang_predicate.cpp
    blang_predicate_factory.cpp
    blang_type_fmt.h
    blang_expression_fmt.h
    blang_predicate_fmt.h
    blang_fmt.h
    blang_tinyxml2_reader.h
    blang_tinyxml2_reader.cpp
    blang_xml_writer.h
    blang_xml_writer.cpp
)

# Add threading support
target_link_libraries(BLang 
    PRIVATE
        Threads::Threads
        fmt::fmt
        tinyxml2::tinyxml2
)

