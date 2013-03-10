project(jserve)

add_executable(jserve main.cpp)

install(TARGETS jserve RUNTIME DESTINATION bin)
