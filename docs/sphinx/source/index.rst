.. Tsunami Lab documentation master file, created by
   sphinx-quickstart on Sun Apr 12 11:15:43 2026.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Tsunami Lab Dokumentation
=========================

Dies ist die Dokumentationswebseite der Gruppe Philipp Prell und Marvin Doering 
von dem Tsunami Lab, angeboten im Sommersemester 2026, an der Uni Jena. 
Das Tsunami Lab findet unter der Leitung von Max Engel und Stefan Remke statt.

.. toctree::
   :maxdepth: 2
   :caption: Contents:



Build 
-----

Für das Tsunami Lab nutzen wir `Scons`_ als Build-Tool.

.. _Scons: https://scons.org/

Zum Bauen der Software muss ``scons`` im Wurzelverzeichnis ausgeführt werden.
Die entsprechenden Dateien sollten dann im Unterverzeichnis ``build`` entstehen.

Benutzung
---------

Die Software ``tsunami_lab`` und Unit-Tests sind nach dem Bauen des Projektes im Unterverzeichnis ``build`` zu finden.


.. toctree::
  woche1 
  woche2
  woche3
