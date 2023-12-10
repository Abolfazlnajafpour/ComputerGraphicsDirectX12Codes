// Operations on Matrices.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#define n 4
using namespace std;
int h = 1;
void quanti(int a[][n]) {           /*  تابع مقدار دهی به ماتریس  */
    
    cout << "enter the values of matrix "<< h <<": \n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << "Enter m"<<h<<" [" << i << "][" << j << "] : ";
            cin >> a[i][j];
        }
    }
    cout << "\n";

    h++;
}

void display(int a[][n]) {              /*  تابع نمایش ماتریس  */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << a[i][j] << "\t";
        }
        cout << "\n";
    }
    cout << "-----------------------------\n";
}

void add(int a[][n], int b[][n], int c[][n]) {      /*  تابع جمع دو ماتریس  */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            c[i][j] = a[i][j] + b[i][j];
        }
    }
    cout << "   <- sum matrix -> \n\n";
    display(c);
}

void sub(int a[][n], int b[][n], int c[][n]) {      /*  تابع تفریق دو ماتریس  */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            c[i][j] = a[i][j] - b[i][j];
        }
    }
    cout << "   <- subtraction matrix -> \n\n";
    display(c);
}

void multiply(int a[][n], int b[][n], int c[][n]) {         /*  تابع ضرب دو ماتریس  */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {

                c[i][j] += a[i][k] + b[k][j];
            }
        }
    }
    cout << "   <- multiplication matrix -> \n\n";
    display(c);
}

int main()
{
 
    int matrix1[n][n], matrix2[n][n],matrix_anw[n][n];
    quanti(matrix1);
    quanti(matrix2);
    cout << "   <- matrix1 -> \n\n";
    display(matrix1);
    cout << "   <- matrix2 -> \n\n";
    display(matrix2);
    add(matrix1, matrix2, matrix_anw);
    sub(matrix1, matrix2, matrix_anw);
    multiply(matrix1, matrix2, matrix_anw);

}

