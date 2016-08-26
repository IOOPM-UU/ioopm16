import java.util.Scanner;

public class Die {
  private int numberOfSides;
  private int value;

  public Die() {
      numberOfSides = 6;
  }

  public Die(int _numberOfSides) {
      numberOfSides = _numberOfSides;
  }

  public int roll() {
    return value =  (int) (Math.random()*numberOfSides) + 1;
  }

  public int get() {
      return value;
  }

  public static void main(String [] args) {
      Scanner sc = new Scanner(System.in);
      System.out.print("Number of sides: ");
      int sides = sc.nextInt()
      Die d = new Die(sides);
      System.out.println("Alea iacta est: " + d.roll());
  }
}
