Subtheme
--------

Set-up
--------
This only works with the following lines included in template.php:

Note: If you include this snippet in template.php, you don't have 
to additionally include the snippet from Module Theme's README file.

if (module_exists('mtheme') && module_exists('subtheme')) {
  mtheme_add_css();  
  subtheme_add_css();  
}


File structure
---------------
files/subtheme/subtheme_name/subtheme_name.css
files/subtheme/subtheme_name/images/imageA.png
files/subtheme/subtheme_name/images/imageB.jpg
files/subtheme/subtheme_name/images/imageC.gif


the $subtheme object
--------------------------------------------------------------------
$subtheme->name = 'name';             // machine, unique identifier
$subtheme->title = 'title';           // human readable
$subtheme->description = 'name';
$subtheme->selectors = array(
  'selector_name_1' => (object) stdClass
  'selector_name_2' => (object) stdClass
  'selector_name_N' => (object) stdClass
);
$subtheme->grps = array(
  'selector_name_1' => 'Group Name', 
  'selector_name_2' => 'Another Group Name',
  'selector_name_N' => 'A Group name',
);
$subtheme->is_active = 0; // or 1;

