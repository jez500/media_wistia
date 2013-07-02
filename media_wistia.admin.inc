<?php

function media_wistia_global_settings() {

  $form = array();

  $form['media_wistia_api_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Wistia Api Url'),
    '#default_value' => variable_get('media_wistia_api_url', MEDIA_WISTIA_DEFAULT_API_URL),
  );

  $form['media_wistia_oembed_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Wistia oEmbed Url'),
    '#default_value' => variable_get('media_wistia_oembed_url', MEDIA_WISTIA_DEFAULT_OEMBED_URL),
  );

  $form['media_wistia_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Wistia Api Key'),
    '#default_value' => variable_get('media_wistia_api_key'),
  );

  return system_settings_form($form);

}