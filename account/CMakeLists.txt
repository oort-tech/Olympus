file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_library(account ${sources} ${headers})

target_link_libraries(account)