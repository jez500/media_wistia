<?php

/**
 * @file media_wistia/includes/MediaInternetWistiaHandler.inc
 *
 * Contains MediaInternetWistiaHandler.
 */

/**
 * Implementation of MediaInternetBaseHandler.
 *
 * @see hook_media_internet_providers().
 */
class MediaInternetWistiaHandler extends MediaInternetBaseHandler {

  public function parse($embedCode) {
    $patterns = array(
      '/https?:\/\/(.+)?(wistia\.com|wi\.st)\/(medias|embed)\/(.*)/',
    );
    foreach ($patterns as $pattern) {
      preg_match($pattern, $embedCode, $matches);
      if (isset($matches[4])) {
        return file_stream_wrapper_uri_normalize('wistia://v/' . $matches[4]);
      }
    }
  }

  /**
   * checks to make sure the url is a wistia url
   * @param string $embedCode the wistia url
   * @return bool
   */
  public function claim($embedCode) {
    if ($this->parse($embedCode)) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Gets the wistia file object
   * @param null $uri
   * @return mixed|StdClass
   */
  public function getFileObject($uri = null) {
    if(!$uri) {
      $uri = $this->parse($this->embedCode);
    }
    $file = file_uri_to_object($uri, TRUE);

    if (empty($file->fid) && $info = $this->getOEmbed($uri)) {
      $file->filename = truncate_utf8($info->name, 255);
    }

    return $file;
  }

  /**
   * Returns information about the media. See http://www.oembed.com/.
   *
   * @return
   *   If oEmbed information is available, an array containing 'title', 'type',
   *   'url', and other information as specified by the oEmbed standard.
   *   Otherwise, NULL.
   */
  public function getOEmbed($uri = null) {
    if(!$uri) {
      $uri = $this->parse($this->embedCode);
    }
    $response = file_create_url($uri);
    if (!empty($response)) {
      return $response;
    }
    else {
      throw new Exception("Error Processing Request. (Error: {$response->code}, {$response->error})");
      return;
    }
  }
}
