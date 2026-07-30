package javax.microedition.lcdui.game;

import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Display;

public class LayerManager {
    private int nlayers;
    private Layer component[] = new Layer[4];
    private int viewX, viewY, viewWidth, viewHeight;

    public LayerManager() {
        setViewWindow(0, 0, Integer.MAX_VALUE, Integer.MAX_VALUE);
    }

    public void append(Layer l) {
	    removeImpl(l);
        addImpl(l, nlayers);
    }

    public void insert(Layer l, int index) {
	    if((index < 0) || (index > nlayers) || (exist(l) && (index >= nlayers)))
	        throw new IndexOutOfBoundsException();
        removeImpl(l);
        addImpl(l, index);
    }

    public Layer getLayerAt(int index) {
        if((index < 0) || (index >= nlayers))
            throw new IndexOutOfBoundsException();
        return component[index];
    }

    public int getSize() {
	    return nlayers;
    }

    public void remove(Layer l) {
	    removeImpl(l);
    }

    public void paint(Graphics g, int x, int y) {
        int clipX = g.getClipX();
	    int clipY = g.getClipY();
        int clipW = g.getClipWidth();
        int clipH =  g.getClipHeight();

	    g.translate(x - viewX, y - viewY);
        g.clipRect(viewX, viewY, viewWidth, viewHeight);

        for(int i = nlayers; --i >= 0; ) {
            Layer comp = component[i];
            if(comp.visible)
                comp.paint(g);
        }

	    g.translate(-x + viewX, -y + viewY);
        g.setClip(clipX, clipY, clipW, clipH);
    }

    public void setViewWindow(int x, int y, int width, int height) {
        if(width < 0 || height < 0)
            throw new IllegalArgumentException();
	    viewX = x;
        viewY = y;
        viewWidth = width;
        viewHeight = height;
    }

    private void addImpl(Layer layer, int index) {
        if(nlayers == component.length) {
            Layer newcomponents[] = new Layer[nlayers + 4];
            System.arraycopy(component, 0, newcomponents, 0, nlayers);
            System.arraycopy(component, index, newcomponents, index + 1, nlayers - index);
            component = newcomponents;
        }
        else
            System.arraycopy(component, index, component, index + 1, nlayers - index);

        component[index] = layer;
        nlayers++;
    }

    private void removeImpl(Layer l) {
        if(l == null)
            throw new NullPointerException();

	    for(int i = nlayers; --i >= 0; ) {
            if(component[i] == l)
                remove(i);
        }
    }

    private boolean exist(Layer l) {
        if(l == null)
            return false;

        for(int i = nlayers; --i >= 0; ) {
            if(component[i] == l)
                return true;
        }
        return false;
    }

    private void remove(int index) {
	    System.arraycopy(component, index + 1, component, index, nlayers - index - 1);
	    component[--nlayers] = null;
    }
}
