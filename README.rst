nvidia-gpu-monitoring
=====================

Synopsis
--------

This repository contains an example of programmatic monitoring
of Nvidia GPUs in C++ using NVML library.

Refer to `Monitoring Nvidia GPUs using API`_ article for details.


Deliverables
------------

Core deliverables of this project are contained in separate
directories and are the following:

``monitor``
  Collects metrics from GPU devices.

  Console application, written in C++17.

``data_extractor``
  Extracts metrics data from monitor's output.

  Console application, written in Python 3.

``device_data_filter``
  Filters metrics of a specific device from full data set obtained
  via ``data_extractor``.

  Console application, written in Python 3.

``data_visualizer``
  Plots metrics of a specific GPU device.

  Console application, written in Python 3.


Building
--------

This section briefly describes steps needed to be taken to get each
deliverable ready for execution.

``monitor``
~~~~~~~~~~~

The ``monitor`` component is written in C++17 with build instructions
defined in ``cmake``.

Hence, one needs a ``cmake`` and a C++ frontend supporting ``std:c++17``
to build the monitor.

Having those both installed, the monitor build steps are the following:

.. code-block:: bash

   mkdir build
   cd build
   cmake ..


``data_extractor``
~~~~~~~~~~~~~~~~~~

The ``data_extractor`` component uses Python 3.7+.

Has no build steps.


``device_data_filter``
~~~~~~~~~~~~~~~~~~~~~~

The ``device_data_filter`` component uses Python 3.7+.

Its dependencies are listed at:

.. code-block::

   device_data_filter/requirements.txt

Dependencies installation example:

.. code-block:: bash

   pip install -r device_data_filter/requirements.txt


``data_visualizer``
~~~~~~~~~~~~~~~~~~~

The ``data_visualizer`` component uses Python 3.7+.

Its dependencies are listed at:

.. code-block::

   data_visualizer/requirements.txt

Dependencies installation example:

.. code-block:: bash

   pip install -r data_visualizer/requirements.txt


Usage
-----

Current section contains brief usage description for components
provided by this repository.


Prerequisites
~~~~~~~~~~~~~

NVML shared library must be present in the system and
its directory must be listed in system's ``PATH`` variable
in order to run the monitor.

Usually, it comes installed with graphics driver on Windows
and has to be installed additionally on Linux-based systems.


``monitor``
~~~~~~~~~~~

The ``monitor`` does not take any parameters.

Basic usage:

.. code-block:: bash

   ./monitor

Watching monitor and persisting logs on Windows:

.. code-block:: bash

   ./monitor | Tee-Object -FilePath "monitor.log"

Watching monitor and persisting logs on Linux-base systems:

.. code-block:: bash

   ./monitor | tee "monitor.log"


``data_extractor``
~~~~~~~~~~~~~~~~~~

Executable of the ``data_extractor`` component is located at:

.. code-block::

   ./data_extractor/run.py

Its usage doc is listed below:

.. code-block::

   usage: run.py [-h] [monitor_log] [extracted_data]

   Extract data from monitor log

   positional arguments:
     monitor_log     path to monitor log file (default: -)
     extracted_data  path to extracted data file (default: -)

   optional arguments:
     -h, --help      show this help message and exit


As stated in the doc, the component accepts 2 positional arguments.

Both of them are optional and default to std streams.

Example of reading from stdin and writing to stdout:

.. code-block::

  cat /path/to/captured/monitor.log | ./data_extractor/run.py


Same with specifying std streams explicitly:

.. code-block::

  cat /path/to/captured/monitor.log | ./data_extractor/run.py - -


Example of using files:

.. code-block::

  ./data_extractor/run.py /path/to/captured/monitor.log /path/to/output/data_all.csv


Or same using output redirection:

.. code-block::

  ./data_extractor/run.py /path/to/captured/monitor.log > /path/to/output/data_all.csv


Example of reading from a file and writing to stdout:

.. code-block::

  ./data_extractor/run.py /path/to/captured/monitor.log


``device_data_filter``
~~~~~~~~~~~~~~~~~~~~~~

Executable of the ``device_data_filter`` component is located at:

.. code-block::

   ./device_data_filter/run.py


Its usage doc is listed below:

.. code-block::

   usage: run.py [-h] [device_index] [input_data] [output_data]

   Filter monitoring data for a specific device

   positional arguments:
     device_index  index of device to filter data by (default: 0)
     input_data    path to monitor log file (default: -)
     output_data   path to extracted data file (default: -)

   optional arguments:
     -h, --help    show this help message and exit


As stated in the doc, the component accepts 3 optional positional
arguments.

``device_index`` defaults to ``0`` and the rest defaults to std streams,
just as in case of ``data_extractor``.

Example of filtering data of device with index ``0`` using files:

.. code-block::

  ./device_data_filter/run.py 0 /path/to/output/data_all.csv /path/to/output/data_0.csv


``data_visualizer``
~~~~~~~~~~~~~~~~~~~

Executable of the ``data_visualizer`` component is located at:

.. code-block::

   ./data_visualizer/run.py


Its usage doc is listed below:

.. code-block::

   usage: run.py [-h] [output_file_path_format] [input_data]

   Visualize device data

   positional arguments:
     output_file_path_format
                           format of output file paths (default:
                           monitor.{suffix}.png)
     input_data            path to device data file (default: -)

   optional arguments:
     -h, --help            show this help message and exit


As stated in the doc, the component accepts 2 optional positional
arguments:

- path format for output files, where ``{suffix}`` will be replaced
  with a suffix like ``full``, ``0``, ``1`` and so on
  (see ``docs/monitor.*.png`` for output examples).
- path to data filtered by ``device_data_filter`` component.

Usage plotting data for device ``0``:

.. code-block::

  ./data_visualizer/run.py '/path/to/output/monitor.{suffix}.csv' /path/to/output/data_0.csv


..

  .. _Monitoring Nvidia GPUs using API: https://medium.com/@oblalex/monitoring-nvidia-gpus-cd174bf89311
