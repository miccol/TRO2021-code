# Simulated Experiments for the paper: Handing Concurrency in Behavior Trees

In this repository, you will find the code related to the examples and experiments of the paper: <br>
"Handling Concurrency in Behavior Trees" <br>
Submitted at Transactions on Robotics by Michele Colledanchise and Lorenzo Natale.

## Dependencies

[BehaviorTree.CPP](https://github.com/BehaviorTree/BehaviorTree.CPP)

[Qt5](https://doc.qt.io/qt-5/gettingstarted.html)
(You may need to install Qt5Charts separately by running: `sudo apt install libqt5charts5-dev)


[Groot](https://github.com/BehaviorTree/Groot) (optional)



## Setup

Open a terminal and clone the repository

```bash
$ cd /path/to/folder
$ git clone https://github.com/miccol/tro2020-code.git
```

Once you have the repository, compile the library:

### Gnu/Linux or MacOS
In Unix-based operating system, open a terminal and run the following commands:
```bash
$ cd /path/to/folder/
$ mkdir ./build
$ cd build
$ cmake ..
$ make
```

### Windows
In Windows, run [Cmake](https://cmake.org/).
Open the visual studio solution file (`.sln`)

### Check your build

In the build folder, run:

```bash
$./simple_demos
```

In the terminal, you should see the following message:





 then should see information about the execution as the following:



At the end of the execution, a window should popup as the following:



### Run the examples and the experiments:

To run the examples of the experiments of the paper, just run the corresponding binary file.

E.g to run Experiment1 run:



```bash
$ ./experiment1
```



### Run the example with Groot

Open Groot in monitor mode, then run:

 

```bash
$ ./simple_demo_with_groot
```



In Groot, you should see the following BT running:



### Use the library

the file `simple_demos.cpp` shows an example of how to use the library, with an example



## LICENSE

The MIT License (MIT)

Copyright (c) 2020 Michele Colledanchise

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
