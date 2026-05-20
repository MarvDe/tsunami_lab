Woche 5
=======

In der fünften Woche des Tsunami-Labs soll das einlesen und ausgeben von großen 
Daten durch die NetCdf Bibilothek ermöglicht werden. Zum test der Funktionalität 
wird ein Daten abhängiges und ein Daten unabhängiges Setup implementiert, wodurch 
ein Vergleich möglich ist.

Beiträge der Gruppenmitglieder
------------------------------

Gemeinsam
*********

Das Integrieren der NetCdf Bibilothek in das Projekt haben wir durch ein Check von dem Buildsystem Scons
durchgeführt. Dabei bricht der Build ab, wenn die NetCdf Bibilothek nicht auf dem System installiert ist und 
eine Fehlermeldung wird ausgegeben. Damit der Code Auch mit Github Actions funktioniert, mussten wir
die Bibilothek als Abhängigkeit in die ``main.yml`` Datei einfügen. 

Marvin Döring
*************

Schreiben von NetCdf Dateien
____________________________

Für das Schreiben von ``.nc`` Dateien haben wir die Klasse ``io::NetCdf`` erstell.
Der Konstruktor dieser Klasse erstellt eine nc Datei und fügt x, y und eine Zeit 
Dimension ein. Mit der Funktion ``io::NetCdf::write`` kann dann in jedem 
Zeitschritt der Simulation die Höhe, Momentum und Bathymetry in die Datei geschrieben werden. 
Beim Schreiben der Datei halten wir uns an die `COARDS`_ Konvention.

.. _COARDS: https://ferret.pmel.noaa.gov/Ferret/documentation/coards-netcdf-conventions

Tsunami Event 2d
________________

Das Setup ``setups::TsunamiEvent2d`` nimmt Bathymetry und Displacement Daten als 
Eingabe. Die Besonderheit ist, dass die Arrays nicht unbedingt die gleichen Dimensionen 
haben müssen. Bei der Abfrage der Höhe und der Bathymetry wird auf Grundlage 
des nächsten-Nachbarn-Verfahrens der entsprechende Wert errechnet.
Die `Bathymetry`_ und `Displacement`_ Daten haben wir verwendet um folgende Simulation mit der 
``setups::TsunamiEvent2d`` Klasse zu generieren:

.. image:: ../images/tsu2d.gif

.. _Bathymetry: https://scalable.uni-jena.de/assets/tsunami_lab/artificialtsunami_bathymetry_1000.nc
.. _Displacement: https://scalable.uni-jena.de/assets/tsunami_lab/artificialtsunami_displ_1000.nc

Philipp Prell
*************

Lesen von NetCdf Dateien 
________________________

Um ``.nc`` Dateien einzulesen wurde die Klasse ``io::NetCdf`` mit der Funktion ``io::NetCdf::read``
erweitert. Dabei lesen wir zunächst die Größen der verschiedenen Dimensionen ein und 
erstellen dann dynamisch ein Array, in welches die Daten aus der Datei geschrieben werden. Die 
Ausgabe dieser Funktion sind die Größen der Dimensionen und das Array mit den eingelesenen Daten. 

Test des Tsunami Event 2d Setups
________________________________

Zum Testen des Setups ``setups::TsunamiEvent2d`` haben wir das Setup ``setups::ArtificialTsunami2d`` implementiert.
Das ``setups::TsunamiEvent2d`` sollte mit den gegebenen Bathymetry und Displacement Datein ein gleiches
Ergebnis wie das ``setups::ArtificialTsunami2d`` Setup generieren.
Hier sind nochmal Animationen der beiden Setups im Vergleich:

**Tsunami Event 2d Animation** 

.. image:: ../images/tsu2d.gif

**Artificial Tsunami 2d Animation**

.. image:: ../images/artiTsu.gif

**Differenz der beiden Setups**

.. image:: ../images/diffTsu.gif