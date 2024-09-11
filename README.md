# Flight Statistics Tool for MSFS

This is a fork of [SkyDolly](SkyDolly.md), an external, stand-alone application that provides flight recording and replay functionality in Microsoft Flight Simulator (MSFS).

## Completed Features

1. General flight statistics graphs in custom panel UI
 - Altitude
 - Rate of climb
 - G-force
 - AoA -> compare to stall conditions
 - Airspeed -> Airspeed, Ground Speed
2. 2D Visualization diagrams
 - AoA, pitch
 - Bank angle, velocity components
3. Replay pause and play functionality
4. Replay timeline scrubbing 

## To Do

1. Full in-sim control of recording and replay
 - Replace server module with plugin (similar to MSFSSimConnectPlugin)
 - Add logbook module functionality in server plugin 

2. Stage-specific statistics
 - Takeoff
 - Landing / climbing (landing missions)

3. Waypoint based missions
 - Make use of MSFS missions system
 - Judge deviation from flight path
 - Integrated with flight maneuvers 

4. Frontend upgrades
 - Rewrite using typescript, react
 - Redux for state management