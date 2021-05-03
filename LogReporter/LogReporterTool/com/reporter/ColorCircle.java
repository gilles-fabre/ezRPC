package reporter;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.image.BufferedImage;

import javax.swing.JPanel;

import reporter.ARGBColorPicker.ARGBColorPickerPanel;


public class ColorCircle extends JPanel implements MouseListener, MouseMotionListener {
    /**
	 *
	 */
	private static final long serialVersionUID = 1L;

	static final int        POINTER_RADIUS = 3;
    private boolean         draggingWheel = false;
    private Rectangle       wheelRect;
    private Graphics        wheelGraphics;
    private BufferedImage   wheelImage;
    private int             wheelCenterX;
    private int             wheelCenterY;

    ColorModel           model = new ColorModel();

    public ColorCircle() {
        super();
        addMouseMotionListener(this);
        addMouseListener(this);
    }

    /**
    ColorModel - stores the current selected colour
    */

    class ColorModel {
        private float brightness = (float)1.0;
        private float hue = (float)0.0;
        private float saturation = (float)0.0;
        private Color color;

        public void setBrightness(float b) {
            if (b < 0.0) b = 0f;
            if (b >= 1.0) b = 1f;

            brightness = b;
            color = Color.getHSBColor(hue, saturation, brightness);
        }

        public void setHue(float h) {
            if (h < 0.0) h= 0f;
            if (h >= 1.0) h= 0.999f;

            hue = h;
            color = Color.getHSBColor(hue, saturation, brightness);
        }

        public void setSaturation(float s) {
            if (s < 0.0) s = 0f;
            if (s >= 1.0) s = 1f;

            saturation = s;
            color = Color.getHSBColor(hue, saturation, brightness);
        }

        public void setRGB(int r, int g, int b) {
            if (r < 0) r = 0;
            if (r > 255) r = 255;
            if (g < 0) g = 0;
            if (g > 255) g = 255;
            if (b < 0) b = 0;
            if (b > 255) b = 255;

            float[] vals = new float[3];
            Color.RGBtoHSB(r , g, b, vals);
            hue = vals[0];
            saturation = vals[1];
            brightness = vals[2];
            color = new Color(r, g, b);
        }

        public void setColor(Color c) {
        	if (c != null)
        		setRGB(c.getRed(), c.getGreen(), c.getBlue());
        }

        public Color getColor() {
            return color;
        }

        public float getHue() {
            return hue;
        }

        public float getSaturation() {
            return saturation;
        }

        public float getBrightness() {
            return brightness;
        }
    }


    private void computeSize() {
        Dimension d = getSize();
        int       min = d.width < d.height ? d.width : d.height;

        wheelRect = new Rectangle((d.width - min) >> 1,
                                 (d.height - min) >> 1,
                                 min,
                                 min);

        wheelCenterX = wheelRect.x + wheelRect.width >> 1;
        wheelCenterY = wheelRect.y + wheelRect.height >> 1;
    }

    public void setBackground(Color color) {
        if (model != null)
            model.setColor(color);
        super.setBackground(color);
    }

    public void paint(Graphics g) {
        super.paint(g);

        computeSize();

        // draw wheel
        if (wheelImage == null) {
            wheelImage = new BufferedImage(wheelRect.width, wheelRect.height, BufferedImage.TYPE_INT_ARGB);
            wheelGraphics = wheelImage.getGraphics();
            Color background = getParent().getBackground();
            wheelGraphics.setColor(background);
            wheelGraphics.fillRect(0, 0, wheelRect.width, wheelRect.height);
            int saturation_step = 1;
            int hue_step = 1;
            for (int s = 100; s >= 0; s -= saturation_step) {
                int arcw = wheelRect.width * s / 100;
                int arch = wheelRect.height * s / 100;
                float sat = s / 100F;
                for (int h = 0; h <= 360; h += hue_step) {
                    float hue = h / 360F;
                    if (hue >= 1F)
                        hue = 0F;

                    Color c = Color.getHSBColor(hue, sat, 1F);
                    wheelGraphics.setColor(c);
                    wheelGraphics.fillArc(wheelCenterX - (arcw >> 1), wheelCenterY - (arch >> 1), arcw, arch, h, hue_step);
                }
            }
        }

        // draw image
        g.drawImage(wheelImage, wheelRect.x, wheelRect.y, null);

        // draw circle around the current colour
        g.setXORMode(Color.white);
        int arcw = (int)(wheelRect.width * model.getSaturation()) >> 1;
        int arch = (int) (wheelRect.height * model.getSaturation()) >> 1;
        double th = model.getHue() * 2.0 * Math.PI;
        g.drawOval((int)(wheelCenterX + arcw * Math.cos(th) - POINTER_RADIUS),
                   (int)(wheelCenterY - arch * Math.sin(th) - POINTER_RADIUS),
                   POINTER_RADIUS << 1,
                   POINTER_RADIUS << 1);
    }


    public void mouseDragged(MouseEvent e) {
        int x = e.getX();
        int y = e.getY();
        int dX = x - wheelCenterX;
        int dY = y - wheelCenterY;

        if (draggingWheel) {
            double s;
            s = Math.sqrt((double)((dX * dX) + (dY * dY))) / ((double)(wheelRect.height) / 2.0);
            if (s <= 1) {
                int color = wheelImage.getRGB(x, y);
                int  red = (color & 0x00ff0000) >> 16;
                int  green = (color & 0x0000ff00) >> 8;
                int  blue = color & 0x000000ff;

                setBackground(new Color(red, green, blue));
                ((ARGBColorPickerPanel)getParent()).update();
                repaint();
            }
        }
    }

    public void mousePressed(MouseEvent e) {
        draggingWheel = false;

        int x = e.getX();
        int y = e.getY();

        int dX = x - wheelCenterX;
        int dY = y - wheelCenterY;
        double s;
        s = Math.sqrt((double)((dX * dX) + (dY * dY))) / ((double)(wheelRect.height) / 2.0);
        if (s <= 1) {
            draggingWheel = true;

            int color = wheelImage.getRGB(x, y);
            int  red = (color & 0x00ff0000) >> 16;
            int  green = (color & 0x0000ff00) >> 8;
            int  blue = color & 0x000000ff;

            setBackground(new Color(red, green, blue));
            ((ARGBColorPickerPanel)getParent()).update();
            repaint();
        }
    }

	public void mouseMoved(MouseEvent arg0) {
		// nop
	}

	public void mouseEntered(MouseEvent arg0) {
		// nop
	}

	public void mouseReleased(MouseEvent arg0) {
		// nop
	}

	public void mouseExited(MouseEvent arg0) {
		// nop
	}

    public void mouseClicked(MouseEvent arg0) {
		// nop
	}
}
