//
//  MainWindowController.m
//  kopsik_ui_osx
//
//  Created by Tambet Masik on 9/24/13.
//  Copyright (c) 2013 kopsik developers. All rights reserved.
//

#import "MainWindowController.h"
#import "LoginViewController.h"
#import "TimeEntryListViewController.h"
#import "TimerViewController.h"
#import "TimerEditViewController.h"
#import "TimeEntryEditViewController.h"
#import "TimeEntryViewItem.h"
#import "UIEvents.h"
#import "Context.h"
#import "Bugsnag.h"
#import "User.h"
#import "ModelChange.h"

@interface MainWindowController ()
@property (nonatomic,strong) IBOutlet LoginViewController *loginViewController;
@property (nonatomic,strong) IBOutlet TimeEntryListViewController *timeEntryListViewController;
@property (nonatomic,strong) IBOutlet TimerViewController *timerViewController;
@property (nonatomic,strong) IBOutlet TimerEditViewController *timerEditViewController;
@property (nonatomic,strong) IBOutlet TimeEntryEditViewController *timeEntryEditViewController;
@end

@implementation MainWindowController

- (id)initWithWindow:(NSWindow *)window
{
  self = [super initWithWindow:window];
  if (self) {
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(eventHandler:)
                                                 name:kUIEventUserLoggedIn
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(eventHandler:)
                                                 name:kUIEventUserLoggedOut
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(eventHandler:)
                                                 name:kUIEventTimerRunning
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(eventHandler:)
                                                 name:kUIEventTimerStopped
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(eventHandler:)
                                                 name:kUIEventTimeEntrySelected
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(eventHandler:)
                                                 name:kUIEventTimeEntryDeselected
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(eventHandler:)
                                                 name:kUIEventError
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(eventHandler:)
                                                 name:kUIEventModelChange
                                               object:nil];
    
    self.loginViewController = [[LoginViewController alloc]
                                initWithNibName:@"LoginViewController" bundle:nil];
    self.timeEntryListViewController = [[TimeEntryListViewController alloc]
                                        initWithNibName:@"TimeEntryListViewController" bundle:nil];
    self.timerViewController = [[TimerViewController alloc]
                                initWithNibName:@"TimerViewController" bundle:nil];
    self.timerEditViewController = [[TimerEditViewController alloc]
                                      initWithNibName:@"TimerEditViewController" bundle:nil];
    self.timeEntryEditViewController = [[TimeEntryEditViewController alloc]
                                    initWithNibName:@"TimeEntryEditViewController" bundle:nil];
    
    [self.loginViewController.view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    [self.timerViewController.view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    [self.timerEditViewController.view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    [self.timeEntryListViewController.view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
  }
  return self;
}

- (void)windowDidLoad
{
  [super windowDidLoad];
  
  NSLog(@"MainWindow windowDidLoad");
  
  kopsik_set_change_callback(ctx, onModelChange);
  
  char err[KOPSIK_ERR_LEN];
  KopsikUser *user = kopsik_user_init();
  if (KOPSIK_API_SUCCESS != kopsik_current_user(ctx, err, KOPSIK_ERR_LEN, user)) {
    [[NSNotificationCenter defaultCenter] postNotificationName:kUIEventError
                                                        object:[NSString stringWithUTF8String:err]];
    kopsik_user_clear(user);
    return;
  }

  User *userinfo = nil;
  if (user->ID) {
    userinfo = [[User alloc] init];
    [userinfo load:user];
  }
  kopsik_user_clear(user);
  
  if (userinfo == nil) {
    [[NSNotificationCenter defaultCenter] postNotificationName:kUIEventUserLoggedOut object:nil];
  } else {
    [[NSNotificationCenter defaultCenter] postNotificationName:kUIEventUserLoggedIn object:userinfo];
  }
}

void websocket_action_finished(kopsik_api_result result, char *err, unsigned int errlen) {
  NSLog(@"MainWindow websocket_action_finished");
  if (KOPSIK_API_SUCCESS != result) {
    [[NSNotificationCenter defaultCenter] postNotificationName:kUIEventError
                                                        object:[NSString stringWithUTF8String:err]];
    
    free(err);
    return;
  }
}

-(void)eventHandler: (NSNotification *) notification
{
  NSLog(@"osx_ui.%@ %@", notification.name, notification.object);

  if ([notification.name isEqualToString:kUIEventUserLoggedIn]) {
    User *userinfo = notification.object;
    [Bugsnag setUserAttribute:@"user_id" withValue:[NSString stringWithFormat:@"%ld", userinfo.ID]];
    
    // Hide login view
    [self.loginViewController.view removeFromSuperview];

    // Show time entry list
    [self.contentView addSubview:self.timeEntryListViewController.view];
    [self.timeEntryListViewController.view setFrame:self.contentView.bounds];
    
    // Show header and footer
    [self.footerView setHidden:NO];
    [self.headerView setHidden:NO];
    
    renderRunningTimeEntry();
    
    [self startSync];
    
    NSLog(@"MainWindow starting websocket");
    kopsik_websocket_start_async(ctx, websocket_action_finished);
    NSLog(@"MainWindow websocket started");

  } else if ([notification.name isEqualToString:kUIEventUserLoggedOut]) {
    [Bugsnag setUserAttribute:@"user_id" withValue:nil];
    
    NSLog(@"MainWindow stopping websocket");
    
    kopsik_websocket_stop_async(ctx, websocket_action_finished);

    // Show login view
    [self.contentView addSubview:self.loginViewController.view];
    [self.loginViewController.view setFrame:self.contentView.bounds];
    [self.loginViewController.email becomeFirstResponder];

    // Hide all other views
    [self.timeEntryListViewController.view removeFromSuperview];
    [self.footerView setHidden:YES];
    [self.headerView setHidden:YES];
    [self.timerViewController.view removeFromSuperview];

  } else if ([notification.name isEqualToString:kUIEventTimerRunning]) {
    [self.headerView addSubview:self.timerViewController.view];
    [self.timerViewController.view setFrame: self.headerView.bounds];
    
    [self.timerEditViewController.view removeFromSuperview];
    
  } else if ([notification.name isEqualToString:kUIEventTimerStopped]) {
    [self.timerViewController.view removeFromSuperview];

    [self.headerView addSubview:self.timerEditViewController.view];
    [self.timerEditViewController.view setFrame:self.headerView.bounds];

  } else if ([notification.name isEqualToString:kUIEventTimeEntrySelected]) {
    [self.headerView setHidden:YES];
    [self.timeEntryListViewController.view removeFromSuperview];
    [self.contentView addSubview:self.timeEntryEditViewController.view];
    [self.timeEntryEditViewController.view setFrame:self.contentView.bounds];

  } else if ([notification.name isEqualToString:kUIEventTimeEntryDeselected]) {
    [self.headerView setHidden:NO];
    [self.timeEntryEditViewController.view removeFromSuperview];
    [self.contentView addSubview:self.timeEntryListViewController.view];
    [self.timeEntryListViewController.view setFrame:self.contentView.bounds];
  
  } else if ([notification.name isEqualToString:kUIEventError]) {
    // Proxy all app errors through this notification.

    NSString *msg = notification.object;
    NSLog(@"Error: %@", msg);

    // Ignore offline errors
    if ([msg rangeOfString:@"host not found"].location != NSNotFound) {
      return;
    }

    [self performSelectorOnMainThread:@selector(showError:) withObject:msg waitUntilDone:NO];

    [Bugsnag notify:[NSException
                     exceptionWithName:@"UI error"
                     reason:msg
                     userInfo:nil]];
  }
}

- (void)showError:(NSString *)msg {
  NSAlert *alert = [[NSAlert alloc] init];
  [alert setMessageText:msg];
  [alert addButtonWithTitle:@"Dismiss"];
  [alert beginSheetModalForWindow:self.window
                    modalDelegate:self
                   didEndSelector:nil
                      contextInfo:nil];
}

- (IBAction)logout:(id)sender {
  char err[KOPSIK_ERR_LEN];
  if (KOPSIK_API_SUCCESS != kopsik_logout(ctx, err, KOPSIK_ERR_LEN)) {
    [[NSNotificationCenter defaultCenter] postNotificationName:kUIEventError
                                                        object:[NSString stringWithUTF8String:err]];
    return;
  }
  [[NSNotificationCenter defaultCenter] postNotificationName:kUIEventUserLoggedOut object:nil];
}

- (IBAction)sync:(id)sender {
  NSLog(@"MainWindow sync");
  [self startSync];
}

void sync_finished(kopsik_api_result result, char *err, unsigned int errlen) {
  NSLog(@"MainWindow sync_finished");
  if (KOPSIK_API_SUCCESS != result) {
    [[NSNotificationCenter defaultCenter] postNotificationName:kUIEventError
                                                        object:[NSString stringWithUTF8String:err]];
    
    free(err);
    return;
  }
  renderRunningTimeEntry();
}

void renderRunningTimeEntry() {
  KopsikTimeEntryViewItem *item = kopsik_time_entry_view_item_init();
  int is_tracking = 0;
  char err[KOPSIK_ERR_LEN];
  if (KOPSIK_API_SUCCESS != kopsik_running_time_entry_view_item(ctx,
                                                                err, KOPSIK_ERR_LEN,
                                                                item, &is_tracking)) {
    [[NSNotificationCenter defaultCenter] postNotificationName:kUIEventError
                                                        object:[NSString stringWithUTF8String:err]];
    kopsik_time_entry_view_item_clear(item);
    return;
  }

  if (is_tracking) {
    TimeEntryViewItem *te = [[TimeEntryViewItem alloc] init];
    [te load:item];
    [[NSNotificationCenter defaultCenter] postNotificationName:kUIEventTimerRunning object:te];
  } else {
    [[NSNotificationCenter defaultCenter] postNotificationName:kUIEventTimerStopped object:nil];
  }
  kopsik_time_entry_view_item_clear(item);
}

void onModelChange(kopsik_api_result result,
                     char *errmsg,
                     unsigned int errlen,
                     KopsikModelChange *change) {
  if (KOPSIK_API_SUCCESS != result) {
    [[NSNotificationCenter defaultCenter] postNotificationName:kUIEventError
                                                          object:[NSString stringWithUTF8String:errmsg]];
    free(errmsg);
    return;
  }

  ModelChange *mc = [[ModelChange alloc] init];
  [mc load:change];

  [[NSNotificationCenter defaultCenter] postNotificationName:kUIEventModelChange object:mc];
}

- (void)startSync {
  NSLog(@"MainWindow startSync");
  kopsik_sync_async(ctx, 1, sync_finished);
  NSLog(@"MainWindow startSync done");
}

@end
