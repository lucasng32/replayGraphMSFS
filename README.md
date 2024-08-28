# Flight Statistics Tool for MSFS

This is a fork of [SkyDolly](SkyDolly.md), an external, stand-alone application that provides flight recording and replay functionality in Microsoft Flight Simulator (MSFS).

## Completed Features

1. General flight statistics graphs in custom panel UI
 - Altitude
 - Rate of climb
 - G-force
2. Replay pause and play functionality
3. Replay timeline scrubbing 

## To Do

1. Add more general flight statistics (UI, simvar problem)
 - AoA -> compare to stall conditions 
 - Airspeed -> IAS, TAS, GS
 - Control input smoothness -> variance / rate of change 
 - Environment: wind speed and direction
 - Some intuitive way to show pitch, roll, yaw etc in flight model 

2. Full in-sim control of recording and replay
 - Replace server module with plugin (similar to MSFSSimConnectPlugin)
 - Add logbook module functionality in server plugin 

## Future work

1. Stage-specific statistics
 - Takeoff
 - Landing / climbing (easiest)
 - Turning / other flight maneuvers 
 - Need to change data structure to support flight stages
 - User to input stage of flight 

2. Waypoint based missions
 - Judge deviation from flight path
 - Integrated with flight maneuvers 