# arduino-car-project

The project uses FreeRTOS on an Arduino mega to control a 4WD car using 2 DC motors. The car features are mainly divided into 3 categories:
I. Lane departure warning
a. Lane is represented as thick black line in the middle of the road.
b. A buzzer immediately produces a warning in case of leaving a lane.
c. A red LED immediately turns on once the car leaves the lane
d. Lane detection is done using a Line Follower Sensor
II. LCD Display
a. General purpose LCD screen to display general warnings and some readings
b. Readings include rain detection ,seatbelt status and fuel level
c. Ability to control a mirrors using a joystick (represented as 2 servo motors)
III. Keyless Entry
a. The car is initially locked (All features are turned off, including the engine)
b. Car must be unlocked using an RFID sensor with a unique ID.
c. Engine must be started, using a button, to turn on all the features.
d. A car may be locked by scanning the RFID after unlocking
e. Trying to lock a running car will have no effect until the engine is stopped
