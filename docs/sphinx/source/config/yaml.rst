Yaml
====

args datei
----------

config
******

.. code-block:: yaml

   args:
        solverName: Solver Name # kann durch flag printSolvers ausgegeben werden
        setupName: Setup name # kann durch flag printSetups ausgegeben werden
        cellSize: Size of Cell in m
        cellx: Numbers of Cells in x direction
        celly: Numbers of Cells in y direction
        endTime: Maximum simulated time in s
        timeSteps: Maximum number of iterations

    setup:
        # setup spezifisch, kann durch flag printSetup=Setup Name ausgegeben werden
    
    tweaks:
        useEntropyFix: true
        manningFactor: 0.02
    
    output:
        formatName: Outputformat # kann durch flag printFormats ausgegeben werden
        outputInterval: Intervall of iterations per output write
        outputResolution: # output auflösung 
        stations: path to yaml station file
        compressionLevel: 0-3
        checkpointInterval: Intervall of iterations per output sync
    
    omp:
        omp_num_threads: 8
        omp_schedeule: static,8

beispiel
********
.. code-block:: yaml

    args:
        solverName: hybrid
        setupName: tsunamiEvent2d
        cellSize: 250
        cellx: 10800
        celly: 6000
        endTime: 0
        timeSteps: 50

    setup:
        displacement: "large_data/tohoku_gebco20_ucsb3_250m_displ.nc"
        bathymetry: "large_data/tohoku_gebco20_ucsb3_250m_bath.nc"
        startCoordX: -200000
        startCoordY: -750000

        tweaks:
        useEntropyFix: true
        manningFactor: 0.02

    output:
        formatName: nc
        outputInterval: 10
        outputResolution: 1
        compressionLevel: 0
        checkpointInterval: 100
        stations: ""

    omp:
        omp_num_threads: 8
        omp_schedule: static,8


stations datei
--------------

config
******
.. code-block:: yaml

    stations:
    - name: name of station
        locX: x pos of station
        locY: y pos of station
    - ...

beispiel
********

.. code-block:: yaml

    stations:
    - name: "station1_2d"
        locX: 30
        locY: 50
    - name: "station2_2d"
        locX: 10
        locY: 50
    - name: "station3_2d"
        locX: 60
        locY: 50