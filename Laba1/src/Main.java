import java.util.InputMismatchException;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        MathCalculator calculator = new MathCalculator();

        double x = 0;
        int k = 0;
        boolean isValidInput;

        isValidInput = false;
        while (!isValidInput) {
            System.out.print("Введите действительное число x: ");
            try {
                x = scanner.nextDouble();
                isValidInput = true; 
            } catch (InputMismatchException e) {
                System.out.println("Ошибка: нужно ввести число!");
                scanner.nextLine();
            }
        }

        isValidInput = false;
        while (!isValidInput) {
            System.out.print("Введите натуральное число k: ");
            try {
                k = scanner.nextInt();
                if (k > 0) {
                    isValidInput = true; 
                } else {
                    System.out.println("Ошибка: число k должно быть натуральным.");
                }
            } catch (InputMismatchException e) {
                System.out.println("Ошибка: нужно ввести целое число!");
                scanner.nextLine();
            }
        }
        double epsilon = Math.pow(10, -k);

        double customResult = calculator.calculateSinh(x, epsilon);
        
        double standardResult = Math.sinh(x);

        System.out.printf("Сумма ряда Тейлора: %.3f\n", customResult);
        System.out.printf("Стандартная функция: %.3f\n", standardResult);

        scanner.close();
    }
}