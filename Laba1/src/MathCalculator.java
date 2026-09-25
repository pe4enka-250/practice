public class MathCalculator {
    public double calculateSinh(double x, double epsilon) {
        double sum = 0.0;
        double term = x; 
        int n = 1;       

        while (Math.abs(term) >= epsilon) {
            sum += term; 
            n++;         
            term = term * (x * x) / ((2 * n - 2) * (2 * n - 1));
        }

        return sum;
    }
}