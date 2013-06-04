<?php

/**
 * @file media_wistia/includes/themes/media_wistia.theme.inc
 *
 * Theme and preprocess functions for Media: Wistia.
 */

/**
 * Preprocess function for theme('media_wistia_video').
 */
function media_wistia_preprocess_media_wistia_video(&$variables) {

  // Build the URI.
  $wrapper = file_stream_wrapper_get_instance_by_uri($variables['uri']);
  $parts = $wrapper->get_parameters();
  $variables['video_id'] = check_plain($parts['v']);

  // Add some options as their own template variables.
  foreach (array('width', 'height') as $theme_var) {
    $variables[$theme_var] = $variables['options'][$theme_var];
  }

  // Do something useful with the overridden attributes from the file
  // object. We ignore alt and style for now.
  if (isset($variables['options']['attributes']['class'])) {
    if (is_array($variables['options']['attributes']['class'])) {
      $variables['classes_array'] = array_merge($variables['classes_array'], $variables['options']['attributes']['class']);
    }
    else {
      // Provide nominal support for Media 1.x.
      $variables['classes_array'][] = $variables['options']['attributes']['class'];
    }
  }

  if ($variables['options']['protocol_specify']) {
    $protocol = $variables['options']['protocol'];
  }
  else {
    $protocol = '//';
  }

  // Make the file object available.
  $file_object = file_uri_to_object($variables['uri']);

  // Add template variables for accessibility.
  $variables['title'] = check_plain($file_object->filename);
  // @TODO: Find an easy/ not too expensive way to get the Wistia description
  // to use for the alternative content.
  $variables['alternative_content'] = t('Video of @title', array('@title' => $variables['title']));

  $query = array(
    'version' => 'v1',
    'playButton' => $variables['options']['play_button'] ? 'true' : 'false',
    'smallPlayButton' => $variables['options']['small_play_button'] ? 'true' : 'false',
    'playbar' => $variables['options']['playbar'] ? 'true' : 'false',
    'controlsVisibleOnLoad' => $variables['options']['controls_on_load'] ? 'true' : 'false',
    'videoHeight' => $variables['options']['height'],
    'videoWidth' => $variables['options']['width'],
    'volumeControl' => $variables['options']['volume_control'] ? 'true' : 'false',
    'fullscreenButton' => $variables['options']['fullscreen'] ? 'true' : 'false',
    'autoPlay' => $variables['options']['autoplay'] ? 'true' : 'false',
    'playerColor' => $variables['options']['color'],
    'endVideoBehavior' => $variables['options']['end_video'],
  );

  $url = $protocol . 'fast.wistia.net/embed/iframe/' . $variables['video_id'] . '?' . http_build_query($query);

  drupal_alter('media_wistia_iframe', $url);

  // Build the iframe URL with options query strings
  $variables['url'] = $url;
}