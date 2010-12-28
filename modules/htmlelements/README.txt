This is an example module, extending the subtheme module by hooking into the Subtheme module by altering the subtheme form.

TO DO
------
Find out if subtheme modules can add additional files to be loaded at path admin/build/subtheme with hook_menu like this:
  $items['admin/subtheme']['file'][] = 'subtheme.form.inc';
  $items['admin/subtheme']['file'][] = 'htmlelements.form.inc';
  etc.
