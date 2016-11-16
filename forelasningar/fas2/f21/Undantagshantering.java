class UEA extends RuntimeException {}
class UEB extends RuntimeException {}
class UEC extends RuntimeException {}
class CEA extends Exception {}
class CEB extends Exception {}
class CEC extends Exception {}

public class Undantagshantering {

    void method1(final int i) throws CEA {
        if (i < 0) throw new CEA();
    }

    void method2(final int i) throws CEB {
        if (i > 0) throw new CEB();
    }

    void method3(final int i) throws CEA, CEB, CEC {
        this.method1(i);
        this.method2(i);

        if (i == 0) throw new CEC();
    }

    void method3_suppress(final int i) {
        try {
            this.method1(i);
            this.method2(i);
        } catch (CEA e) {
            System.out.println("method3_suppress handler 1 CEA");
        } catch (CEB e) {
            System.out.println("method3_suppress handler 1 CEB");
        }

        try {
            if (i == 0) throw new CEC();
        } catch (CEC e) {
            System.out.println("method3_suppress handler 2 CEC");
        }
    }

    void method4(final int i) {
        if (i < 0) throw new UEA();
    }

    void method5(final int i) {
        this.method4(i);

        try {
            this.method4(i);
        } catch (RuntimeException e) {
            System.out.println("method5");
        }

        if (i > 0) throw new UEB();
    }

    void method6(int i) {
        try {
            this.method5(i);
        } catch (UEA e) {
            System.out.println("method6, handler1 UEA");
            throw e; // Continue processing this error
        } catch (UEB e) {
            System.out.println("method6, handler1 UEB");
        } catch (RuntimeException e) {
            System.out.println("method6, handler1 RuntimeException");
        } finally {
            i = 0;
        }

        if (i == 0) throw new UEC();
    }



    public static void main(String[] args) {
        final Undantagshantering u = new Undantagshantering();

        try {
            System.out.println("Skriv in ett tal A: ");
            int i = Integer.parseInt(System.console().readLine());

            u.method4(i);
        } catch (Exception e) {
            e.printStackTrace(System.err);
        }

        try {
            System.out.println("Skriv in ett tal B: ");
            int i = Integer.parseInt(System.console().readLine());

            u.method5(i);
        } catch (Exception e) {
            e.printStackTrace(System.err);
        }

        try {
            System.out.println("Skriv in ett tal C: ");
            int i = Integer.parseInt(System.console().readLine());

            u.method6(i);
        } catch (Exception e) {
            e.printStackTrace(System.err);
        }

        System.out.println("Skriv in ett tal D: ");
        int i = Integer.parseInt(System.console().readLine());
        u.method6(i);

    }
}
