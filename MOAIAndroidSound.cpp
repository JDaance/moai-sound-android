
#include <jni.h>
#include "moai-core/pch.h"
#include <moai-android/moaiext-jni.h>
#include <moai-sound-android/MOAIAndroidSound.h>

extern JavaVM* jvm;

namespace {
	STLString javaClassName = "com/ziplinegames/moai/MoaiSound";

	jmethodID getStaticMethod(jclass clazz, STLString methodName, STLString methodSignature) {

		JNI_GET_ENV (jvm, env);

		jmethodID method = env->GetStaticMethodID (clazz, methodName, methodSignature);
		if (method == NULL) {
			ZLLog::Print ("Unable to find static java method %s with signature %s", methodName.c_str(), methodSignature.c_str());
			return NULL;
		} else {
			return method;
		}
	}
}

//================================================================//
// local
//================================================================//


//----------------------------------------------------------------//
/**	@name	isPlaying
	@text	Check if sound is currently playing. This is only supported for mediaPlayer sounds.

	@in		MOAIAndroidSound self
	@out	boolean 
*/
int MOAIAndroidSound::_isPlaying ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIAndroidSound, "U" )

	if (!self->mUseMediaPlayer) {
		ZLLog::Print ( "Warning: Cannot use isPlaying on a SoundPool sound - there is no way to tell");
		return 0;
	}

	JNI_GET_ENV (jvm, env);
	
	jclass javaSoundClass = env->FindClass(javaClassName);
	jmethodID method = getStaticMethod (javaSoundClass, "isMediaPlayerSoundPlaying", "(I)Z");
	bool isPlaying = env->CallStaticBooleanMethod(javaSoundClass, method, self->mSoundId);

	state.Push ( isPlaying );

	return 1;
}

//----------------------------------------------------------------//
/**	@name	load
	@text	Loads the specified sound from file.

	@in		MOAIAndroidSound self
	@in		string filename			The path to the sound to load from file.
	@in		boolean useMediaPlayer	This will use the Android MediaPlayer class and as such stream the sound, use for bigger sound files.
	@out	nil
*/
int MOAIAndroidSound::_load ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIAndroidSound, "U" )

	cc8* filename			= state.GetValue < cc8* >( 2, "" );	
	bool useMediaPlayer		= state.GetValue < bool >( 3, false );

	JNI_GET_ENV (jvm, env);
	
	JNI_GET_JSTRING (filename, jfilename);

	jclass javaSoundClass = env->FindClass(javaClassName);
	jmethodID method = getStaticMethod (javaSoundClass, "loadSound", "(Ljava/lang/String;Z)I");
	int soundId = env->CallStaticIntMethod(javaSoundClass, method, jfilename, useMediaPlayer);

	self->mSoundId = soundId;
	self->mUseMediaPlayer = useMediaPlayer;

	return 0;
}

//----------------------------------------------------------------//
/**	@name	play
	@text	Play the sound.

	@in		MOAIAndroidSound self
	@out	nil
*/
int MOAIAndroidSound::_play ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIAndroidSound, "U" )

	JNI_GET_ENV (jvm, env);
	
	jclass javaSoundClass = env->FindClass(javaClassName);
	jmethodID method = getStaticMethod (javaSoundClass, "playSound", "(IFZZ)V");
	env->CallStaticVoidMethod(javaSoundClass, method, self->mSoundId, self->mVolume, self->mLoop, self->mUseMediaPlayer);

	return 0;
}

//----------------------------------------------------------------//
/**	@name	release
	@text	Releases the sound data from memory.

	@in		MOAIAndroidSound self
	@out	nil
*/
int MOAIAndroidSound::_release ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIAndroidSound, "U" )

	self->ReleaseSound ();

	return 0;
}

//----------------------------------------------------------------//
/**	@name	stop
	@text	Stop the sound.

	@in		MOAIAndroidSound self
	@out	nil
*/
int MOAIAndroidSound::_stop ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIAndroidSound, "U" )

	JNI_GET_ENV (jvm, env);
	jclass javaSoundClass = env->FindClass(javaClassName);
	jmethodID method = getStaticMethod (javaSoundClass, "stopSound", "(IZ)V");
	env->CallStaticVoidMethod(javaSoundClass, method, self->mSoundId, self->mUseMediaPlayer);

	return 0;
}

//----------------------------------------------------------------//
/**	@name	setLooping
	@text	Configure whether sound should loop. This must be set
			before playing the sound or it will have no effect.

	@in		MOAIAndroidSound self
	@in 	boolean loop 			default is false
	@out	nil
*/
int MOAIAndroidSound::_setLooping ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIAndroidSound, "U" )

	self->mLoop	= state.GetValue < bool >( 2, false );

	return 0;
}

//----------------------------------------------------------------//
/**	@name	setVolume
	@text	Immediately set the volume of this sound

	@in		MOAIAndroidSound self
	@in 	number volume 			The volume of this sound ranging 
									from 0 to 1
	@out	nil
*/
int MOAIAndroidSound::_setVolume ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIAndroidSound, "U" )

	self->mVolume = state.GetValue < float >( 2, 1.0 );

	JNI_GET_ENV (jvm, env);
	jclass javaSoundClass = env->FindClass(javaClassName);
	jmethodID method = getStaticMethod (javaSoundClass, "setSoundVolume", "(IFZ)V");
	env->CallStaticVoidMethod(javaSoundClass, method, self->mSoundId, self->mVolume, self->mUseMediaPlayer);

	return 0;
}

//================================================================//
// MOAIAndroidSound
//================================================================//

//----------------------------------------------------------------//
MOAIAndroidSound::MOAIAndroidSound () :
	mLoop ( false ),
	mSoundId ( 0 ),
	mUseMediaPlayer ( false ),
	mVolume ( 1.0 ) {

	RTTI_SINGLE ( MOAILuaObject )
}

//----------------------------------------------------------------//
MOAIAndroidSound::~MOAIAndroidSound () {

	this->ReleaseSound ();
}

//----------------------------------------------------------------//
void MOAIAndroidSound::RegisterLuaClass ( MOAILuaState& state ) {
	UNUSED ( state );
}

//----------------------------------------------------------------//
void MOAIAndroidSound::RegisterLuaFuncs ( MOAILuaState& state ) {

	luaL_Reg regTable [] = {
		{ "isPlaying",		_isPlaying },
		{ "load",			_load },
		{ "play",			_play },
		{ "release",		_release },
		{ "stop",			_stop },
		{ "setLooping",		_setLooping },
		{ "setVolume",		_setVolume },
		{ NULL, NULL }
	};

	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAIAndroidSound::ReleaseSound () {

	JNI_GET_ENV (jvm, env);
	
	jclass javaSoundClass = env->FindClass(javaClassName);
	jmethodID method = env->GetStaticMethodID (javaSoundClass, "releaseSound", "(IZ)V");
	env->CallStaticVoidMethod(javaSoundClass, method, mSoundId, mUseMediaPlayer);
}

