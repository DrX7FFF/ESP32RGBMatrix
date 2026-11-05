#pragma once

#include <Arduino.h>

#define CMD(prefix, argc, code, usage) { prefix, argc, [](char *argv[], int argc_, Print &out) code, usage }

struct Command {
  const char *prefix;
  int argCount;
  void (*handler)(char *argv[], int argc, Print &out);
  const char *usage;
};

extern const Command COMMANDS[];
extern const size_t COMMANDS_COUNT;

void printHelp(Print &out);
// Executes one command line; replies go to out (Serial, TCP client...).
void handleConsole(const char *line, Print &out);

// Redirects the next completed serial line to handler instead of the console; reset automatically by handleSerialInput.
typedef void (*LineHandler)(const char *line);
void setLineHandler(LineHandler handler);
void handleSerialInput();
