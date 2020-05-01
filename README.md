# arduino-car-project

The project uses FreeRTOS on an Arduino mega to control a 4WD car using 2 DC motors. The car features are mainly divided into 3 categories:
1. Lane departure warning
* Lane is represented as thick black line in the middle of the road.
* A buzzer immediately produces a warning in case of leaving a lane.
* A red LED immediately turns on once the car leaves the lane
* Lane detection is done using a Line Follower Sensor
2. LCD Display
* General purpose LCD screen to display general warnings and some readings
* Readings include rain detection ,seatbelt status and fuel level
* Ability to control a mirrors using a joystick (represented as 2 servo motors)
3. Keyless Entry
* The car is initially locked (All features are turned off, including the engine)
* Car must be unlocked using an RFID sensor with a unique ID.
* Engine must be started, using a button, to turn on all the features.
* A car may be locked by scanning the RFID after unlocking
* Trying to lock a running car will have no effect until the engine is stopped
