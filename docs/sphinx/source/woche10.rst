Woche 10
========

In der zehnten Woche des Tsunami Labs wollen wir den Solver erweitern, sodass 
Ein Trocken/Nass Dammbruch Setup gelöst werden kann. Aktuell behandeln wir 
die Grenzen zwischen trockenen und nassen Zellen als reflektierende Wände, an 
welchen das Wasser abprallt. Um die Simulation noch realistischer zu machen 
soll das Wasser von der trockenen Fläche auf die nasse fließen.

Hydrostatische Rekonstruktion
-----------------------------

Als erstes haben wir die Idee von Audusse et al. implementiert um die Eingabeparameter
zu modefizieren. Das Paper garantiert ein well-balanced, nicht negatives Schema, welches 
eine semidiskrete Entropieungleichung erfüllt. Dadurch sollte das Wasser 
von einer nassen Zelle auf eine trockene fließen können. Desweiteren geht 
die Methode auch auf Bathymetry ein. Im Paper wird behaupted, dass 
diese Vorverarbeitung den Umgang von Bathymetry Daten mit jedem Solver erlaubt. Der 
Roe Solver, welcher in diesem Projekt implementiert ist, kann ja von sich aus 
nicht direkt mit Bathymetry umgehen, doch durch die vorgenommene Erweiterung sollte das nun auch funktionieren.

Jetzt haben wir das Trocken/Nass Dammbruch Setup und hydrostatische Rekonstruktion verwendet um folgende 
Simulation zu erzeugen:


Wie man sieht fließt nur ein Teil des Wassers auf die trockene Fläche und es entsteht ein Bruch beim Übergange.
Leider bewegt sich die Simulation jetzt nicht mehr weiter und der Wasserstand gleicht sich nicht aus, so wie 
man das eigentlich erwarten würde. Zunächst haben wir versucht die Zeitschrittweite zu verringern, damit 
das Wasser sich vielleicht doch noch gleichmäßig verteilt. Dies hat aber auch nicht funktioniert. 
Um die Ursache des Problems zu finden haben wir die Berechnung einmal per Hand durchgeführt um zu 
gucken, was hier genau passiert:

.. math::
    :name: Berechnung stationärer Zustand 

    h_l = 0.0624 \\
    h_r = 0.0245 \\
    hu_l = hu_r = 0.0255 \\
    waveSpeed_l = 0.0 \\
    waveSpeed_r = 1.306 \\

    Froude Zahle: \\
        F = u / \sqrt(g*h) = 0.653 / \sqrt(9.81 * 0.0435) = 1.


Wir erhalten also eine Froude Zahl von ca. 1 und sind somit nicht mehr 
im subkritischen Bereich. Damit kann der Fwave Solver nicht umgehen und 
deshalb entsteht das unphysikalische Verhalten. Doch so schnell wollten wir noch nicht aufgeben und 
haben weiter recherchiert. 

Der HLLE Solver
---------------

Die naheliegenste potentielle Lösung des Problems wäre es einen neuen Solver zu verwenden, 
welcher besser mit superkritischen Bereichen umgehen kann. Wir haben uns über verschiedene 
Arten von Solvern informiert und kamen so auf den HLLE (Harten-Lax-van Leer-Einfeldt) Solver.
Dieser Ansatz ist besser geeignet für stark kinematische Zustände, kann aber auf der anderen Seite 
langsame Wellen nicht so präzise darstellen wie der Fwave Solver. 
Bei der Anwendung des HLLE Solvers auf unser Trocken/Nass Dammbruch Setup kam folgendes raus: 

.. HLLE gif hier einfügen

Lax-Friedrichs Viskosität 
-------------------------

Eine weiter Idee wäre es den Fwave Solver zu modefizieren um das unphysikalische Verhalten zu korregieren.
Auch hier haben wir eine Möglichkeit gefunden, welche genau diesen Ansatz verfolgt.
Die Lax-Friedrichs Viskosität ist ein diffusions Term, welcher nicht wirklich physikalisch motiviert ist,
aber trotzdem bei numerischen Simulationen verwendet werden kann. Die Idee ist es, dass man bei dem 
Fall von superkritischen Szenarien auf beide Seiten der Zellen kleine Updates addiert und somit den 
unphysikalischen statischen Fluss unterbricht.
Als wir die Erweiterung des F-wave Solvers mit Lax-Friedrichs Viskosität auf den Trocken/Nass Dammbruch los gelassen 
haben erhielten wir folgendes Ergebnis: 

.. Fwave mit fix gif hier einfügen 

Manning Reibung
---------------

Als letztes könnte man noch überlegen außerhalb des Solvers eine weitere Kraft einzuführen, welche 
den statischen Fluss unterbricht. Für diesen Fall haben wir die Manning Reibung gefunden. 
Es handelt sich hier bei um eine Formel, welche den Wiederstand von fließenden Gewässern berechnet.

.. math::
    :name: Manning Reibung

    v = 1/n * R^{2/3} * S^{1/2} \\
      \\
    v: durchschnittliche Geschwindigkeit \\
    n: Manning-Rauheit \\
    R: hydraulischer Radius \\
    S: Anstieg des Energiegradienten.


Somit erhalten wir eine weitere Kontrollschraube in der Simulation.
Die Manning Reibung schafft es das unphysikalische Verhalten des Fwave Solvers soweit zu stören, 
dass sich am Ende der Trocken/Nass Dammbruch Simulation das System stabilisiert: 

.. Manning Reibung + Fwave gif hier einfügen 

Der Hybrid
----------

Nun haben wir das Trocken/Nass Dammbruch Problem auf verschieden Weisen gelöst. 
Es spricht nichts dagegen diese Lösungen zu kombinieren, vielleicht kommt ja 
was besseres raus als die einzelnen Ansätze. Da der Fwave Solver besser im Fall von 
einer kleinen Froude Zahl ist als der HLLE Solver, haben wir eine Kombination 
aus beiden Ansätzen implementiert. Dieser Hybride Solver berechnet zunächst die 
Froude Zahl und entscheidet dann ob der HLLE oder Fwave Solver verwendet werden soll.
Dieser Ansatz liefert unserer Meinung nach die besten Ergebnisse. Zusätzlich dazu
kann noch die Manning Reibung angewendet werden. 
Unser Ergebnis ist hier zu sehen: 

.. Hybrid Solver gif hier einfügen

Zusätzlich haben wir noch das superkritische Setup mit dem neuen Solver simuliert, 
hier ist die Visualisierung: 

.. Hybrid Solver superkritisch hier einfügen




