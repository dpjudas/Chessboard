﻿cmake_minimum_required(VERSION 3.8)

set(SOURCES
	"Chessboard.cpp"
	"Chessboard.h"
	"SPIDevice.cpp"
	"MFRC522.cpp"
	"MFRC522.h"
	"GPIODevice.cpp"
	"GPIODevice.h"
	"Timer.cpp"
	"Timer.h"
	"Main.cpp")

add_executable(chessboard ${SOURCES})
target_link_libraries(chessboard "gpiod" "curses")

install(TARGETS chessboard DESTINATION /home/pi/chess)
