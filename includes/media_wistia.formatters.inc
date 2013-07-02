<?php

/**
 * @file media_wistia/includes/media_wistia.formatters.inc
 *
 * Formatters for Media: Wistia.
 */

/**
 * Implements hook_file_formatter_info().
 */
function media_wistia_file_formatter_info() {
  $formatters['media_wistia_video'] = array(
    'label' => t('Wistia Video'),
    'file types' => array('video'),
    'default settings' => array(),
    'view callback' => 'media_wistia_file_formatter_video_view',
    'settings callback' => 'media_wistia_file_formatter_video_settings',
  );

  foreach(media_wistia_variable_default() as $setting => $value) {
    $formatters['media_wistia_video']['default settings'][$setting] = media_wistia_variable_get($setting);
  }

  $formatters['media_wistia_image'] = array(
    'label' => t('Wistia Preview Image'),
    'file types' => array('video'),
    'default settings' => array(
      'image_style' => '',
    ),
    'view callback' => 'media_wistia_file_formatter_image_view',
    'settings callback' => 'media_wistia_file_formatter_image_settings',
  );

  return $formatters;
}

/**
 * Implements hook_file_formatter_FORMATTER_view().
 */
function media_wistia_file_formatter_video_view($file, $display, $langcode) {
  $scheme = file_uri_scheme($file->uri);
  // WYSIWYG does not yet support video inside a running editor instance.
  if ($scheme == 'wistia' && empty($file->override['wysiwyg'])) {
    $element = array(
      '#theme' => 'media_wistia_video',
      '#uri' => $file->uri,
      '#options' => array(),
    );

    // Fake a default for attributes so the ternary doesn't choke.
    $display['settings']['attributes'] = array();

    foreach (media_wistia_variable_default() as $setting => $value) {
      $element['#options'][$setting] = isset($file->override[$setting]) ? $file->override[$setting] : $display['settings'][$setting];
    }
    return $element;
  }
}

/**
 * Implements hook_file_formatter_FORMATTER_settings().
 */
function media_wistia_file_formatter_video_settings($form, &$form_state, $settings) {
  $element = array();

  $element['width'] = array(
    '#title' => t('Width'),
    '#type' => 'textfield',
    '#default_value' => $settings['width'],
    '#element_validate' => array('_wistia_validate_video_width_and_height'),
  );
  $element['height'] = array(
    '#title' => t('Height'),
    '#type' => 'textfield',
    '#default_value' => $settings['height'],
    '#element_validate' => array('_wistia_validate_video_width_and_height'),
  );

  // Single Options.
  $element['color'] = array(
    '#title' => t('Controls visible on load'),
    '#type' => module_exists('jquery_colorpicker') ? 'jquery_colorpicker' : 'textfield',
    '#default_value' => $settings['color'],
  );
  $element['autoplay'] = array(
    '#title' => t('Autoplay'),
    '#type' => 'checkbox',
    '#default_value' => $settings['autoplay'],
  );
  $element['end_video'] = array(
    '#title' => t('Autoplay'),
    '#type' => 'radios',
    '#options' => array(
      '' => 'Pause on the last frame',
      'reset' => 'Reset to the first frame',
      'loop' => 'Loop the video',
    ),
    '#default_value' => $settings['end_video'],
  );
  $element['play_button'] = array(
    '#title' => t('Big play button'),
    '#type' => 'checkbox',
    '#default_value' => $settings['play_button'],
  );
  $element['small_play_button'] = array(
    '#title' => t('Small play button'),
    '#type' => 'checkbox',
    '#default_value' => $settings['small_play_button'],
  );
  $element['playbar'] = array(
    '#title' => t('Playbar'),
    '#type' => 'checkbox',
    '#default_value' => $settings['playbar'],
  );
  $element['volume_control'] = array(
    '#title' => t('Volume Control'),
    '#type' => 'checkbox',
    '#default_value' => $settings['volume_control'],
  );
  $element['fullscreen'] = array(
    '#title' => t('Fullscreen button'),
    '#type' => 'checkbox',
    '#default_value' => $settings['fullscreen'],
  );
  $element['controls_on_load'] = array(
    '#title' => t('Controls visible on load'),
    '#type' => 'checkbox',
    '#default_value' => $settings['controls_on_load'],
  );

  $element['protocol_specify'] = array(
    '#title' => t('Specify a http protocol'),
    '#type' => 'checkbox',
    '#default_value' => $settings['protocol_specify'],
    '#description' => t('An explicit protocol may be neccesary for videos embedded in RSS feeds and emails. If no protocol is specified, iframes will be protocol relative.'),
  );
  $element['protocol'] = array(
    '#title' => t('Iframe protocol'),
    '#type' => 'radios',
    '#default_value' => $settings['protocol'],
    '#options' => array(
      'http:' => 'http://',
      'https:' => 'https://',
    ),
    '#states' => array(
      'invisible' => array(
        ':input[name="displays[media_wistia_video][settings][protocol_specify]"]' => array('checked' => FALSE),
      ),
    ),
  );

  return $element;
}

