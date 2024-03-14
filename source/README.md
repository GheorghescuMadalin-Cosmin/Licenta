# Projects

## Application (project)

### [MDS (executable)](./application/README.md)
Main GUI application.

#### MDS depends on:
* [Engine](./core/engine/README.md)
* [Logger](./core/logger/README.md)

## Core (project)
### [Engine (library)](./core/engine/README.md)

The heart of the MDS framework. 
#### The engine is responsable:
* To scan all the modular objects and to create their corresponding objects.
* To intialization of data distribution and communication between objects.
* To initialization and destroy all the measuremetn objects.
* To keep an eye for any hardware deadlock using the watchdog feature.

#### The engine depends on:
* [Logger](./core/logger/README.md)

### [Logger (library)](./core/logger/README.md)

Logging mechanism.

#### The logger is responsable:
* To log all the informations it receives.
* Perform logging from multiple sources at a time.

#### The logger doesn't depend on anybody

## [Definitions (project)](./definitions/README.md)

It contains all the definitions. It doesn't produce any artifact

## [Processors](./processors/README.md)

## [Recording](./recording/README.md)

## [Replay](./replay/README.md)

## [Test](./test/README.md)

## [Transmitters](./transmitters/README.md)

## [Utilis](./utilis/README.md)
