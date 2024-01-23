# wx2fs (wx2pfpx extension)
Simplest script to inject weather data in Simconnect(v.10.0.61259.0 or FSX_XPACK) compatible simulator from the output of wx2pfpx.

## How to use :
- The executable must be in the same folder as the output folder from wx2pfpx, it will take the selected dataset on the wx2pfpx app.
- Run the script after the simulator is started but before loading a flight. The window must be kept open (or minimized) to maintain the weather active in the sim.
- Closing the script might make your sim crash, also some lag is expected at loading.
- When using full grid, set the config to include also the airports data and the grid to a resolution of 1. 

## How it works :
- Data used is what is provided from wx2pfpx (metars and wind aloft) and directly injected as is into the sim after a bit of cleaning, low cloud realism is to be expected.
- Weather stations allow the sim to interpolate the data and generate the global weather, some change on a station can impact the others. Also, a station that is too much in contrast with neighbours stations will tend to align its value.
- When the script sets a weather station in the sim, it has to provide a metar AND the wind alofts, what is not provided is reset to zero. This means that setting the metar only will delete altitude winds and providing only altitude winds (additionnal stations in wx2pfpx), will set ground variables (which are also the only ones generating clouds) to zero and should be ideally far from airports. That's why it's important to change the settings to add the wind data also to the airports with metars on the full grid mode.
- There is a trade off between having a lot of stations for a realistic wind, and fewer stations to avoid impacting the data provided by the metars and emptying the weather visually.
