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

/**
 * Callback for admin/build/subtheme/customize/%
 */
function subtheme_form() {
  $name = arg(4);
  $subtheme = subtheme_get($name);
//dsm($subtheme);
  $default_selectors = mtheme_get_selectors();

  // Organize by groups
  $groups = $subtheme->grps;
  asort($groups);
  $group_fieldset = array();
  foreach ($groups as $selector_name => $group) {

    // Create new fieldset
    if (!$group_fieldset[$group]) { 
      $group_fieldset[$group] = TRUE;
      $group_title = $group;
      $group = strtolower(preg_replace('/[^a-zA-Z0-9-]+/', '-', $group_title));
      $form[$group] = array(
        '#type' => 'fieldset',
        '#title' => t($group_title),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

      // Add selectors
      $selector = $subtheme->selectors[$selector_name];
      $form[$group][$selector->name] = array(
        '#type' => 'textarea',
        '#title' => t($selector->title),
        '#default_value' => $selector->css_properties,
        '#description' => t('CSS selector: ') . $selector->css_selector .'<br />'. t('default: ') . $default_selectors[$selector->name]->css_properties, 
      );
    }
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 *  $subtheme->selectors = array(
 *    'selector_name_1' => (object) stdClass
 *    'selector_name_2' => (object) stdClass
 *    'selector_name_N' => (object) -> css_properties 
 *  );
 */
function subtheme_form_submit(&$form, &$form_state) {
  dsm($form, 'form'); 
  dsm($form_state, 'form_state'); 

  // CONTINUE HERE
  
}
