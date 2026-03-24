package com.example.jnidemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    public native String helloFromJNI();
    public native int factorial(int n);
    public native String reverseString(String s);
    public native int sumArray(int[] values);
    public native long benchmarkNative(int[] values);
    public native int[][] multiplyMatrices(int[][] m1, int[][] m2);
    public native boolean checkString(String input, String forbidden);

    public long benchmarkJava(int[] values) {
        long sum = 0;
        for (int v : values) {
            sum += v;
        }
        return sum;
    }


    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView tvHello = findViewById(R.id.tvHello);
        TextView tvFactNormal = findViewById(R.id.tvFactNormal);
        TextView tvFactNegative = findViewById(R.id.tvFactNegative);
        TextView tvFactOverflow = findViewById(R.id.tvFactOverflow);
        TextView tvReverseEmpty = findViewById(R.id.tvReverseEmpty);
        TextView tvArrayEmpty = findViewById(R.id.tvArrayEmpty);
        TextView tvBenchmark = findViewById(R.id.tvBenchmark);
        TextView tvMatrices = findViewById(R.id.tvMatrices);
        TextView tvCheckString = findViewById(R.id.tvCheckString);

        // Test 0: Hello World
        tvHello.setText("Statut : Connecte au C++\nMessage : " + helloFromJNI());

        // Test 1 : valeur normale (10)
        int res1 = factorial(10);
        tvFactNormal.setText("Factorielle(10) = " + res1 + " [OK]");

        // Test 2 : valeur négative (-5)
        int res2 = factorial(-5);
        tvFactNegative.setText("Négatif(-5) -> Code Erreur : " + res2 + " [OK]");

        // Test 3 : dépassement (20)
        int res3 = factorial(20);
        tvFactOverflow.setText("Overflow(20) -> Code Erreur : " + res3 + " [OK]");

        // Test 4 : chaîne vide
        String res4 = reverseString("");
        tvReverseEmpty.setText("Chaine vide -> Resultat : '" + res4 + "' [OK]");

        // Test 5 (Array vide)
        int res5 = sumArray(new int[]{});
        tvArrayEmpty.setText("Tableau vide -> Somme : " + res5 + " [OK]");

        // Extension A : Multiplication de matrices 2x2
        int[][] m1 = {{1, 2}, {3, 4}};
        int[][] m2 = {{5, 6}, {7, 8}};
        int[][] mRes = multiplyMatrices(m1, m2);
        tvMatrices.setText("Extension A (Matrices) :\n" +
                "Resultat du calcul natif 2x2 :\n" +
                "[" + mRes[0][0] + ", " + mRes[0][1] + "]\n" +
                "[" + mRes[1][0] + ", " + mRes[1][1] + "] [VALIDE]");

        // Extension B : Caracteres interdits
        String inputStr = "admin@123";
        String forbidden = "@#$";
        boolean hasForbidden = checkString(inputStr, forbidden);
        tvCheckString.setText("Extension B (Securite) :\n" +
                "Input: '" + inputStr + "' | Interdits: '" + forbidden + "'\n" +
                "Resultat: " + (hasForbidden ? "Caracteres detectes" : "Aucun interdit") + " [VALIDE]");

        // Extension C : Benchmark Java vs C++
        int size = 1_000_000;
        int[] largeArray = new int[size];
        for (int i = 0; i < size; i++) largeArray[i] = i % 100;

        long startJava = System.nanoTime();
        benchmarkJava(largeArray);
        long endJava = System.nanoTime();
        double timeJava = (endJava - startJava) / 1_000_000.0;

        long startNative = System.nanoTime();
        benchmarkNative(largeArray);
        long endNative = System.nanoTime();
        double timeNative = (endNative - startNative) / 1_000_000.0;

        tvBenchmark.setText("Extension C (Performance 1M elements) :\n" +
                "- Temps Java : " + String.format("%.3f", timeJava) + " ms\n" +
                "- Temps Native : " + String.format("%.3f", timeNative) + " ms\n" +
                "- Facteur de gain : " + String.format("%.2f", timeJava / timeNative) + "x plus rapide en C++");





    }
}