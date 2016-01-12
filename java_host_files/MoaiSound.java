package com.ziplinegames.moai;

import android.app.Activity;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.SoundPool;

import java.io.IOException;
import java.util.HashMap;

public class MoaiSound {

    private static Activity activity;
    private static SoundPool soundPool;
    private static AssetManager assetManager;

    private static int mediaPlayerNextIndex = 0;
    private static HashMap<Integer, MediaPlayerRef> mediaPlayerRefMap = new HashMap<Integer, MediaPlayerRef>();

    private static HashMap<Integer, Integer> soundPoolLatestStreamMap = new HashMap<Integer, Integer>();

    public static void onCreate(Activity activity) {
        MoaiSound.activity = activity;

        activity.setVolumeControlStream(AudioManager.STREAM_MUSIC);

        // Init the SoundPool instance
        soundPool = new SoundPool(8, AudioManager.STREAM_MUSIC, 0);

        assetManager = activity.getAssets();
    }

    public static void onPause() {
        for(MediaPlayerRef mediaPlayerRef : mediaPlayerRefMap.values()) {
            if (mediaPlayerRef.player.isPlaying()) {
                mediaPlayerRef.player.pause();
                mediaPlayerRef.wasPlaying = true;
            }
        }

        soundPool.autoPause();
    }

    public static void onResume() {
        for(MediaPlayerRef mediaPlayerRef : mediaPlayerRefMap.values()) {
            if (mediaPlayerRef.wasPlaying) {
                mediaPlayerRef.player.start();
                mediaPlayerRef.wasPlaying = false;
            }
        }

        soundPool.autoResume();
    }

    public static void onDestroy() {
        for(MediaPlayerRef mediaPlayerRef : mediaPlayerRefMap.values()) {
            mediaPlayerRef.player.release();
        }
        mediaPlayerRefMap.clear();

        soundPool.release();
        soundPool = null;
    }

    public static int loadSound(String filename, boolean useMediaPlayer) {
        AssetFileDescriptor fileDescriptor = null;
        try {
            fileDescriptor = assetManager.openFd("lua/" + filename);
        } catch (IOException e) {
            MoaiLog.e("Failed to load sound: " + filename);
            return 0;
        }

        if (useMediaPlayer)
            return loadMediaPlayerSound(fileDescriptor);
        else
            return loadSoundPoolSound(fileDescriptor);
    }

    public static void playSound(final int soundId, float volume, final boolean loop, boolean useMediaPlayer) {
        final float mixedVolume = volume * getMasterVolume();

        if (useMediaPlayer) {
            MediaPlayerRef mediaPlayerRef = mediaPlayerRefMap.get(soundId);
            mediaPlayerRef.player.setVolume(mixedVolume, mixedVolume);
            mediaPlayerRef.player.setLooping(loop);
            mediaPlayerRef.player.start();
        } else {
            activity.runOnUiThread(new Runnable() {
                public void run() {
                    int streamId = soundPool.play(soundId, mixedVolume, mixedVolume, 1, loop ? -1 : 0, 1f);
                    soundPoolLatestStreamMap.put(soundId, streamId);
                }
            });
        }
    }

    public static boolean isMediaPlayerSoundPlaying(int soundId) {
        MediaPlayerRef mediaPlayerRef = mediaPlayerRefMap.get(soundId);
        return mediaPlayerRef.player.isPlaying();
    }

    public static void stopSound(int soundId, boolean useMediaPlayer) {
        if (useMediaPlayer) {  
            MediaPlayerRef mediaPlayerRef = mediaPlayerRefMap.get(soundId);  
            mediaPlayerRef.player.stop();
            mediaPlayerRef.player.reset();
            try {
                mediaPlayerRef.player.setDataSource(mediaPlayerRef.fileDescriptor.getFileDescriptor(), mediaPlayerRef.fileDescriptor.getStartOffset(), mediaPlayerRef.fileDescriptor.getLength());
                mediaPlayerRef.player.prepare();
            } catch (IOException e) {
                throw new RuntimeException(e);
            }      
        } else {
            if (soundPoolLatestStreamMap.containsKey(soundId)) {
                int streamId = soundPoolLatestStreamMap.get(soundId);
                soundPool.stop(streamId);
                soundPoolLatestStreamMap.remove(soundId);
            }
        }
    }

    public static void setSoundVolume(int soundId, float volume, boolean useMediaPlayer) {
        float mixedVolume = volume * getMasterVolume();
        if (useMediaPlayer) {
            MediaPlayerRef mediaPlayerRef = mediaPlayerRefMap.get(soundId);
            mediaPlayerRef.player.setVolume(mixedVolume, mixedVolume);
        } else {
            if (soundPoolLatestStreamMap.containsKey(soundId)) {
                int streamId = soundPoolLatestStreamMap.get(soundId);
                soundPool.setVolume(streamId, mixedVolume, mixedVolume);
            }
        }
    }

    public static void releaseSound(int soundId, boolean useMediaPlayer) {
        // This will be called on lua garbage collection, and there is a high chance that onDestroy
        // has already been executed and killed all sound
        if (useMediaPlayer) {
            if (mediaPlayerRefMap.containsKey(soundId)) {
                MediaPlayerRef mediaPlayerRef = mediaPlayerRefMap.get(soundId);
                mediaPlayerRef.player.release();
                mediaPlayerRefMap.remove(soundId);
            }
        } else {
            if (soundPool != null) {
                soundPool.unload(soundId);
            }
        }
    }

    private static int loadMediaPlayerSound(AssetFileDescriptor fileDescriptor) {
        MediaPlayer mediaPlayer = new MediaPlayer();
        mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);

        try {
            mediaPlayer.setDataSource(fileDescriptor.getFileDescriptor(), fileDescriptor.getStartOffset(), fileDescriptor.getLength());
            mediaPlayer.prepare();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        int mediaPlayerIndex = ++mediaPlayerNextIndex;
        mediaPlayerRefMap.put(mediaPlayerIndex, new MediaPlayerRef(mediaPlayer, fileDescriptor));
        return mediaPlayerIndex;
    }

    private static int loadSoundPoolSound(AssetFileDescriptor fileDescriptor) {
        return soundPool.load(fileDescriptor, 1);
    }

    private static float getMasterVolume() {
        AudioManager audioManager = (AudioManager) activity.getSystemService(Activity.AUDIO_SERVICE);
        float actualVolume = (float)audioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
        float maxVolume = (float)audioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        return actualVolume / maxVolume;
    }

    private static class MediaPlayerRef {
        public final MediaPlayer player;
        public boolean wasPlaying = false;
        public AssetFileDescriptor fileDescriptor = null;

        public MediaPlayerRef(MediaPlayer mediaPlayer, AssetFileDescriptor fDescriptor) {
            this.player = mediaPlayer;
            this.fileDescriptor = fDescriptor;
        }
    }
}