/**
 * Validation for width and height.
 */
function _wistia_validate_video_width_and_height($element, &$form_state, $form) {

  // Check if the value is a number with an optional decimal or percentage sign, or "auto".
  if (!empty($element['#value']) && !preg_match('/^(auto|([0-9]*(\.[0-9]+)?%?))$/', $element['#value'])) {
    form_error($element, t("The value entered for @dimension is invalid. Please insert a unitless integer for pixels, a percent, or \"auto\". Note that percent and auto may not function correctly depending on the browser and doctype.", array('@dimension' => $element['#title'])));
  }
}

/**
 * Implements hook_file_formatter_FORMATTER_view().
 */
function media_wistia_file_formatter_image_view($file, $display, $langcode) {
  $scheme = file_uri_scheme($file->uri);
  if ($scheme == 'wistia') {
    $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);
    $image_style = $display['settings']['image_style'];
    $valid_image_styles = image_style_options(FALSE);

    if (empty($image_style) || !isset($valid_image_styles[$image_style])) {
      $element = array(
        '#theme' => 'image',
        '#path' => $wrapper->getOriginalThumbnailPath(),
        '#alt' => isset($file->override['attributes']['alt']) ? $file->override['attributes']['alt'] : $file->filename,
      );
    }
    else {
      $element = array(
        '#theme' => 'image_style',
        '#style_name' => $image_style,
        '#path' => $wrapper->getLocalThumbnailPath(),
        '#alt' => isset($file->override['attributes']['alt']) ? $file->override['attributes']['alt'] : $file->filename,
      );
    }

    return $element;
  }
}

/**
 * Implements hook_file_formatter_FORMATTER_settings().
 */
function media_wistia_file_formatter_image_settings($form, &$form_state, $settings) {
  $element = array();
  $element['image_style'] = array(
    '#title' => t('Image style'),
    '#type' => 'select',
    '#options' => image_style_options(FALSE),
    '#default_value' => $settings['image_style'],
    '#empty_option' => t('None (original image)'),
  );
  return $element;
}

/**
 * Implements hook_file_default_displays().
 */
function media_wistia_file_default_displays() {

  // Default settings for displaying as a video.
  $default_displays = array();

  $default_video_settings = array(
    'default' => array(
      'width' => 640,
      'height' => 390,
    ),
    'teaser' => array(
      'width' => 560,
      'height' => 340,
      'autoplay' => FALSE,
    ),
    // Legacy view modes included to support older versions of Media,
    // before #1051090 went through. They do no harm and can sit here
    // until there is a Media 2.x stable.
    // @TODO: Remove when Media 2.0 is released.
    'media_large' => array(
      'width' => 560,
      'height' => 340,
      'autoplay' => FALSE,
    ),
    'media_original' => array(
      'width' => 640,
      'height' => 390,
      'autoplay' => media_wistia_variable_get('autoplay'),
    ),
  );
  foreach ($default_video_settings as $view_mode => $settings) {
    $display_name = 'video__' . $view_mode . '__media_wistia_video';
    $default_displays[$display_name] = (object) array(
      'api_version' => 1,
      'name' => $display_name,
      'status' => 1,
      'weight' => 1,
      'settings' => $settings,
    );
  }

  // Default settings for displaying a video preview image.
  // We enable preview images even for view modes that also play video
  // for use inside a running WYSIWYG editor. We weight them so video
  // formatters come first in the cascade to make sure the video formatter
  // is used whenever possible.
  $default_image_styles = array(
    'default' => 'large',
    'preview' => 'square_thumbnail',
    'teaser' => 'large',
    // Legacy view modes, see note above.
    'media_preview' => 'square_thumbnail',
    'media_large' => 'large',
    'media_original' => '',
  );
  foreach ($default_image_styles as $view_mode => $image_style) {
    $display_name = 'video__' . $view_mode . '__media_wistia_image';
    $default_displays[$display_name] = (object) array(
      'api_version' => 1,
      'name' => $display_name,
      'status' => 1,
      'weight' => 2,
      'settings' => array('image_style' => $image_style),
    );
  }

  return $default_displays;
}