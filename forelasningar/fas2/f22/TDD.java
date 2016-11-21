/// Imports assertTrue, assertEquals, et al. as methods that can
/// be used as if they were global functions
import static org.junit.Assert.*;

/// Give me all the JUnit standard "crap"
import org.junit.*;

import lecture21.List;
import lecture21.List.BadIndexException;
import java.util.Iterator;

public class TDD {
    // @Before
    // public void deeo1() {
    //     System.out.println("deeo?");
    // }

    // @After
    // public void deeo2() {
    //     System.out.println("deeo!");
    // }

    @Test
    public void testRemove() {
        List<Integer> list = new List<Integer>();
        for (int i = 0; i < 5; ++i) {
            list.append(i);
        }
        Iterator<Integer> iter = list.iterator();
        int v = list.get(0);
        assertEquals(v, 0); 
        iter.remove();
        v = list.get(0);
        assertEquals(v, 1); 
    }
    @Test
    public void testGet() {
        List<Integer> list = new List<Integer>();
        for (int i = 0; i < 5; ++i) {
            list.append(i);
        }
        for (int i = 0; i < 5; ++i) {
            try {
                int v = list.get(i);
                assertEquals(v, i);
            } catch (BadIndexException e) {
                e.printStackTrace(System.err);
            }
        }
    }
}
