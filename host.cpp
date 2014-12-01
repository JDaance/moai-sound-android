// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include <moai-sound-android/host.h>
#include <moai-sound-android/MOAIAndroidSound.h>

//================================================================//
// aku
//================================================================//

//----------------------------------------------------------------//
void MOAIAndroidSoundAppInitialize () {
}

//----------------------------------------------------------------//
void MOAIAndroidSoundAppFinalize () {
}

//----------------------------------------------------------------//
void MOAIAndroidSoundContextInitialize () {
	REGISTER_LUA_CLASS ( MOAIAndroidSound )
}