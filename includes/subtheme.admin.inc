<?php
/**
 * @file subtheme.admin.inc
 *  All custom (non Ctools) stuff related to admin/build/subtheme/...
 */

/**
 * Callback for admin/build/subtheme/activate.
 */
function subtheme_activation_form() {
  $subthemes = subtheme_get_subthemes();
  $form['subtheme'] = array(
    '#type' => 'radios',
    '#title' => t('Activate a subtheme by making a selection here'),
    '#options' => _subtheme_options(1, $subthemes),
    '#default_value' => '',// TODO
  );  
  $form['save'] = array(
    '#type' => 'button', 
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * @param $subthemes
 *  array of subtheme objects
 * 
 * @param $number
 *  integer, form number (e.g. form1, form2, for path)
 */
function _subtheme_options($number, $subthemes) {
  $options = array();
  foreach ($subthemes as $subtheme) {
    $options[$subtheme->name] = $subtheme->title;
  }

  return $options;
}

/**
 * Callback for admin/build/subtheme/customize/%
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

/**
 * Callback for admin/build/subtheme/customize
 * 
 * Links to subtheme edit forms.
 */
function subtheme_customize_page() {
  $subthemes = subtheme_get_subthemes();
  $output = theme('subtheme_customize_links', $subthemes);

  return $output;
}

function theme_subtheme_customize_links($subthemes) {
  $output = '';
  foreach ($subthemes as $subtheme) {
    $path = 'admin/build/subtheme/customize/'. $subtheme->name;
    $link =  l(t($subtheme->title), $path);  
    $output .= '<div class="subtheme-customize-link subtheme">'. $link .'</div>';
  }
    
  return $output;
}
