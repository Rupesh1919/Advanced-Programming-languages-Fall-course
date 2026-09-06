import java.util.ArrayList;

public class MemoryDemo {
    public static void main(String[] args) {
        ArrayList<Integer> data = new ArrayList<>();
        for (int i = 1; i <= 5; i++) data.add(i);
        System.out.println("Sum: " +
            data.stream().mapToInt(Integer::intValue).sum());
        data = null;      // now eligible for GC
        System.gc();      // suggests collection
    }
}