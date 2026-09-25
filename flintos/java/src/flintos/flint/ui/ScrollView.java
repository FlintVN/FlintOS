package flint.ui;

import flint.drawing.ClipMode;
import flint.drawing.Graphics;

public class ScrollView extends PanelView {
    private static final int FP_PRECISION = 4;

    private static final int ANIMATION_DURATION = 200;
    private static final int SCROLL_THROW = 7;
    private static final int SCROLL_OVER_THROW = 15;
    private static final int SCROLL_ELASTIC_STOP = 2 << FP_PRECISION;
    private static final int ELASTIC_FACTOR = 20;

    protected View content;
    protected int mode = 2;

    private View actView;

    private boolean isPressed;
    private boolean scrolling;
    private int offsetX, offsetY;

    private int touchX, touchY;
    private int vX, vY;

    public ScrollView() {

    }

    @Override
    protected void onDraw(Graphics g) {
        super.onDraw(g);

        if(!isPressed && scrolling) {
            boolean requestUpdate = false;
            if(vX != 0) {
                momentumEffectX();
                requestUpdate |= true;
            }
            else
                requestUpdate |= elasticEffectX();

            if(vY != 0) {
                momentumEffectY();
                requestUpdate |= true;
            }
            else
                requestUpdate |= elasticEffectY();

            if(requestUpdate)
                invalidate(true);
            else
                scrolling = false;
        }

        View v = content;
        if(v != null) {
            int gClipX = g.getClipX();
            int gClipY = g.getClipY();
            int gClipW = g.getClipWidth();
            int gClipH = g.getClipHeight();

            int thk = getBorderThickness();
            int thk2 = thk << 1;

            g.setClip(this.x + thk, this.y + thk, actualWidth - thk2, actualHeight - thk2, ClipMode.INTERSECT);

            v.onDraw(g);

            g.setClip(gClipX, gClipY, gClipW, gClipH, ClipMode.REPLACE);
        }
    }

    private void momentumEffectX() {
        int over = isOverX();
        if(over != 0) {
            int tmp = vX * SCROLL_OVER_THROW / 100;
            if(tmp == 0) tmp = (vX > 0) ? 1 : -1;
            vX -= tmp;
            tmp = applyElasticDamping(over, vX);
            offsetX += tmp;
            if(Math.abs(tmp) <= SCROLL_ELASTIC_STOP)
                vX = 0;
        }
        else {
            int tmp = vX * SCROLL_THROW / 100;
            if(tmp == 0) tmp = (vX > 0) ? 1 : -1;
            vX -= tmp;
            offsetX += vX;
        }
    }

    private void momentumEffectY() {
        int over = isOverY();
        if(over != 0) {
            int tmp = vY * SCROLL_OVER_THROW / 100;
            if(tmp == 0) tmp = (vY > 0) ? 1 : -1;
            vY -= tmp;
            tmp = applyElasticDamping(over, vY);
            offsetY += tmp;
            if(Math.abs(tmp) <= SCROLL_ELASTIC_STOP)
                vY = 0;
        }
        else {
            int tmp = vY * SCROLL_THROW / 100;
            if(tmp == 0) tmp = (vY > 0) ? 1 : -1;
            vY -= tmp;
            offsetY += vY;
        }
    }

    private boolean elasticEffectX() {
        int over = isOverX();
        if(over != 0) {
            if(over > 0) {
                int diff = (5 << FP_PRECISION) + over * ELASTIC_FACTOR / 100;
                offsetX = (offsetX > diff) ? (offsetX - diff) : 0;
            }
            else {
                int diff = (5 << FP_PRECISION) - over * ELASTIC_FACTOR / 100;
                int min = getMinOffsetX();
                offsetX = (offsetX < (min - diff)) ? (offsetX + diff) : min;
            }
            return true;
        }
        return false;
    }

    private boolean elasticEffectY() {
        int over = isOverY();
        if(over != 0) {
            if(over > 0) {
                int diff = 5 + over * ELASTIC_FACTOR / 100;
                offsetY = (offsetY > diff) ? (offsetY - diff) : 0;
            }
            else {
                int diff = 5 - over * ELASTIC_FACTOR / 100;
                int min = getMinOffsetY();
                offsetY = (offsetY < (min - diff)) ? (offsetY + diff) : min;
            }
            return true;
        }
        return false;
    }

    private int applyElasticDamping(int overValue, int v) {
        return (100 * v) / (100 + ((7 * Math.abs(overValue)) >>> FP_PRECISION));
    }

    private int getMinOffsetX() {
        if(content != null && content.actualWidth > actualWidth)
            return (actualWidth - content.actualWidth) << FP_PRECISION;
        return 0;
    }

    private int getMinOffsetY() {
        if(content != null && content.actualHeight > actualHeight)
            return (actualHeight - content.actualHeight) << FP_PRECISION;
        return 0;
    }

    private int isOverX() {
        if(offsetX > 0)
            return offsetX;
        else {
            int min = getMinOffsetX();
            if(offsetX < min)
                return offsetX - min;
        }
        return 0;
    }

