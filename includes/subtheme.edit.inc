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

  // Site logo
  $form['logo'] = array(
    '#type' => 'fieldset',
    '#title' => t('Site-wide: Logo/Banner'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  // Prompt user to upload own graphic for main background here
  $form['logo']['logo'] = array(
    '#type' => 'file',
    '#title' => t('Logo/Banner'),
    '#size' => 30,
    '#element_validate' => array('subtheme_save_logo'),
  );
  // Path to logo
  if ($logo_path = subtheme_get_logo($subtheme)) {
    global $base_url;
    $logo = $base_url .'/'. $logo_path;
    // Display background image.
    $form['logo']['logo-display'] = array(
      '#type' => 'item',
      '#title' => "<img src='$logo' />",
    );
  }

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
    $prefix = 'selector_'; // Use this to keep garbage out of $subtheme->selectors when processing data in subtheme_form_submit(). 
    $key = $prefix . $selector->name;
    $form[$group][$key] = array(
      '#type' => 'textarea',
      '#title' => '/*** ' . check_plain($selector->title) .' ***/<br/>'. t(check_plain($selector->css_selector)),
      '#default_value' => $selector->css_properties,
      '#description' => t('CSS selector: ') . $selector->css_selector .'<br />'. t('default: ') . $default_selectors[$selector->name]->css_properties, 
    );
  }
  // Custom CSS
  $form['custom'] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom CSS'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['custom']['custom_css'] = array(
    '#type' => 'textarea',
    '#title' => t('Add your own custom CSS'),
    '#default_value' => $subtheme->custom_css,
    '#description' => t('Anything added here will be appended to the bottom of the last stylesheet loaded by the site.'), 
  );
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
 * Process input in subtheme form, construct a new $subtheme object and save it. 
 * (During subtheme_save(), CSS for the subtheme will be re-written).
 * 
 * Form being submited is from path: admin/build/subtheme/edit/%
 * 
 * Process form then save $subtheme object like this: 
 *  $subtheme->selectors = array(
 *    'selector_name_1' => (object) stdClass
 *    'selector_name_2' => (object) stdClass
 *    'selector_name_N' => (object) -> css_properties 
 *  );
 *  $subtheme->custom_css = '.example1 { property: value; } '
 *                         .'.example2 { property: value; } ';
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
  // Check for selector prefix.
    if (substr($selector_name, 0, 9) == 'selector_') { // Check prefix to keep misc other $form['values'] out of $subtheme->selectors.
      $selector_name = substr($selector_name, 9);
      $subtheme->selectors[$selector_name]->css_properties = $css_properties;
    }
  }

  // Add custom css.
  $custom_css = $form_state['values']['custom_css'];
  $subtheme->custom_css = $custom_css;

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

/**
 * "logo" is a special file name. There can only
 * be one file named logo in any subtheme at any time.
 * This lets us store different logos with different subthemes,
 * while deactivated subthemes' logos are not in use.
 *
 * @param $subtheme
 *  string, subtheme name
 */
function subtheme_remove_logo($subtheme) {
  $path = subtheme_images_path($subtheme); //Path to files dir or features module w files.
  foreach (@file_scan_directory($path, '.*') as $file) {
    // Add form items
    if ($file->name == 'logo') {
      if (!$result = file_delete($file->filename)) {
        $txt = t('Old logo could not be removed: '). $file->filename;
        drupal_set_message($txt); 
      }
    }
  }
}

/**
 * Save new logo.
 */
function subtheme_save_logo($form, &$form_state) {
  // Get parent (the form element that called this function)
  $parent = $form['#parents'][0];
  // Check for a new uploaded file, and use that if available.
  if ($file = file_save_upload($parent)) {
    $subtheme = arg(4);
    subtheme_remove_logo($subtheme);
    // Get file extension.
    $parts = pathinfo($file->filename);
    $extension = $parts['extension'];
    // Check for file subtheme file directories. Create if needed.
    $directory = subtheme_images_path($subtheme);
    file_check_directory($directory, FILE_CREATE_DIRECTORY);
    // Name the file.
    $file_path = $directory .'/logo.'. $extension;
    // Save new subtheme logo. 
    if (file_copy($file, $file_path, FILE_EXISTS_REPLACE)) {
      $txt = t('Your new logo has been saved.'); 
      drupal_set_message($txt);
      // If subtheme is active, update site logo.
      if ($subtheme == subtheme_get_active_subtheme()) { 
        $settings = variable_get('theme_settings', NULL);
        $settings['logo_path'] = $file_path;
        variable_set('theme_settings', $settings);
      }
    }
  }

  return $form;
}
