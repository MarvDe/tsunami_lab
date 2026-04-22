
Woche 2
=======

In der zweiten Woche des Tsunami Labs soll der F-Wave-Solver integriert werden.
Dabei muss die Korrektheit überprüft werden und neben dem Dammbruch Setup sollen 
noch die Rare-Rare und Shock-Shock Setups implementiert werden. 


Beiträge der Gruppenmitglieder
------------------------------

Gemeinsam
*********

Das Integrieren des F-Wave-Solvers in die ``WavePropagation1d`` Klasse haben wir mit einem 
zusätzlichen Parameter umgesetzt. Die Option ``solverId`` wird dem Konstruktor übergeben. 
Der Wert 0 entspricht dabei dem Roe-Solver und 1 dem F-Wave-Solver.


Marvin Döring
*************

Sanity Check
~~~~~~~~~~~~

Zu den Tests des F-Wave-Solvers wurde ein weiterer Fall hinzugefügt, welcher die ersten
zehn Werte aus der `middle_states.csv`_ überprüft.

.. _middle_states.csv: https://scalable.uni-jena.de/assets/tsunami_lab/middle_states.csv 

Runtime Parsing
~~~~~~~~~~~~~~~

Um das wechseln zwischen verschiedenen Einstellungen wie zum Beispiel die Anzahl der Zellen,
die Art des Solvers oder das Setup zu wählen wurde die Klasse ``Parser`` eingeführt. Runtime
Argumente können somit in der Form ``arg_name=arg_value`` dem Programm übergeben werden.  

Rare-Rare Setup
~~~~~~~~~~~~~~~

Die Klasse ``RareRare1d`` dient als Setup für den Rare-Rare Fall. Dabei werden die Wasserhöhe, 
das Wassermomentum und die Position der Diskontinuität übergeben. Die initialen Werte der Wassersäulen 
werden dann nach der Formel berechnet:

.. math::
    :name: rare

    f(x_{i}) 
        = (x_{i} > x_{dis}) ? q_{l} : q_{r}

Dammbruch Experimente
~~~~~~~~~~~~~~~~~~~~~

**Für das Dammbruch Setup haben wir folgende Tests durchgeführt:**

.. plot::

    import pandas as pd
    import matplotlib.pyplot as plt

    df = pd.read_csv("dh10_h5_solution_1.csv")

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 5))

    ax1.plot(df.iloc[:, 0], df.iloc[:, 2])
    ax1.set_xlabel("X-Position") 
    ax1.set_ylabel("Y-Wasserhöhe")
    ax1.set_title("Höhe links: 10, Höhe rechts: 5, Zeitschritt: 1")

    ax2.plot(df.iloc[:, 0], df.iloc[:, 3])
    ax2.set_xlabel("X-Position") 
    ax2.set_ylabel("Y-Wassermomentum")
   

.. plot::

    import pandas as pd
    import matplotlib.pyplot as plt

    df = pd.read_csv("dh20_h5_solution_1.csv")

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 5))

    ax1.plot(df.iloc[:, 0], df.iloc[:, 2])
    ax1.set_xlabel("X-Position") 
    ax1.set_ylabel("Y-Wasserhöhe")
    ax1.set_title("Höhe links: 20, Höhe rechts: 5, Zeitschritt: 1")

    ax2.plot(df.iloc[:, 0], df.iloc[:, 3])
    ax2.set_xlabel("X-Position") 
    ax2.set_ylabel("Y-Wassermomentum")

Man erkennt im Plot zum Wassermomentum, dass das Momentum deutlich stärker 
ist, wenn die initiale linke Wasserhöhe steigt. 

Github Actions
~~~~~~~~~~~~~~

Wir nutzen Github Actions um beim Pushen automatisch die Unit-Tests auszuführen.


Philipp Prell 
*************

Shock-Shock Setup
~~~~~~~~~~~~~~~~~

Für die Klasse ``ShockShock1d`` wurde die Formel:

.. math::
    :name: shock

    f(x_{i}) 
        = (x_{i} < x_{dis}) ? q_{l} : q_{r}

verwendet. Sie ist der ``RareRare1d`` Klasse in der Funktionsweise sehr ähnlich.

Zusammenhang zwischen Wasserhöhe und Wassermomentum auf die Wellengeschwindigkeiten
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Für das Shock-Shock Setup haben wir folgende Tests durchgeführt:**

.. plot::

    import pandas as pd
    import matplotlib.pyplot as plt

    df = pd.read_csv("h1u4solution_1.csv")

    plt.plot(df.iloc[:, 0], df.iloc[:, 2])
    plt.xlabel("X-Position") 
    plt.ylabel("Y-Wasserhöhe")
    plt.title("Höhe: 1, Momentum: 4, Zeitschritt: 1")

.. plot::

    import pandas as pd
    import matplotlib.pyplot as plt

    df = pd.read_csv("h1u0_8solution_1.csv")

    plt.plot(df.iloc[:, 0], df.iloc[:, 2])
    plt.xlabel("X-Position") 
    plt.ylabel("Y-Wasserhöhe")
    plt.title("Höhe: 1, Momentum: 0.8, Zeitschritt: 1")

