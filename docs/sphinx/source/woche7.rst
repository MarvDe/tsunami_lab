Woche 7
=======

In der siebten Woche des Tsunami Labs soll die Möglichkeit von Checkpoints implementiert werden.
Dieses Feature kann dafür verwendet werden eine Simulation fort zu setzten falls das Programm abgestürzt ist.
Des Weiteren soll die Ausgabe so erweitert werden, dass nicht jede Zelle abgespeichert wird sondern in einem 
bestimmten Quadrat nur der Durchschnitt aller dort befindlichen Zellen ausgegeben wird.
Dadurch ist die Ausgabedatei kleiner.

Beiträge der Gruppenmitglieder
------------------------------

Gemeinsam
*********

Zunächst mussten wir unser Programm umschreiben, sodass die interne Struktur mit tatsächlichen Koordinaten arbeitet.
Dies war bis letzte Woche nicht der Fall, für den weitern Verlauf des Projektes ist es jedoch deutlich einfacher mit 
den eingelesenen Koordinaten zu arbeiten. Im zuge dessen mussten wir also den Parser anpassen, die NetCdf Klasse und 
die Stations. 

Marvin Döring
*************

Um Zellen in der Ausgabe zu verschmelzen haben wir den ``output resolution`` Parameter in das Programm eingebaut, welcher
die Größe des Quadrates angibt. Alle Zellen, die sich in diesem Quadrat befinden werden, in der 
Ausgabedatei, auf einen Wert abgebildet. Dieser entspricht dem Durchschnitt aller Zellen in dem Quadrat. Somit hat die 
entstehende Ausgabedatei eine Größe von ``(Height / output resolution) x (Width / output resolution)`` und ist somit deutlich kleiner 
als das Simulationsgrid. Zur Visualisierung der Funktionalität haben wir nochmal das 250m Gitter von Tohoku verwendet, denn 
die Datei mit einem 50m Gitter konnten wir weder lokal noch über den Uni PC simulieren. Der Ram hat nicht gereicht... 

**Demonstration des verschmelzens von Zellen**

.. image:: ../images/downscaled2d.0000.png


.. image:: ../images/downscaled3d.0000.png


Philipp Prell
*************

Zur Implementierung der CheckPoint-Funktionalität wird nach jedem Schreibvorgang, der
``NetCdf``-Klasse in die Solutions-Datei, ein ``nc_sync()`` aufgerufen.
Dadurch wird der gesamte Puffer vollständig in den Speicher geschrieben.

Das Setup ``setups::CheckPoint`` liest den letzten Zeitschritt aus der bestehenden NetCDF-Datei aus, indem die
Größe der Dimension ``time`` abgefragt und die Simulation an genau dieser Stelle fortgesetzt
wird. Daten wie Bathymetrie und Geschwindigkeit werden in den Puffer eingelesen und über
``Setup`` an die Wellenausbreitung (``WavePropagation``) übergeben.

Der Nutzer hat die Wahl, ob eine neue NetCDF-Datei angelegt oder die neue Simulation an
eine bestehende Datei angehängt werden soll.
