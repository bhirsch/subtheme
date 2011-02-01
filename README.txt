Module Theme
--------------

[TODO write readme documentation]


This only works with the following line included in template.php
New version:
if (module_exists('mtheme')) {
  mtheme_add_css();  
}

Legacy: 
$mtheme_css = file_directory_path() .'/mtheme/mtheme.css';
if (file_exists($mtheme_css)) {
  drupal_add_css($mtheme_css, 'theme', 'all', TRUE);
}


Comments like this: 
------------------
/**
 * Group ======================================================
 */

/* selector title */
#selector  
{
  property: value; /* comment from text area */
  property2: value; /* comment from text area */ 
}


For Module Developers
----------------------
- write CSS with GUI
- export as feature module: mymtheme
- include mymtheme in your mymodule/ or mymodule/modules
- include the CSS by declaring it in the .info file (or hook_init() if you're a rebel)
- to enable/disable mymtheme when your module is enabled/disabled, use hook_enable and hook_disable
