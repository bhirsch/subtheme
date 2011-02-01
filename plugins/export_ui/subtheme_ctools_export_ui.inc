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
  // Include files field group for uploading background images
  // Include enctype, otherwise uploads won't work!
  if (arg(5) == 'edit') { // Only add files after first save, we need a name.
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
    $i = 0; // Use counter in case of conflicting $file->basename values
    file_check_directory($path, FILE_CREATE_DIRECTORY);
    foreach (@file_scan_directory($path, '.*') as $file) {
      $title = "images/$name/$file->basename";
      $link = l($title, $file->filename);
      // Add form items
      $item = $file->basename .'-'. $i;
      $form['files'][$item] = array(
        '#type' => 'item',
        '#title' => t("File "). $i,
        '#value' => $link,
        '#description' => t("Add this file to you CSS like this: "
                           ."background: url('". $title ."');"),
      );
      $i++; 
    }
  } else if (arg(3) == 'add') {  
    $form['files'] = array(
      '#type' => 'item',
      '#title' => t('Files'),
      '#value' => t('Save then click Edit to add background files here.'),
    );  
  }
  // TODO subtheme_get('name');
  // TODO process array to get checkboxes. see subtheme_admin_get().
  //$options = subtheme_admin_options_get(arg(4));
  $options = subtheme_admin_options_get();
  $form['selectors'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Include these selectors in your subtheme'),
    '#description' => t('Checked items will be available at admin/subtheme/name'),
    '#default_value' => subtheme_admin_options_default_values(arg(4), $options),
    '#options' => $options,
    '#prefix' => "<div class='subtheme subtheme-admin-checkboxes'>",
    '#suffix' => "</div>",
  );
  // Enable user to reorganize selectors into groups.
  if (arg(2) == 'subtheme' && arg(5) == 'edit') {
    $form['groups'] = array(
      '#type' => 'item',
      '#title' => t('Organize included selectors into groups'),
    );
    $subtheme = subtheme_get(arg(4));
    $selectors = $subtheme->selectors;
    $grps = $subtheme->grps;
    $groups = array(); // array( $selector_name => $group_name )
    // Organize included selectors by group
    if (!$grps) { // no groups. new subtheme.
      foreach ($selectors as $selector) {
        $groups[$selector->name] = $selector->grp;
      }
    } else {
      foreach ($selectors as $selector) {
        $groups[$selector->name] = $grps[$selector->name];
      } 
    }
    // Organize selectors by group
    asort($groups);
    foreach ($groups as $selector => $group) {
      $form[$selector] = array(
        '#type' => 'textfield',
        '#title' => $options[$selector],
        '#default_value' => $group, 
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
 * Process input.
 * 
 * $subtheme = array(
 *   'selector_name_B' => $selector, //  mtheme_selector object
 * );
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

  if (arg(3) == 'add') { // new subtheme
    foreach ($selectors as $selector_name => $include) {
      if ($include) {
        $processed_selectors[$selector_name] = mtheme_selector_load($selector_name);
      } else {
        unset($processed_selectors[$selector_name]);
      }
    }
  } 

  if (arg(5) == 'edit') { // update existing subtheme
    $subtheme_name = $form_state['values']['name'];
    $subtheme = subtheme_get($subtheme_name);
    $saved_selectors = $subtheme->selectors;
    foreach ($selectors as $selector_name => $include) {
      if (!$include) {
        unset($processed_selectors[$selector_name]);
      } else if ($include && array_key_exists($selector_name, $saved_selectors)) {
        // Store already-saved value, in case user has made changes.
        $processed_selectors[$selector_name] = $saved_selectors[$selector_name];
      } else if ($include && !array_key_exists($selector_name, $saved_selectors)) {
        // New selector being added to existing subtheme.
        $processed_selectors[$selector_name] = mtheme_selector_load($selector_name);
      }
    // Process groups
    $processed_groups[$selector_name] = $form_state['values'][$selector_name];
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
  ctools_include('subtheme', 'subtheme'); 
  $subtheme = $form_state['item'];
  subtheme_css($subtheme); 
}
