#include "cmd_interpreter.h"
#include <cstring>

static constexpr size_t COMMAND_BUFFER_SIZE = 512;

void printHelp(Print &out) {
  out.println("Available commands:");
  for (size_t i = 0; i < COMMANDS_COUNT; ++i) {
    out.printf("  %s\n", COMMANDS[i].usage);
  }
}

static int splitArgs(char *input, char *out[], int maxArgs) {
  int argc = 0;
  char *token = strtok(input, " \t\r\n");
  while (token != nullptr) {
    if (argc >= maxArgs) return -1;
    out[argc++] = token;
    token = strtok(nullptr, " \t\r\n");
  }
  return argc;
}

void handleConsole(const char *line, Print &out) {
  if (line != nullptr) {
    char buffer[COMMAND_BUFFER_SIZE];
    strncpy(buffer, line, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char *argv[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
    int argc = splitArgs(buffer, argv, 5);

    if (argc < 0)
      out.printf("ERR: too many arguments\n");
    else if (argc > 0) {
      bool found = false;
      for (size_t i = 0; i < COMMANDS_COUNT; ++i) {
        const Command &cmd = COMMANDS[i];
        if (strcmp(argv[0], cmd.prefix) == 0) {
          if (argc - 1 == cmd.argCount)
            cmd.handler(cmd.argCount > 0 ? argv + 1 : nullptr, cmd.argCount, out);
          else
            out.printf("ERR: usage %s\n", cmd.usage);
          found = true;
          break;
        }
      }
      if (!found)
        out.printf("ERR: unknown command\n");
    }
    memset(buffer, 0, sizeof(buffer));
  }
}

static void serialConsole(const char *line) {
  handleConsole(line, Serial);
  Serial.printf("\n> ");
}

static char consoleLine[COMMAND_BUFFER_SIZE];
static LineHandler currentLineHandler = serialConsole;

void setLineHandler(LineHandler handler) {
  currentLineHandler = handler != nullptr ? handler : serialConsole;
}

// Owns the console byte stream: accumulates a line, then dispatches it to whichever handler is currently set.
void handleSerialInput() {
  if (!Serial.available()) return;
  char c = static_cast<char>(Serial.read());
  if (c == '\r') {
    return;
  } else if (c == '\n') {
    Serial.println();
    LineHandler handler = currentLineHandler != nullptr ? currentLineHandler : serialConsole;
    currentLineHandler = serialConsole;
    handler(consoleLine);
    memset(consoleLine, 0, sizeof(consoleLine));
  } else if (c == '\b' || c == 0x7F) {
    size_t length = strlen(consoleLine);
    if (length > 0) {
      consoleLine[length - 1] = '\0';
      Serial.print("\b \b");
    }
  } else if (strlen(consoleLine) < sizeof(consoleLine) - 1) {
    Serial.print(c);
    size_t length = strlen(consoleLine);
    consoleLine[length] = c;
    consoleLine[length + 1] = '\0';
  }
}

