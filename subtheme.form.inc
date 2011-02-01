<?php
/**
 * @file subtheme.form.inc
 *  All things related to admin/subtheme/%/formN
 */

/**
 * Callback for admin/build/subtheme.
 */
function subtheme_selection_form() {
  $form['subtheme'] = array(
    '#type' => 'radios',
    '#title' => t('Select a Subtheme'),
    '#options' => _subtheme_options(1),
  );  
  $form['save'] = array(
    '#type' => 'button', 
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * 
 */
function _subtheme_options($number) {
  $subthemes = subtheme_get_subthemes();
  $options = array();
  $txt = t('Edit'); 
  foreach ($subthemes as $subtheme) {
    $name = $subtheme->name;
    $path = 'admin/subtheme/'. $name .'/form'. $number;
    $link = l($txt, $path);
    $options[$name] = $subtheme->title .', '. $link;
  }

  return $options;
}

/**
 * Callback for admin/build/subtheme/%/form1.
 */
function subtheme_form() {
  $name = arg(2); // subtheme name
  $form['test'] = array(
    '#type' => 'item',
    '#title' => 'testing 1 2 3',
  );  
  dsm("it works! subtheme name: $name");

  return $form;
}
