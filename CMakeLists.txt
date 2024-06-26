# BDSP (Binary Data Separation Protocol)
# https://github.com/KobaProduction/BDSP
# MIT License

cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

project(BDSP
        VERSION 0.1.1
        DESCRIPTION "The library implements a binary data separation protocol."
        LANGUAGES CXX
)

set(BDSP_SRCS
        src/encoders/cobs/decoder.h
        src/encoders/cobs/decoder.cpp
        src/encoders/cobs/encoder.h
        src/encoders/cobs/encoder.cpp
        src/checksums/crc/crc8.h
        src/checksums/crc/crc8.cpp
        src/bdsp_mixins.h
        src/bdsp_mixins.cpp
        src/bdsp_receiver.h
        src/bdsp_receiver.cpp
        src/bdsp_transceiver.h
        src/bdsp_transceiver.cpp
        src/bdsp_transmitter.h
        src/bdsp_transmitter.cpp
        src/bdsp_types.h
        src/packet.h
        src/packet.cpp
)

idf_component_register(
        SRC_DIRS
            src
            src/checksums/crc
            src/encoders/cobs
        INCLUDE_DIRS
            src
            src/checksums/crc
            src/encoders/cobs
        REQUIRES
            arduino
)

target_compile_options(${COMPONENT_LIB} PUBLIC -DEFINE_OPTION=1)

add_library(BDSP BDSP_SRCS)