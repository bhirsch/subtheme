<?php 
/**
 * @file mtheme/includes/css.inc
 *  This file contains all the functions that write
 *  CSS for the mtheme module.
 */


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

