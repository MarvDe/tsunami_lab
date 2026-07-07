Woche 8
=======

In dieser Woche ging es um Optimierung und Analyse des Programmes. 
Als erstes soll der Code auf dem Draco Cluster der Universität Jena 
compiliert und ausgeführt werden. Im Anschluss wird eine Analyse durch 
Compiler-Optionen und das Tool Vtune durchgeführt.

Beiträge der Gruppenmitglieder
------------------------------

Gemeinsam
*********

Zunächst haben wir den Code auf den Draco Cluster geschoben und 
compiliert. Für die Analyse wurde eine Benchmark Konfiguration
erstellt. Mit dieser werden wir die Tests für Vergleiche von
Compiler-Optionen (-O3, ...), Compilern (GCC, clang) und Systemen 
(Draco, Lokal) durchführen. Das Scons-Skript wurde von uns so angepasst, 
dass der, in der ``CXX`` Umgebunsvariable befindliche Compiler ausgewählt wird.
Wenn also ``CXX=clang++ scons`` ausgeführt wird, dann verwendet das Build-Skript 
den clang Compiler.


Marvin Döring
*************

Um einen Performanzvergleiche durchzuführen verwenden wir die 
``std::chrono::steady_clock::now()`` Funktion um Zeit zu messen.
Dabei wird nur die Zeit der Berechnung gemessen und nicht das Setup 
oder die I/O-Operationen. Für die folgenden Vergleiche haben wir die selbe 
Benchmark Konfiguration verwendet. Der Release Build hat die ``-O2`` Flag aktiviert.
Bei dem Optimization Build werden die Flags ``-O3``, ``-ffast-math`` und ``march=native`` verwendet.
Die normalisierte Zeit ist die gesamte Laufzeit geteilt durch die Anzahl an 
Zellen multipliziert mit der Anzahl an Zeitschritten.

==================================   ==============      ======================
Konfiguration                        Gesamtzeit (s)      normalisierte Zeit (s)
==================================   ==============      ======================
Draco & Release Build & gcc          20.0219             3.19328e-08
Draco & Optimization Build & gcc     15.1645             2.41858e-08
Draco & Release Build  & clang       22.3432             3.5635e-08
Draco & Optimization Build & clang   17.6711             2.81836e-08
Local & Release Build & gcc          17.7471             2.83048e-08
Local & Optimization Build & gcc     13.1028             2.08976e-08
==================================   ==============      ======================

Es lässt sich also sagen, dass die Optimierungsflags durchaus einen Unterschied machen.
Des Weiteren scheint der gcc Compiler einwenig effizienteren Code zu produzieren als 
der clang Compiler. 
Bei dem Setzten von Compiler Flags muss man darauf achten, dass durch mache dieser Flags 
das Programm andere bzw. inkorrekte Ausgaben liefern kann. Die ``-ffast-math`` Flag 
optimiert Float Berechnungen, dabei wird allerdings der IEEE Standard verletzt und 
deshalb können sich die Ausgaben des Programms ändern.

Philipp Prell
*************

Um das Programm weiter zu optimieren haben wir uns zusätzliche Informationen von dem
Compiler ausgeben lassen und das Tool Vtune verwendet um kritische Stellen im
Programm zu finden. Besonders in der ``WavePropagation2d`` Klasse konnten wir 
viele kleine Optimierungen vornehmen.
Wir verwenden nun die Funktionen ``std::copy_n``, ``std::fill_n`` und ``std::transform``, dies hilft dem 
Compiler die Operationen auf Arrays zu vektorisieren. Des Weiteren wird das Keyword ``__restrict__``
an gegebenen Stellen verwendet, somit muss der Compiler sicht nicht um Aliasing von Pointern kümmern und 
der Code wird schneller. Dann haben wir eine ``if``-Bedingung aus einer doppelten For-Schleife entfernt, dadurch 
muss diese nicht unnötig ausgewertet werden.  
Der unoptimierte Code führt die Benchmark-Konfiguration in 23.0412 Sekunden aus. Mit den Optimierungen kommt das 
Programm unter gleichen Compliereinstellungen immerhin auf 18.2675 Sekunden. Somit lässt sich sagen, 
dass die Optimierungen durchaus einen Unterschied machen.  



