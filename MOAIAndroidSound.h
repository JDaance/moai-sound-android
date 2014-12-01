#ifndef	MOAIANDROIDSOUND_H
#define	MOAIANDROIDSOUND_H

#include <moai-core/headers.h>


//================================================================//
// MOAIAndroidSound
//================================================================//
/**	@name	MOAIAndroidSound
	@text	Android based sound implementation. For in-memory sounds
			SoundPool is used, and for streaming sound MediaPlayer
			is used.
*/
class MOAIAndroidSound :
	public virtual MOAILuaObject {
private:

	bool mLoop;
	int mSoundId;
	bool mUseMediaPlayer;
    float mVolume;

	//----------------------------------------------------------------//
	static int	_isPlaying			( lua_State* L );
	static int	_load				( lua_State* L );
	static int	_play				( lua_State* L );
	static int	_release			( lua_State* L );
	static int	_stop				( lua_State* L );
	static int	_setLooping			( lua_State* L );
	static int	_setVolume			( lua_State* L );

public:

	DECL_LUA_FACTORY ( MOAIAndroidSound )

	//----------------------------------------------------------------//
				MOAIAndroidSound		();
				~MOAIAndroidSound		();
	void		RegisterLuaClass		( MOAILuaState& state );
	void		RegisterLuaFuncs		( MOAILuaState& state );
	void		ReleaseSound 			();
};

#endif
