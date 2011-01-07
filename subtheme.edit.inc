<?php
/**
 * @file subtheme module form.
 */
function subtheme_edit_page() {
  return drupal_get_form('subtheme_edit');  
}

function subtheme_edit(&$form_state) {
  // Flush image cache images, incase user saved new images.
  $file_path = file_directory_path() .'/subtheme';
  imagecache_image_flush($path);

  $form['subtheme_css'] = array(
    '#type' => 'item',
    '#title' => _subtheme_get_css(), 
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  // Logo  
  $form['logo'] = array(
    '#type' => 'fieldset',
    '#title' => t('Website Logo/Banner'),
    '#collapsible' => TRUE, 
    '#collapsed' => FALSE,
  );
  // Prompt user to upload own graphic for main background here
  $form['logo']['logo'] = array(
    '#type' => 'file',
    '#title' => t('Logo/Banner'),
    '#size' => 30, 
    '#element_validate' => array('subtheme_save_logo'),
  );  
  // Path to logo 
  global $base_url;
  $s = variable_get('theme_settings', NULL);
  $logo_path = $s['logo_path']; 
  if ($logo_path) {
    $logo = $base_url .'/'. $logo_path; 
    // Display background image.
    $form['logo']['logo-display'] = array(
      '#type' => 'item',
      '#title' => "<img src='$logo' />",
    );  
    // URL to the background image. (This cannot be modified directly.)
    $link = l("logo/banner image", $logo);
    $form['logo']['logo-url'] = array(
      '#type' => 'item',
      '#title' => $link, 
    );  
  }

   $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 10,
  );
  $form['reset'] = array(
//    '#type' => 'button',
    // TODO This seems to work now. #type=>button doesn't. Why? 
    '#type' => 'submit',
    '#value' => t('Restore to defaults'),
    '#weight' => 10,
    '#submit' => array('subtheme_reset'),
  );

  return $form;
}

function _subtheme_get_css() {
  // Set the location of the custom.css file
  $file_path = file_directory_path() .'/subtheme/custom.css';

  // If the directory doesn't exist, create it
  file_check_directory(dirname($file_path), FILE_CREATE_DIRECTORY);

  // Notify user about new CSS 
  $output = t('Custom CSS: !file',
          array('!file' => l($file_path, $file_path)));

  return $output;

}

/**
 * Reset subtheme to defaults by unsetting all 
 * subtheme variables. Let modules re-set default values
 * next time the form loads with no variables set.
 */
function subtheme_reset($form, $form_state) {
  // Set the location of the custom.css file
  $file_path = file_directory_path() .'/subtheme/custom.css';
  // Delete custom CSS
  file_delete($file_path);
  // If the directory doesn't exist, create it
  $dir = file_check_directory(dirname($file_path), FILE_CREATE_DIRECTORY);
  $file_contents = '/** subtheme custom CSS has been reset to defaults **/';
  $saved = file_save_data($file_contents, $file_path, FILE_EXISTS_REPLACE);
  // Set all subtheme variables in variable table to null
  $results = db_query("SELECT * FROM {variable} WHERE name LIKE 'subtheme%'");
  while ($var = db_fetch_object($results)) {
    variable_set($var->name, NULL);
  }
  // Re-set logo 
  $settings = variable_get('theme_settings', NULL);
  $settings['logo_path'] = '';
  variable_set('theme_settings', $settings);
}

/**
 *  hook_form_submit().
 */
function subtheme_form_submit(&$form, &$form_state) {
  // rebuild files/subtheme/custom.css from scratch.
  $css = '';
  $a = array();
  
  $values = $form_state['values'];
  foreach ($values as $key => $value) {
    if (substr($key, 0, 8) == 'subtheme') {
      // set variable in variables table
      variable_set($key, $value);
      // format $a, array of selectors, properties and values
      // Like this: 
      //   $a[selector][propertyA] = X
      //   $a[selector][propertyB] = Y 
      //   $a[selector1---selectorN][propertyC] = Q 
      $k = subtheme_format_key($key);
      if ($k) {
        $s = $k['s']; // selectors
        $p = $k['p']; // property
        $a[$s][$p] = $value;
      }
    }
  }
  
  // format selectors
  $a = subtheme_format_selectors($a);
  // build css
  $css = subtheme_compose_css($a); 
  // write css
  subtheme_write_css($css);

}

/** 
 * @param
 *   title of the link, for which we are creating an id attribute
 * @return 
 *   E.g. A link titled "Press Release" gets the id "press-release"
 */
