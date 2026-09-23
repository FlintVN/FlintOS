package flint.ui;

import flint.drawing.ClipMode;
import flint.drawing.Graphics;

public class StackView extends GroupView {
    protected Orientation orientation;

    public StackView() {
        width = View.MATCH_PARENT;
        height = View.MATCH_PARENT;
        orientation = Orientation.VERTICAL;
    }

    @Override
    protected void onDraw(Graphics g) {
        super.onDraw(g);

        int gClipX = g.getClipX();
        int gClipY = g.getClipY();
        int gClipW = g.getClipWidth();
        int gClipH = g.getClipHeight();

        int thk = getBorderThickness();
        int thk2 = thk << 1;
        g.setClip(this.x + thk, this.y + thk, actualWidth - thk2, actualHeight - thk2, ClipMode.INTERSECT);

        for(int i = 0; i < childrenCount; i++) {
            if(children[i].isVisible(g))
                children[i].onDraw(g);
        }

        g.setClip(gClipX, gClipY, gClipW, gClipH, ClipMode.REPLACE);
    }

    @Override
    protected void updateLocation(int x, int y) {
        this.x = x;
        this.y = y;

        int thk = getBorderThickness();
        if(orientation.value == Orientation.VERTICAL.value) {
            int yoff = y + thk;
            for(int i = 0; i < childrenCount; i++) {
                View v = children[i];
                yoff += v.marginTop;
                int xoff = x + switch(v.hAlignment.value) {
                    case 0 -> thk + v.marginLeft;
                    case 1 -> (actualWidth - v.actualWidth) / 2 + v.marginLeft - v.marginRight;
                    default -> actualWidth - v.marginRight - v.actualWidth - thk;
                };
                v.updateLocation(xoff, yoff);
                yoff += v.actualHeight + v.marginBottom;
            }
        }
        else if(orientation.value == Orientation.HORIZONTAL.value) {
            int xoff = x + thk;
            for(int i = 0; i < childrenCount; i++) {
                View v = children[i];
                xoff += v.marginLeft;
                int yoff = y + switch(v.vAlignment.value) {
                    case 0 -> thk + v.marginTop;
                    case 1 -> (actualHeight - v.actualHeight) / 2 + v.marginTop - v.marginBottom;
                    default -> actualHeight - v.marginBottom - v.actualHeight - thk;
                };
                v.updateLocation(xoff, yoff);
                xoff += v.actualWidth + v.marginRight;
            }
        }
        else {
            for(int i = 0; i < childrenCount; i++) {
                View v = children[i];
                int xoff = x + switch(v.hAlignment.value) {
                    case 0 -> thk + v.marginLeft;
                    case 1 -> (actualWidth - v.actualWidth) / 2 + v.marginLeft - v.marginRight;
                    default -> actualWidth - v.marginRight - v.actualWidth - thk;
                };
                int yoff = y + switch(v.vAlignment.value) {
                    case 0 -> thk + v.marginTop;
                    case 1 -> (actualHeight - v.actualHeight) / 2 + v.marginTop - v.marginBottom;
                    default -> actualHeight - v.marginBottom - v.actualHeight - thk;
                };
                v.updateLocation(xoff, yoff);
            }
        }
    }

    @Override
    protected void updateActualWidth(int availableW) {
        if(orientation.value == Orientation.HORIZONTAL.value)
            updateActualWidthHorizontal(availableW);
        else
            updateActualWidthVertical(availableW);
    }

    @Override
    protected void updateActualHeight(int availableH) {
        if(orientation.value == Orientation.VERTICAL.value)
            updateActualHeightVertical(availableH);
        else
            updateActualHeightHorizontal(availableH);
    }

    private void updateActualWidthHorizontal(int availableW) {
        int contentW = 0;
        int matchParentCount = 0;
        int matchParentIdx = -1;
        for(int i = 0; i < childrenCount; i++) {
            View v = children[i];
            int w = v.width;
            contentW += v.marginLeft + v.marginRight;
            if(w >= 0 || w == View.WRAP_CONTENT || availableW < 0) {
                v.updateActualWidth(w);
                contentW += v.actualWidth;
            }
            else {
                matchParentCount++;
                if(matchParentIdx < 0) matchParentIdx = i;
            }
        }
        actualWidth = width >= 0 ? width : ((width == View.WRAP_CONTENT || availableW < 0) ? contentW : availableW);
        if(matchParentCount > 0 && availableW >= 0) {
            int remaining = actualWidth - contentW;
            int elementW = remaining / matchParentCount;
            int remainder = remaining % matchParentCount;
            for(; matchParentCount > 0; matchParentIdx++) {
                View v = children[matchParentIdx];
                if(v.width == View.MATCH_PARENT) {
                    int w = elementW;
                    if(remainder > 0) {
                        w++;
                        remainder--;
                    }
                    v.updateActualWidth(w);
                    matchParentCount--;
                }
            }
        }
    }

