<?php
/**
 * @file subtheme.activate.inc
 *  Activate subtheme form functions.
 */
/**
 * Callback for admin/build/subtheme/activate.
 */
function subtheme_activation_form($form, &$form_state) {
  $subthemes = subtheme_get_subthemes();

  $options = _subtheme_activate_options($subthemes);
  $options['disabled'] = 'disabled';

  $default = subtheme_get_active_subtheme();
  if (!$default) {
    $default = 'disabled';
  }

  $form['activate'] = array(
    '#type' => 'radios',
    '#title' => t('Activate a subtheme by making a selection here'),
    '#options' => $options,
    '#default_value' => $default,
    '#prefix' => '<div class="subtheme subtheme-admin-activate-radios">',
    '#suffix' => '</div>',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['#theme'] = 'subtheme_activate';

  return $form;
}

/**
 * Implement theme function for subtheme activate form.
 * 
 * Displays the form as a table with radio buttons and edit links.
 */
function theme_subtheme_activate($variables) {
  $form = $variables['form'];
  $output = '';
  $output .= '<table>';
  $output .= '<tr>';
  $output .=   '<th></th>';
  $output .=   '<th>' . t('Subtheme') . '</th>';
  $output .=   '<th>' . t('Description') . '</th>';
  $output .=   '<th>' . t('Edit') . '</th>';
  $output .= '</tr>';

  // List options in alpha order by title
  $list = array();
  foreach ($form['activate']['#options'] as $name => $subtheme) {
    // Convert objects to arrays and sort for consistency.
    $list[$name] = t(check_plain($subtheme->title));
  }
  asort($list);
  $default = $form['activate']['#default_value'];
  $count = 0;
  // Add a row for each available subtheme
  foreach ($list as $name => $title) {
    $zebra = ($count % 2) ? 'odd' : 'even';
    //    $checked = ($name == 'red') ? 'checked="checked" ' : '';
    $checked = ($default == $name) ? 'checked="checked" ' : '';
    $description = t(check_plain($form['activate']['#options'][$name]->description));
    $edit_link = ($name == 'disabled') ? '' : l(t('Edit'), "admin/structure/subtheme/edit/$name");
    $title = ($name == 'disabled') ? t('No active subtheme') : $title;
    $output .=   '<tr class="' . $zebra . '" >';
    $output .=     "<td>"
                   . '<input type="radio" '
                     . 'class="form-radio" '
                     . 'value="' . $name . '" '
                     . $checked
                     . 'name="activate" '
                     . 'id="edit-activate-' . $name . '" '
                   . '>'
                  . "</td>";
    $output .=     "<td>$title</td>";
    $output .=     "<td>$description</td>";
    $output .=     "<td>$edit_link</td>";
    $output .=   '</tr>';

    $count++;
  }

  $output .= '</table>';
  $output .= '<input type="submit" class="form-submit" value="Save" id="edit-submit" name="op">';
  $output .= drupal_render($form['form_build_id']);
  $output .= drupal_render($form['form_token']);
  $output .= drupal_render($form['form_id']);

  return $output;
}

/**
 * Implements hook_FORM_ID_submit().
 *
 * Activate selected subtheme.
 */
function subtheme_activation_form_submit(&$form, &$form_state) {
  $subtheme_name = $form_state['values']['activate'];
  subtheme_activate($subtheme_name);
}

/**
 * @param $subthemes
 *  array of subtheme objects
 */
function _subtheme_activate_options($subthemes) {
  $options = array();
  foreach ($subthemes as $subtheme) {
    if (!$subtheme->disabled) {
      $options[$subtheme->name] = $subtheme;
    }
  }

  return $options;
}

/**
 * Implement theme function for activate options. 
 */
function theme_subtheme_activate_options($variables) {
  $subtheme = $variables['subtheme'];

  $output = '';

  $output .= '<class="subtheme subtheme-activate-option">';

  $output .= '<div class="subtheme subtheme-activate-title">';
  $output .= check_plain($subtheme->title);
  $output .= '</div>';

  $output .= '<div class="subtheme subtheme-activate-description">';
  $output .= check_plain($subtheme->description);
  $output .= '</div>';

  $output .= '</div>';

  return $output;
}
