<?php
/**
 * @file subtheme.inc
 */
/**
 * (Re-)Write style sheet(s).
 *
 * Stylesheets are organized like this:
 *    files/subtheme/subtheme_name/subtheme_name.css
 *    files/subtheme/subtheme_name/images/imageA.png
 *    files/subtheme/subtheme_name/images/imageB.jpg
 *    files/subtheme/subtheme_name/images/imageC.gif
 *
 * @param $subtheme
 *  object, $subtheme
 */
function subtheme_css($subtheme) {
  $css = subtheme_generate_css($subtheme);
  $written = subtheme_write_css($css);
  if (!$written) {
    // @todo handle error here. Notify user.
  }
}

/**
 * Generate CSS for a subtheme.
 *
 * @param $selector
 *  object, new / updated subtheme.
 *
 * @return $css
 *  Array, array('name' => $name, 'css' => $css);
 *
 */
function subtheme_generate_css($subtheme) {
  $css = _mtheme_start_css(); // @todo Move css-writing helper functions into own module.

  // Module Theme CSS overrides
  $groups = $subtheme->grps;
  asort($groups);
  $group_comments = array();
  foreach ($groups as $selector_name => $group) {
    // TODO Make sure this is working. Revised to handle undefined property error.
    // if (!$group_comments[$group]) { // One group comment line per group please.
    if (empty($group_comments[$group])) { // One group comment line per group please.
      $css = _mtheme_group_comment($css, array('group_title' => $group)); // @todo move func to css module.
      $group_comments[$group] = TRUE;
    }
    $selector = $subtheme->selectors[$selector_name];
    $css = _mtheme_write_selector_css($css, $selector);
  }

  // Append custom CSS at the end
  if ($subtheme->custom_css) {
    $txt = t('Custom CSS');
    $css = _mtheme_group_comment($css, array('group_title' => $txt)); // @todo move func to css module.
    $css .= $subtheme->custom_css; // Note: This is not being filtered for XSS. Only privileged users should be allowed to add custom CSS.
  }

  return array('name' => $subtheme->name, 'css' => $css);
}

/**
 * Write CSS and store it
 * in files/subtheme/subtheme_name/subtheme_name.css.
 *
 * @param $css
 *  Array. $css = array('name' => $name, 'css' => $css);
 */
function subtheme_write_css($css) {
  $file_contents = $css['css'];
  $name = $css['name'];

  // Make sure subtheme directory exists.
  $destination = file_build_uri('subtheme');
  $destination = file_stream_wrapper_uri_normalize($destination);
  // If the directory doesn't exist, create it
  //$dir = file_check_directory(dirname($file_path), FILE_CREATE_DIRECTORY);
  $dir = file_prepare_directory($destination, FILE_CREATE_DIRECTORY);
  if (!$dir) {
    $txt = t('The function file_prepare_directory() failed. Please notify your system administrator.');
    drupal_set_message($txt, 'error');
  }
  // Set the location of the CSS file
  $destination = file_build_uri("/subtheme/$name/$name.css");
  // If the directory doesn't exist, create it
  //$dir = file_check_directory(dirname($file_path), FILE_CREATE_DIRECTORY);
  $dir = file_prepare_directory(drupal_dirname($destination), FILE_CREATE_DIRECTORY);
  if (!$dir) {
    $txt = t('The function file_prepare_directory() failed. Please notify your system administrator.');
    drupal_set_message($txt, 'error');
  }

  // Create CSS file.
  $saved = file_save_data($file_contents, $destination, FILE_EXISTS_REPLACE);
  if (!$saved) {
    $txt = t('The function file_save_data() failed. Please notify your system administrator.');
    drupal_set_message($txt, 'error');
  }

  // Notify user about new CSS
  $public = new DrupalPublicStreamWrapper;
  $file_path = $public->getDirectoryPath() . "/subtheme/$name/$name.css";
  $txt = t('New CSS file created: !file',
          array('!file' => l($file_path, $file_path)));

  drupal_set_message($txt);
}
