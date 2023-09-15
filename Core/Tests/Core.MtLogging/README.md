# Purpose

- Output should not be scrambled when multiple threads are logging at the same time.
- The threads loop log messages at all levels continuously but they should be filtered by the log level of the Logger.