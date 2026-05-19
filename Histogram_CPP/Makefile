# File: Makefile
# Task: Histogram — структура для статистичної обробки даних (завдання 13)
# Group: К-24
# Author: Курлов Микита
# Date: 2026-05-19
# Description: Makefile для збірки та тестування C++ версії проекту Histogram,
#              а також для збірки крос-тесту з С-реалізацією.

CXX = g++
CC = gcc
CXXFLAGS = -Wall -Wextra -std=c++11 -O2
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lm

TARGET_CPP = histogram_test_cpp
TARGET_CROSS = histogram_cross_test

OBJS_CPP = Histogram.o Histogram_test.o
OBJS_CROSS = Histogram.o Histogram_cross_test.o Histogram_c.o

all: $(TARGET_CPP) $(TARGET_CROSS)

$(TARGET_CPP): $(OBJS_CPP)
	$(CXX) $(CXXFLAGS) -o $(TARGET_CPP) $(OBJS_CPP) $(LDFLAGS)

$(TARGET_CROSS): $(OBJS_CROSS)
	$(CXX) $(CXXFLAGS) -o $(TARGET_CROSS) $(OBJS_CROSS) $(LDFLAGS)

Histogram.o: Histogram.cpp Histogram.h
	$(CXX) $(CXXFLAGS) -c Histogram.cpp

Histogram_test.o: Histogram_test.cpp Histogram.h
	$(CXX) $(CXXFLAGS) -c Histogram_test.cpp

Histogram_cross_test.o: Histogram_cross_test.cpp Histogram.h ../Histogram_C/Histogram.h
	$(CXX) $(CXXFLAGS) -c Histogram_cross_test.cpp

Histogram_c.o: ../Histogram_C/Histogram.c ../Histogram_C/Histogram.h
	$(CC) $(CFLAGS) -c ../Histogram_C/Histogram.c -o Histogram_c.o

run: $(TARGET_CPP)
	./$(TARGET_CPP)

run_cross: $(TARGET_CROSS)
	./$(TARGET_CROSS)

clean:
	rm -f $(TARGET_CPP) $(TARGET_CROSS) $(OBJS_CPP) $(OBJS_CROSS) Histogram_c.o test_hist_cpp.txt test_hist_cpp.bin histogram_cpp_saved.txt histogram_cpp_saved.bin histogram_cpp_report.txt

.PHONY: all run run_cross clean
