<?php 
/**
 * @file mtheme/includes/css.inc
 *  This file contains all the functions that write
 *  CSS for the mtheme module.
 */

/**
 * (Re-)Write style sheet(s).
 *
 * Stylesheets are organized like this:
 *   files/mtheme/selector_name1.css
 *   files/mtheme/selector_name2.css
 *   files/mtheme/selector_name3.css
 *   files/mtheme/selector_nameN.css
 *
 * @param $selector
 *  object, new mtheme_selector 
 */
function mtheme_css($selector = NULL) {
  $css = mtheme_generate_css($selector);   
  $written = mtheme_write_css($css);
  if (!$written) {
    // TODO handle error here. Notify user.  
  }
}

/**
 * When a selector is saved (added or edited)
 * re-write css to include new selector. 
 * 
 * @todo There's probably a more efficient way to do this than
 * re-writing the CSS after each new selector is saved, but 
 * for now, doing it this way keeps it simple. 
 * 
 * There is no extravagant naming convention magic here.
 * This function is just called mymodule_ctoolform_submit, that is
 * mtheme _ ctools_export_ui_edit_item_form _ submit.
 */
function mtheme_ctools_export_ui_edit_item_form_submit(&$form, &$form_state) {
  $selector = $form_state['item'];
  mtheme_css($selector); // @todo add error handling here.
}

/**
 * When a selector is deleted or reverted,
 * delete the files/mtheme/myselector.css
 * and any related files.
 */
function mtheme_ctools_export_ui_delete_confirm_form_submit(&$form, &$form_state) {
  $name = $form_state['item']->name;
  // delete style sheet
  $path = file_directory_path() ."/mtheme/$name.css";
  file_delete($path);
  // delete files 
  $path = file_directory_path() .'/mtheme/images/'. $name;  
  foreach (@file_scan_directory($path, '.*') as $file) {
    file_delete($file->filename);
  }
}

/**
 * Callback for admin/build/mtheme/css.
 * 
 * Generate all CSS and display as a single style sheet.
 */
function mtheme_css_page() {
  $txt = t('The style sheet below displays the current '
    .'state of all your Module Theme CSS selectors compiled into '
    .'a single style sheet. NOTE: This is not "the real stylesheet". '
    .'Disabled selectors are displayed '
    .'here, even though Drupal does not actually load CSS from disabled '
    .'selectors.');
  drupal_set_message($txt);

  $css = mtheme_generate_css(FALSE, TRUE);  
  $output = theme('mtheme_css_page', $css);

  return $output;
}


/**
 * Save image files.
 * 
 * Callback for #element_validate on 
 * #files element of admin/build/mtheme/add and /edit form.
 */
function mtheme_save_files($form, &$form_state) {
  // Get parent (the form element that called this function)
  $parent = $form['#parents'][0];
  // Check for a new uploaded file, and use that if available.
  if ($file = file_save_upload($parent)) {
    // Get file extension.
    $parts = pathinfo($file->filename);
    //$extension = $parts['extension'];
    // Check for images file directory. Create if needed.
    $directory = file_directory_path() .'/mtheme/images/';
    file_check_directory($directory, FILE_CREATE_DIRECTORY);
    // Check for images/myselector file directory. Create if needed.
    $directory = $directory ."/". $form_state['item']->name;
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
 * Generate CSS for a single selector
 * or for all mtheme selectors.
 *
 * mtheme.css:
 *   (This is currently only used for 
 *   documentation.)
 *   Get all the mtheme CSS selectors,
 *   organize them into groups,
 *   sort by weight (within groups),
 *   print CSS.  
 * 
 * @param $selector
 *  object, new mtheme_selector
 *
 * @param $compile
 *  boolean, $compile all selectors into a single stylesheet: mtheme.css.
 *  mtheme.css is only used for documentation.  
 */
function mtheme_generate_css($new_selector = NULL, $compile = FALSE) {
  $css = _mtheme_start_css();

  if ($new_selector) {
    $name = $new_selector->name;
    $css = _mtheme_write_selector_css($css, $new_selector); 
  } else if ($compile) {
    $name = 'mtheme';
    if ($groups = mtheme_get_groups()) {
      foreach ($groups as $group) {
        $css = _mtheme_group_comment($css, $group); 
        foreach ($group['selectors'] as $selector) {
          if ($selector->grp == $group['group_title'] && $selector->name != $new_selector->name) {
            $css = _mtheme_write_selector_css($css, $selector); 
          } else { // A selector is being updated. Use the new one.
            $css = _mtheme_write_selector_css($css, $new_selector); 
          } 
        }
      }
    } else {
      $name = 'mtheme';
      $txt = t('there are no mtheme modules enabled');
      $css .= "\n  /* $txt */ ";
    }
  }

  return array('name' => $name, 'css' => $css); 
}

/**
 * Write CSS comment for group of selectors.
 */
function _mtheme_group_comment($css = NULL, $group) {
  $txt = t($group['group_title']) .' ';
  $line = 60 - strlen($txt);
  for ($i = 0; $i < $line; $i++) {
    $txt .= '=';
  }

  $css .= "\n";
  $css .= '/**'. "\n";
  $css .= ' * '. $txt ."\n";
  $css .= ' */'. "\n";
  $css .= "\n";

  return $css; 
}

/**
 * Write CSS for selector.
 */
function _mtheme_write_selector_css($css = NULL, $selector) {
  // Add comment.
  $css = _mtheme_selector_comment($css, $selector); 
  // Add selector and properties. 
  $css .= '  '. $selector->css_selector ."\n";
  $css .= "  {\n";
  $properties = explode("\n", $selector->css_properties);
  foreach($properties as $property) {
    $css .= "    $property"; // seems like $property hangs onto carriage return 
  }
  $css .= "\n  }\n";

  return $css; 
}

/**
 * Write CSS comment for selector.
 */
function _mtheme_selector_comment($css, $selector) {
  $txt = '';
  $txt = t($selector->title) .' -- ';
  $txt .= t('unique identifier:') . $selector->name; 

  $css .= "  /* $txt */ \n";

  return $css; 
}

/**
 * Begin CSS with timestamp in comment at the top.
 */
function _mtheme_start_css() {
  $time = time();
  $date = date("F j, Y, g:i a");
  global $base_url;

  $txt = "Generated $date -- $time -- $base_url";  

  $css = '';
  $css .= "/* $txt */ \n \n";  
  
  return $css; 
}


/**
 * Compose mtheme.css based on settings entered by user.
 */
function mtheme_compose_css($a) {
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
 * Write mtheme.css and store it 
 * in files/mtheme/name.css.
 * 
 * @param $css
 *  Array. $css = array('name' => $name, 'css' => $css);
 */ 
function mtheme_write_css($css) {
  $file_contents = $css['css'];
  $name = $css['name'];
  // Set the location of the mtheme.css file
  $file_path = file_directory_path() .'/mtheme/'. $name .'.css';

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
  $txt = t('New CSS file created: !file',
          array('!file' => l($file_path, $file_path)));

  drupal_set_message($txt);
}
