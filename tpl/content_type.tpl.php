<?php
// $Id$

/**
 * @file 内容类型列表页面默认模板文件
 * @param object $type 内容类型对象
 *
 * 模板文件加载优化级：
 *  content_type_{$type->type}.tpl.php
 *  content_type.tpl.php
 */

?>

<div class="content_type_view">

  <h1 class="content_type_title"><?php echo $type->name?></h1>

  <div class="content_type_view_content">
<?php
if (!empty($type->fetch)) {
  $items = array();
  foreach ($type->fetch as $o) {
    $items[] = l($o->title, 'content/' . $type->type . '/' . $o->nid);
  }
  echo theme('item_list', $items, NULL, 'ul', array('class' => 'content_type_view_list'));
  echo $type->pager;
} else {
  echo system_no_content();
}
?>
  </div>

</div>
