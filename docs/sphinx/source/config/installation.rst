Installation
============

Voraussetzungen
---------------

Zum Bauen des Projekts werden folgende Werkzeuge benötigt:

- `CMake <https://cmake.org/>`_
- `SCons <https://scons.org/>`_

Zusätzlich verwendet das Projekt die folgenden Git-Submodule:

- ``yaml-cpp`` – Einlesen und Verarbeiten von YAML-Konfigurationsdateien.
- ``Catch2`` – Framework für Unit-Tests.

Repository vorbereiten
----------------------

Nach dem Klonen des Repositories müssen zunächst die Submodule initialisiert werden.
Dies kann bequem mit dem bereitgestellten Setup-Skript erfolgen:

.. code-block:: bash

   ./setup.sh

Alternativ können die Submodule auch manuell geladen werden:

.. code-block:: bash

   git submodule update --init --recursive

Projekt bauen
-------------

Das Tsunami Lab verwendet **SCons** als Build-System.

Zum Erstellen des Projekts genügt der folgende Befehl im Wurzelverzeichnis des Repositories:

.. code-block:: bash

   scons

Die erzeugten Binärdateien werden anschließend im Verzeichnis

::

   build/

abgelegt.

Optimierter Build
~~~~~~~~~~~~~~~~~

Für eine optimierte Version der Software kann der folgende Befehl verwendet werden:

.. code-block:: bash

   scons mode=optimization

Benutzung
---------

Nach erfolgreichem Build befinden sich sowohl die Anwendung als auch die Unit-Tests
im Verzeichnis

::

   build/

Die Hauptanwendung kann beispielsweise mit

.. code-block:: bash

   ./build/tsunami_lab

gestartet werden.

Programmoptionen
----------------

Eine Übersicht über alle verfügbaren Kommandozeilenoptionen erhält man mit

.. code-block:: bash

   ./build/tsunami_lab help

Die Konfiguration der Simulation kann auf zwei Arten erfolgen:

1. Über Kommandozeilenparameter.
2. Über eine YAML-Konfigurationsdatei.

Die Verwendung und der Aufbau der YAML-Datei werden im Kapitel
:doc:`yaml` beschrieben.