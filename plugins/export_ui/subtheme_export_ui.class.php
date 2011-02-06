<?php
/**
 * CTools export UI extending class. 
 *
 */
class subtheme_export_ui extends ctools_export_ui {

  function list_css() {
    ctools_add_css('export-ui-list');
    drupal_add_css(drupal_get_path("module", "subtheme") ."/subtheme.css");
  }

  function list_render(&$form_state) {
    $list_header = $this->list_table_header();
    $title = array(array('data' => 'Title', 'class' => 'ctools-export-ui-name'));
    $list_header = array_merge($title, $list_header);

    return theme('table', $list_header, $this->rows, array('class' => 'subtheme-admin', 'id' => 'ctools-export-ui-list-items'));
  }

  function list_build_row($item, &$form_state, $operations) {
    $name = $item->name;

    // Build row for each item.
    $this->rows["{$group}:{$name}"]['data'] = array();
    $this->rows["{$group}:{$name}"]['class'] = !empty($item->disabled) ? 'ctools-export-ui-disabled' : 'ctools-export-ui-enabled';

    $this->rows["{$group}:{$name}"]['data'][] = array(
      'data' => check_plain($item->title),
      'class' => 'ctools-export-ui-title'
    );
    // */

    $this->rows["{$group}:{$name}"]['data'][] = array(
      'data' => check_plain($name) . "<div class='description'>" . check_plain($item->description) . "</div>",
      'class' => 'ctools-export-ui-name'
    );
    $this->rows["{$group}:{$name}"]['data'][] = array(
      'data' => check_plain($item->type),
      'class' => 'ctools-export-ui-storage'
    );
    $this->rows["{$group}:{$name}"]['data'][] = array(
      'data' => theme('links', $operations, array('class' => 'links inline')),
      'class' => 'ctools-export-ui-operations'
    );

    // Sort by group, name.
    $this->sorts["{$group}:{$name}"] = $group . $name;
  }
  
}


