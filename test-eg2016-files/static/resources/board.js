/**
 * Created by neilthompson on 12/02/2016.
 */

/**
 *
 * Cells contain icons and text;
 * On activation of a cell - play a beep or say the word;
 * And/or go to another page;
 * Optionally have a speaking area - and words go up there:
 * speak this by looking at it (or at an icon to the right of it)
 * Rest cell - nothing happens.
 *
 * Audio formats:
 * - provide WAV option as mp3 not always available due to patenting;
 * - provide mp3 option
 * See: https://developer.mozilla.org/en-US/docs/Web/HTML/Supported_media_formats
 * Chrome has ? for all audio formats in mobile
 * Vorbis in ogg should also give us opera mobile
 * On desktop wav covers everything except IE (9) - but that will do mp3
 * See: http://hpr.dogphilosophy.net/test/ to test out browsers
 *
 * the following styles should be in the style attribute on the page:
 * - border colour
 * - border colour - active - is in css - could be change in opacity? lightness?
 *
 * Check: view-source:http://hpr.dogphilosophy.net/test/ to see if we can get
 * this to play on iPad, tablets...
 *
 * For click - tablets - works best on android and FireFox, not so good on iOS.
 *
 *
 * Click on button works for mobile browsers
 *
 * TODO: semantic colours - check colourful semantics
 *
 */

var App = {

  settings: {
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
  },

  // click-and-gaze is to solve a problem that might not exist! Mobile browsers
  // are set up to respond to clicks - and not other "mouse" interactions -
  // this lets timing start with a click and then continue as long as gaze is
  // maintained in a cell - if "gaze" does not work on an eye gaze device
  // connected to a tablet, then you could try the click-and-gaze setting - and
  // have the interaction settings for the eye gaze device set to mouse click
  // with minimum dwell time that you can possibly set. You can set a higher
  // dwell time here. It doesn't work for mobile though anyway.

  dweller: {
    savedTime: 0,
    lastPlayedTime: 0 // The last time a cell was played.
  },

  // not implemented yet:
  initBoard: function () {
    board = new Board(this.settings);
  },


  dwellAction: function (context) {
    var canvas = $('#dwell-timer-canvas')[0];
    var ctx = canvas.getContext("2d");
    var offset = $('.board-cell-content', context).offset();
    var x = offset.left + $('.board-cell-content', context).width() / 2;
    var y = offset.top + $('.board-cell-content', context).height() / 2;

    var targetTimePassed = App.settings.dwellTimeSeconds * 1000 / 360;
    var currentTime = new Date().getTime();
    if (!App.dweller.savedTime) {
      App.angle = App.angle + 1;
    }
    else {
      // Keeps the timer icon moving to schedule even if there are execution
      // delays:
      App.angle = App.angle + 1 * (currentTime - App.dweller.savedTime) / targetTimePassed;
    }
    App.dweller.savedTime = currentTime;

    var endangle = 2 * Math.PI * App.angle / 360;
    var radius = App.settings.dwellTimerRadius;

    ctx.beginPath();
    ctx.arc(x, y, radius, 0, endangle, false);
    ctx.lineTo(x, y);
    ctx.fillStyle = App.settings.dwellTimerColor;
    ctx.globalAlpha = App.settings.dwellTimerOpacity;
    ctx.fill();
    ctx.globalAlpha = 1;

    // Action when dwell time reached:
    if (endangle > Math.PI * 2) {
      App.activatedCellAction(context);
      App.dwellTimerStop();
    }
  },

  activatedCellAction: function (context) {

    // Ensure no re-activation before/whilst audio is playing:
    App.dweller.lastPlayedTime = new Date().getTime() + 100000;

    var audio = $('audio', context).get(0);
    if (audio !== undefined) {
      audio.load();
      App.playWhenReady(audio);
    }

    $('audio').on('ended', function() {
      App.dweller.lastPlayedTime = new Date().getTime();
      // $(this).parents().removeClass('activated');
    });

    // Need to play sound *before* jumping to page (audio can't load
    // automatically on a new page in mobile browsers).
    if ($(context).attr('data-jump-to-page') !== undefined) {
      window.location.href = $(context).attr('data-jump-to-page');
    }
  },

  dwellShapeClear: function (context) {
    var canvas = $('#dwell-timer-canvas')[0];
    var ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.beginPath();
  },

  dwellTimerStart: function (context) {
    if (App.timerId) {
      return;
    }
    var interval = App.settings.dwellTimeSeconds * 1000 / 360;
    App.timerId = setInterval(App.dwellAction, interval, context);
  },

  dwellTimerStop: function (context) {
    clearInterval(App.timerId);
    App.dwellShapeClear(context);
    App.timerId = null;
    App.angle = 0;
    App.dweller.savedTime = 0;
  },

  sizeBoardElements: function () {

    // In case need this to stop cells stacking - fixing the cellheight/width as
    // indicated below does the trick though.
    var offsetX =  0;
    var offsetY =  0;

    var board_width = $(window).width() - offsetX;
    var board_height = $(window).height() - offsetY;

    // this is the way to resize the canvas without it scaling its contents:
    var canvas = document.getElementById("dwell-timer-canvas");
    canvas.width = board_width;
    canvas.height = board_height;

    // Have 2 * App.settings.cellMargin - seem to need this extra to stop cells
    // stacking on browser resize.
    var cell_height = (board_height / App.settings.cellsDown) - (2 * (App.settings.cellBorderWidth + App.settings.cellMargin)) - 2 * App.settings.cellMargin;
    var cell_width = (board_width / App.settings.cellsAcross) - (2 * (App.settings.cellBorderWidth + App.settings.cellMargin)) - 2 * App.settings.cellMargin;

    $('.board-cell, .blank-board-cell').height(cell_height);
    $('.board-cell, .blank-board-cell').width(cell_width);

    var display_text_div_height = $('div.board-cell-content .display-text').height();
    $('div.board-cell-content .display-text').css('font-size', cell_height / 8);

  },

  playWhenReady: function (audioElement) {
    //wait for media element to be ready, then play
    var audioReady = audioElement.readyState;

    if (audioReady > 2) {
      audioElement.play();
    }
    else if (audioElement.error) { //For testing only!
      //var errorText = ['(no error)', 'User interrupted download', 'Network error caused interruption', 'Miscellaneous problem with media data', 'Cannot actually decode this media'];
      //alert("Something went wrong!\n" + errorText[audioElement.error.code]);
    }
    else { //check for media ready again in 100mS:
      setTimeout(App.playWhenReady, 100, audioElement);
    }
  }
};



