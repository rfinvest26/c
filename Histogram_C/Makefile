# File: Makefile
# Task: Histogram — структура для статистичної обробки даних (завдання 13)
# Group: К-24
# Author: Курлов Микита
# Date: 2026-05-19
# Description: Makefile для збірки та тестування C-версії проекту Histogram.

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lm

TARGET = histogram_test
OBJS = Histogram.o Histogram_test.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

Histogram.o: Histogram.c Histogram.h
	$(CC) $(CFLAGS) -c Histogram.c

Histogram_test.o: Histogram_test.c Histogram.h
	$(CC) $(CFLAGS) -c Histogram_test.c

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) test_hist.txt test_hist.bin histogram_saved.txt histogram_saved.bin histogram_report.txt

.PHONY: all run clean
