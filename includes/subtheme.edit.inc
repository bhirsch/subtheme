<?php
/**
 * @file subtheme.edit.inc
 *  Edit subtheme form functions.
 */

/**
 * Callback for admin/build/subtheme/edit/%
 */
function subtheme_form($form, &$form_state) {
  $path = drupal_get_path('module', 'subtheme') . '/subtheme.css';
  drupal_add_css($path);

  $name = arg(4);
  $subtheme = subtheme_get($name);
  $default_selectors = mtheme_get_selectors();

  $form['stylesheet'] = array(
    '#type' => 'item',
    '#title' => 'Style Sheet',
    '#value' => l("$name.css", file_directory_path() . "/subtheme/$name/$name.css"),
  );

  // Site logo
  $form['logo'] = array(
    '#type' => 'fieldset',
    '#title' => t('Site-wide: Logo/Banner'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  // Prompt user to upload own graphic for main background here
  $form['logo']['logo_upload'] = array(
    '#type' => 'file',
    '#title' => t('Logo/Banner'),
    '#size' => 30,
// TODO move this file handling to validate function.
//    '#element_validate' => array('subtheme_save_logo'),
  );
  // Path to logo
  if ($logo_path = subtheme_get_logo($subtheme)) {
    global $base_url;
    $logo = $base_url . '/' . $logo_path;
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
    $title = ($selector->title) ? '/*** ' . t(check_plain($selector->title)) . ' ***/<br/>' . check_plain($selector->css_selector) : check_plain($selector->css_selector);
    $form[$group][$key] = array(
      '#type' => 'textarea',
      '#title' => $title,
      '#default_value' => $selector->css_properties,
      '#description' => t('CSS selector: ') . $selector->css_selector . '<br />' . t('default: ') . $default_selectors[$selector->name]->css_properties,
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
  $form['files']['file_upload'] = array(
    '#type' => 'file',
    '#title' => t('Files'),
    '#size' => 30,
  );
  // Check for available files
  $name = arg(4);
  $path = subtheme_images_path($subtheme); //Path to files dir or features module w files.
  $i = 0; // Use counter in case of conflicting $file->name values
  foreach (@file_scan_directory($path, '/.*/') as $file) {
    $title = "images/$file->filename";
    $link = l($title, $file->uri);
    // Add form items
    $item = $file->filename;
    $form['files'][$item] = array(
      '#type' => 'item',
      '#title' => $link,
      '#description' => t("Add this file to your CSS like this: "
                         . "background: url('" . $title . "');"),
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
 * Implements hook_FORM_ID_validate().
 *
 * Prepare uploaded files to be saved.
 *
 * TODO Move processing out of submit function and put here too.
 */
function subtheme_form_validate(&$form, &$form_state) {
  // Handle file uploads.
  // This is the validator used for uploading a theme logo.
  $validators = array('file_validate_is_image' => array());
  // These validators are used for uploading favicons. 
  // @todo Would these validators be more appropriate here?
  //$validators = array('file_validate_extensions' => array('ico png gif jpg jpeg apng svg'));

  // Check for a new uploaded files.
  $uploads = array('logo_upload', 'file_upload');
  foreach ($uploads as $upload) {
    $file = file_save_upload($upload, $validators);
    if (isset($file)) {
      // File upload was attempted.
      if ($file) {
        // Put the temporary file in form_values so we can save it on submit.
        $form_state['values'][$upload] = $file;
      }
      else {
        // File upload failed.
        form_set_error('file_upload', t('The file could not be uploaded.'));
        $form_state['values'][$upload] = FALSE; // TODO does this solve the undefined index error on submit? 
      }
    }
  }
}

/**
 * Implements hook_FORM_ID_submit().
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
  }
  else {
    $op = array();
  }

  $values = $form_state['values'];
  foreach ($values as $selector_name => $css_properties) {
    // Check for selector prefix.
    // Check prefix to keep misc other $form['values'] out of $subtheme->selectors.
    if (substr($selector_name, 0, 9) == 'selector_') {
      $selector_name = substr($selector_name, 9);
      $subtheme->selectors[$selector_name]->css_properties = $css_properties;
    }
  }

  // Add custom css.
  $custom_css = $form_state['values']['custom_css'];
  $subtheme->custom_css = $custom_css;

  subtheme_save($subtheme, $op);

  // Handle new file uploads. Do this after subtheme_save() in case
  // a new file is being uploaded to override a default file.
  // Otherwise subtheme_save() can move files over from a feature module
  // when a subtheme is being newly overridden
  // and blow away the newly uploaded files.
  //
  // If the user uploaded a new file, save it to a permanent location.
  $uploads = array();
  if (isset($values['file_upload'])) {
    $uploads['file_upload'] = $values['file_upload'];
    unset($values['file_upload']);
  }
  if (isset($values['logo_upload'])) {
    $uploads['logo_upload'] = $values['logo_upload'];
    unset($values['logo_upload']);
  }
  foreach ($uploads as $file) {
    $source = $file->uri;
    $destination = array();
    // Save file to subtheme-specific directory here:
    // files/subtheme/mysubtheme/images/imageX.jpg
    $destination = subtheme_images_path($subtheme, 'files', 'uri');
    if (!file_prepare_directory($destination, FILE_CREATE_DIRECTORY)) {
      $txt = t("!destination could not be created.", 
             array('!destination' => $destination));
      drupal_set_message($txt);
    }
    if ($filename = file_unmanaged_copy($source, $destination, FILE_EXISTS_REPLACE)) {
      $file_path = subtheme_images_path($subtheme, 'files', 'relative') . '/' . $file->filename;
      $txt = t("New file saved: ") . l($file_path, $file_path);
      drupal_set_message($txt);
    }
  }
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
  foreach (@file_scan_directory($path, '/.*/') as $file) {
    // Add form items
    if ($file->name == 'logo') {
      if (!$result = file_delete($file->filename)) {
        $txt = t('Old logo could not be removed: ') . $file->filename;
        drupal_set_message($txt);
      }
    }
  }
}

/**
 * Save new logo.
 *
 * TODO Handle this on validate and submit. See mtheme.
 */
function subtheme_save_logo($form, &$form_state) {
  // Get parent (the form element that called this function)
  $parent = $form['#parents'][0];
  // Check for a new uploaded file, and use that if available.
  if ($file = file_save_upload($parent)) {
    $subtheme = arg(4);
    subtheme_remove_logo($subtheme);
    // Get file extension.
// TODO handle logo naming/handling.
    $parts = pathinfo($file->filename);
    $extension = $parts['extension'];
    // Check for file subtheme file directories. Create if needed.
    $directory = subtheme_images_path($subtheme);
    file_prepare_directory($directory, FILE_CREATE_DIRECTORY);
    // Name the file.
    $file_path = $directory . '/logo.' . $extension;
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
