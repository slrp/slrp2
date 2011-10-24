(function ($) {

/**
 * SystematicReview node form interface enhancments.
 */

Drupal.behaviors.systematicreviewAdmin = {};
Drupal.behaviors.systematicreviewAdmin.attach = function(context) {
  // Apply special behaviors to fields with default values.
  Drupal.systematicreview.defaultValues(context);
  // On click or change, make a parent radio button selected.
  Drupal.systematicreview.setActive(context);
  // Update the template select list upon changing a template.
  Drupal.systematicreview.updateTemplate(context);
  // Enhance the normal tableselect.js file to support indentations.
  Drupal.systematicreview.tableSelectIndentation(context);
}

Drupal.systematicreview = Drupal.systematicreview || {};

Drupal.systematicreview.defaultValues = function(context) {
  var $fields = $('.systematicreview-default-value:not(.error)', context);
  var $forms = $fields.parents('form:first');
  $fields.each(function() {
    this.defaultValue = $(this).attr('rel');
    if (this.value != this.defaultValue) {
      $(this).removeClass('systematicreview-default-value');
    }
    $(this).focus(function() {
      if (this.value == this.defaultValue) {
        this.value = '';
        $(this).removeClass('systematicreview-default-value');
      }
    });
    $(this).blur(function() {
      if (this.value == '') {
        $(this).addClass('systematicreview-default-value');
        this.value = this.defaultValue;
      }
    });
  });

  // Clear all the form elements before submission.
  $forms.submit(function() {
    $fields.focus();
  });
};

Drupal.systematicreview.setActive = function(context) {
  var setActive = function(e) {
    $('.form-radio', $(this).parent().parent()).attr('checked', true);
    e.preventDefault();
  };
  $('.systematicreview-set-active', context).click(setActive).change(setActive);
};

Drupal.systematicreview.updateTemplate = function(context) {
  var defaultTemplate = $('#edit-templates-default').val();
  var $templateSelect = $('#systematicreview-template-fieldset select#edit-template-option', context);
  var $templateTextarea = $('#systematicreview-template-fieldset textarea:visible', context);

  var updateTemplateSelect = function() {
    if ($(this).val() == defaultTemplate) {
      $templateSelect.val('default');
    }
    else {
      $templateSelect.val('custom');
    }
  }

  var updateTemplateText = function() {
    if ($(this).val() == 'default' && $templateTextarea.val() != defaultTemplate) {
      if (confirm(Drupal.settings.systematicreview.revertConfirm)) {
        $templateTextarea.val(defaultTemplate);
      }
      else {
        $(this).val('custom');
      }
    }
  }

  $templateTextarea.keyup(updateTemplateSelect);
  $templateSelect.change(updateTemplateText);
}

Drupal.systematicreview.tableSelectIndentation = function(context) {
  var $tables = $('th.select-all', context).parents('table');
  $tables.find('input.form-checkbox').change(function() {
    var $rows = $(this).parents('table:first').find('tr');
    var row = $(this).parents('tr:first').get(0);
    var rowNumber = $rows.index(row);
    var rowTotal = $rows.size();
    var indentLevel = $(row).find('div.indentation').size();
    for (var n = rowNumber + 1; n < rowTotal; n++) {
      if ($rows.eq(n).find('div.indentation').size() <= indentLevel) {
        break;
      }
      $rows.eq(n).find('input.form-checkbox').attr('checked', this.checked);
    }
  });
}

})(jQuery);