    private int isOverY() {
        if(offsetY > 0)
            return offsetY;
        else {
            int min = getMinOffsetY();
            if(offsetY < min)
                return offsetY - min;
        }
        return 0;
    }

    @Override
    protected View hitTest(int x, int y) {
        if(containsPoint(x, y)) {
            if(scrolling)
                return this;
            if(content != null) {
                View v = content.hitTest(x, y);
                if(v == null)
                    return this;
                while(v != this) {
                    if(v.manipulationMode() == true)
                        return v;
                    v = v.parent;
                }
            }
            return this;
        }
        return null;
    }

    @Override
    protected boolean manipulationMode() {
        return true;
    }

    @Override
    protected void onTouchEvent(MotionEvent event) {
        switch(event.action) {
            case MotionEvent.ACTION_DOWN: {
                if(scrolling)
                    actView = null;
                else if(content != null) {
                    actView = content.hitTest(event.x, event.y);
                    if(actView != null)
                        actView.dispatchTouchEvent(event);
                }
                touchX = event.x;
                touchY = event.y;
                vX = vY = 0;
                isPressed = true;
                return;
            }
            case MotionEvent.ACTION_UP: {
                if(actView != null) {
                    actView.dispatchTouchEvent(event);
                    actView = null;
                }
                isPressed = false;
                invalidate(true);
                return;
            }
            case MotionEvent.ACTION_MOVE: {
                if((mode & 1) != 0) {
                    int diff = (event.x - touchX) << FP_PRECISION;
                    int over = isOverX();
                    offsetX += over != 0 ? applyElasticDamping(over, diff) : diff;
                    touchX = event.x;
                    vX = scrolling ? ((vX + diff) / 2) : diff;
                }
                if((mode & 2) != 0) {
                    int diff = (event.y - touchY) << FP_PRECISION;
                    int over = isOverY();
                    offsetY += over != 0 ? applyElasticDamping(over, diff) : diff;
                    touchY = event.y;
                    vY = scrolling ? ((vY + diff) / 2) : diff;
                }

                scrolling = true;
                invalidate(true);
                if(actView != null)
                    actView.dispatchTouchEvent(event);
                return;
            }
        }
    }

    @Override
    protected void updateLocation(int x, int y) {
        this.x = x;
        this.y = y;

        View v = content;
        if(v != null)
            v.updateLocation(x + v.marginLeft + (offsetX >> FP_PRECISION), y + v.marginTop + (offsetY >> FP_PRECISION));
    }

    @Override
    protected void updateActualWidth(int availableW) {
        View v = content;
        if(width == View.WRAP_CONTENT || (width == View.MATCH_PARENT && availableW < 0)) {
            int contentW = getBorderThickness() << 1;

            if(v != null) {
                v.updateActualWidth(availableW - v.marginLeft - v.marginRight);
                contentW += v.actualWidth + v.marginLeft + v.marginRight;
            }

            actualWidth = width >= 0 ? width : ((width == View.WRAP_CONTENT || availableW < 0) ? contentW : availableW);
        }
        else {
            if(v != null)
                v.updateActualWidth(availableW - v.marginLeft - v.marginRight);
            actualWidth = width >= 0 ? width : availableW;
        }
    }

    @Override
    protected void updateActualHeight(int availableH) {
        View v = content;
        if(height == View.WRAP_CONTENT || (height == View.MATCH_PARENT && availableH < 0)) {
            int contentH = getBorderThickness() << 1;

            if(v != null) {
                v.updateActualHeight(availableH - v.marginTop - v.marginBottom);
                contentH += v.actualHeight + v.marginTop + v.marginBottom;
            }

            actualHeight = height >= 0 ? height : ((height == View.WRAP_CONTENT || availableH < 0) ? contentH : availableH);
        }
        else {
            if(v != null)
                v.updateActualHeight(availableH - v.marginTop - v.marginBottom);
            actualHeight = height >= 0 ? height : availableH;
        }
    }

    public View getContent() {
        return content;
    }

    public void setContent(View v) {
        FlintUI.checkThread();
        if(v != null) {
            if(v.parent != null)
                throw new IllegalStateException("The specified child already has a parent");
            v.parent = this;
        }
        if(content != null)
            content.parent = null;
        content = v;
    }

    public boolean getHorizontalScrollMode() {
        return (mode & 1) != 0 ? true : false;
    }

    public void setHorizontalScrollMode(boolean enabled) {
        FlintUI.checkThread();
        if(enabled)
            mode |= 1;
        else {
            mode &= ~1;
            offsetX = 0;
            invalidate(false);
        }
    }

    public boolean getVerticalScrollMode() {
        return (mode & 2) != 0 ? true : false;
    }

    public void setVerticalScrollMode(boolean enabled) {
        FlintUI.checkThread();
        if(enabled)
            mode |= 2;
        else {
            mode &= ~2;
            offsetY = 0;
            invalidate(false);
        }
    }
}
