<?php
/**
 * Define this Export UI plugin.
 */
$plugin = array(
  'schema' => 'subtheme_subtheme',  // As defined in hook_schema(), mymodule_myobj.
  'access' => 'administer subtheme',  // Define a permission users must have to access these pages.

  // Define the menu item.
  'menu' => array(
    'menu item' => 'subtheme',
    'menu title' => t('Subtheme'),
    'menu description' => t('Administer subthemes.'),
  ),

  // Define user interface texts.
  'title singular' => t('subtheme'),
  'title plural' => t('subthemes'),
  'title singular proper' => t('Subtheme'),
  'title plural proper' => t('Subthemes'),
  // Define the names of the functions that provide the add/edit forms.
  'form' => array(
    'settings' => 'subtheme_ctools_export_ui_form',
    // 'submit' and 'validate' are also valid callbacks.
  ),
  // Use this to modify the default export UI interface.
  'handler' => array(
    'class' => 'subtheme_export_ui',
    'parent' => 'ctools_export_ui',
  ),
);

/**
* Define the preset add/edit form.
*/
function subtheme_ctools_export_ui_form(&$form, &$form_state) {
  $subtheme = $form_state['item'];

  // subtheme's configuration interface.
  /* Ctools includes this field automatically on its own.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Machine Name'),
    '#description' => t('lowercase, no spaces or punctuation'),
    '#default_value' => $subtheme->name,
    '#required' => true,
  );
  // */
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Human readable Subtheme name. '),
    '#default_value' => $subtheme->title,
    '#required' => FALSE,
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('Optional description.'),
    '#default_value' => $subtheme->description,
    '#required' => FALSE,
  );
  $options = subtheme_admin_options_get();
  $form['selectors'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Include these selectors in your subtheme'),
    '#description' => t('Checked items will be available to override with your subtheme.'),
    '#default_value' => subtheme_admin_options_default_values(arg(4), $options),
    '#options' => $options,
    '#prefix' => "<div class='subtheme subtheme-admin-checkboxes'>",
    '#suffix' => "</div>",
  );

  // Enable user to reorganize selectors into groups.
  if (arg(2) == 'subtheme' && ( arg(5) == 'edit' || arg(5) == 'clone' )) {
    $form['groups'] = array(
      '#type' => 'item',
      '#title' => t('Organize included selectors into groups'),
      '#value' => t('Selectors with the same group name will '
                   .'be "grouped" together on the Customize form and '
                   .'in CSS comments.'),
    );
    $subtheme = subtheme_get(arg(4));
    $selectors = $subtheme->selectors;
    $grps = $subtheme->grps;
    // Organize included selectors by group
    foreach ($selectors as $selector) {
      $groups[$selector->name] = $grps[$selector->name];
    } 
    // Organize selectors by group
    asort($grps);
    foreach ($grps as $selector => $group) {
      $form[$selector] = array(
        '#type' => 'textfield',
        '#title' => $selector,
        '#default_value' => $group, 
        '#required' => TRUE,
      );
    } 
  } else {
    $form['groups'] = array(
      '#type' => 'item',
      '#title' => t('Organize included selectors into custom groups here'),
      '#value' => t('Click Save and then Edit to customize groups here.'),
    );
  }
}

/**
 * Callback for form validation. 
 *
 * Process input.
 * 
 * The form being validated here is responsible for two things:
 * 1. Determining which mtheme_selectors a subtheme includes
 * 2. Giving the administrator an opportunity to re-organize
 *    mtheme_selectors into their own (end-user friendly) groups,
 *    used to organize selectors on the subtheme edit form
 *    and in the actual CSS file.
 * 
 * *NOTE: Selectors aren't actually modified here. They're in or they're out.
 * Selectors are modified on the edit form stored in includes/subtheme.edit.inc.
 * 
 * For selectors to be included, check to see if this value
 * has already been included and possibly overridden. (If it show up in 
 * subtheme_get(), it's already included.) If it's newly being 
 * included with the subthem, grab the values from the mtheme_selector. 
 */
