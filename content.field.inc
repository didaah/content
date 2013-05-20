<?php
// $Id$

// public content_admin_field() {{{ 
/**
 * content_admin_field
 * 内容类型管理
 * 
 * @access public
 * @return string
 */
function content_admin_type() {
  $tabs = array(
    l(t('content', '内容类型'), 'admin/content/type'),
    l(t('content', '添加类型'), 'admin/content/type/add'),
  );
  
  $args = arg();
  
  switch ($args[3]) {
    case 'fields':
      $field = new stdClass;
      if (!empty($args[4]) && $type = content_type_load($args[4])) {
        $field->type = 'ext_' . $type->type;
      } else {
        dd_set_message(t('content', '错误的请求'));
        dd_goto('admin/content/type');
      }

      $tabs[] = l(t('content', '!name属性管理', array('!name' => $type->name)), 'admin/content/type/fields/' . $type->type);
      $tabs[] = l(t('content', '!name属性添加', array('!name' => $type->name)), 'admin/content/type/fields/' . $type->type . '/add');
      $tabs[] = l(t('content', '!name属性导出', array('!name' => $type->name)), 'admin/content/type/fields/' . $type->type . '/export');
      $tabs[] = l(t('content', '!name属性分组', array('!name' => $type->name)), 'admin/content/type/fields/' . $type->type . '/groups');
      
      switch ($args[5]) {
        case 'add':
          $field->module = 'content';
          $field->redirect = url('admin/content/type/fields/' . $type->type);
          $output = dd_get_form('field_form', $field);
        break;
        
        case 'export':
          $output = field_export(0, 'content', $field->type);
        break;
        
        case 'groups':
          $output = dd_get_form('field_groups', 'content', $field->type);
        break;

        default:
          $output = field_lists('content', $field->type);
      }
    break;
    
    case 'add': case 'edit': // 添加/编辑内容类型
      if (!empty($args[4]) && is_numeric($args[4])) {
        $type = content_type_load($args[4]);
        $tabs[] = l(t('content', '编辑类型'), $_GET['q']);
      } else {
        $type = NULL;
      }
      $output = dd_get_form('content_admin_type_form', $type);
    break;
    
    case 'delete': // 删除内容类型
      if ($args[4] && $type = content_type_load($args[4])) {
        if (db_query('SELECT nid FROM {content_ext_' . $type->type . '}',
          array(), array('return' => 'column', 'limit' => 1))) {
          echo t('content', '该类型不为空，无法删除。请先删除类型下的所有内容，再尝试删除');
        } else if (content_type_delete($type)) {
          // ajax 返回值
          echo 'tr';
        } else {
          echo t('content', '删除失败');
        }
      } else {
        echo t('content', '不合法的操作');
      }
    exit;
    
    default:
      if ($fetch = db_query('SELECT * FROM {content_type} ORDER BY tid ASC')) {
        $table = array();
        foreach ($fetch as $o) {
          $o->data = unserialize($o->data);
          $links = array(
            l(t('content', '编辑'), 'admin/content/type/edit/' . $o->tid),
            l(t('content', '删除'), 'admin/content/type/delete/' . $o->tid, array('attributes' => array('class' => 'confirmajax'))),
            l('属性管理', 'admin/content/type/fields/' . $o->type)
          );
          $table[] = array(
            l($o->name, 'content/type/' . $o->type),
            $o->type,
            $o->count,
            $o->data['comment'] ? t('content', '是') : t('content', '否'),
            dd_imp($links)
          );
        }
    
        $header = array(
          t('content', '类型名称'),
          t('content', '系统名称'),
          t('content', '内容数量'),
          t('content', '允许评论'),
          t('content', '操作'),
        );

        $output = theme('table', $header, $table, array('class' => 'center_table'));
        $output .= pager_view();
      } else {
        $output = system_no_content();
      }
  }
  
  dd_set_tabs($tabs);
  return $output;
}
// }}}

// public content_admin_type_form($type = NULL) {{{ 
/**
 * content_admin_type_form
 *  内容类型添加/编辑表单
 * 
 * @param object $type 
 * @access public
 * @return array
 */
function content_admin_type_form($type = NULL) {
  $form = array(
    'fields' => array(),
    'settings' => array(
      '#validate' => array('content_admin_type_form_validate'),
      '#ajax_validate' => 1
    )
  );
 
  $form['fields']['name'] = array(
    '#type' => 'textfield',
    '#required' => 1,
    '#default_value' => $type->name,
    '#title' => t('content', '类型名称')
  );
  
  if (!empty($type->type)) {
    $form['fields']['type'] = array(
      '#value' => $type->type,
      '#weight' => -105,
      '#title' => t('content', '系统名称')
    );
  } else {
    $form['fields']['type'] = array(
      '#type' => 'textfield',
      '#required' => 1,
      '#default_value' => $type->type,
      '#description' => t('content', '只能是数字、小写字母，长度 12 个字符内，设定后无法修改'),
      '#title' => t('content', '系统名称')
    );
  }
  
  $form['fields']['data']['comment'] = array(
    '#type' => 'radio',
    '#title' => t('content', '允许评论'),
    '#default_value' => !empty($type->data) ? $type->data['comment'] : 1,
    '#required' => 1,
    '#options' => array(t('content', '否'), t('content', '是'))
  );
 
  $form['fields']['data']['visit'] = array(
    '#type' => 'radio',
    '#title' => t('content', '浏览计数'),
    '#default_value' => !empty($type->data) ? $type->data['visit'] : 0,
    '#required' => 1,
    '#description' => t('content', '每访问一次，计数 +1，将加重数据库负担'),
    '#options' => array(t('content', '否'), t('content', '是'))
  );
  
  $form['fields']['data']['title'] = array(
    '#type' => 'textfield',
    '#required' => 1,
    '#title' => t('content', 'Title 名称'),
    '#default_value' => !empty($type->data) ? $type->data['title'] : t('content', '标题'),
    '#description' => t('content', '内容表单 title 字段的可视名称，默认为：标题')
  );
   
  $form['fields']['data']['body'] = array(
    '#type' => 'textfield',
    '#required' => 1,
    '#title' => t('content', 'Body 名称'),
    '#default_value' => !empty($type->data) ? $type->data['body'] : t('content', '内容'),
    '#description' => t('content', '内容表单 body 字段的可视名称，默认为：内容')
  );
  
  $form['fields']['data']['meta'] = dd_get_form_meta('asc', $type->data ? $type->data['meta'] : array());

  $form['fields']['tid'] = array(
    '#type' => 'hidden',
    '#default_value' => !empty($type->tid) ? $type->tid : 0,
    '#constant' => 1
  );
   
  $form['fields']['button'] = array(
    '#type' => 'submit',
    '#submit' => 'content_admin_type_form_submit'
  );
  
  return $form;
}
// }}}

function content_admin_type_form_validate(&$form, &$v) {
  return content_type_validate($v['_dida_values'], $form);
}

function content_admin_type_form_submit(&$form, &$v) {
  if (content_type_save($v['_dida_values'], $form)) {
    if (empty($form['setting']['#redirect'])) $form['settings']['#redirect'] = url('admin/content/type');
  }
}

