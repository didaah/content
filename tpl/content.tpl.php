<?php
// $Id$

/**
 * @file 内容浏览页面默认模板文件
 * @param object $content 内容对象
 *
 * 模板文件加载优化级：
 *  content_{$content->type}_{$content->nid}.tpl.php
 *  content_{$content->type}.tpl.php
 *  content.tpl.php
 */

?>

<div id="content_view_<?php echo $content->nid?>">

  <h1 class="content_title"><?php echo $content->title?></h1>

  <div class="content_view_content">
    <div class="content_view_content_header">
      <span class="name"><?php echo theme('username', $content);?></span>
      <span class="time"><?php echo t('content', '发布于 !time', array('!time' => format_date($content->created)))?></span>
    </div>
    <?php if (!empty($content->field_html)) : ?>
    <div class="content_view_content_fields"><?php echo $content->field_html; ?></div>
    <?php endif; ?>
    <div class="content_view_content_body clearfix"><?php echo $content->body?></div>
  </div>

  <div class="content_view_links">
    <?php if ($content->comment_count) : ?>
    <?php echo '<a href="#content_view_comment_wrapper">' . t('content', '共 !count 条评论', array('!count' => $content->comment_count)) . '</a>'; ?>
    <?php endif?>
    
    <?php if ($content->is_update) : ?>
    <?php echo l(t('content', '编辑'), 'cotnent/' . $content->type . '/' . $content->nid . '/edit', dd_get_redirect()); ?>
    <?php endif?>

    <?php if ($content->is_delete) : ?>
    <?php echo l(t('content', '删除'), 'cotnent/' . $content->type . '/' . $content->nid . '/delete'); ?>
    <?php endif?>
  </div>

  <?php if (!empty($content->is_comment)) : ?>
  <div id="content_view_comment_wrapper">
    <?php if ($content->comment_view): ?>
      <?php echo $content->comment_view?>
      <?php echo $content->comment_pager?>
    <?php endif?>

    <?php if ($content->comment_form) { ?>
      <?php echo $content->comment_form?>
    <?php }?>
  </div>
  <?php endif ;?>

</div>
