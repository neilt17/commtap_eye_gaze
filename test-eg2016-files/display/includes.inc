<?php

function url($path = NULL, $options = array()) {
  // Merge in defaults.
  $options += array(
    'fragment' => '',
    'query' => '',
    'absolute' => FALSE,
    'alias' => FALSE,
    'prefix' => ''
  );
  // A duplicate of the code from menu_path_is_external() to avoid needing
  // another function call, since performance inside url() is critical.
  if (!isset($options['external'])) {
    // Return an external link if $path contains an allowed absolute URL. Avoid
    // calling filter_xss_bad_protocol() if there is any slash (/), hash (#) or
    // question_mark (?) before the colon (:) occurrence - if any - as this
    // would clearly mean it is not a URL. If the path starts with 2 slashes
    // then it is always considered an external URL without an explicit protocol
    // part.
    $colonpos = strpos($path, ':');
    $options['external'] = (strpos($path, '//') === 0)
      || ($colonpos !== FALSE
        && !preg_match('![/?#]!', substr($path, 0, $colonpos))
        && filter_xss_bad_protocol($path, FALSE) == check_plain($path));
  }

  if ($options['fragment']) {
    $options['fragment'] = '#' . $options['fragment'];
  }
  if (is_array($options['query'])) {
    $options['query'] = drupal_query_string_encode($options['query']);
  }

  if ($options['external']) {
    // Split off the fragment.
    if (strpos($path, '#') !== FALSE) {
      list($path, $old_fragment) = explode('#', $path, 2);
      if (isset($old_fragment) && !$options['fragment']) {
        $options['fragment'] = '#' . $old_fragment;
      }
    }
    // Append the query.
    if ($options['query']) {
      $path .= (strpos($path, '?') !== FALSE ? '&' : '?') . $options['query'];
    }
    // Reassemble.
    return $path . $options['fragment'];
  }

  // Strip leading slashes from internal paths to prevent them becoming external
  // URLs without protocol. /example.com should not be turned into
  // //example.com.
  $path = ltrim($path, '/');

  global $base_url;
  static $script;

  if (!isset($script)) {
    // On some web servers, such as IIS, we can't omit "index.php". So, we
    // generate "index.php?q=foo" instead of "?q=foo" on anything that is not
    // Apache.
    $script = (strpos($_SERVER['SERVER_SOFTWARE'], 'Apache') === FALSE) ? 'index.php' : '';
  }

  if (!isset($options['base_url'])) {
    // The base_url might be rewritten from the language rewrite in domain mode.
    $options['base_url'] = $base_url;
  }

  // Preserve the original path before aliasing.
  $original_path = $path;

  // The special path '<front>' links to the default front page.
  if ($path == '<front>') {
    $path = '';
  }

  $base = $options['absolute'] ? $options['base_url'] . '/' : base_path();
  $prefix = empty($path) ? rtrim($options['prefix'], '/') : $options['prefix'];
  $path = drupal_urlencode($prefix . $path);

  // with clean urls:
  if ($options['query']) {
    return $base . $path .'?'. $options['query'] . $options['fragment'];
  }
  else {
    return $base . $path . $options['fragment'];
  }

}

function filter_xss_bad_protocol($string, $decode = TRUE) {
  static $allowed_protocols;
  if (!isset($allowed_protocols)) {
    $allowed_protocols = array_flip(array('http', 'https', 'ftp', 'news', 'nntp', 'tel', 'telnet', 'mailto', 'irc', 'ssh', 'sftp', 'webcal', 'rtsp'));
  }

  // Get the plain text representation of the attribute value (i.e. its meaning).
  if ($decode) {
    $string = decode_entities($string);
  }

  // Iteratively remove any invalid protocol found.

  do {
    $before = $string;
    $colonpos = strpos($string, ':');
    if ($colonpos > 0) {
      // We found a colon, possibly a protocol. Verify.
      $protocol = substr($string, 0, $colonpos);
      // If a colon is preceded by a slash, question mark or hash, it cannot
      // possibly be part of the URL scheme. This must be a relative URL,
      // which inherits the (safe) protocol of the base document.
      if (preg_match('![/?#]!', $protocol)) {
        break;
      }
      // Per RFC2616, section 3.2.3 (URI Comparison) scheme comparison must be case-insensitive
      // Check if this is a disallowed protocol.
      if (!isset($allowed_protocols[strtolower($protocol)])) {
        $string = substr($string, $colonpos + 1);
      }
    }
  } while ($before != $string);
  return check_plain($string);
}

