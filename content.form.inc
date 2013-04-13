<?php
// $Id$

// public content_form($content = NULL, $type = NULL) {{{ 
/**
 * content_form
 *  内容表单
 * 
 * @param object $content 
 *  一条内容记录，若有，则编辑该内容
 * @param object $type 
 *  一个内容类型对象，在新增内容时，必需传递此值
 * @access public
 * @return array
 */
function content_form($type, $content = NULL) {
  if (empty($type) && empty($content->type)) {
    dd_set_message(t('content', '无法添加内容，请先选择内容类型'), 'error');
    return false;
  }

  if (empty($content->type)) $content->type = $type->type;

  if (!$data = content_type_load($content->type)) {
    dd_set_message(t('content', '不正确的内容类型'), 'error');
    return false;
  }
  
  // 隐藏其它内容类型的编辑器实例
  global $editor_disabled;

  foreach (content_get_type_by_type() as $key => $_v) {
    if ($key != $content->type) {
      $editor_disabled['content_' . $key] = true;
    }
  }
  
  $tabs = array();

  if (user_access('admin content view')) {
    $tabs[] = l(t('content', '!name管理', array('!name' => $data->name)), 'admin/content/list/' . $data->type);
  }

  if (empty($content->nid)) {
    $tabs[] = l(t('content', '添加内容'), 'content/add/' . $content->type);
  } else {
    $tabs[] = l(t('content', '编辑内容'), 'content/' . $content->type . '/' . $content->nid . '/edit');
    $tabs[] = l(t('content', '浏览内容'), 'content/' . $content->type . '/' . $content->nid);
  }

  dd_set_tabs($tabs);

  $form = array(
    'fields' => array(),
    'settings' => array(
      '#ajax_validate' => 1,
      '#attributes' => array('class' => 'content_form_type_' . $data->type),
      '#validate' => array('content_form_validate'),
      '#redirect' => !empty($content->redirect) ? $content->redirect : NULL
    )
  );

  $form['fields']['title'] = array(
    '#title' => $type->data['title'],
    '#type' => 'textfield',
    '#weight' => -100,
    '#default_value' => $content->title,
    '#required' => 1,
  );
   
  $form['fields']['type'] = array(
    '#type' => 'hidden',
    '#default_value' => $content->type,
    '#constant' => 1,
  );
  
  $form['fields']['nid'] = array(
    '#type' => 'hidden',
    '#default_value' => !empty($content->nid) ? $content->nid : 0,
    '#constant' => 1,
  );
   
  $form['fields']['uid'] = array(
    '#type' => 'hidden',
    '#default_value' => !empty($content->uid) ? $content->uid : $GLOBALS['user']->uid,
    '#constant' => 1,
  );
  
  $form['fields']['body'] = array(
    '#title' => $type->data['body'],
    '#type' => 'textarea',
    '#default_value' => $content->body,
    '#required' => 1,
    '#weight' => 100,
    '#attributes' => array(
      'id' => 'content_form_type_body',
    )
  );
   
  $form['fields']['format'] = filter_form_field($content->format, 1);
  $form['fields']['format']['#weight'] = 105;

  $form['fields']['status'] = array(
    '#type' => 'radio',
    '#title' => t('content', '状态'),
    '#default_value' => isset($content->status) ? $content->status : 1,
    '#required' => 1,
    '#weight' => 110,
    '#options' => array(t('content', '暂不发布'), t('content', '立即发布'))
  );
  
  field_get_node_form('content', 'ext_' . $content->type, $content, $form);

  module_alter_all('content_form', $form, $content);
  
  $form['fields']['button'] = array(
    '#type' => 'submit',
    '#weight' => 150,
    '#submit' => 'content_form_submit'
  );
   
  return $form;
}
// }}}

function content_form_validate(&$form, &$v) {
  return content_validate($v['_dida_values'], $form);
}

function content_form_submit(&$form, &$v) {
  if (content_save($v['_dida_values'], 0)) {
    // 将文件关联到内容
    db_exec('UPDATE {files} SET ext_type = ?, ext_id = ?, status = 1 WHERE ext_type = ?', 
    array('content_' . $v['_dida_values']->type, $v['_dida_values']->nid, $form['#form_token']));
    dd_set_message(t('content', '操作成功'));
  } else {
    dd_set_message(t('content', '抱歉，出现异常，无法保存'), 'error');
  }
}

// public content_delete_form($content) {{{ 
/**
 * content_delete_form
 * 删除内容表单
 * 
 * @param object $content 
 * @access public
 * @return array
 */
function content_delete_form($content) {

}
// }}}
