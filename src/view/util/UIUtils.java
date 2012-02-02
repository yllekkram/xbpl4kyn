package view.util;

import javax.swing.Box;
import javax.swing.JComponent;

public class UIUtils {

	public static final int PADDING_HORIZONTAL_PX = 25;
	public static final int PADDING_VERTICAL_PX = 10;


	public static JComponent centralizeComponent(JComponent child){
		Box horizontal = Box.createHorizontalBox();
		horizontal.add(Box.createHorizontalGlue());
		horizontal.add(Box.createVerticalGlue());
		horizontal.add(child);
		horizontal.add(Box.createVerticalGlue());
		horizontal.add(Box.createHorizontalGlue());
		return horizontal;
	}

}
