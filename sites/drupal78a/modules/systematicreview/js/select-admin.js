
/**
 * @file
 * Enhancements for select list configuration options.
 */

(function ($) {

Drupal.behaviors.systematicreviewSelectLoadOptions = {};
Drupal.behaviors.systematicreviewSelectLoadOptions.attach = function(context) {
  settings = Drupal.settings;

  $('#edit-extra-options-source', context).change(function() {
    var url = settings.systematicreview.selectOptionsUrl + '/' + this.value;
    $.ajax({
      url: url,
      success: Drupal.systematicreview.selectOptionsLoad,
      dataType: 'json'
    });
  });
}

Drupal.systematicreview = Drupal.systematicreview || {};

Drupal.systematicreview.selectOptionsOriginal = false;
Drupal.systematicreview.selectOptionsLoad = function(result) {
  if (Drupal.optionsElement) {
    if (result.options) {
      // Save the current select options the first time a new list is chosen.
      if (Drupal.systematicreview.selectOptionsOriginal === false) {
        Drupal.systematicreview.selectOptionsOriginal = $(Drupal.optionElements[result.elementId].manualOptionsElement).val();
      }
      $(Drupal.optionElements[result.elementId].manualOptionsElement).val(result.options);
      Drupal.optionElements[result.elementId].disable();
      Drupal.optionElements[result.elementId].updateWidgetElements();
    }
    else {
      Drupal.optionElements[result.elementId].enable();
      if (Drupal.systematicreview.selectOptionsOriginal) {
        $(Drupal.optionElements[result.elementId].manualOptionsElement).val(Drupal.systematicreview.selectOptionsOriginal);
        Drupal.optionElements[result.elementId].updateWidgetElements();
        Drupal.systematicreview.selectOptionsOriginal = false;
      }
    }
  }
  else {
    if (result.options) {
      $('#' + result.elementId).val(result.options).attr('readonly', 'readonly');
    }
    else {
      $('#' + result.elementId).attr('readonly', '');
    }
  }
}

})(jQuery);
