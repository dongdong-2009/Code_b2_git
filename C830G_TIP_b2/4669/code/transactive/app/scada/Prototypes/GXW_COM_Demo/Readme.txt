This prototype demonstrates how (using VC++) it is possible to build COM objects that can be used by GraphWorX.
These objects can be used as a 2 way interface: they can accept data from graphworx (through method calls) or push data into graphworx by firing COM events.

This prototype is composed of 2 parts:
* RippleUtil: this is a Visual C++ project which, when compiled, creates and registers a small COM component. The component can be used in any application that supports hosting of COM objects (including graphworx).
The component is a timer. Its ProgID is RippleUtil.RippleTimer
It has the following three methods:
- SetPeriod: sets the timer period (in milliseconds)
- Start: starts the timer. The timer will fire a Tick event at whatever interval was defined in SetPeriod
- Stop: stops the timer.
The source code is mostly self explanatory.

* Display1.gdf : This is a graphworx display that exercises the RippleTimer. It displays a rectangle. The timer is used to periodically change the color of the rectangle. The source code is self explanatory.
