
/**
 * JavaScript behaviors for the front-end display of systematicreviews.
 */

(function ($) {

Drupal.behaviors.systematicreview = Drupal.behaviors.systematicreview || {};

Drupal.behaviors.systematicreview.attach = function(context) {
  // Calendar datepicker behavior.
  Drupal.systematicreview.datepicker(context);
};

Drupal.systematicreview = Drupal.systematicreview || {};

Drupal.systematicreview.datepicker = function(context) {
  $('div.systematicreview-datepicker').each(function() {
    var $systematicreviewDatepicker = $(this);
    var $calendar = $systematicreviewDatepicker.find('input.systematicreview-calendar');
    var startDate = $calendar[0].className.replace(/.*systematicreview-calendar-start-(\d{4}-\d{2}-\d{2}).*/, '$1').split('-');
    var endDate = $calendar[0].className.replace(/.*systematicreview-calendar-end-(\d{4}-\d{2}-\d{2}).*/, '$1').split('-');
    var firstDay = $calendar[0].className.replace(/.*systematicreview-calendar-day-(\d).*/, '$1');
console.log(startDate);
    // Convert date strings into actual Date objects.
    startDate = new Date(startDate[0], startDate[1] - 1, startDate[2]);
    endDate = new Date(endDate[0], endDate[1] - 1, endDate[2]);

    // Ensure that start comes before end for datepicker.
    if (startDate > endDate) {
      var laterDate = startDate;
      startDate = endDate;
      endDate = laterDate;
    }

    var startYear = startDate.getFullYear();
    var endYear = endDate.getFullYear();

    console.log(startDate);
    console.log(endDate);
    // Set up the jQuery datepicker element.
    $calendar.datepicker({
      dateFormat: 'yy-mm-dd',
      yearRange: startYear + ':' + endYear,
      firstDay: parseInt(firstDay),
      minDate: startDate,
      maxDate: endDate,
      onSelect: function(dateText, inst) {
        var date = dateText.split('-');
        $systematicreviewDatepicker.find('select.year, input.year').val(+date[0]);
        $systematicreviewDatepicker.find('select.month').val(+date[1]);
        $systematicreviewDatepicker.find('select.day').val(+date[2]);
      },
      beforeShow: function(input, inst) {
        // Get the select list values.
        var year = $systematicreviewDatepicker.find('select.year, input.year').val();
        var month = $systematicreviewDatepicker.find('select.month').val();
        var day = $systematicreviewDatepicker.find('select.day').val();

        // If empty, default to the current year/month/day in the popup.
        var today = new Date();
        year = year ? year : today.getFullYear();
        month = month ? month : today.getMonth() + 1;
        day = day ? day : today.getDate();

        // Make sure that the default year fits in the available options.
        year = (year < startYear || year > endYear) ? startYear : year;

        // jQuery UI Datepicker will read the input field and base its date off
        // of that, even though in our case the input field is a button.
        $(input).val(year + '-' + month + '-' + day);
      }
    });

    // Prevent the calendar button from submitting the form.
    $calendar.click(function(event) {
      $(this).focus();
      event.preventDefault();
    });
  });
}

})(jQuery);