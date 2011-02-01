Subtheme
--------

File structure
---------------
files/subtheme/subtheme_name/subtheme_name.css
files/subtheme/subtheme_name/images/imageA.png
files/subtheme/subtheme_name/images/imageB.jpg
files/subtheme/subtheme_name/images/imageC.gif


 * Format the subtheme field.
 * 
 * $subtheme = array(
 *   'selector_name_A' => FALSE, // This selector is not included in the subtheme
 *   'selector_name_B' => array( // This one will be included
 *     'group' => 'Group Name',
 *     'obj' => $selector, // mtheme_selector object
 *   ), 
 * );
