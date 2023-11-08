add_library(radio_drivers
    radio_driver.cpp
    satellite_radio_driver.cpp
    ground_radio_driver.cpp)
target_link_libraries(radio_drivers PRIVATE ppp uart_temp raii_gpio)
