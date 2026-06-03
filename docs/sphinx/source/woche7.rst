Woche 7
=======

In der siebten Woche des Tsunami Labs soll die Möglichkeit von Checkpoints implementiert werden.
Dieses Feature kann dafür verwendet werden eine Simulation fort zu setzten falls das Programm abgestürzt ist.
Des weiteren soll die Ausgabe so erweitert werden, dass nicht jede Zelle abgespeichert wird sondern in einem 
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
entstehenden Ausgabedatei eine Größe von ``(Height / output resolution) x (Width / output resolution)`` und ist somit deutlich kleiner 
als das Simulationsgrid.

**Original Grid 250m**



**Demonstration des verschmelzens von Zellen mit output resolution 10**



Philipp Prell
*************


