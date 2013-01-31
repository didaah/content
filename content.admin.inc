<?php
// $Id$

// public content_admin_list() {{{ 
/**
 * content_admin_list
 *  按类型管理内容
 * @param object $type
 *  一条类型记录
 * @access public
 * @return string
 */
function content_admin_list($type) {
  dd_set_tabs(array(
    l(t('content', '!name管理', array('!name' => $type->name)), 'admin/content/list/' . $type->type),
    l(t('content', '添加内容'), 'content/add/' . $type->type, dd_get_redirect()),
    l(t('content', '!name属性', array('!name' => $type->name)), 'admin/content/type/fields/' . $type->type),
  ));
  
  if (!empty($_GET['op']) && $_GET['op'] == 'delete') {
    if (user_access('content ' . $type->type . ' delete') && content_delete($type->type, $_GET['id'])) {
      echo 'tr';
    } else {
      echo t('content', '您没有删除内容的权限');
    }
    exit;
  }
 
  $args = NULL;

  $query = 'SELECT n.title, n.nid, n.uid, n.visit, n.comment_count, n.status, 
  n.created, u.name FROM {content_ext_' . $type->type . '} n INNER JOIN {users} u ON n.uid = u.uid';
  
  $query = 'SELECT n.*, n.created, u.name FROM {content_ext_' . $type->type . '} n INNER JOIN {users} u ON n.uid = u.uid';

  if (!empty($_GET['keyword'])) {
    $args[] = '%' . urldecode($_GET['keyword']) . '%';
    $query .= ' WHERE n.title LIKE ?';
  }
  
  $output = dd_get_form('form_admin_search');
  
  $header = array(
    array('data' => t('content', '标题'), 'field' => 'n.nid', 'sort' => 'desc'),
    array('data' => t('content', '创建人'), 'field' => 'n.uid'),
    array('data' => t('content', '评论数量'), 'field' => 'n.comment_count'),
    array('data' => t('content', '访问次数'), 'field' => 'n.visit'),
    array('data' => t('content', '状态'), 'field' => 'n.status'),
    array('data' => t('content', '发布时间'), 'field' => 'n.created'),
    array('data' => t('content', '更新时间'), 'field' => 'n.updated'),
    t('content', '操作')
  );
  
  if ($fetch = pager_query($query . table_sql($header), 20, 0, NULL, $args)) {
    $delete_url = url('admin/content/list/' . $type->type, array('query' => 'op=delete'));
    foreach ($fetch as $o) {
      $table[] = array(
        l($o->title, 'content/' . $type->type . '/' . $o->nid, array('attributes' => array('target' => '_blank'))),
        $o->name,
        $o->comment_count,
        $o->visit,
        $o->status ? t('content', '已发布') : t('content', '未发布'),
        format_date($o->created),
        format_date($o->updated),
        dd_imp(array(
          l(t('content', '编辑'), 'content/' . $type->type . '/' .  $o->nid . '/edit', dd_get_redirect()),
          '<a href="' . $delete_url . '&id=' . $o->nid . '" class="confirmajax">' . t('content', '删除') . '</a>'
        ))
      );
    }
    $output .= theme('table', $header, $table, array('class' => 'center_table'));
    $output .= pager_view();
  } else {
    $output .= system_no_content();
  }

  return $output;
}
// }}}