function decode_entities($text, $exclude = array()) {
  static $html_entities;
  if (!isset($html_entities)) {
    include_once 'unicode.entities.inc';
  }

  // Flip the exclude list so that we can do quick lookups later.
  $exclude = array_flip($exclude);

  // Use a regexp to select all entities in one pass, to avoid decoding
  // double-escaped entities twice. The PREG_REPLACE_EVAL modifier 'e' is
  // being used to allow for a callback (see
  // http://php.net/manual/en/reference.pcre.pattern.modifiers).
  return preg_replace('/&(#x?)?([A-Za-z0-9]+);/e', '_decode_entities("$1", "$2", "$0", $html_entities, $exclude)', $text);
}

function check_plain($text) {
  static $php525;

  if (!isset($php525)) {
    $php525 = version_compare(PHP_VERSION, '5.2.5', '>=');
  }
  // We duplicate the preg_match() to validate strings as UTF-8 from
  // drupal_validate_utf8() here. This avoids the overhead of an additional
  // function call, since check_plain() may be called hundreds of times during
  // a request. For PHP 5.2.5+, this check for valid UTF-8 should be handled
  // internally by PHP in htmlspecialchars().
  // @see http://www.php.net/releases/5_2_5.php
  // @todo remove this when support for either IE6 or PHP < 5.2.5 is dropped.

  if ($php525) {
    return htmlspecialchars($text, ENT_QUOTES, 'UTF-8');
  }
  return (preg_match('/^./us', $text) == 1) ? htmlspecialchars($text, ENT_QUOTES, 'UTF-8') : '';
}

function drupal_query_string_encode($query, $exclude = array(), $parent = '') {
  $params = array();

  foreach ($query as $key => $value) {
    $key = rawurlencode($key);
    if ($parent) {
      $key = $parent . '[' . $key . ']';
    }

    if (in_array($key, $exclude)) {
      continue;
    }

    if (is_array($value)) {
      $params[] = drupal_query_string_encode($value, $exclude, $key);
    }
    else {
      $params[] = $key . '=' . rawurlencode($value);
    }
  }

  return implode('&', $params);
}

