/**
 * Created by neilthompson on 13/03/2016.
 */
$(document).ready(function () {
  function init() {
    if (localStorage["dwellTimeSeconds"]) {
      $('#dwellTimeSeconds').val(localStorage["dwellTimeSeconds"]);
    }
    if (localStorage["dwellReactivateSeconds"]) {
      $('#dwellReactivateSeconds').val(localStorage["dwellReactivateSeconds"]);
    }
    if (localStorage["accessMethod"]) {
      $('input:radio[name="accessMethod"][value="' + localStorage["accessMethod"] + '"]').prop('checked', true);
    }
  }
  init();
});

$('.stored').change(function () {
  localStorage[$(this).attr('name')] = $(this).val();
});

$('#localStorageTest').submit(function() {
  localStorage.clear();
});