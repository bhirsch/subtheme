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

  $options = _subtheme_activate_options($subthemes);
  $options['disabled'] = t('No active subtheme');

  $default = subtheme_get_active_subtheme();
  if (!$default) {
    $default = 'disabled';  
  }

  $form['activate'] = array(
    '#type' => 'radios',
    '#title' => t('Activate a subtheme by making a selection here'),
    '#options' => $options,
    '#default_value' => $default,
    '#prefix' => '<div class="subtheme subtheme-admin-activate-radios">', 
    '#suffix' => '</div>',
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
function _subtheme_activate_options($subthemes) {
  $options = array();
  foreach ($subthemes as $subtheme) {
    $options[$subtheme->name] = t(check_plain($subtheme->title)) .' ('. t(check_plain($subtheme->description)) .')';
    //$options[$subtheme->name] = theme('subtheme_activate_options', $subtheme);
  }
  asort($options);

  return $options;
}

function theme_subtheme_activate_options($subtheme) {

  $output = '';

  $output .= '<class="subtheme subtheme-activate-option">';

    $output .= '<div class="subtheme subtheme-activate-title">';
      $output .= check_plain($subtheme->title);
    $output .= '</div>';

    $output .= '<div class="subtheme subtheme-activate-description">';
      $output .= check_plain($subtheme->description);
    $output .= '</div>';

  $output .= '</div>';

  return $output;  
}
