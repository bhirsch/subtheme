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

  // Include files field group for uploading background images
  // Include enctype, otherwise uploads won't work!
  $form['#attributes'] = array('enctype' => 'multipart/form-data');
  $form['files'] = array(
    '#type' => 'fieldset',
    '#title' => t('Files'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['files']['file'] = array(
    '#type' => 'file',
    '#title' => t('Files'),
    '#size' => 30,
    '#element_validate' => array('subtheme_save_files'),
  );
  // Check for available files
  $name = arg(4);
  $path = file_directory_path() .'/subtheme/'. $name .'/images';  
  $i = 0; // Use counter in case of conflicting $file->name values
  foreach (@file_scan_directory($path, '.*') as $file) {
    $title = "images/$file->basename";
    $link = l($title, $file->filename);
    // Add form items
    $item = $file->basename;
    $form['files'][$item] = array(
      '#type' => 'item',
      '#title' => t("File "). $i,
      '#value' => $link,
      '#description' => t("Add this file to your CSS like this: "
                         ."background: url('". $title ."');"),
    );
    $i++; 
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

/**
 * Save image files.
 * 
 * Callback for #element_validate on 
 * #files element of subtheme_form.
 */
function subtheme_save_files($form, &$form_state) {
  $name = arg(4); 
  // Get parent (the form element that called this function)
  $parent = $form['#parents'][0];
  // Check for a new uploaded file, and use that if available.
  if ($file = file_save_upload($parent)) {
    // Get file extension.
    $parts = pathinfo($file->filename);
    //$extension = $parts['extension'];
    // Check for images file directory. Create if needed.
    $directory = file_directory_path() .'/subtheme/'. $name. '/images';
    file_check_directory($directory, FILE_CREATE_DIRECTORY);
    //Name the file. 
    $file_path = $directory ."/". $file->filename;
    // The image was saved using file_save_upload() and was added to the
    // files table as a temporary file. We'll make a copy and let the garbage
    // collector delete the original upload.
    file_copy($file, $file_path, FILE_EXISTS_REPLACE);
  }
  return $form;
}
