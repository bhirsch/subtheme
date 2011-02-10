<?php
/**
 * @file subtheme.edit.inc
 *  Edit subtheme form functions. 
 */

/**
 * Callback for admin/build/subtheme/edit/%
 */
function subtheme_form() {
  $name = arg(4);
  $subtheme = subtheme_get($name);
  $default_selectors = mtheme_get_selectors();

  $form['stylesheet'] = array(
    '#type' => 'item',
    '#title' => 'Style Sheet',
    '#value' => l("$name.css", file_directory_path() ."/subtheme/$name/$name.css"),
  );

  // Organize by groups
  $groups = $subtheme->grps;
  asort($groups);
  $group_fieldset = array();
  foreach ($groups as $selector_name => $group) {
    $group_title = $group;
    $group = strtolower(preg_replace('/[^a-zA-Z0-9-]+/', '-', $group_title));

    // Create new fieldset
    if (!$group_fieldset[$group]) { 
      $group_fieldset[$group] = TRUE;
      $form[$group] = array(
        '#type' => 'fieldset',
        '#title' => t($group_title),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
    }

    // Add selectors
    $selector = $subtheme->selectors[$selector_name];
    $form[$group][$selector->name] = array(
      '#type' => 'textarea',
      '#title' => '/*** ' . check_plain($selector->title) .' ***/<br/>'. t(check_plain($selector->css_selector)),
      '#default_value' => $selector->css_properties,
      '#description' => t('CSS selector: ') . $selector->css_selector .'<br />'. t('default: ') . $default_selectors[$selector->name]->css_properties, 
    );
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
  $path = subtheme_images_path($subtheme); //Path to files dir or features module w files.
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
  // Keep track of whether the subtheme was in code only when form 
  // was built.
  $form['in_code_only'] = array(
    '#type' => 'hidden',
    '#value' => $subtheme->in_code_only,
  );
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
 * Form being submited is from path: admin/build/subtheme/edit/%
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
  if ($form_state['values']['in_code_only']) {
    $op = array('in code only'); 
  } else {
    $op = array();  
  }

  $values = $form_state['values'];
  foreach ($values as $selector_name => $css_properties) {
    $subtheme->selectors[$selector_name]->css_properties = $css_properties;
  }

  subtheme_save($subtheme, $op); 
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
