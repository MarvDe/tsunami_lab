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


OpenMP-Parallelisierung und NUMA-Optimierung
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Zur Beschleunigung der Simulation wurde die Klasse ``WavePropagation2d`` mit OpenMP parallelisiert. Die Parallelisierung erfolgt auf Ebene der Zeilen des zweidimensionalen Gitters. Jeder Thread bearbeitet einen zusammenhängenden Block von Zeilen, wodurch die Arbeitslast gleichmäßig verteilt wird und die Speicherlokalität erhalten bleibt.

Innerhalb der Methode ``timeStep`` wird zunächst ein OpenMP-Parallelbereich erzeugt:

.. code-block:: cpp

   #pragma omp parallel
   {
       const int tid = omp_get_thread_num();
       const int num_threads = omp_get_num_threads();

       const t_idx rows_per_thread =
           (m_yCells + 2 + num_threads - 1) / num_threads;

       const t_idx row_start = tid * rows_per_thread;
       const t_idx row_end =
           std::min<t_idx>(row_start + rows_per_thread, m_yCells + 2);

       ...
   }

Jeder Thread berechnet dabei seinen eigenen Bereich des Gitters und kopiert
zunächst die alten Zustände in die neuen Speicherbereiche. Anschließend werden
die X- und Y-Sweeps unabhängig voneinander durchgeführt. Zwischen den einzelnen
Phasen werden Synchronisationspunkte mittels

.. code-block:: cpp

   #pragma omp barrier

verwendet, um sicherzustellen, dass alle Threads ihre Berechnungen abgeschlossen
haben, bevor die nächste Phase beginnt.

Zusätzlich wurde für die inneren Schleifen SIMD-Vektorisierung aktiviert:

.. code-block:: cpp

   #pragma omp simd

Dadurch kann der Compiler mehrere Zellupdates gleichzeitig mit Vektor-
Instruktionen ausführen und die Rechenleistung moderner CPUs besser ausnutzen.

NUMA-Optimierung
________________

Das Zielsystem besitzt eine NUMA-Architektur (*Non-Uniform Memory Access*).
Bei NUMA-Systemen ist der Hauptspeicher auf mehrere NUMA-Knoten verteilt,
die jeweils bestimmten CPU-Sockeln oder Kerngruppen zugeordnet sind.
Der Zugriff auf lokalen Speicher ist deutlich schneller als auf Speicher,
der einem anderen NUMA-Knoten zugeordnet ist.

Um die Speicherlokalität zu verbessern, wurde das sogenannte
*First-Touch-Prinzip* verwendet. Die Arrays werden zunächst mit
``std::aligned_alloc`` allokiert und anschließend parallel initialisiert:

.. code-block:: cpp

   #pragma omp parallel for schedule(static)
   for (t_idx i = 0; i < n; i++) {
       m_h[0][i] = 0;
       m_h[1][i] = 0;

       m_hu[0][i] = 0;
       m_hu[1][i] = 0;

       m_hv[0][i] = 0;
       m_hv[1][i] = 0;

       m_bathymetry[i] = 0;
   }

Da unter Linux die physische Speicherzuordnung beim ersten Zugriff erfolgt,
werden die Speicherseiten auf dem NUMA-Knoten angelegt, auf dem der jeweilige
Thread läuft. Da die Berechnung später dieselbe Datenaufteilung verwendet,
arbeiten die Threads überwiegend auf lokalem Speicher. Dadurch werden
langsame Speicherzugriffe über NUMA-Grenzen hinweg reduziert.

Verwendete OpenMP-Konfiguration
_______________________________

Die Simulation wurde mit den folgenden OpenMP-Umgebungsvariablen ausgeführt:

.. code-block:: bash

   export OMP_NUM_THREADS=48
   export OMP_PLACES=cores
   export OMP_PROC_BIND=close

``OMP_NUM_THREADS=48``


Diese Variable legt fest, dass OpenMP insgesamt 48 Threads erzeugt.
Die Anzahl wurde an die verfügbare Hardware angepasst, sodass alle
verfügbaren Rechenkerne auf dem draco Cluster genutzt werden können.

``OMP_PLACES=cores``


Mit dieser Einstellung werden Threads an einzelne CPU-Kerne gebunden.
Dadurch wird verhindert, dass das Betriebssystem Threads während der
Ausführung zwischen verschiedenen Kernen verschiebt. Dies verbessert
die Cache-Lokalität und reduziert den Verwaltungsaufwand des Schedulers.


Nvidia Grace
~~~~~~~~~~~~

Leider konnten wir bislang keinen Zeitpunkt finden, an dem das NVIDIA-Grace-System
vollständig frei verfügbar war. Aus diesem Grund wurden die Experimente auf dem
Draco-Cluster mit 48 CPU-Kernen durchgeführt.

