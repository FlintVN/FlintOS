package javax.microedition.media;

import flintos.device.Audio;

/** Lightweight square-wave tone player. */
final class TonePlayer extends AbstractPlayer {
    private int note = 60;
    private int durationMillis = 1000;

    TonePlayer() {
    }

    TonePlayer(int note, int durationMillis) {
        this.note = note;
        this.durationMillis = durationMillis;
    }

    public long getDuration() {
        return durationMillis * 1000L;
    }

    protected void playOnce() throws Exception {
        int sampleRate = Audio.getSampleRate();
        int frameSize = Audio.getFrameSize();
        int frequency = midiFrequency(note);
        int period = Math.max(2, sampleRate / frequency);
        int totalSamples = durationMillis * sampleRate / 1000;
        int generated = (int)(getMediaTime() * sampleRate / 1000000L);
        byte[] output = new byte[Math.max(2, frameSize)];

        try(Audio audio = new Audio()) {
            audio.open();
            while(!isStopRequested() && generated < totalSamples) {
                int samples = Math.min(output.length / 2, totalSamples - generated);
                for(int i = 0; i < samples; i++) {
                    int phase = (generated + i) % period;
                    int sample = phase < period / 2 ? 8192 : -8192;
                    sample = getVolumeControl().scale(sample);
                    output[i * 2] = (byte)sample;
                    output[i * 2 + 1] = (byte)(sample >>> 8);
                }
                for(int i = samples * 2; i < output.length; i++)
                    output[i] = 0;
                audio.write(output);
                generated += samples;
                setPlaybackTime((long)generated * 1000000L / sampleRate);
                Thread.sleep(Math.max(1, samples * 1000L / sampleRate));
            }
        }
    }

    protected long seek(long mediaTime) {
        return Math.min(getDuration(), mediaTime);
    }

    protected String contentType() {
        return "audio/x-tone-seq";
    }

    private static int midiFrequency(int note) {
        double frequency = 440.0 * Math.pow(2.0, (note - 69) / 12.0);
        return Math.max(1, (int)frequency);
    }
}
