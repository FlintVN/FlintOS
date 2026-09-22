package flint.ui;

import flint.drawing.ClipMode;
import flint.drawing.Graphics;

public class ScrollView extends PanelView {
    private static final int ANIMATION_DURATION = 200;
    private static final float SCROLL_THROW = 0.93f;
    private static final float SCROLL_OVER_THROW = 0.85f;
    private static final float SCROLL_STOP = 0.5f;
    private static final float ELASTIC_FACTOR = 0.2f;

    protected View content;
    protected int mode = 2;

    private View actView;

    private boolean isPressed;
    private boolean scrolling;
    private float offsetX, offsetY;

    private int touchX, touchY;
    private float vX, vY;

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
                invalidateInternalLayout();
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
        float over = isOverX();
        if(over != 0) {
            vX = vX * SCROLL_OVER_THROW;
            float tmp = applyElasticDamping(over) * vX;
            offsetX += tmp;
            if(Math.abs(tmp) < 1.5f)
                vX = 0;
        }
        else {
            vX = vX * SCROLL_THROW;
            offsetX += vX;
            if(Math.abs(vX) < SCROLL_STOP)
                vX = 0;
        }
    }

    private void momentumEffectY() {
        float over = isOverY();
        if(over != 0) {
            vY = vY * SCROLL_OVER_THROW;
            float tmp = applyElasticDamping(over) * vY;
            offsetY += tmp;
            if(Math.abs(tmp) < 1.5f)
                vY = 0;
        }
        else {
            vY = vY * SCROLL_THROW;
            offsetY += vY;
            if(Math.abs(vY) < SCROLL_STOP)
                vY = 0;
        }
    }

    private boolean elasticEffectX() {
        float over = isOverX();
        if(over != 0) {
            if(over > 0) {
                float diff = 5 + over * ELASTIC_FACTOR;
                offsetX = (offsetX > diff) ? (offsetX - diff) : 0;
            }
            else {
                float diff = 5 - over * ELASTIC_FACTOR;
                int min = getMinOffsetX();
                offsetX = (offsetX < (min - diff)) ? (offsetX + diff) : min;
            }
            return true;
        }
        return false;
    }

    private boolean elasticEffectY() {
        float over = isOverY();
        if(over != 0) {
            if(over > 0) {
                float diff = 5 + over * ELASTIC_FACTOR;
                offsetY = (offsetY > diff) ? (offsetY - diff) : 0;
            }
            else {
                float diff = 5 - over * ELASTIC_FACTOR;
                int min = getMinOffsetY();
                offsetY = (offsetY < (min - diff)) ? (offsetY + diff) : min;
            }
            return true;
        }
        return false;
    }

    private float applyElasticDamping(float overValue) {
        return 1.0f / (1.0f + 0.07f * Math.abs(overValue));
    }

    private int getMinOffsetX() {
        if(content != null && content.actualWidth > actualWidth)
            return actualWidth - content.actualWidth;
        return 0;
    }

    private int getMinOffsetY() {
        if(content != null && content.actualHeight > actualHeight)
            return actualHeight - content.actualHeight;
        return 0;
    }

    private float isOverX() {
        if(offsetX > 0)
            return offsetX;
        else {
            int min = getMinOffsetX();
            if(offsetX < min)
                return offsetX - min;
        }
        return 0;
    }

    private float isOverY() {
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
                scrolling = false;
                return;
            }
            case MotionEvent.ACTION_UP: {
                if(actView != null) {
                    actView.dispatchTouchEvent(event);
                    actView = null;
                }
                isPressed = false;
                invalidateInternalLayout();
                return;
            }
            case MotionEvent.ACTION_MOVE: {
                int x = event.x;
                int y = event.y;

                if((mode & 1) != 0) {
                    int diff = x - touchX;
                    float over = isOverX();
                    offsetX += over != 0 ? applyElasticDamping(over) * diff : diff;
                    touchX = x;
                    vX = scrolling ? ((vX + diff) / 2) : diff;
                }
                if((mode & 2) != 0) {
                    int diff = y - touchY;
                    float over = isOverY();
                    offsetY += over != 0 ? applyElasticDamping(over) * diff : diff;
                    touchY = y;
                    vY = scrolling ? ((vY + diff) / 2) : diff;
                }

                scrolling = true;
                invalidateInternalLayout();
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
            v.updateLocation(x + v.marginLeft + Math.round(offsetX), y + v.marginTop + Math.round(offsetY));
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
        if(enabled)
            mode |= 1;
        else {
            mode &= ~1;
            offsetX = 0;
            invalidateVisual();
        }
    }

    public boolean getVerticalScrollMode() {
        return (mode & 2) != 0 ? true : false;
    }

    public void setVerticalScrollMode(boolean enabled) {
        if(enabled)
            mode |= 2;
        else {
            mode &= ~2;
            offsetY = 0;
            invalidateVisual();
        }
    }
}
