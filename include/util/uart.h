#ifndef _UART_
#define _UART_

#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <exception>
#include <string>
#include <cstring>


class UARTException : public std::exception {
private:
    std::string what_;
public:
    UARTException(std::string what) : what_(what) {}

    const char* what() const throw() {
        return what_.c_str();
    }
};

class UART {
private:
    int fd_;
    struct termios init_settings_;

    struct termios getSettings();
    void setSettings(const struct termios &options);

public:
    UART(const std::string& port, struct termios options);
    ~UART();

    ssize_t read(void *buf, size_t count);
    size_t write(const void *buf, size_t count);
    void flush();

    int getFD();

    static inline struct termios emptySettings(speed_t baudrate) {
        struct termios options;

        options.c_iflag = 0;
        options.c_oflag = 0;

        options.c_lflag = 0;

        options.c_cflag = CS8 | CRTSCTS | CREAD | CLOCAL;

        options.c_cc[VTIME] = 0;
        options.c_cc[VMIN] = 0;

        cfsetispeed(&options, baudrate);
        cfsetospeed(&options, baudrate);

        return options;
    }

    static inline struct termios disableHardwareFlowControl(struct termios options) {
        options.c_cflag &= ~CRTSCTS;
        return options;
    }

    UART(const UART &other) = delete;
    UART& operator=(const UART &other) = delete;
};

#endif