$(document).ready(function () {

  // Update with locally stored settings:

  if (localStorage[App.settings.set + ':' + 'dwellTimeSeconds'] !== undefined) {
    App.settings.dwellTimeSeconds = localStorage[App.settings.set + ':' + 'dwellTimeSeconds'];
  }
  if (localStorage[App.settings.set + ':' + 'dwellReactivateSeconds'] !== undefined) {
    App.settings.dwellReactivateSeconds = localStorage[App.settings.set + ':' + 'dwellReactivateSeconds'];
  }
  if (localStorage[App.settings.set + ':' + 'accessMethod'] !== undefined) {
    App.settings.accessMethod = localStorage[App.settings.set + ':' + 'accessMethod'];
  }


  App.timerId = null;
  App.angle = 0;

  var title_audio = $('audio.title-audio').get(0);
  if (title_audio !== undefined) {
    //title_audio.play();
    title_audio.load();
    App.playWhenReady(title_audio);
  }

  App.sizeBoardElements();

  if (App.settings.accessMethod === 'gaze') {
    $(".board-cell").mouseenter(function () {
      $(this).addClass('activated');
      App.dwellTimerStart(this);
    });

    $(".board-cell").mousemove(function () {
      var currentTime = new Date().getTime();
      if (currentTime > App.dweller.lastPlayedTime + (App.settings.dwellReactivateSeconds * 1000)) {
        $(this).addClass('activated');
        App.dwellTimerStart(this);
      }
    });

    $(".board-cell").mouseleave(function () {
      $(this).removeClass('activated');
      App.dwellTimerStop(this);
    });
  }
  else if (App.settings.accessMethod === 'click') {
    $(".board-cell").click(function () {
      $(this).addClass('activated');
      App.activatedCellAction(this);
    });
    $('audio').on('ended', function() {
      $('.board-cell').removeClass('activated');
    });
  }
  else if (App.settings.accessMethod === 'click-and-gaze') {
    $(".board-cell").click(function () {
      $('activated').removeClass('activated');
      $(this).addClass('activated');
      App.dwellTimerStart(this);
    });
    $(".board-cell").mouseleave(function () {
      $(this).removeClass('activated');
      App.dwellTimerStop(this);
    });
  }
});

$(window).resize(function () {
  App.sizeBoardElements();
});
