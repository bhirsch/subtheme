<?php
/**
 * Define this Export UI plugin.
 */
$plugin = array(
  'schema' => 'mtheme_selector',  // As defined in hook_schema(), mymodule_myobj.
  'access' => 'administer mtheme',  // Define a permission users must have to access these pages.

  // Define the menu item.
  'menu' => array(
    'menu item' => 'mtheme',
    'menu title' => t('Module Theme'),
    'menu description' => t('Administer Module Theme selectors.'),
  ),

  // Define user interface texts.
  'title singular' => t('selector'),
  'title plural' => t('selectors'),
  'title singular proper' => t('Module Theme Selector'),
  'title plural proper' => t('Module Theme Selectors'),
  // Define the names of the functions that provide the add/edit forms.
  'form' => array(
    'settings' => 'mtheme_ctools_export_ui_form',
    // 'submit' and 'validate' are also valid callbacks.
  ),
  // Use this to modify the default export UI interface.
  'handler' => array(
    'class' => 'mtheme_export_ui',
    'parent' => 'ctools_export_ui',
  ),
);

/**
* Define the preset add/edit form.
*/
function mtheme_ctools_export_ui_form(&$form, &$form_state) {
  $selector = $form_state['item'];

  // mtheme's configuration interface.
  /* Ctools includes this field automatically on its own.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Machine Name'),
    '#description' => t('lowercase, no spaces or punctuation'),
    '#default_value' => $selector->name,
    '#required' => true,
  );
  // */
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Human readable CSS selector name. '
                       .'This will be included in .css file as a comment '
                       .'above the selector.'),
    '#default_value' => $selector->title,
    '#required' => FALSE,
  );
  $form['css_selector'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS Selector'),
    '#description' => t('CSS selector. E.g. div or  div#example .demo '),
    '#default_value' => $selector->css_selector,
    '#required' => true,
  );
  $form['css_properties'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS Properties'),
    '#default_value' => $selector->css_properties,
    '#description' => t('E.g. background-color: white; <br/> font-size: 12px; '),
    '#required' => true,
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
      '#element_validate' => array('mtheme_save_files'),
    );
    // Check for available files

    $name = arg(4);
    $path = file_directory_path() .'/mtheme/images/'. $name;  
    $i = 0; // Use counter in case of conflicting $file->basename values
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
  $form['grp'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS Selector Group'),
    '#description' => t("Group selectors together. (Use your module name if you're not sure what to put here.)"),
    '#default_value' => $selector->grp,
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('Optional description.'),
    '#default_value' => $selector->description,
    '#required' => FALSE,
  );
  $form['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Weight'),
    '#description' => t('Sort order of selectors for multiple identical selectors.'),
    '#default_value' => $selector->weight,
    '#required' => FALSE,
  );

}
