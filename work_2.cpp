// ==============================================
// 任务2源代码（VS2022 C++）
// 功能：在任务1基础上，对两个数进行加减运算 + 判断溢出
// 加减法为串行按位操作（ripple carry），可视化每一位过程
// 完全包含任务1所有功能（补码转换 + 可视化）
// 使用8位补码
// ==============================================

#include <iostream>
#include <string>

using namespace std;

// ================== 任务1核心函数（复用）==================
string positive_to_binary(long long num, int bits) {
    if (num == 0) return string(bits, '0');
    string bin = "";
    while (num > 0) {
        bin = char('0' + (num % 2)) + bin;
        num /= 2;
    }
    while (bin.length() < static_cast<size_t>(bits)) bin = "0" + bin;
    return bin;
}

string invert_bits(const string& bin) {
    string inv = bin;
    for (char& c : inv) c = (c == '0') ? '1' : '0';
    return inv;
}

string add_one(const string& bin) {
    string result = bin;
    int n = result.length();
    int carry = 1;
    for (int i = n - 1; i >= 0; --i) {
        if (carry == 0) break;
        int sum = (result[i] - '0') + carry;
        result[i] = char('0' + (sum % 2));
        carry = sum / 2;
    }
    return result;
}

// 补码转十进制（用于结果显示）
int twos_to_decimal(const string& bin) {
    int n = bin.length();
    int val = 0;
    for (char c : bin) {
        val = val * 2 + (c - '0');
    }
    if (bin[0] == '1') val -= (1 << n);
    return val;
}

// ================== 任务2新增：串行按位加法（可视化）==================
pair<string, bool> serial_add(const string& a_bin, const string& b_bin, int bits) {
    string sum(bits, '0');
    int carry = 0;
    int carry_into_msb = 0;
    cout << "\n【串行按位加法过程】（从LSB开始，每位详细展示）：" << endl;
    for (int i = bits - 1; i >= 0; --i) {  // i=bits-1为LSB
        int a_bit = a_bin[i] - '0';
        int b_bit = b_bin[i] - '0';
        if (i == 0) carry_into_msb = carry;  // 符号位进位
        int sum_bit = a_bit ^ b_bit ^ carry;
        int new_carry = (a_bit & b_bit) | (a_bit & carry) | (b_bit & carry);
        sum[i] = '0' + sum_bit;
        cout << "  第" << (bits - 1 - i) << "位（LSB为第0位）：a=" << a_bit
            << " b=" << b_bit << " cin=" << carry
            << " → sum_bit=" << sum_bit << " cout=" << new_carry << endl;
        carry = new_carry;
    }
    bool overflow = (carry_into_msb != carry);
    cout << "符号位进位（cin）= " << carry_into_msb << "，最终进位输出（cout）= " << carry << endl;
    cout << "溢出判断：" << (overflow ? "是（溢出！）" : "否") << endl;
    cout << "串行加法结果补码：" << sum << endl;
    return { sum, overflow };
}

// 取二进制补码的相反数（用于减法）
string negate(const string& bin) {
    return add_one(invert_bits(bin));
}

// ================== 任务1可视化函数（复用）==================
string show_twos_complement(int num, int bits, const string& label) {
    cout << "\n" << label << " (" << num << ") 的补码转换过程：" << endl;
    bool is_negative = (num < 0);
    long long abs_num = is_negative ? -static_cast<long long>(num) : num;
    string magnitude_bin = positive_to_binary(abs_num, bits);
    cout << "  步骤1：原码（除基取余）： " << magnitude_bin << endl;
    string final_complement;
    if (!is_negative) {
        final_complement = magnitude_bin;
        cout << "  正数：补码 = 原码" << endl;
    }
    else {
        string ones = invert_bits(magnitude_bin);
        cout << "  步骤2：反码（取反）： " << ones << endl;
        final_complement = add_one(ones);
        cout << "  步骤3：补码（反码+1）： " << final_complement << endl;
    }
    cout << "  最终补码：" << final_complement << endl;
    return final_complement;
}

int main() {
    cout << "=== 任务2：在任务1基础上实现加减运算 + 溢出判断 ===" << endl;
    int bits = 8;
    cout << "使用 " << bits << " 位补码（串行按位加减）" << endl;

    int num1, num2;
    cout << "请输入第一个整数: ";
    cin >> num1;
    cout << "请输入第二个整数: ";
    cin >> num2;

    long long min_val = -(1LL << (bits - 1));
    long long max_val = (1LL << (bits - 1)) - 1;
    if (num1 < min_val || num1 > max_val || num2 < min_val || num2 > max_val) {
        cout << "错误：输入超出范围！" << endl;
        return 1;
    }

    // 任务1部分：显示补码转换（完全复用）
    string bin1 = show_twos_complement(num1, bits, "数1");
    string bin2 = show_twos_complement(num2, bits, "数2");

    // 任务2新增：选择加减
    int op;
    cout << "\n请选择运算（1=加法  2=减法）: ";
    cin >> op;

    pair<string, bool> result;
    bool is_sub = (op == 2);
    if (op == 1) {
        cout << "\n执行加法：" << num1 << " + " << num2 << endl;
        result = serial_add(bin1, bin2, bits);
    }
    else if (op == 2) {
        cout << "\n执行减法：" << num1 << " - " << num2 << " （= " << num1 << " + (-" << num2 << ")）" << endl;
        string neg_bin2 = negate(bin2);
        result = serial_add(bin1, neg_bin2, bits);
    }
    else {
        cout << "无效操作！" << endl;
        return 1;
    }

    string sum_bin = result.first;
    bool overflow = result.second;

    if (!overflow) {
        int decimal_result = twos_to_decimal(sum_bin);
        cout << "\n最终结果（十进制）: " << decimal_result << endl;
    }
    else {
        cout << "\n警告：结果溢出！" << endl;
    }

    cout << "\n任务2完成！已实现任务1全部功能 + 串行按位加减 + 溢出判断。" << endl;
    return 0;
}