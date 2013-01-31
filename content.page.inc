<?php
// $Id$

// public content_page($type, $content) {{{ 
/**
 * content_page
 * 内容浏览页面
 * 
 * @param object $type 
 * @param object $content 
 * @access public
 * @return string
 */
function content_page($type, $content) {
  global $user;

  // 浏览计数
  if (!empty($type->data['visit'])) {
    db_exec('UPDATE {' . $content->table . '} SET visit = visit+1 WHERE nid = ?', array($content->nid));
  }

  dd_set_title(array($content->title, $type->name));

  $content->body = filter_view($content->body, $content->format);

  if (!empty($content->fields)) {
    field_get_node_view('content', 'ext_' . $content->type, $content, 2);
    $content->field_html = field_get_node_html('content', $content);
  }

  // 编辑权限
  $content->is_update = content_is_update($type, $content);
  // 删除权限
  $content->is_delete = content_is_delete($type, $content);

  if (!empty($type->data['comment'])) {
    // 允许评论
    $content->is_comment = true;
    if ($content->comment_count) {
      if (user_access('content ' . $type->type . ' comment view')) {
        if ($data = comment_ext_view(
          'content_' . $type->type,
          $content->nid,
          !empty($type->data['comment_page_count']) ? $type->data['comment_page_count'] : 20,
          true
        )) {
          $content->comment_view = $data->view;
          $content->comment_pager = $data->pager;
        } else if (!empty($_GET['filter_comment'])) {
          $content->comment_view = system_no_content(
            l(t('content', '该用户没有发布评论，返回全部评论'), 'content/' . $type->type . '/' . $content->nid)
          );
        }
        $content->filter_comment = url('content/' . $type->type . '/' . $content->nid, array('query' => 'filter_comment=' . $content->uid));
      } else {
        $content->comment_view = system_no_content(t('content', '你没有查看评论的权限'));
      }
    }
    
    // 允许发布评论
    if (user_access('content ' . $type->type . ' comment create')) {
      $comment = new stdClass;
      $comment->ext_type = 'content_' . $type->type;
      $comment->ext_id = $content->nid;
      $comment->ext_id2 = 0;
      $comment->title = t('content', '回复：!title', array('!title' => $content->title));
      $content->comment_form = dd_get_form('comment_form', $comment);
    } else if (!empty($user->uid)) {
      $content->comment_form = t('content', '根据权限设定，你不能发布评论');
    } else {
      $content->comment_form = l(t('content', '你需要登录才能发布评论'), 'user/login', dd_redirect());
    }
  }
 
  module_invoke_all('content_view', $content);

  dd_add_css(dd_get_path('module', 'content') . '/content.css');

  return theme('content', $content);
}
// }}}
