<?php

// Loading the API functions if not already loaded
module_load_include('inc', 'user_relationships_api', 'user_relationships.api');

function user_relationships_services_types() {
  return user_relationships_types_load();
}

function user_relationships_services_list() {
  try {
    global $user;
    $param = array('user' => $user->uid);
    $rels = user_relationships_load($param, array('include_user_info' => TRUE));
    
    if (!$rels || !is_array($rels)) {
      return array();
    }
    
    $result = array();
    $authorizations = array();
    
    foreach (array_values($rels) as $item) {
      $title = $item->plural_name;
      $status = '';
      
      if ($item->name == 'Contact') {
        $title = $item->requester_id == $user->uid ? 'Sent requests' : 'Received requests';
        // $title = t('Contacts');
        // if ($item->requester_id == $user->uid) {
          $status = t('Request pending');
        // }
      }
      
      if ($item->name == 'Contact' && $item->approved == 1) {
        $title = 'Contacts';
        $status = t('Idle');
      }
      
      $relative_user = $item->requester_id == $user->uid ? $item->requestee : $item->requester;
      
      if ($item->name == 'Authorization' && $item->approved == 1) {
        $authorizations[] = $relative_user->uid;
      }
      
      unset($relative_user->data);
      
      $result[$title][] = array(
        'rid' => $item->rid,
        'user' => $relative_user,
        'status' => $status,
      );
    }
    
    if (is_array($result['Contacts'])) {
      foreach ($result['Contacts'] as $key => $contact) {
        if (in_array($contact['user']->uid, $authorizations)) {
          $result['Contacts'][$key]['authorized'] = TRUE;
          $result['Contacts'][$key]['status'] = t('Available');
        }
        else {
          $result['Contacts'][$key]['authorized'] = TRUE;
        }
      }
    }
    
    // ksort($result);
    
    $output = array();
    
    if (is_array($result['Received requests'])) {
      $output['Received requests'] = $result['Received requests'];
    }
    
    if (is_array($result['Sent requests'])) {
      $output['Sent requests'] = $result['Sent requests'];
    }
    
    if (is_array($result['Contacts'])) {
      $output['Contacts'] = $result['Contacts'];
    }
    
    return $output;
  }
  catch (Exception $ex) {
    return services_error(
      t('Error updating contacts: @msg', array('@msg' => $ex->getMessage()))
    );
  }
}

function user_relationships_services_approve($rid) {
  try {
    global $user;
    
    $rels = user_relationships_load(array('rid' => $rid , 'requestee_id' => $user->uid, 'approved' => 0));
    
    if (!$rels || !is_array($rels) || count($rels) != 1) {
      // throw new Exception("User relationship load failed.");
      return array();
    }
    
    $rel = array_shift($rels);
    
    if ($rel->requestee_id != $user->uid) {
      throw new Exception("Access denied.");
    }
    
    user_relationships_save_relationship($rel, 'approve');
    
    return $rel;
  }
  catch (Exception $ex) {
    return services_error(
      t('Error approving relationship: @msg', array('@msg' => $ex->getMessage()))
    );
  }
}

function user_relationships_services_delete($rid, $reason) {
  try {
    global $user;
    
    $rels = user_relationships_load(array('rid' => $rid , 'user' => $user->uid));
    
    if (!$rels || !is_array($rels) || count($rels) != 1) {
      // throw new Exception("User relationship load failed");
      return array();
    }
    
    $rel = array_shift($rels);
    
    if ($rel->requestee_id != $user->uid && $rel->requester_id != $user->uid) {
      throw new Exception("Access denied");
    }
    
    user_relationships_delete_relationship($rel, $user, $reason);
    
    return $rel;
  }
  catch (Exception $ex) {
    return services_error(
      t('Error removing relationship: @msg',
      array('@msg' => $ex->getMessage()))
    );
  }
}

function user_relationships_services_request($mail, $type_name) {
  try {
    if (strlen($mail) == 0) {
      throw new Exception(t('You must enter an e-mail address.'));
    }
    
    if ($error = user_validate_mail($mail)) {
      throw new Exception($error);
    }
    
    global $user;
    
    if ($mail == $user->mail) {
      throw new Exception(t('You cannot add yourself as a contact.'));
    }
    
    $type = user_relationships_type_load(array('name' => $type_name));
    
    if (!$type) {
      throw new Exception(t('No such relationship type.'));
    }
    
    $requestee = user_load(array('mail' => $mail));
    
    if (!$requestee) {
      global $language;
                  
      $subject = invite_get_subject();
      $message = NULL;

      if (!variable_get('invite_use_users_email', 0)) {
        $from = variable_get('invite_manual_from', '');
      }
      else if ($user->uid) {
        $from = $user->mail;
      }
      
      if (!$from) {
        // Never pass an empty string to drupal_mail()
        $from = NULL;
      }

      $code = invite_generate_code();

      $invite = _invite_substitutions(array(
        'email' => $mail,
        'code'  => $code,
        'resent'  => 0,
        'data'  => array('subject' => $subject, 'message' => $message),
      ));

    
      // Send e-mail.
      $params = array('invite' => $invite);
      $result = drupal_mail('invite', 'invite', $mail, $language, $params, $from, TRUE);

      $args = array('inviter' => $invite->inviter, 'email' => $invite->email, 'code' => $invite->code, 'rtype' => $type);
      module_invoke_all('invite', 'invite', $args);

      return t("!mail has been invited by email.", array('!mail' => $mail));
    }
    
    $ret = user_relationships_request_relationship($user, $requestee->uid, $type);
    
    if ($ret === FALSE) {
      throw new Exception(t("Access denied."));
    }
    // else if (!is_object($ret)) {
    //   throw new Exception($ret);
    // }
    
    return t("Request sent successfully.");
  }
  catch (Exception $ex) {
    return services_error(t('@msg', array('@msg' => $ex->getMessage())));
  }
}

function user_relationships_services_remind($rid) {
  try {
    global $user;
    
    $rels = user_relationships_load(array('rid' => $rid, 'requester_id' => $user->uid, 'approved' => 0), array('include_user_info' => TRUE));
    
    if (!$rels || !is_array($rels) || count($rels) != 1) {
      // throw new Exception("User relationship load failed.");
      return array();
    }
    
    $rel = array_shift($rels);
    
    if ($rel->requester_id != $user->uid) {
      throw new Exception("Access denied.");
    }
    
    // $requestee = user_load(array('mail' => $item->requestee->mail));
    // 
    // if (!$requestee) {
    //   // TODO: Send invite mail.
    //   return;// t("!mail has been invited by email.", array('!mail' => $mail));
    // }
    
    _user_relationships_invoke('request', $rel);
    
    return array();
    // return $rel;
  }
  catch (Exception $ex) {
    return services_error(
      t('Error reminding user: @msg', array('@msg' => $ex->getMessage()))
    );
  }
}
