<?php
/**
 * @file subtheme.activate.inc
 *  Activate subtheme form functions. 
 */
/**
 * Callback for admin/build/subtheme/activate.
 */
function subtheme_activation_form() {
  $subthemes = subtheme_get_subthemes();

  $options = _subtheme_options($subthemes);
  $options['disabled'] = t('(No active subtheme)');

  $default = subtheme_get_active_subtheme();
  if (!$default) {
    $default = 'disabled';  
  }

  $form['activate'] = array(
    '#type' => 'radios',
    '#title' => t('Activate a subtheme by making a selection here'),
    '#options' => $options,
    '#default_value' => $default,
  );  
  $form['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Save'),
  );

  return $form;
}
/**
 * Implementation of hook_FORM_ID_submit().
 *
 * Activate selected subtheme.
 */
function subtheme_activation_form_submit(&$form, &$form_state) {
  $subtheme_name = $form_state['values']['activate'];
  subtheme_activate($subtheme_name);
}
/**
 * @param $subthemes
 *  array of subtheme objects
 */
function _subtheme_options($subthemes) {
  $options = array();
  foreach ($subthemes as $subtheme) {
    $options[$subtheme->name] = $subtheme->title;
  }
  asort($options);

  return $options;
}
