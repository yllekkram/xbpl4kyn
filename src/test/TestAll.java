package test;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;






@RunWith(Suite.class)
@Suite.SuiteClasses({
    GUIRegistrationTest.class,
    MessageTest.class,
    UpdaterRunnableTest.class
})

public class TestAll {
	//no-op
}