    private void updateActualHeightHorizontal(int availableH) {
        int contentH = 0;
        int matchParentCount = 0;
        int matchParentIdx = -1;
        for(int i = 0; i < childrenCount; i++) {
            View v = children[i];
            int h = v.height;
            if(h >= 0 || h == View.WRAP_CONTENT || availableH < 0) {
                v.updateActualHeight(h);
                int tmp = v.actualHeight + v.marginTop + v.marginBottom;
                if(tmp > contentH) contentH = tmp;
            }
            else {
                matchParentCount++;
                if(matchParentIdx < 0) matchParentIdx = i;
            }
        }
        actualHeight = height >= 0 ? height : ((height == View.WRAP_CONTENT || availableH < 0) ? contentH : availableH);
        if(matchParentCount > 0 && availableH >= 0) {
            for(; matchParentCount > 0; matchParentIdx++) {
                View v = children[matchParentIdx];
                if(v.height == View.MATCH_PARENT) {
                    v.updateActualHeight(actualHeight - v.marginTop - v.marginBottom);
                    matchParentCount--;
                }
            }
        }
    }

    private void updateActualWidthVertical(int availableW) {
        int contentW = 0;
        int matchParentCount = 0;
        int matchParentIdx = -1;
        for(int i = 0; i < childrenCount; i++) {
            View v = children[i];
            int w = v.width;
            if(w >= 0 || w == View.WRAP_CONTENT || availableW < 0) {
                v.updateActualWidth(w);
                int tmp = v.actualWidth + v.marginLeft + v.marginRight;
                if(tmp > contentW) contentW = tmp;
            }
            else {
                matchParentCount++;
                if(matchParentIdx < 0) matchParentIdx = i;
            }
        }
        actualWidth = width >= 0 ? width : ((width == View.WRAP_CONTENT || availableW < 0) ? contentW : availableW);
        if(matchParentCount > 0 && availableW >= 0) {
            for(; matchParentCount > 0; matchParentIdx++) {
                View v = children[matchParentIdx];
                if(v.width == View.MATCH_PARENT) {
                    v.updateActualWidth(actualWidth - v.marginLeft - v.marginBottom);
                    matchParentCount--;
                }
            }
        }
    }

    private void updateActualHeightVertical(int availableH) {
        int contentH = 0;
        int matchParentCount = 0;
        int matchParentIdx = -1;
        for(int i = 0; i < childrenCount; i++) {
            View v = children[i];
            int h = v.height;
            contentH += v.marginTop + v.marginBottom;
            if(h >= 0 || h == View.WRAP_CONTENT || availableH < 0) {
                v.updateActualHeight(h);
                contentH += v.actualHeight;
            }
            else {
                matchParentCount++;
                if(matchParentIdx < 0) matchParentIdx = i;
            }
        }
        actualHeight = height >= 0 ? height : ((height == View.WRAP_CONTENT || availableH < 0) ? contentH : availableH);
        if(matchParentCount > 0 && availableH >= 0) {
            int remaining = actualHeight - contentH;
            int elementH = remaining / matchParentCount;
            int remainder = remaining % matchParentCount;
            for(; matchParentCount > 0; matchParentIdx++) {
                View v = children[matchParentIdx];
                if(v.height == View.MATCH_PARENT) {
                    int h = elementH;
                    if(remainder > 0) {
                        h++;
                        remainder--;
                    }
                    v.updateActualHeight(h);
                    matchParentCount--;
                }
            }
        }
    }

    public Orientation getOrientation() {
        return orientation;
    }

    public void setOrientation(Orientation orientation) {
        FlintUI.checkThread();
        this.orientation = orientation;
        FlintUI.setInvalidateAll();
    }
}
