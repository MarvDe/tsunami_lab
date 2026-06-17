Woche 9
=======


Beiträge der Gruppenmitglieder
------------------------------

Gemeinsam
*********

Marvin Döring
*************

Um die For-Schleifen des 2D-Solvers zu parallelisieren können verschiedene Strategien 
verwendet werden. Man könnte nur die äußere Schleife parallelisieren, nur die inner oder 
beide schleifen mit der OpenMP Funktion ``collapse(2)``. Wir haben die verschiedenen 
Möglichkeiten auf einer lokalen Maschine getestet.

==================================   ==============      ======================
Konfiguration                        Gesamtzeit (s)      normalisierte Zeit (s)
==================================   ==============      ======================
Keine Parallelisierung               18.5217             2.95402e-08
Innere For-Schleife mit 28 Threads   170.753             2.72334e-07
Innere For-Schleife mit 4 Threads    6.74605             1.07593e-08
Äußere For-Schleife mit 28 Threads   4.64874             7.41426e-09
Äußere For-Schleife mit 4 Threads    5.32677             8.49564e-09
Beide For-Schleifen mit 28 Threads   4.84144             7.7216e-09
Beide For-Schleifen mit 4 Threads    5.72226             9.1264e-09
==================================   ==============      ======================

Man kann erkennen, dass die Parallelisierung im allgemeinen eine starke 
Verbesserung der Laufzeit herbei führt. 
Die beste Laufzeit konnte durch die Parallelisierung der äußeren For-Schleife mit 28 Threads erreicht werden.
Dabei beobachten wir ein Speedup von :math:`S_p = 18.5217s / 4.64874s = 3.984240891`.
Interessant ist auch die extrem schlechte Laufzeit bei der Parallelisierung der inneren For-Schleife mit 
28 Threads. Unsere Theorie ist, dass in diesem Fall Cache Konflikte durch False-Sharing auftreten könnten und 
der Overhead von dem mehrfachen Erstellen der parallelen Umgebung die Laufzeit stark nach oben treibt. 



Philipp Prell
*************