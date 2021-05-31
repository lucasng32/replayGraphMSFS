# Sky Dolly
Sky Dolly - The black sheep for your flight recordings.

## About
Sky Dolly connects to a running Flight Simulator 2020 instance and records various simulation variables, for replay.

![Sky Dolly v0.5.0](./img/SkyDolly-v0.6.0.png)

The connection is made via SimConnect, a standard API introduced with Flight Simulator X to connect with the flight simulator.

## Features
* Record / Pause / Play
* Settable recording sample rate: fixed or variable ("auto")
* Adjustable replay speed ("slow motion", "timelapse")
* Position, velocity and basic flight control simulation variables (ailerons, rudder, flaps, gears, ...) are recorded
* Basic CSV import & export
* Database (SQLite) based persistence ("logbook")

Watch the (preview) release trailer on [YouTube](https://www.youtube.com/watch?v=_n4qRtm78_I):

[![Sky Dolly - Flight Recorder & Replay for Flight Simulator 2020 (Cinematic Teaser Trailer)](https://img.youtube.com/vi/_n4qRtm78_I/0.jpg)](https://www.youtube.com/watch?v=_n4qRtm78_I "Sky Dolly - Flight Recorder & Replay for Flight Simulator 2020 (Cinematic Teaser Trailer)")

For more videos created with Sky Dolly also refer to [SHOWCASE](SHOWCASE.md). New and upcoming features are found in the [CHANGELOG](CHANGELOG.md).

## Basic Usage

* Start a flight
* Click on "Record" (key R)
* You may pause recording (key P)
* Stop recording (key R) and click on "Play" (spacebar)
* Adjust the replay speed and seek the play position, also during replay
* Recorded flights are automatically persisted in the "logbook"
* Select a flight in the logbook and press "Load" (or double-click the row) to load the persisted flight

Why don't you try it with one of the guided ["Tell Tours"](https://github.com/till213/Tell-Tours)? :)

## Download

Go to [Releases](https://github.com/till213/SkyDolly/releases) and download the latest release.

Latest (pre-)release: **Sky Dolly 0.6.2**

## Build

Refer to the [BUILD](BUILD.md) documentation.

## Related Projects

* https://github.com/nguyenquyhy/Flight-Recorder - A .Net based flight recording & replay application
* https://github.com/saltysimulations/saltyreplay/ - A recording & replay with a web user interface
* https://github.com/SAHorowitz/MSFS2020-PilotPathRecorder - Records the flight and generates Google Earth (KML format) "replays"

## Other Useful Resources

* https://obsproject.com/ - Screen capture / broadcast application
* https://sqlitebrowser.org/ - Browse the Sky Dolly logbook (*.sdlog) database files, execute your own SQL queries