Für die Messungen wurde das folgende Szenario verwendet:

.. code-block:: yaml

   args:
     - solverName: "fwave"
       formatName: NONE
       setupName: "tsunamiEvent2d"
       startCoordX: -200000.
       startCoordY: -750000.
       cellSize: 250
       cellx: 10800
       celly: 6000
       timeSteps: 10
       stations: ""

       bathymetry:
         - startCoordX: -200000.
           startCoordY: -750000.
           cellSize: 250
           filePath: "large_data/tohoku_gebco20_ucsb3_250m_bath.nc"

       displacement:
         - startCoordX: -200000.
           startCoordY: -750000.
           cellSize: 250
           filePath: "large_data/tohoku_gebco20_ucsb3_250m_displ.nc"

Untersucht wurde insbesondere der Einfluss verschiedener OpenMP-Bindungsstrategien
auf die Laufzeit. Die Ergebnisse sind in der folgenden Tabelle zusammengefasst:

+----------------------+----------------+--------------------+
| OMP_NUM_THREADS      | OMP_PLACES     | OMP_PROC_BIND      |
+======================+================+====================+
| 48                   | cores          | close              |
+----------------------+----------------+--------------------+
| 48                   | cores          | spread             |
+----------------------+----------------+--------------------+
| 48                   | threads        | spread             |
+----------------------+----------------+--------------------+
| 48                   | sockets        | spread             |
+----------------------+----------------+--------------------+
| 48                   | sockets        | close              |
+----------------------+----------------+--------------------+

Für alle oben genannten Konfigurationen ergab sich eine nahezu identische
Laufzeit von ungefähr **4,9 Sekunden**.

Zum Vergleich wurde die Anwendung auch sequentiell ausgeführt:

.. code-block:: bash

   export OMP_NUM_THREADS=1
   export OMP_PLACES=cores
   export OMP_PROC_BIND=close

Hier betrug die Laufzeit etwa **157,3 Sekunden**. Die parallelisierte
Implementierung erreicht somit einen Speedup von ungefähr

.. math::

   \frac{157.3\,\mathrm{s}}{4.9\,\mathrm{s}} \approx 32.

Dies entspricht einer Beschleunigung um den Faktor 32 gegenüber der
sequentiellen Ausführung.

Die Parallelisierung basiert auf einer selbst implementierten statischen
Arbeitsaufteilung, bei der jedem Thread ein zusammenhängender Bereich von
Zeilen des Gitters zugewiesen wird. Dieses Vorgehen wurde gewählt, um die
Speicherlokalität auf NUMA-Systemen zu verbessern. Zusammen mit dem
First-Touch-Prinzip bei der Speicherinitialisierung wird sichergestellt,
dass Threads überwiegend auf Speicher zugreifen, der auf ihrem lokalen
NUMA-Knoten liegt.

Eine mögliche Erweiterung wäre die Verwendung eines dynamischen OpenMP-
Schedulers, beispielsweise

.. code-block:: cpp

   #pragma omp for schedule(dynamic, 64)

Dadurch könnten Lastungleichgewichte reduziert werden, die insbesondere durch
große Bereiche trockener Zellen entstehen. Threads, die ihre Arbeit früher
abschließen, könnten zusätzliche Aufgaben übernehmen, anstatt auf langsamere
Threads zu warten.

Allerdings würde eine solche dynamische Verteilung die NUMA-Lokalität
verschlechtern, da Threads dann häufiger auf Daten zugreifen müssten, die auf
einem anderen NUMA-Knoten liegen. Ob die bessere Lastverteilung den Verlust an
Speicherlokalität kompensiert, müsste in zukünftigen Experimenten untersucht
werden.


Benchmark
~~~~~~~~~

um zu benchmarken haben wir folgende configuration genutzt:

.. code-block:: yaml

   args:
     - solverName: "fwave"
       formatName: "nc"
       setupName: "tsunamiEvent2d"
       startCoordX: -200000.
       startCoordY: -750000.
       cellSize: 250
       cellx: 10800
       celly: 6000
       timeSteps: 10000
       writeFreq: 100
       stations: ""

       bathymetry:
         - startCoordX: -200000.
           startCoordY: -750000.
           cellSize: 250
           filePath: "large_data/tohoku_gebco20_ucsb3_250m_bath.nc"

       displacement:
         - startCoordX: -200000.
           startCoordY: -750000.
           cellSize: 250
           filePath: "large_data/tohoku_gebco20_ucsb3_250m_displ.nc"

den benchmark haben wir nicht auf der nvidia grace, sondern auf dem draco cluser 
mit ``OMP_NUM_THREADS=48`` ``OMP_PLACES=cores`` ``OMP_PROC_BIND=close`` ausgeführt.

Ergebnis:

