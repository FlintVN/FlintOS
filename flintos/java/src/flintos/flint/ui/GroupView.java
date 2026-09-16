package flint.ui;

public class GroupView extends PanelView {
    private static final int MIN_CAPACITY = 8;

    protected View[] children;
    protected int childrenCount;

    protected GroupView() {
        children = new View[MIN_CAPACITY];
    }

    public void addView(View v) {
        if(v == null)
            throw new NullPointerException("view cannot be null");
        if(v.parent != null)
            throw new IllegalStateException("The specified child already has a parent");
        ensureCapacityInternal(childrenCount + 1);
        v.parent = this;
        children[childrenCount] = v;
        childrenCount++;
    }

    public void removeView(View v) {
        for(int i = 0; i < childrenCount; i++) {
            if(v == children[i]) {
                removeViewAt(i);
                break;
            }
        }
    }

    public void removeViewAt(int index) {
        if(index < 0 || index >= childrenCount)
            throw new IndexOutOfBoundsException("Index: " + index);
        childrenCount--;
        children[index].parent = null;
        if(index < childrenCount)
            System.arraycopy(children, index + 1, children, index, childrenCount - index);
        else
            children[index] = null;
    }

    public void removeAllView() {
        childrenCount = 0;
        if(children.length > MIN_CAPACITY * 4) {
            for(int i = 0; i < childrenCount; i++)
                children[i].parent = null;
            children = new View[MIN_CAPACITY];
        }
        else {
            for(int i = 0; i < childrenCount; i++) {
                children[i].parent = null;
                children[i] = null;
            }
        }
    }

    private void ensureCapacityInternal(int minimumCapacity) {
        if(minimumCapacity > children.length) {
            minimumCapacity += MIN_CAPACITY;
            View[] buff = new View[minimumCapacity];
            System.arraycopy(children, 0, buff, 0, childrenCount);
            children = buff;
        }
    }
}
