# Flight Statistics Tool for MSFS

This is a fork of [SkyDolly](SkyDolly.md), an external, stand-alone application that provides flight recording and replay functionality in Microsoft Flight Simulator (MSFS).

## Completed Features

1. General flight statistics graphs in custom panel UI
 - Altitude
 - Rate of climb
 - G-force
 - AoA -> compare to stall conditions
 - Airspeed -> IAS, TAS, GS
 - Primary flight controls
2. Replay pause and play functionality
3. Replay timeline scrubbing 

## To Do

1. Flight information
 - Control input smoothness -> variance / rate of change 
 - Some intuitive way to show pitch, roll, yaw, flight controls
 - Terrain map and flight path page (using leaftlet)

2. Full in-sim control of recording and replay
 - Replace server module with plugin (similar to MSFSSimConnectPlugin)
 - Add logbook module functionality in server plugin 

3. Stage-specific statistics
 - Takeoff
 - Landing / climbing (landing missions)

4. Waypoint based missions
 - Make use of MSFS missions system
 - Judge deviation from flight path
 - Integrated with flight maneuvers 

## UI Layout

1. Tabs
 - Altitude -> altitude and vertical speed
 - Speed -> true and indicated airspeed, ground speed
 - Flight Dyanmics -> AOA graph, 2d cross section of airplane with bank angle, vertical and horizontal speed
 - Control -> control surfaces graphs