add_subdirectory(driver)

add_library(ppp ppp_packet.cpp)
target_link_libraries(ppp PRIVATE udp)

add_executable(radio_service radio_service.cpp)
target_link_libraries(radio_service PRIVATE socket ppp fdmanager udp radio_drivers)
