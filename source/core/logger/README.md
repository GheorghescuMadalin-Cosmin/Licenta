# Logger readme

[Back](../../README.md)

Logging interface that is responsable for logging all the received entries. Currently the logging is done in stdout, but the plans are to be done into a .log file or a .xml file. This is a todo topic.

A requirement for this interface is to be thread safe, this means that it can be called from multiple threads and should be safe to log the entries.