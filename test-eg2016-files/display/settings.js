/**
 * Created by neilthompson on 13/03/2016.
 */
$(document).ready(function () {
  function init() {
    if (localStorage[App_settings.default_settings.set + ":" + "dwellTimeSeconds"]) {
      $('#dwellTimeSeconds').val(localStorage[App_settings.default_settings.set + ':' + "dwellTimeSeconds"]);
    }
    if (localStorage[App_settings.default_settings.set + ":" + "dwellReactivateSeconds"]) {
      $('#dwellReactivateSeconds').val(localStorage[App_settings.default_settings.set + ":" + "dwellReactivateSeconds"]);
    }
    if (localStorage[App_settings.default_settings.set + ":" + "accessMethod"]) {
      $('input:radio[name="accessMethod"][value="' + localStorage[App_settings.default_settings.set + ":" + "accessMethod"] + '"]').prop('checked', true);
    }
  }
  init();
});

$('.stored').change(function () {
  localStorage[App_settings.default_settings.set + ":" + $(this).attr('name')] = $(this).val();
});

$('.stored').keyup(function () {
  localStorage[App_settings.default_settings.set + ":" + $(this).attr('name')] = $(this).val();
});

$('#localStorageSettings').submit(function() {

  $('#dwellTimeSeconds').val(App_settings.default_settings.dwellTimeSeconds);
  localStorage[App_settings.default_settings.set + ':' + 'dwellTimeSeconds'] = App_settings.default_settings.dwellTimeSeconds;

  $('#dwellReactivateSeconds').val(App_settings.default_settings.dwellReactivateSeconds);
  localStorage[App_settings.default_settings.set + ":" + 'dwellReactivateSeconds'] = App_settings.default_settings.dwellReactivateSeconds;

  $('input:radio[name="accessMethod"][value="' + App_settings.default_settings.accessMethod + '"]').prop('checked', true);
  localStorage[App_settings.default_settings.set + ":" + 'accessMethod'] = App_settings.default_settings.accessMethod;

});