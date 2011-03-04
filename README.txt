Subtheme 7.x-1.x
-----------------
The Subtheme module enables you to: 
- Create subthemes through Drupal's web interface
- Export subthemes with the Features module
- Review a list of CSS elements provided by modules and themes, 
  and select the elements you want to include and/or override 
  in your subtheme

Dependencies
--------------
Features
Module Theme

Installation 
---------------
Place the entire mtheme folder into your modules directory.
Go to Administer -> Modules and enable the Subtheme 
module. 

Recommended:
Install a patched version of Features, available here:
https://github.com/bhirsch/features
These patches add a new hook to Features to enable modules like Module Theme
to export CSS and image files. The patches have been submitted and are being 
reviewed by the module maintainers.

Use 
----------
If you are installing Subtheme because it is a dependency
of another module in your site, you're done. No additional set-up 
or configuration required. 

If you are a site builder or module developer interested in using
Subtheme to build and export subthemes, read on.

For Site Builders and Module Developers
-----------------------------------------
Go to Administer -> Structure -> Subtheme.

Here you will see a list of all the subthemes available to your site.

Click the Add tab to create a new subtheme.

Click Activate to activate one of your subthemes.

To export a subtheme (or subthemes) go to
Administer -> Site building -> Features -> Create Feature

Under Edit Components select Subtheme, then select the component (subtheme)
you want to export. When you're done click Download feature. Now your subtheme
is included in the module you just downloaded.

