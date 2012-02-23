package view.util;

import javax.swing.Box;
import javax.swing.JComponent;

import util.Constants;

import exception.IllegalDirectionException;

public class UIUtils {

	public static final int PADDING_HORIZONTAL_PX = 25;
	public static final int PADDING_VERTICAL_PX = 10;

	//unicode arrows
	public static final String UP_ARROW = "\u25B2";
	public static final String RIGHT_ARROW = "\u2192";
	public static final String LEFT_ARROW = "\u2190";
	public static final String DOWN_ARROW = "\u25BC";
	
	
	public static String getDirectionArrow( int direction ){
  		if(direction == Constants.DIRECTION_UP){
  			return UIUtils.UP_ARROW;
  		}else if(direction == Constants.DIRECTION_DOWN){
  			return UIUtils.DOWN_ARROW;
  		}else{
  			throw new IllegalDirectionException();
  		}
		
	}
	
	public static String getDirectionString( int direction ){
		if(direction == Constants.DIRECTION_UP){
			return "UP";
		} else if(direction == Constants.DIRECTION_DOWN){
			return "DOWN";
		} else {
			throw new IllegalDirectionException("directionRequested is invalid. A valid direction is either HALLCALL_DIRECTION_UP or HALLCALL_DIRECTION_DOWN");
		}
	}
	
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
