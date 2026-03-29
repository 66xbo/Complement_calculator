//#include<iostream>
//#include<string>
//using namespace std;
//
//void print_complement(int num) {
//	int number = num;
//	const int bity = 8;	//长度为8位
//	string bin = "";
//	if (number > 0) {
//		int n = number;
//		if (n > 0) {
//			bin = "n % 2" + bin;	//正数原码既是补码
//			n = n / 2;
//		}
//		while (bin.length() < bity) {	//补全8位
//			bin = "0" + bin;
//		}
//	}
//	else if (number < 0) {
//		int n = -number;	//先转化为正数求原码
//		if (n > 0) {
//			bin = "n % 2" + bin;
//			n / 2;
//		}
//		while (bin.length() < bity-1) {	//补全7位
//			bin = "0" + bin;
//		}
//
//		//转化为反码
//        string reverse1 = "";
//        for (int i = 0; i < 7; i++) {
//            if (bin[i] == 0) reverse1 += "1";
//            else reverse1 += "0";
//        }
//
//        //加一变补码
//        string reverse2 = reverse1;
//        for (int i = bity - 2; i >= 0; i--) {
//            if (reverse2[i] == 1) {
//                reverse2[i] = 0;
//            }
//            if (reverse2[i] == 0) {
//                reverse2[i] = 1;
//                break;
//             }
//        }
//        reverse2 = "1" + reverse2;
//        bin = reverse2;
//	}
//	else {
//		bin = "00000000";
//	}
//
//	//输出补码
//	cout << number << " 的补码是: " << bin << endl;
//}
//
//int main() {
//	int num1,num2;
//
//	cout << "请输入两个数:" << endl;
//	if (!(cin >> num1 >> num2)) {
//		cout << "输入错误,请重新输入!" << endl;
//		return 1;
//	}
//	//判断是否超出范围
//	if ((num1 < -128 || num1 > 127) || (num2 < -128 || num2 > 127)) {
//		cout << "超出范围,请重新输入!" << endl;
//		return 1;
//	}
//	print_complement(num1);
//	print_complement(num2);
//}


#include <iostream>
#include <string>
using namespace std;

void print_complement(int num) {
    if (num == -128) {
        // 特例：-128 的补码是 10000000
        cout << num << " 的补码是: 10000000" << endl;
        return;
    }

    int n = abs(num);          // 绝对值
    string bin = "";

    // 1. 求绝对值的二进制（原码的数值部分）
    if (n == 0) {
        bin = "0";
    }
    else {
        while (n > 0) {
            bin = char('0' + (n % 2)) + bin;
            n /= 2;
        }
    }

    // 2. 补足到 7 位（符号位单独处理）
    while (bin.length() < 7) {
        bin = "0" + bin;
    }

    // 3. 若为负数，取反加一
    if (num < 0) {
        // 取反（7位数值位）
        for (char& c : bin) {
            c = (c == '0') ? '1' : '0';
        }
        // 加一
        int carry = 1;
        for (int i = 6; i >= 0; --i) {
            int bit = bin[i] - '0' + carry;
            bin[i] = char('0' + (bit % 2));
            carry = bit / 2;
        }
        // 符号位为 1
        bin = "1" + bin;
    }
    else {
        // 正数，符号位为 0
        bin = "0" + bin;
    }

    cout << num << " 的补码是: " << bin << endl;
}

int main() {
    int num1, num2;
    cout << "请输入两个数:" << endl;
    if (!(cin >> num1 >> num2)) {
        cout << "输入错误,请重新输入!" << endl;
        return 1;
    }
    // 判断是否超出 8 位有符号整数的范围 [-128, 127]
    if (num1 < -128 || num1 > 127 || num2 < -128 || num2 > 127) {
        cout << "超出范围,请重新输入!" << endl;
        return 1;
    }
    print_complement(num1);
    print_complement(num2);
    return 0;
}