package flint.ui;

public interface OnSeekBarChangeListener {
    void onProgressChanged(SeekBar seekBar, boolean fromUser);

    void onStartTrackingTouch(SeekBar seekBar);

    void onStopTrackingTouch(SeekBar seekBar);
}