.. plot::

    import pandas as pd
    import matplotlib.pyplot as plt

    df = pd.read_csv("h4u0_8solution_1.csv")

    plt.plot(df.iloc[:, 0], df.iloc[:, 2])
    plt.xlabel("X-Position") 
    plt.ylabel("Y-Wasserhöhe")
    plt.title("Höhe: 4, Momentum: 0.8, Zeitschritt: 1")

Man kann also beobachten, dass eine Erhöhung des Momentums dazu führt, dass die Wasserhöhe
deutlich mehr ansteigt.

**Für das Rare-Rare Setup haben wir folgende Tests durchgeführt:**

.. plot::

    import pandas as pd
    import matplotlib.pyplot as plt

    df = pd.read_csv("rh1u2solution_1.csv")

    plt.plot(df.iloc[:, 0], df.iloc[:, 2])
    plt.xlabel("X-Position") 
    plt.ylabel("Y-Wasserhöhe")
    plt.title("Höhe: 1, Momentum: 2")

.. plot::

    import pandas as pd
    import matplotlib.pyplot as plt

    df = pd.read_csv("rh1u0_8solution_1.csv")

    plt.plot(df.iloc[:, 0], df.iloc[:, 2])
    plt.xlabel("X-Position") 
    plt.ylabel("Y-Wasserhöhe")
    plt.title("Höhe: 1, Momentum: 0.8")

.. plot::

    import pandas as pd
    import matplotlib.pyplot as plt

    df = pd.read_csv("rh4u0_8solution_1.csv")

    plt.plot(df.iloc[:, 0], df.iloc[:, 2])
    plt.xlabel("X-Position") 
    plt.ylabel("Y-Wasserhöhe")
    plt.title("Höhe: 4, Momentum: 0.8")

Die Rarefaction-Welle scheint linear zu sein, wenn das Momentum hoch ist.
Im Gegensatz wirk sie bei niedrigerem Mometum eher abgerundet.

**Mathematische Beobachtung der Wellengeschwindigkeiten an der Diskontinuität**

.. math::

  h_r &= h_l \\
  u_r &= -u_l \\
  h^{Roe} &= \frac{h_l + h_r}{2} = h_l \\
  u^{Roe} &= \frac{u_l \sqrt{h_l} + u_r \sqrt{h_r}}{\sqrt{h_l} + \sqrt{h_r}} \\
  &= \frac{u_l \sqrt{h_l}  -u_l \sqrt{h_l}}{\sqrt{h_l} + \sqrt{h_l}} \\
  &= 0 \\
  \lambda^{Roe}_1 &= u^{Roe} - \sqrt{g h^{Roe}} \\
  &= -\sqrt{g h_l} \\
  \lambda^{Roe}_2 &= u^{Roe} + \sqrt{g h^{Roe}} \\
  &= \sqrt{g h_l}

An der Diskontinuität scheint das Momentum mathematisch keinen Einfluss auf die 
Wellengeschwindigkeiten zu haben.

Dorf Evakuierung 
~~~~~~~~~~~~~~~~

Eine Flutwelle aus einem großen Reservoir bewegt sich 25 km stromabwärts auf ein Dorf zu.
Gesucht ist die Zeit bis zum Eintreffen der Welle im Dorf.

Gegeben:

.. math::

  q_l =  \begin{bmatrix}
            14 \\ 0
          \end{bmatrix}, \quad
  q_r =  \begin{bmatrix}
            3.5 \\ 0.7
          \end{bmatrix},  \quad
  s = 25000\text{ m}

Lösung:

.. math::

  h^{Roe}(q_l, q_r) &= \frac{14 + 3.5}{2} \\
  &= 8.75 \\
  u^{Roe}(q_l, q_r) &= \frac{ 0 \sqrt{14} + 0.7 \sqrt{3.5}}{\sqrt{14} + \sqrt{3.5}} \\
  &\approx 0.2333

.. math::

  \lambda^{Roe}_1(q_l,q_r) &= u^{Roe}(q_l,q_r) - \sqrt{g h^{Roe}(q_l, q_r)} \\
  &= 0.2333 - 9.26327 \\
  &= -9.02997

.. math::
  \lambda^{Roe}_2(q_l,q_r) &= u^{Roe}(q_l,q_r) + \sqrt{g h^{Roe}(q_l, q_r)} \\
  &= 0.2333 + 9.26327 \\
  &= 9.49657

.. math::
  \lambda_{max} &= \lambda_2^{Roe} \\
  &= 9.49657 \frac{m}{s}

.. math::
  t &= \frac{25000 \ m}{9.49657 \frac{m}{s}} \\
  &\approx 2633 \ s \\
  &\approx 43.884 \ min  \\
  &\approx 40 \ min \ 53 \ s

Die Dorfbewohner haben etwa 41 Minuten Zeit, um das Gebiet zu verlassen.
