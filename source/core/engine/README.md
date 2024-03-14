# Engine readme

[Back](../../README.md)

The engine is responsable for creating and destroying measurement objects.

## Construction flags
It has multiple initialization flags:

* Normal - the engine is initialize with all his features and addons.
* Silent - the engine is initialized with the logger set to silent. This means that all the debug messages will be ignored. Only the high priority entries will be logged.
* No metrics - the engine is initialized without the creation of the watchdog. This means that there will no no monitoring for the possible hardware deadlocks.
* No exception handler - the engine will not do anything since this is still under construction and it is not implemented.
* Peformance - the engine will be initialized with the silent, no metrics and no exception handler flags. This will increase performance but will reduce the visibility.

For more inforamtion about the engine initialization flags and their value read the [EngineInitFlag](../../definitions/include/defs/MdsInterface.hpp) enum class documentation generated using the doxygen.

## Engine initialization / termination

Using the [EngineInit](../../definitions/include/defs/MdsInterface.hpp) interface the engine with all its components will be initialized.

The following components will be initialized and terminated:
* Watchdog (if the performance flag or no metrics flag is not present).
* Configuration manager.
* Data distribution manager.
* Logging interface.