function subtheme_ctools_export_ui_edit_item_form_validate(&$form, &$form_state) {
  ctools_include('export');
  $selectors = $form_state['values']['selectors'];

  $processed_selectors = array();
  $processed_groups = array();

  if (arg(3) == 'add') {
    // process selectors, new subthee
    foreach ($selectors as $selector_name => $include) {
      if ($include) {
        $selector = mtheme_selector_load($selector_name);
        //$selector->css_properties = "/******** uncomment to override default CSS properties ********* \n"
        //                           .$selector->css_properties
        //                           ."\n **************************************************************/"; 
        $processed_selectors[$selector_name] = $selector;
        $processed_groups[$selector_name] = $selector->grp;
      } else {
        unset($processed_selectors[$selector_name]);
        unset($processed_groups[$selector_name]);
      }
    }
  }
  // process selectors, update existing subtheme
  else if (arg(5) == 'edit' || arg(5) == 'clone') {
      $subtheme_name = arg(4);
      $subtheme = subtheme_get($subtheme_name);
      $saved_selectors = $subtheme->selectors;
      foreach ($selectors as $selector_name => $include) {
        if (!$include) {
          unset($processed_selectors[$selector_name]);
        } else if ($include && array_key_exists($selector_name, $saved_selectors)) {
          // Store already-saved value, in case user has made changes.
          // (Selectors aren't modified here. This form only determines if they're 
          // included or excluded from the subtheme being edited.)
          $processed_selectors[$selector_name] = $saved_selectors[$selector_name];
        } else if ($include && !array_key_exists($selector_name, $saved_selectors)) {
          // New selector being added to existing subtheme.
          $processed_selectors[$selector_name] = mtheme_selector_load($selector_name);
        }

        // process groups. every selector needs a group.
        foreach ($selectors as $selector_name => $include) {
          if ($include && array_key_exists($selector_name, $saved_selectors)) {
            // Store user-defined group.
            $processed_groups[$selector_name] = $form_state['values'][$selector_name]; 
          } else if ($include && !array_key_exists($selector_name, $saved_selectors)) {
            // New selector being added to existing subtheme. Store default (mtheme_selector) group.
            $processed_groups[$selector_name] = $processed_selectors[$selector_name]->grp;
          } else {
            unset($processed_groups[$selector_name]);
          }
        }
      }
    }

  $form_state['values']['selectors'] = $processed_selectors;
  $form_state['values']['grps'] = $processed_groups;

}

/**
 * Callback for form submission.
 *
 * When subthemes are created or updated, re-write CSS.
 */
function subtheme_ctools_export_ui_edit_item_form_submit(&$form, &$form_state) {
  $subtheme = $form_state['item'];
  $op = (arg(5) == 'clone') ? array('ctools save', 'clone') : array('ctools save');
  subtheme_save($subtheme, $op);  
}

/**
 * Callback for delete subtheme form.
 * 
 * When a subtheme is deleted or reverted,
 * delete all the files in 
 * files/subtheme/mysubtheme/.
 * Also check to see if deleted subtheme is enabled. 
 * If it is, disable it.
 * 
 * @todo D6 doesn't provide a function for removing directories. 
 *  Use PHP rmdir. In D7 use FileTransfer::removeDirectoryJailed
 */
function subtheme_ctools_export_ui_delete_confirm_form_submit(&$form, &$form_state) {
  $subtheme = $form_state['item'];
  $name = $subtheme->name;
  $revert = (arg(5) == 'revert') ? TRUE : FALSE; // delete form used for revert too.

  // If subtheme is being reverted, and if default status is active
  // make this module active. (Otherwise, if there are multiple subthemes
  // that default to is_active == TRUE, the subtheme that gets activated 
  // will be--or seem--random.)
  if ($revert) {
    ctools_include('export');
    $schema = ctools_export_get_schema('subtheme_subtheme');
    $export = $schema['export'];
    $defaults = _ctools_export_get_defaults('subtheme_subtheme', $export);
    foreach ($defaults as $default) {
      if ($default->name == $name && $default->is_active) {
        subtheme_activate($name);
      }
    } 

  }

  // delete image files 
  $path = file_directory_path() ."/subtheme/$name/images";  
  if (file_check_directory($path)) {
    foreach (@file_scan_directory($path, '.*') as $file) {
      file_delete($file->filename);
    }
  }
  if (file_check_directory($path)) {
    $result = rmdir($path);
    if ($result) {
      $txt = $path . t(' deleted.');  
      drupal_set_message($txt);
    } else {
      $txt = $path . t(' could not be deleted.'); 
    }
  }

  // delete style sheet and subtheme directory
  $path = file_directory_path() ."/subtheme/$name/$name.css";
  file_delete($path);
  $path = file_directory_path() ."/subtheme/$name";
  if (file_check_directory($path)) {
    $result = rmdir($path);
  }
  if (file_check_directory($path)) {
    $result = rmdir($path);
    if ($result) {
      $txt = $path . t(' deleted.');  
      drupal_set_message($txt);
    } else {
      $txt = $path . t(' could not be deleted.'); 
    }
  }

}