function conf_init() {
  global $base_url, $base_path, $base_root;

  // Export the following settings.php variables to the global namespace
  global $db_url, $db_prefix, $db_collation, $cookie_domain, $conf, $installed_profile, $update_free_access;
  $conf = array();

  if (!isset($_SERVER['SERVER_PROTOCOL']) || ($_SERVER['SERVER_PROTOCOL'] != 'HTTP/1.0' && $_SERVER['SERVER_PROTOCOL'] != 'HTTP/1.1')) {
    $_SERVER['SERVER_PROTOCOL'] = 'HTTP/1.0';
  }

  if (isset($_SERVER['HTTP_HOST'])) {
    // As HTTP_HOST is user input, ensure it only contains characters allowed
    // in hostnames. See RFC 952 (and RFC 2181).
    // $_SERVER['HTTP_HOST'] is lowercased here per specifications.
    $_SERVER['HTTP_HOST'] = strtolower($_SERVER['HTTP_HOST']);
    if (!drupal_valid_http_host($_SERVER['HTTP_HOST'])) {
      // HTTP_HOST is invalid, e.g. if containing slashes it may be an attack.
      header($_SERVER['SERVER_PROTOCOL'] . ' 400 Bad Request');
      exit;
    }
  }
  else {
    // Some pre-HTTP/1.1 clients will not send a Host header. Ensure the key is
    // defined for E_ALL compliance.
    $_SERVER['HTTP_HOST'] = '';
  }


  if (isset($base_url)) {
    // Parse fixed base URL from settings.php.
    $parts = parse_url($base_url);
    if (!isset($parts['path'])) {
      $parts['path'] = '';
    }
    $base_path = $parts['path'] . '/';
    // Build $base_root (everything until first slash after "scheme://").
    $base_root = substr($base_url, 0, strlen($base_url) - strlen($parts['path']));
  }
  else {
    // Create base URL
    $base_root = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] == 'on') ? 'https' : 'http';

    $base_url = $base_root .= '://' . $_SERVER['HTTP_HOST'];

    // $_SERVER['SCRIPT_NAME'] can, in contrast to $_SERVER['PHP_SELF'], not
    // be modified by a visitor.
    if ($dir = trim(dirname($_SERVER['SCRIPT_NAME']), '\,/')) {
      $base_path = "/$dir";
      $base_url .= $base_path;
      $base_path .= '/';
    }
    else {
      $base_path = '/';
    }
  }

  if ($cookie_domain) {
    // If the user specifies the cookie domain, also use it for session name.
    $session_name = $cookie_domain;
  }
  else {
    // Otherwise use $base_url as session name, without the protocol
    // to use the same session identifiers across HTTP and HTTPS.
    list(, $session_name) = explode('://', $base_url, 2);
    // We escape the hostname because it can be modified by a visitor.
    if (!empty($_SERVER['HTTP_HOST'])) {
      $cookie_domain = check_plain($_SERVER['HTTP_HOST']);
      // Strip leading periods, www., and port numbers from cookie domain.
      $cookie_domain = ltrim($cookie_domain, '.');
      if (strpos($cookie_domain, 'www.') === 0) {
        $cookie_domain = substr($cookie_domain, 4);
      }
      $cookie_domain = explode(':', $cookie_domain);
      $cookie_domain = '.' . $cookie_domain[0];
    }
  }
  // To prevent session cookies from being hijacked, a user can configure the
  // SSL version of their website to only transfer session cookies via SSL by
  // using PHP's session.cookie_secure setting. The browser will then use two
  // separate session cookies for the HTTPS and HTTP versions of the site. So we
  // must use different session identifiers for HTTPS and HTTP to prevent a
  // cookie collision.
  if (ini_get('session.cookie_secure')) {
    $session_name .= 'SSL';
  }
  // Per RFC 2109, cookie domains must contain at least one dot other than the
  // first. For hosts such as 'localhost' or IP Addresses we don't set a cookie domain.
  if (count(explode('.', $cookie_domain)) > 2 && !is_numeric(str_replace('.', '', $cookie_domain))) {
    ini_set('session.cookie_domain', $cookie_domain);
  }
  session_name('SESS' . md5($session_name));
}

function drupal_valid_http_host($host) {
  // Limit the length of the host name to 1000 bytes to prevent DoS attacks with
  // long host names.
  return strlen($host) <= 1000
  // Limit the number of subdomains and port separators to prevent DoS attacks
  // in conf_path().
  && substr_count($host, '.') <= 100
  && substr_count($host, ':') <= 100
  && preg_match('/^\[?(?:[a-zA-Z0-9-:\]_]+\.?)+$/', $host);
}

function drupal_urlencode($text) {
  return str_replace('%2F', '/', rawurlencode($text));
}

function l($text, $path, $options = array()) {
  global $language;

  // Merge in defaults.
  $options += array(
    'attributes' => array(),
    'html' => FALSE,
  );

  // Remove all HTML and PHP tags from a tooltip. For best performance, we act only
  // if a quick strpos() pre-check gave a suspicion (because strip_tags() is expensive).
  if (isset($options['attributes']['title']) && strpos($options['attributes']['title'], '<') !== FALSE) {
    $options['attributes']['title'] = strip_tags($options['attributes']['title']);
  }

  return '<a href="' . check_url(url($path, $options)) . '"' . drupal_attributes($options['attributes']) . '>' . ($options['html'] ? $text : check_plain($text)) . '</a>';
}

function check_url($uri) {
  return filter_xss_bad_protocol($uri, FALSE);
}


function base_path() {
  return $GLOBALS['base_path'];
}

/**
 * Format an attribute string to insert in a tag.
 *
 * @param $attributes
 *   An associative array of HTML attributes.
 * @return
 *   An HTML string ready for insertion in a tag.
 */
function drupal_attributes($attributes = array()) {
  if (is_array($attributes)) {
    $t = '';
    foreach ($attributes as $key => $value) {
      $t .= " $key=".'"'. check_plain($value) .'"';
    }
    return $t;
  }
}
