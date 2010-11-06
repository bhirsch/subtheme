<?php
// $Id$

/**
 * @file
 * administration page callbacks for subtheme module.
 */

/**
 * Form builder. Configre subtheme settings.
 *
 * @ingroup forms
 * @see system_settings_form().
 */
function subtheme_admin_settings() { 

// TODO Add checkbox for enable subtheme.
//      Have template.php check to see if subtheme is enabled before calling drupal_add_css()

/*
  $form['subtheme'] = array(
    '#type' => 'fieldset',
    '#title' => 'Press Release Settings',
  );
  $form['pr']['sws_pr_org'] = array( 
    '#type' => 'textfield',
    '#title' => t('Organization Name'),
    '#description' => t('This will be displayed at the top '
                       .'of every press release. ' 
                       .'E.g. "The White House"'), 
    '#default_value' => variable_get('sws_pr_org', variable_get('site_name', NULL)),
  ); 
  $form['pr']['sws_pr_dept'] = array( 
    '#type' => 'textfield',
    '#title' => t('Office or Department'),
    '#description' => t('Displayed at the top '
                       .'of press releases below organization name. ' 
                       .'E.g. "Office of the Press Secretary"'), 
    '#default_value' => variable_get('sws_pr_dept', NULL),
);
// */ 
  return system_settings_form($form);
} 
