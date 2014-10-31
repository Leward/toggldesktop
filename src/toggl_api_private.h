// Copyright 2014 Toggl Desktop developers.

#ifndef SRC_TOGGL_API_PRIVATE_H_
#define SRC_TOGGL_API_PRIVATE_H_

#include <string>

#include "./lib/include/toggl_api.h"
#include "./project.h"
#include "./client.h"
#include "./autocomplete_item.h"
#include "./workspace.h"
#include "./time_entry.h"
#include "./settings.h"
#include "./proxy.h"

int compare_string(const char_t *s1, const char_t *s2);
char_t *copy_string(const std::string s);
std::string to_string(const char_t *s);

TogglGenericView *view_item_init();

TogglGenericView *project_to_view_item(
    toggl::Project * const);

TogglGenericView *tag_to_view_item(
    const std::string tag_name);

TogglGenericView *workspace_to_view_item(
    toggl::Workspace * const);

TogglGenericView *client_to_view_item(
    toggl::Client * const);

TogglAutocompleteView *autocomplete_item_init(
    const toggl::AutocompleteItem item);

void view_item_clear(TogglGenericView *item);

void autocomplete_item_clear(TogglAutocompleteView *item);

TogglTimeEntryView *time_entry_view_item_init(
    toggl::TimeEntry *te,
    const std::string project_and_task_label,
    const std::string task_label,
    const std::string project_label,
    const std::string client_label,
    const std::string color,
    const std::string date_duration,
    const bool time_in_timer_format);

void time_entry_view_item_clear(TogglTimeEntryView *item);

TogglSettingsView *settings_view_item_init(
    const _Bool record_timeline,
    const toggl::Settings settings,
    const _Bool use_proxy,
    const toggl::Proxy proxy);

void settings_view_item_clear(TogglSettingsView *view);

// For testing only
_Bool testing_set_logged_in_user(
    void *context,
    const char *json);

#endif  // SRC_TOGGL_API_PRIVATE_H_
