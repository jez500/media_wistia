<?php

/**
 *  @file media_wistia/includes/MediaWistiaStreamWrapper.inc
 *
 *  Create a Wistia Stream Wrapper class for the Media/Resource module.
 */

include_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'media_wistia') . '/includes/media_wistia.browser.inc';

include_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'media_wistia') . '/includes/DrupalWistiaApi.php';

/**
 *  Create an instance like this:
 *  $wistia = new MediaWistiaStreamWrapper('wistia://v/[video-code]');
 */
class MediaWistiaStreamWrapper extends MediaReadOnlyStreamWrapper {

  protected $api;

  public function __construct() {
    $this->api = new DrupalWistiaApi(variable_get('media_wistia_api_key'));
  }

  /**
   * Overrides interpolateUrl() defined in MediaReadOnlyStreamWrapper.
   * Checks to see if the video is in our cache, if not we request from the api.
   */
  function interpolateUrl() {
    $media = array();
    if ($parameters = $this->get_parameters()) {
      $media = _media_wistia_get_video_from_cache($parameters['v']);
      if(empty($media)) {
        $media = $this->api->mediaShow($parameters['v']);
      }
    }
    return $media;
  }

  /**
   * Returns the wistia mimetype.
   * @param $uri
   * @param null $mapping
   * @return string
   */
  static function getMimeType($uri, $mapping = NULL) {
    return 'video/wistia';
  }

  /**
   * Gets the original thumbnail for the wistia video
   * @return mixed
   */
  function getOriginalThumbnailPath() {
    $parts = $this->get_parameters();
    $media = $this->api->mediaShow($parts['v']);
    return $media->thumbnail->url;
  }

  /**
   * Returns the url to the thumbnail for the wistia video
   * @return string
   */
  function getLocalThumbnailPath() {
    $parts = $this->get_parameters();
    $local_path = file_default_scheme() . '://media-wistia/' . check_plain($parts['v']) . '.jpg';
    if (!file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
      $response = drupal_http_request($this->getOriginalThumbnailPath());
      if (!isset($response->error)) {
        file_unmanaged_save_data($response->data, $local_path, TRUE);
      }
      else {
        @copy($this->getOriginalThumbnailPath(), $local_path);
      }
    }
    return $local_path;
  }
}
