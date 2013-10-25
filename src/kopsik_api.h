// Copyright 2013 Tanel Lebedev

#ifndef SRC_KOPSIK_API_H_
#define SRC_KOPSIK_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>

#define KOPSIK_EXPORT

#define KOPSIK_ERR_LEN 1024

typedef int kopsik_api_result;
#define KOPSIK_API_SUCCESS 0

// Context API.

KOPSIK_EXPORT void *kopsik_context_init(
  const char *app_name, const char *app_version);

KOPSIK_EXPORT void kopsik_context_clear(void *ctx);

// Configuration API

typedef struct {
    int UseProxy;
    char *Host;
    unsigned int Port;
    char *Username;
    char *Password;
} KopsikProxySettings;

KOPSIK_EXPORT kopsik_api_result kopsik_get_proxy(void *ctx,
    char *errmsg,
    unsigned int errlen,
    KopsikProxySettings *settings);

KOPSIK_EXPORT KopsikProxySettings *kopsik_proxy_settings_init();

KOPSIK_EXPORT void kopsik_proxy_settings_clear(KopsikProxySettings *settings);

KOPSIK_EXPORT kopsik_api_result kopsik_set_proxy(void *ctx,
  char *errmsg,
  unsigned int errlen,
  const int use_proxy,
  const char *host,
  const unsigned int port,
  const char *username,
  const char *password);

// For mock testing only
KOPSIK_EXPORT void kopsik_test_set_https_client(void *context,
  void *client);

KOPSIK_EXPORT void kopsik_set_db_path(void *ctx,
  const char *path);

KOPSIK_EXPORT void kopsik_set_log_path(void *ctx,
  const char *path);

KOPSIK_EXPORT void kopsik_set_log_level(void *ctx,
  const char *level);

KOPSIK_EXPORT void kopsik_set_api_url(void *ctx,
  const char *api_url);

KOPSIK_EXPORT void kopsik_set_websocket_url(void *ctx,
  const char *websocket_url);

// User API

typedef struct {
    unsigned int ID;
    char *Fullname;
} KopsikUser;

KOPSIK_EXPORT KopsikUser *kopsik_user_init();

KOPSIK_EXPORT void kopsik_user_clear(KopsikUser *user);

KOPSIK_EXPORT kopsik_api_result kopsik_set_api_token(
  void *ctx,
  char *errmsg, unsigned int errlen,
  const char *api_token);

KOPSIK_EXPORT kopsik_api_result kopsik_get_api_token(
  void *ctx,
  char *errmsg, unsigned int errlen,
  char *str, unsigned int max_strlen);

KOPSIK_EXPORT kopsik_api_result kopsik_current_user(
  void *ctx,
  char *errmsg, unsigned int errlen,
  KopsikUser *user);

KOPSIK_EXPORT kopsik_api_result kopsik_login(
  void *ctx,
  char *errmsg, unsigned int errlen,
  const char *email, const char *password);

KOPSIK_EXPORT kopsik_api_result kopsik_logout(
  void *ctx,
  char *errmsg, unsigned int errlen);

// Sync

typedef struct {
  unsigned int TimeEntries;
} KopsikPushableModelStats;

KOPSIK_EXPORT kopsik_api_result kopsik_sync(
  void *ctx,
  char *errmsg, unsigned int errlen,
  int full_sync);

KOPSIK_EXPORT kopsik_api_result kopsik_push(
  void *ctx,
  char *errmsg, unsigned int errlen);

KOPSIK_EXPORT kopsik_api_result kopsik_pushable_models(
  void *ctx,
  char *errmsg, unsigned int errlen,
  KopsikPushableModelStats *stats);

typedef void (*KopsikResultCallback)(
  kopsik_api_result result,
  const char *errmsg);

KOPSIK_EXPORT void kopsik_sync_async(
  void *ctx,
  int full_sync,
  KopsikResultCallback callback);

KOPSIK_EXPORT void kopsik_push_async(
  void *ctx,
  KopsikResultCallback callback);

// Project list

typedef struct {
  char *Name;
} KopsikProjectSelectItem;

typedef struct {
  KopsikProjectSelectItem **ViewItems;
  unsigned int Length;
} KopsikProjectSelectItemList;

KOPSIK_EXPORT KopsikProjectSelectItemList *
  kopsik_project_select_item_list_init();

KOPSIK_EXPORT void kopsik_project_select_item_list_clear(
  KopsikProjectSelectItemList *list);

KOPSIK_EXPORT kopsik_api_result kopsik_project_select_items(
    void *ctx,
    char *errmsg, unsigned int errlen,
    KopsikProjectSelectItemList *list);

// Time entries view

typedef struct {
  int DurationInSeconds;
  char *Description;
  char *Project;
  char *Duration;
  char *Color;
  char *GUID;
  int Billable;
  char *Tags;
  unsigned int Started;
  unsigned int Ended;
  char *UpdatedAt;
} KopsikTimeEntryViewItem;

typedef struct {
  KopsikTimeEntryViewItem **ViewItems;
  unsigned int Length;
} KopsikTimeEntryViewItemList;

KOPSIK_EXPORT KopsikTimeEntryViewItem *
  kopsik_time_entry_view_item_init();

