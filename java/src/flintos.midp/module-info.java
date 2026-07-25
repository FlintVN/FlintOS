
module flintos.midp {
    requires java.base;
    requires flintos.device;
    requires flint.drawing;

    exports flintos.midp;
    exports flint.drawing;
    exports javax.microedition.io;
    exports javax.microedition.lcdui;
    exports javax.microedition.lcdui.game;
    exports javax.microedition.media;
    exports javax.microedition.media.control;
    exports javax.microedition.midlet;
    exports javax.microedition.rms;
}
