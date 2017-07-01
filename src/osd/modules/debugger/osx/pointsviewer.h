// license:BSD-3-Clause
// copyright-holders:Vas Crabb
//============================================================
//
//  pointsviewer.h - MacOS X Cocoa debug window handling
//
//============================================================

#import "debugosx.h"

#import "debugwindowhandler.h"

#include "emu.h"

#import <Cocoa/Cocoa.h>


@class MAMEDebugConsole;

@interface MAMEPointsViewer : MAMEAuxiliaryDebugWindowHandler
{
	NSTabView   *tabs;
}

- (id)initWithMachine:(running_machine &)m console:(MAMEDebugConsole *)c;

- (IBAction)changeSubview:(id)sender;

@end
