
#include <iostream>
#define n 4
using namespace std;

void display(int a[][n]) {          /*  تابع نمایش ماتریس  */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << a[i][j] << "\t";
        }
        cout << "\n";
    }
    cout << "-----------------------------\n\n";
}

void transpose(int m[][n], int t[][n]) {    /* تابع بدست اوردن ترانهاده ماتریس */
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			t[i][j] = m[j][i];
		}
	}
    display(t);
}
                                            /* تابع بدست اوردن و برگرداندن مقدار دترمینان ماتریس */
int determinant_3x3(int matrix[3][3]) {            
    int det = matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
        matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
        matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
    return det;
}

int determinant_4x4(int matrix[4][4]) {
    int min1[3][3] = { {matrix[1][1], matrix[1][2], matrix[1][3]},
                        {matrix[2][1], matrix[2][2], matrix[2][3]},
                        {matrix[3][1], matrix[3][2], matrix[3][3]} };
    int min2[3][3] = { {matrix[1][0], matrix[1][2], matrix[1][3]},
                        {matrix[2][0], matrix[2][2], matrix[2][3]},
                        {matrix[3][0], matrix[3][2], matrix[3][3]} };
    int min3[3][3] = { {matrix[1][0], matrix[1][1], matrix[1][3]},
                        {matrix[2][0], matrix[2][1], matrix[2][3]},
                        {matrix[3][0], matrix[3][1], matrix[3][3]} };
    int min4[3][3] = { {matrix[1][0], matrix[1][1], matrix[1][2]},
                        {matrix[2][0], matrix[2][1], matrix[2][2]} };

    int det =
        matrix[0][0] * determinant_3x3(min1) -
        matrix[0][1] * determinant_3x3(min2) +
        matrix[0][2] * determinant_3x3(min3) -
        matrix[0][3] * determinant_3x3(min4);
    return det;
}

int main()
{
    int matrix[n][n];
    int trans[n][n];
    cout << "enter the values of matrix : \n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << "Enter m [" << i << "][" << j << "] : ";
            cin >> matrix[i][j];
        }
    }
    cout << "\n <- TRANSPOSE OF MATRIX -> \n";
    transpose(matrix,trans);
    cout << "Determinant value of 4*4 matrix is : " << determinant_4x4(matrix) << "\n";
    return 0;
}
