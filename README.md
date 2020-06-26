# CoronaAppDetector for M5StickC

Simple PoC for counter that shows how many people around you have the Covid 19 Exposure Notification running on their phones on the display of an [M5StickC](https://m5stack.com/products/stick-c). 

![Counter working](https://github.com/frankrieger/CoronaAppDetectorM5StickC/blob/master/Working.png)

With the display power saving fix (display off when no changes to display, LED heartbeat to indicate device still working) the run time on the internal battery is dependent on the number of changes in observed number of app beacons (number of times the display switches on). Roughly 45 minutes is typical (the M5stickC has a very tiny battery).

Todo: 
- scan cycle optimization for power saving (currently optimized for quick reaction)
- battery / voltage display 
- selectable "continuous scan" and "push button to wake up and scan"-modes
- some sort of statistical logging / counters


