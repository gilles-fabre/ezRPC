package reporter;

import java.util.MissingResourceException;
import java.util.ResourceBundle;

public class Strings {

	private static final String bundleName = "reporter.strings";

	private static final ResourceBundle bundle = ResourceBundle.getBundle(bundleName);

	public static String getString(String key) {
		try {
			return bundle.getString(key);
		} catch (MissingResourceException e) {
			return '!' + key + '!';
		}
	}
}