KOPSIK_EXPORT void kopsik_time_entry_view_item_clear(
  KopsikTimeEntryViewItem *item);

KOPSIK_EXPORT kopsik_api_result kopsik_running_time_entry_view_item(
  void *ctx,
  char *errmsg, unsigned int errlen,
  KopsikTimeEntryViewItem *item,
  int *is_tracking);

KOPSIK_EXPORT void kopsik_format_duration_in_seconds_hhmmss(
  int duration_in_seconds, char *str, unsigned int max_strlen);

KOPSIK_EXPORT void kopsik_format_duration_in_seconds_hhmm(
  int duration_in_seconds, char *str, unsigned int max_strlen);

KOPSIK_EXPORT kopsik_api_result kopsik_start(
  void *ctx,
  char *errmsg, unsigned int errlen,
  const char *description,
  KopsikTimeEntryViewItem *item);

KOPSIK_EXPORT kopsik_api_result kopsik_time_entry_view_item_by_guid(
  void *ctx,
  char *errmsg, unsigned int errlen,
  const char *guid,
  KopsikTimeEntryViewItem *item,
  int *was_found);

KOPSIK_EXPORT kopsik_api_result kopsik_continue(
  void *ctx,
  char *errmsg, unsigned int errlen,
  const char *guid,
  KopsikTimeEntryViewItem *item);

KOPSIK_EXPORT kopsik_api_result kopsik_continue_latest(
  void *ctx,
  char *errmsg, unsigned int errlen,
  KopsikTimeEntryViewItem *item,
  int *was_found);

KOPSIK_EXPORT kopsik_api_result kopsik_delete_time_entry(
  void *ctx,
  char *errmsg, unsigned int errlen,
  const char *guid);

KOPSIK_EXPORT kopsik_api_result kopsik_set_time_entry_duration(
  void *ctx,
  char *errmsg, unsigned int errlen,
  const char *guid,
  const char *value);

KOPSIK_EXPORT kopsik_api_result kopsik_set_time_entry_project(
  void *ctx,
  char *errmsg, unsigned int errlen,
  const char *guid,
  const char *value);

KOPSIK_EXPORT kopsik_api_result kopsik_set_time_entry_start_iso_8601(
  void *ctx,
  char *errmsg, unsigned int errlen,
  const char *guid,
  const char *value);

KOPSIK_EXPORT kopsik_api_result kopsik_set_time_entry_end_iso_8601(
  void *ctx,
  char *errmsg, unsigned int errlen,
  const char *guid,
  const char *value);

KOPSIK_EXPORT kopsik_api_result kopsik_set_time_entry_tags(
  void *ctx,
  char *errmsg, unsigned int errlen,
  const char *guid,
  const char *value);

KOPSIK_EXPORT kopsik_api_result kopsik_set_time_entry_billable(
  void *ctx,
  char *errmsg, unsigned int errlen,
  const char *guid,
  int value);

KOPSIK_EXPORT kopsik_api_result kopsik_set_time_entry_description(
  void *ctx,
  char *errmsg, unsigned int errlen,
  const char *guid,
  const char *value);

KOPSIK_EXPORT kopsik_api_result kopsik_stop(
  void *ctx,
  char *errmsg, unsigned int errlen,
  KopsikTimeEntryViewItem *item,
  int *was_found);

KOPSIK_EXPORT KopsikTimeEntryViewItemList *
  kopsik_time_entry_view_item_list_init();

KOPSIK_EXPORT void kopsik_time_entry_view_item_list_clear(
  KopsikTimeEntryViewItemList *list);

KOPSIK_EXPORT kopsik_api_result kopsik_time_entry_view_items(
  void *ctx,
  char *errmsg, unsigned int errlen,
  KopsikTimeEntryViewItemList *list);

// Websocket client

typedef struct {
  char *ModelType;
  char *ChangeType;
  unsigned int ModelID;
  char *GUID;
} KopsikModelChange;

typedef void (*KopsikViewItemChangeCallback)(
  kopsik_api_result result,
  const char *errmsg,
  KopsikModelChange *change);

KOPSIK_EXPORT void kopsik_set_change_callback(
  void *ctx,
  KopsikViewItemChangeCallback callback);

KOPSIK_EXPORT kopsik_api_result kopsik_websocket_start(
  void *ctx,
  char *errmsg, unsigned int errlen);

KOPSIK_EXPORT kopsik_api_result kopsik_websocket_stop(
  void *ctx,
  char *errmsg, unsigned int errlen);

KOPSIK_EXPORT void kopsik_websocket_start_async(
  void *ctx,
  KopsikResultCallback callback);

KOPSIK_EXPORT void kopsik_websocket_stop_async(
  void *ctx,
  KopsikResultCallback callback);

// Updates

typedef void (*KopsikCheckUpdateCallback)(
  kopsik_api_result result,
  const char *errmsg,
  const int is_update_available,
  const char *url,
  const char *version);

KOPSIK_EXPORT void kopsik_check_for_updates_async(
  void *ctx, KopsikCheckUpdateCallback callback);

#undef KOPSIK_EXPORT

#ifdef __cplusplus
}
#endif

#endif  // SRC_KOPSIK_API_H_
