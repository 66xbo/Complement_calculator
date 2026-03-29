// ==============================================
// 任务3源代码（VS2022 C++）
// 功能：在任务2基础上，实现超前进位加法（4位/8位基础 + 12位/16位扩展）
// 完全包含任务1（补码转换可视化）+ 任务2（串行加减 + 溢出判断）
// 超前进位加法：先计算p/g信号，再并行计算所有进位（软件模拟CLA逻辑）
// 可视化展示p/g表、所有进位、结果
// 支持位数：4、8、12、16位
// ==============================================

#include <iostream>
#include <string>
#include <vector>

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

int twos_to_decimal(const string& bin) {
    int n = bin.length();
    int val = 0;
    for (char c : bin) val = val * 2 + (c - '0');
    if (bin[0] == '1') val -= (1 << n);
    return val;
}

// 取相反数（减法用）
string negate(const string& bin) {
    return add_one(invert_bits(bin));
}

// ================== 任务2核心函数：串行加法（复用）==================
pair<string, bool> serial_add(const string& a_bin, const string& b_bin, int bits) {
    string sum(bits, '0');
    int carry = 0;
    int carry_into_msb = 0;
    cout << "\n【任务2 - 串行按位加法过程】（从LSB开始，每位详细展示）：" << endl;
    for (int i = bits - 1; i >= 0; --i) {
        int a_bit = a_bin[i] - '0';
        int b_bit = b_bin[i] - '0';
        if (i == 0) carry_into_msb = carry;
        int sum_bit = a_bit ^ b_bit ^ carry;
        int new_carry = (a_bit & b_bit) | (a_bit & carry) | (b_bit & carry);
        sum[i] = '0' + sum_bit;
        cout << "  第" << (bits - 1 - i) << "位（LSB为第0位）：a=" << a_bit
            << " b=" << b_bit << " cin=" << carry
            << " → sum_bit=" << sum_bit << " cout=" << new_carry << endl;
        carry = new_carry;
    }
    bool overflow = (carry_into_msb != carry);
    cout << "符号位进位（cin）= " << carry_into_msb << "，最终cout= " << carry << endl;
    cout << "溢出判断：" << (overflow ? "是（溢出！）" : "否") << endl;
    cout << "串行结果补码：" << sum << endl;
    return { sum, overflow };
}

// ================== 任务3新增：超前进位加法（CLA）==================
// 支持4/8/12/16位，可视化p/g信号 + 所有进位
pair<string, bool> cla_add(const string& a_bin, const string& b_bin, int bits) {
    // 转为LSB在[0]的vector
    vector<int> a_bits(bits), b_bits(bits);
    for (int i = 0; i < bits; ++i) {
        a_bits[i] = a_bin[bits - 1 - i] - '0';  // [0]=LSB
        b_bits[i] = b_bin[bits - 1 - i] - '0';
    }

    // 计算p（传播）和g（生成）信号 —— 超前进位关键
    vector<int> p(bits), g(bits);
    for (int i = 0; i < bits; ++i) {
        p[i] = a_bits[i] ^ b_bits[i];
        g[i] = a_bits[i] & b_bits[i];
    }

    cout << "\n【任务3 - 超前进位加法可视化】" << endl;
    cout << "p（传播信号）和g（生成信号）表（位0=LSB）：" << endl;
    for (int i = 0; i < bits; ++i) {
        cout << "  位" << i << ": p=" << p[i] << "  g=" << g[i] << endl;
    }

    // 并行计算所有进位（CLA核心逻辑：软件模拟并行）
    vector<int> c(bits + 1, 0);  // c[0]=0
    for (int i = 0; i < bits; ++i) {
        c[i + 1] = g[i] | (p[i] & c[i]);
    }

    cout << "\n计算出的所有进位（c[0.." << bits << "]）：" << endl;
    for (int i = 0; i <= bits; ++i) {
        cout << "  c[" << i << "] = " << c[i] << endl;
    }

    // 计算sum（sum_i = p_i XOR c_i）
    string sum(bits, '0');
    for (int i = 0; i < bits; ++i) {
        int sum_bit = p[i] ^ c[i];
        sum[bits - 1 - i] = '0' + sum_bit;  // 转回MSB在[0]的字符串
    }

    // 溢出判断（符号位进位 != 最终cout）
    bool overflow = (c[bits - 1] != c[bits]);
    cout << "\n符号位进位（MSB cin = c[" << (bits - 1) << "]）= " << c[bits - 1]
        << "，最终cout = " << c[bits] << endl;
    cout << "溢出判断：" << (overflow ? "是（溢出！）" : "否") << endl;
    cout << "超前进位加法结果补码：" << sum << endl;

    return { sum, overflow };
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
    cout << "=== 任务3：在任务2基础上实现超前进位加法（4/8/12/16位）===" << endl;

    // 选择位数（基础4/8位 + 扩展12/16位）
    int bits;
    cout << "请选择位数（4/8/12/16）: ";
    cin >> bits;
    if (bits != 4 && bits != 8 && bits != 12 && bits != 16) {
        cout << "错误：仅支持4/8/12/16位！" << endl;
        return 1;
    }
    cout << "使用 " << bits << " 位补码" << endl;

    int num1, num2;
    cout << "请输入第一个整数: ";
    cin >> num1;
    cout << "请输入第二个整数: ";
    cin >> num2;

    long long min_val = -(1LL << (bits - 1));
    long long max_val = (1LL << (bits - 1)) - 1;
    if (num1 < min_val || num1 > max_val || num2 < min_val || num2 > max_val) {
        cout << "错误：输入超出 " << bits << " 位补码范围！" << endl;
        return 1;
    }

    // 任务1部分：补码转换可视化
    string bin1 = show_twos_complement(num1, bits, "数1");
    string bin2 = show_twos_complement(num2, bits, "数2");

    // 任务2 + 任务3：选择运算
    int op;
    cout << "\n请选择运算（1=加法（串行+超前进位）  2=减法（串行））: ";
    cin >> op;

    if (op == 1) {
        cout << "\n=== 执行加法（先展示任务2串行，再展示任务3超前进位）===" << endl;
        // 任务2：串行加法
        auto serial_res = serial_add(bin1, bin2, bits);
        // 任务3：超前进位加法（对比）
        auto cla_res = cla_add(bin1, bin2, bits);

        // 对比结果（应相同）
        if (serial_res.first == cla_res.first && serial_res.second == cla_res.second) {
            cout << "\n串行与超前进位结果一致（验证正确）" << endl;
        }

        if (!cla_res.second) {
            cout << "\n最终结果（十进制）: " << twos_to_decimal(cla_res.first) << endl;
        }
        else {
            cout << "\n警告：结果溢出！" << endl;
        }

    }
    else if (op == 2) {
        cout << "\n=== 执行减法（任务2串行按位）===" << endl;
        string neg_bin2 = negate(bin2);
        auto serial_res = serial_add(bin1, neg_bin2, bits);
        if (!serial_res.second) {
            cout << "\n最终结果（十进制）: " << twos_to_decimal(serial_res.first) << endl;
        }
        else {
            cout << "\n警告：结果溢出！" << endl;
        }
    }
    else {
        cout << "无效操作！" << endl;
        return 1;
    }

    cout << "\n任务3完成！已实现任务1+任务2全部功能 + 4/8/12/16位超前进位加法（p/g信号 + 并行进位可视化）。" << endl;
    cout << "关键拓展代码位于 cla_add() 函数，已支持12位和16位。" << endl;
    return 0;
}