cmake_minimum_required(VERSION 3.11)
project(test_kingbase_dci)

find_package(Qt4 4.7.2 REQUIRED QtCore QtSql)
# if(MSVC)
# include_directories(E:/Downloads/boost_1_54_0)
# else()
# include_directories(C:/msys64/mingw64/include)
# endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/v8r6_dci_win/include)
link_directories(${CMAKE_SOURCE_DIR}/v8r6_dci_win/lib_x86)

add_executable(test_dci test_dci.cc)
target_link_libraries(test_dci PUBLIC dcikdb)
add_executable(test_qt_odbc test_qt_odbc.cc)

target_link_libraries(test_qt_odbc PUBLIC Qt4::QtCore Qt4::QtSql)
