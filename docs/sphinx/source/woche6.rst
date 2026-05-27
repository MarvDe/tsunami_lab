Woche 6
=======

Für die Woche 6 ist geplant die Tsunami Events von 2010 in Chile und 
2011 in Tohoku zu simulieren. Dafür sollen gegebene Bathymetry und Displacement 
Daten durch die Klasse ``setups::TsunamiEvent2d`` eingelesen werden.

Beiträge der Gruppenmitglieder
------------------------------

Gemeinsam
*********

Zunächst mussten wir einen Bug aus letzter Woche fixen. 
Uns ist aufgefallen, dass wir bei der Addition des Displacements auf die Bathymetry
numerische Fehler bekommen haben. Wir wussten aber zunächst nicht genau warum das der Fall ist.
Nach längerem Probieren merkten wir, dass eine Verringerung der Simulationsschrittgröße (``dt``)
das Problem behebt.   

Marvin Döring
*************

Chile Simulation
________________

Zunächst haben wir versucht das Bathymetry Grid mit einer Auflösung von 250m mit dem Programm Paraview 
darzustellen. Das haben wir aber weder auf unseren lokalen Maschinen noch auf den Uni PCs über SSH hinbekommen.
Es kam ständig zu Abstürzen auf der hohen Auslastung des RAMs.

Die Simulation des Chile Events hat zumindest mit den Daten und einer Größe des Grids von 1400x1180 funktioniert.
Das entspricht dann einer Abtastung der gegebenen Datein bei jedem 10ten Wert und umgerechnet 2500m pro Zelle.
Bei Betrachtung der Simulation ist zu erkennen, dass die ersten Wellen den Simulationsbereich bei dem Zeitschritt ~107 verlassen.
Jeder Zeitschritt entspricht 25*107 = 2675 in Simulationszeitschritten. Als dt wurde ein Wert von 0.000881404 berechnet. Dabei muss noch 
mit 2500 multipliziert werden, da dass die Größe einer Zelle in Metern ist. ``dt_real = 2.20351``. Nun wird also 
der Zeitpunkt für das erste Verlassen von Wellen berechnet durch ``2675 * 2.20351s = 5894.38925s``, das sind dann etwa 98 Minuten.
Da wir für jede Zelle ein X- und ein Y-Update berechnen kommen insgesamt ``2 * 1400 * 1180 * 453 * 25 = 37417800000`` Updates zu stande.
Dabei ist 1400x1180 die Größe des Grids und 453*25 die Anzahl an Zeitschritten der Simulation.

**2010 M 8.8 Chile Event Height**

.. image:: ../images/chile2010.gif 

**2010 M 8.8 Chile Event Momentum**

.. image:: ../images/chile2010_momentum.gif 

Tohoku Simulation
_________________

Für die Tohoku Simulation haben wir eine Größe von 2700x1500 verwendet. Das entspricht jedem 4ten Datenpunkt aus den Bathymetry und Displacement Daten.
Die Simulation lief für 504*25 Zeitschritte und somit kommen wir auf folgende Anzahl an Zellupdates: ``2 * 2700 * 1500 * 504 * 25 = 102060000000``.
Als dt wurde in dieser Simulation 0.000792277 verwendet und die ersten Wellen verlassen die Simulation bei dem Zeitschritt 99.
Insgesamt brauchen die Wellen also ``25 * 99 * (0.000792277 * 1000)s = 1960.885575s``. 
Es ergeben sich also etwa 33 Minuten bis die erste Welle die Simulation verlässt. 

**2011 M 9.1 Tohoku Event**

.. image:: ../images/tohoku2011.gif

Philipp Prell
*************

Um zu schätzen wie lange der Tsunami von Tohoku gebraucht hat um die Stadt Soma zu erreichen 
haben wir den Durchschnitt der negativen Bathymetry aus der `locations`_ Datei berechnet. Dabei sind wir auf 4449.24m gekommen.
Für die Geschwindigkeit ergibt sich dann:

.. _locations: https://scalable.uni-jena.de/assets/tsunami_lab/soma_epicenter_tohoku_2011_bathymetry.csv

.. math::

    \lambda = \sqrt{gh} = \sqrt{(9.81m/s²)*4449.24m} = 208.918m/s

Der euklidische Abstand zwischen dem Epizentrum und der Stadt Soma ist 139.31618km. Somit sind wir auf folgendes Ergebnis gekommen:

.. math::

    139316.18m/(208.918m/s) = 666.84s 

Das entspricht ungefähr 11.114 Minuten bis die Welle Soma erreicht.

Eigentlich sollte nun eine Simulation mit einer eingebauten Station an der Küste vor Soma folgen. 
Diese Aufgabe konnte wir bisher nicht vollständig bearbeiten und haben deshalb hierfür keine Lösung.
Unser Problem ist aktuell, dass wir keine gute Möglichkeit haben um von Simulations-Grid-Koordinaten in 
Koordinaten der echten Welt umzurechnen. Das müssen wir also noch in das Programm einbauen.


    