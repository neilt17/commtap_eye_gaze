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
 *
 */

var App = {

  settings: {
    soundOn: true,
    dwellTimeSeconds: 2,
    dwellTimerRadius: 40,
    dwellTimerColor: '#ff0000',
    dwellTimerOpacity: 0.01
  },

  dweller: {
    savedTime: 0
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

    var targetTimePassed = App.settings.dwellTimeSeconds * 1000/360;
    var currentTime = new Date().getTime();
    if (!App.dweller.savedTime) {
      App.angle = App.angle + 1;
    }
    else {
      // Keeps the timer icon moving to schedule even if there are execution
      // delays:
      App.angle = App.angle + 1 * (currentTime - App.dweller.savedTime)/targetTimePassed;
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
      var audio = $('audio', context).get(0);
      if (audio !== undefined) {
        $('audio', context).get(0).play();
      }
      App.dwellTimerStop();

      if ($(context).attr('data-jump-to-page') !== undefined) {
        window.location.href = $(context).attr('data-jump-to-page');
      };

    }
  },

  dwellShapeClear: function(context) {
    var canvas = $('#dwell-timer-canvas')[0];
    var ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.beginPath();
  },

  dwellTimerStart: function (context) {
    if (App.timerId) {
      return;
    }
    var interval = App.settings.dwellTimeSeconds * 1000/360;
    App.timerId = setInterval(App.dwellAction, interval, context);
  },

  dwellTimerStop: function (context) {
    clearInterval(App.timerId);
    App.dwellShapeClear(context);
    App.timerId = null;
    App.angle = 0;
    App.dweller.savedTime = 0;
  },

  sizeCanvas: function() {
    var board_width = $(window).width();
    var board_height = $(window).height();
    // this is the way to resize the canvas without it scaling its contents:
    var canvas = document.getElementById("dwell-timer-canvas");
    canvas.width = board_width;
    canvas.height = board_height;
  }
};

App.timerId = null;
App.angle = 0;

$(document).ready(function () {

  var title_audio = $('audio.title-audio').get(0);
  if (title_audio !== undefined) {
    title_audio.play();
  }

  App.sizeCanvas();

  $(".board-cell").mouseenter(function () {
    $(this).addClass('activated');
    App.dwellTimerStart(this);
  });

  $(".board-cell").mouseleave(function () {
    $(this).removeClass('activated');
    App.dwellTimerStop(this);
  });

});

$(window).resize(function() {
  App.sizeCanvas();
});
