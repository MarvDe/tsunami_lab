###########
Tsunami Lab
###########

Tsunami lab Sommersemester 2026
Gruppe: Philipp Prell und Marvin Doering

Dokumentation
-------------

Die Dokumentationswebseite ist unter https://marvde.github.io/tsunami_lab/index.html aufrufbar.
Dort steht eine Anleitung für das installieren und nutzen des Programms. Außerdem 
findet man auf der Webseite auch eine Dokumentation, bestehend aus wöchentlichen Einträgen,
über die Entstehung der Software. Wir wünschen euch viel Spaß beim Lesen!

Setups
------

Um schnell mit dem Programm zu starten haben wir ein paar Args Dateien definiert.
`Artificial tsunami`_ ist ein einfaches Setup um das Programm zu testen.
Die NOAA (National Oceanic and Atmospheric Administration) hat standard Tsunami Setups definiert wie zum Beispiel `solitary wave on beach`_ and `single wave on canonical island`_, welche 
implementiert sind. Diese Setups wurden auch mit analytischen und labor Daten verglichen, siehe dazu die Dokumentationswebseite.
Ansonsten sind noch viele weitere `Setups`_. 

.. _Artificial tsunami: https://github.com/MarvDe/tsunami_lab/blob/main/utilities/args/argsArtificialTsunamiEvent.yaml
.. _solitary wave on beach: https://github.com/MarvDe/tsunami_lab/blob/main/utilities/args/argsNTHMP_BP01_SolitaryWaveBeach.yaml
.. _single wave on canonical island: https://github.com/MarvDe/tsunami_lab/blob/main/utilities/args/argsSingleWaveCanonicalIsland.yml
.. _Setups: https://github.com/MarvDe/tsunami_lab/tree/main/utilities/args

Further information is available from: https://scalable.uni-jena.de/opt/tsunami/