// NOTE: This function is copied from template.php.
// Any changes here should be copied there too. 
/*
TODO If subtheme module is going to handle menu items, figure out how to incorporate this:
function subtheme_id($id) {
  $id = check_plain($id);
  $id = trim($id);
  $id = strtolower($id);  $id = str_replace(' ', '-', $id);
  return $id;
}
// */

/**
 * Format by exploding $key to extract selectors and targetted property.
 */
function subtheme_format_key($key) {
  $x = explode('_', $key); 
  // discard $x[0], this is 'subtheme'
  if (!$x[1]) { // this form element is not intended for CSS
    return FALSE;
  }

  $k['s'] = $x[1]; // selector
  $k['p'] = $x[2]; // property

  return $k;
}

/**
 * Format by exploding selectors anc element types.
 */
function subtheme_format_selectors($a) {
  // Rebuild $a with the actual selectors used as keys.
  $a_old = $a;
  $a_new = array();

  foreach ($a_old as $selector => $property) {
    // Replace '-----', 
    // separate selectors by commas.
    $pattern = '/-----/';
    $replacement = ', ';
    $selector = preg_replace($pattern, $replacement, $selector);

    // Replace '----',
    // separate selectors by spaces
    $pattern = '/----/';
    $replacement = ' ';
    $selector = preg_replace($pattern, $replacement, $selector);

    // Replace '---',
    // separate selectors by # and .
    $pattern = '/---/';
    $replacement = ''; // TODO if this works, this step is silly. Get rid of '---'? 
    $selector = preg_replace($pattern, $replacement, $selector);

    // Replace element--, id--, class with 
    // '', '#', and '.'
    // element
    $pattern = '/element--/';
    $replacement = ''; 
    $selector = preg_replace($pattern, $replacement, $selector);
    // id
    $pattern = '/id--/';
    $replacement = '#'; 
    $selector = preg_replace($pattern, $replacement, $selector);
    // class
    $pattern = '/class--/';
    $replacement = '.'; 
    $selector = preg_replace($pattern, $replacement, $selector);

 
    foreach ($property as $p => $value) {
      $a_new[$selector][$p] = $value;
    } 
  }
  return $a_new;
} 


/**
 * Compose custom.css based on settings entered by user.
 */
function subtheme_compose_css($a) {
    // write CSS
    $css = ''; 
    foreach ($a as $selector => $array) {
      $css .= $selector ."\n{\n";
      foreach ($array as $property => $value) {
        $css .= ($value) ?  "  $property: $value;\n" : '' ;
      }
      $css .= "}\n\n";
    }

  return $css; 
}

/**
 * Write custom.css and store it 
 * in /sites/files/subtheme/custom.css.
 */ 
function subtheme_write_css($file_contents) {
  // Set the location of the custom.css file
  $file_path = file_directory_path() .'/subtheme/custom.css';

  // If the directory doesn't exist, create it
  $dir = file_check_directory(dirname($file_path), FILE_CREATE_DIRECTORY);
  if (!$dir) {
    $txt = t('The function file_check_directory() failed. Please notify your system administrator.');
    drupal_set_message($txt, 'error');
  }

  // Create CSS file.
  $saved = file_save_data($file_contents, $file_path, FILE_EXISTS_REPLACE);
  if (!$saved) {
    $txt = t('The function file_save_data() failed. Please notify your system administrator.'); 
    drupal_set_message($txt, 'error');
  }

  // Notify user about new CSS 
  $txt = t('New custom CSS file created: !file',
          array('!file' => l($file_path, $file_path)));

  drupal_set_message($txt);

}

/**
 * Save new logo.
 */
function subtheme_save_logo($form, &$form_state) {
  // Get parent (the form element that called this function)
  $parent = $form['#parents'][0];
  // Check for a new uploaded file, and use that if available.
  if ($file = file_save_upload($parent)) {
    // Get file extension.
    $parts = pathinfo($file->filename);
    $extension = $parts['extension'];
    // Check for file subtheme file directories. Create if needed.
    $directory = file_directory_path() .'/subtheme/';
    file_check_directory($directory, FILE_CREATE_DIRECTORY);
    // Name the file. 
    $file_path = file_directory_path() .'/subtheme/logo.'. $extension;
    // The image was saved using file_save_upload() and was added to the
    // files table as a temporary file. We'll make a copy and let the garbage
    // collector delete the original upload.
    if (file_copy($file, $file_path, FILE_EXISTS_REPLACE)) {
      // Save this file as the new site logo
      $settings = variable_get('theme_settings', NULL);
      $settings['logo_path'] = $file_path;
      variable_set('theme_settings', $settings);
    }
  }
  return $form;
}


