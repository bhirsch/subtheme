<?php
/**
 * @file subtheme.admin.inc
 *  Customize subtheme form functions. 
 */

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
 * Implementation of hook_form_submit().
 * 
 * Save new $subtheme. Re-write CSS.
 * 
 * Form being submited is from path: admin/build/subtheme/customize.
 * 
 * Process form then save $subtheme object like this: 
 *  $subtheme->selectors = array(
 *    'selector_name_1' => (object) stdClass
 *    'selector_name_2' => (object) stdClass
 *    'selector_name_N' => (object) -> css_properties 
 *  );
 */
function subtheme_form_submit(&$form, &$form_state) {
  $name = arg(4);
  $subtheme = subtheme_get($name);

  $values = $form_state['values'];
  foreach ($values as $selector_name => $css_properties) {
    $subtheme->selectors[$selector_name]->css_properties = $css_properties;
  }

  subtheme_save($subtheme);
  ctools_include('subtheme', 'subtheme');
  subtheme_css($subtheme);
}
/**
 * Wrapper function for subtheme_subtheme_save($subtheme).
 * 
 * Original function is a Ctools hook: 
 * mymodule_myobj_save($obj). This is just shorter and sweeter.
 */
function subtheme_save($subtheme) {
  subtheme_subtheme_save($subtheme);  
}
