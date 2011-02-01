 NOTE: This module is undergoing a major re-write. The API below may not be used any more. This file will be updated when the re-write is complete.



 // Pattern for $key => $value
 // [modulename] _ [type--selector] _ [property] => X
 // e.g. subtheme_id--press-releases_width => 150px;
 // or
 // explode '_' to separate modulename _ selector _ property
 //  
 // Make an array keyed by 
 // selector type -- selector
 // types: id, class, element

array $a, reformat keys like this
 Like this: 
    //   $a[selector][propertyA] = X
    //   $a[selector][propertyB] = Y 
    //   $a[selector1---selectorN][propertyC] = Q 



This only works with the following line included in template.php

$custom_css = file_directory_path() .'/subtheme/custom.css';
if (file_exists($custom_css)) {
  drupal_add_css($custom_css, 'theme', 'all', TRUE);
}



Form API keys
----------------------------------------------------------------
subtheme_[type--selector]_[property]
e.g. subtheme_class--main_background


Working with multiple selectors
-----------------------------------------------------------------
Selectors can be separated by ---, ----, and -----. 

subtheme_[type--selector]_[property]
subtheme_[type--selector1---type--selector2---type--selectorN]_[property]
subtheme_[type--selector1----type--selector2----type--selectorN]_[property]
subtheme_[type--selector1-----type--selector2-----type--selectorN]_[property]

e.g. these: 
subtheme_element--h1_background
subtheme_element--h1---id--right-sidebar---class--news_background
subtheme_element--h1----id--right-sidebar----class--news_background
subtheme_element--h1-----id--right-sidebar-----class--news_background

become: 
h1 { background: }
h1#right-sidebar.news { background: }		seperated by ---
h1 #right-sidebar .news { background: } 	seperated by ----
h1, #right-sidebar, .news { background: } 	seperated by -----


Processing multiple selectors
------------------------------------------------------------------
To do something like this: 	h1.navbar, .navbar 
Do this: 			element--h1---class--navbar-----class--navbar

subtheme explodes selectors in this order:
4 dashes (----)
3 dashes (---)
2 dashes (--)



