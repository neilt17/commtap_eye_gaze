/**
 * Created by neilthompson on 14/03/2016.
 */

// TODO: load this separately, use this to reset settings.
// load this for the settings page and board...
// need to have separate settings for each page set
App.settings = {
    dwellTimeSeconds: 1,
    dwellReactivateSeconds: 2, // Delay time, before moving within cell will cause re-activation
    dwellTimerRadius: 40,
    dwellTimerColor: '#ff0000',
    dwellTimerOpacity: 0.01,
    cellsAcross: 3,
    cellsDown: 2,
    cellBorderWidth: 8,
    cellMargin: 6,
    accessMethod: "gaze" // "gaze", "click", "click-and-gaze"
  };
