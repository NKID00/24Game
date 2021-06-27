#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <functional>
#include <array>
#include <algorithm>
#include <cstdint>

class Fraction
{
public:
    using int_t = std::int64_t;

private:
    int_t numerator, denominator;

    static int_t gcd(int_t a, int_t b) noexcept
    {
        while (b != 0)
        {
            auto t{b};
            b = a % b;
            a = t;
        }
        return a;
    }

    static int_t lcm(int_t a, int_t b) noexcept
    {
        return a * b / gcd(a, b);
    }

public:
    Fraction(int_t numerator = 0, int_t denominator = 1) noexcept
        : numerator(numerator), denominator(denominator)
    {
        if (this->denominator == 0)
        {
            this->denominator = 1;
        }
        if (this->denominator < 0)
        {
            this->denominator = -this->denominator;
            this->numerator = -this->numerator;
        }
        auto gcd_v{gcd(this->numerator, this->denominator)};
        this->numerator /= gcd_v;
        this->denominator /= gcd_v;
    }

    const Fraction operator+(const Fraction &rhs) const noexcept
    {
        return Fraction(numerator * rhs.denominator + denominator * rhs.numerator,
                        denominator * rhs.denominator);
    }

    const Fraction operator+(int_t rhs) const noexcept
    {
        return Fraction(numerator + rhs * denominator,
                        denominator);
    }

    const Fraction operator-(const Fraction &rhs) const noexcept
    {
        return Fraction(numerator * rhs.denominator - denominator * rhs.numerator,
                        denominator * rhs.denominator);
    }

    const Fraction operator-(int_t rhs) const noexcept
    {
        return Fraction(numerator - rhs * denominator,
                        denominator);
    }

    const Fraction operator*(const Fraction &rhs) const noexcept
    {
        return Fraction(numerator * rhs.numerator,
                        denominator * rhs.denominator);
    }

    const Fraction operator*(int_t rhs) const noexcept
    {
        return Fraction(numerator * rhs, denominator);
    }

    const Fraction operator/(const Fraction &rhs) const noexcept
    {
        return Fraction(numerator * rhs.denominator,
                        denominator * rhs.numerator);
    }

    const Fraction operator/(int_t rhs) const noexcept
    {
        return Fraction(numerator, denominator * rhs);
    }

    bool operator==(const Fraction &rhs) const noexcept
    {
        return numerator == rhs.numerator && denominator == rhs.denominator;
    }

    bool operator==(int_t rhs) const noexcept
    {
        return is_integer() && numerator == rhs;
    }

    bool is_negative() const noexcept
    {
        return numerator < 0;
    }

    bool is_integer() const noexcept
    {
        return denominator == 1;
    }

    std::string str() const noexcept
    {
        std::stringstream ss;
        ss << numerator;
        if (!is_integer())
        {
            ss << "/" << denominator;
        }
        return ss.str();
    }
};

enum class Operator
{
    Plus,
    Minus,
    Multiplies,
    Divides
};

inline auto call_op(Operator op, const Fraction &a, const Fraction &b)
{
    switch (op)
    {
    case Operator::Plus:
        return a + b;
    case Operator::Minus:
        return a - b;
    case Operator::Multiplies:
        return a * b;
    case Operator::Divides:
        return a / b;
    }
    return Fraction();
}

inline std::string format_op(Operator op) noexcept
{
    switch (op)
    {
    case Operator::Plus:
        return "+";
    case Operator::Minus:
        return "-";
    case Operator::Multiplies:
        return "*";
    case Operator::Divides:
        return "/";
    }
    return "";
}

inline auto calc_plus_minus_count(Operator o0, Operator o1, Operator o2) noexcept
{
    return (o0 == Operator::Plus || o0 == Operator::Minus ? 1 : 0) + (o1 == Operator::Plus || o1 == Operator::Minus ? 1 : 0) + (o2 == Operator::Plus || o2 == Operator::Minus ? 1 : 0);
}

inline void check_negative_fraction(const Fraction &t, bool &negative_flag, bool &fraction_flag) noexcept
{
    if (t.is_negative())
    {
        negative_flag = true;
    }
    if (!t.is_integer())
    {
        fraction_flag = true;
    }
}

// 最佳解法：不出现负数 > 不出现分数 > 加减法较多
inline bool check_best(const Fraction &t, bool negative_flag, bool fraction_flag, int plus_minus_count, bool &best_negative_flag, bool &best_fraction_flag, int &best_plus_minus_count) noexcept
{
    if (t == 24)
    {
        bool best_flag{false};
        if (best_negative_flag && (!negative_flag))
        {
            best_flag = true;
        }
        else if (!(best_negative_flag ^ negative_flag))
        {
            if (best_fraction_flag && (!fraction_flag))
            {
                best_flag = true;
            }
            else if (!(best_fraction_flag ^ fraction_flag))
            {
                if (plus_minus_count > best_plus_minus_count)
                {
                    best_flag = true;
                }
            }
        }
        if (best_flag)
        {
            best_negative_flag = negative_flag;
            best_fraction_flag = fraction_flag;
            best_plus_minus_count = plus_minus_count;
            return true;
        }
    }
    return false;
}

std::string calc_24_best(std::array<int, 4> &n_int) noexcept
{
    constexpr const std::array<Operator, 4> ops{Operator::Plus, Operator::Minus, Operator::Multiplies, Operator::Divides};

    std::string best;
    auto best_negative_flag{true}, best_fraction_flag{true};
    auto best_plus_minus_count{0};

    std::array<int, 4> index{0, 1, 2, 3};
    std::array<const Fraction, 4> n{n_int[0], n_int[1], n_int[2], n_int[3]};

    for (int i = 0; i < 24; std::next_permutation(index.begin(), index.end()), ++i)
    {
        for (auto o0 = ops.cbegin(); o0 != ops.cend(); ++o0)
        {
            for (auto o1 = ops.cbegin(); o1 != ops.cend(); ++o1)
            {
                for (auto o2 = ops.cbegin(); o2 != ops.cend(); ++o2)
                {
                    auto plus_minus_count{calc_plus_minus_count(*o0, *o1, *o2)};

                    // ((n0 o0 n1) o1 n2) o2 n3
                    auto negative_flag{false}, fraction_flag{false};
                    auto t{call_op(*o0, n[index[0]], n[index[1]])};
                    check_negative_fraction(t, negative_flag, fraction_flag);
                    t = call_op(*o1, t, n[index[2]]);
                    check_negative_fraction(t, negative_flag, fraction_flag);
                    t = call_op(*o2, t, n[index[3]]);
                    if (check_best(t, negative_flag, fraction_flag, plus_minus_count, best_negative_flag, best_fraction_flag, best_plus_minus_count))
                    {
                        std::stringstream ss;
                        ss << "((" << n[index[0]].str() << " " << format_op(*o0) << " " << n[index[1]].str() << ") ";
                        ss << format_op(*o1) << " " << n[index[2]].str() << ") ";
                        ss << format_op(*o2) << " " << n[index[3]].str();
                        best = ss.str();
                    }

                    // (n0 o0 (n1 o1 n2)) o2 n3
                    negative_flag = fraction_flag = false;
                    t = call_op(*o1, n[index[1]], n[index[2]]);
                    check_negative_fraction(t, negative_flag, fraction_flag);
                    t = call_op(*o0, n[index[0]], t);
                    check_negative_fraction(t, negative_flag, fraction_flag);
                    t = call_op(*o2, t, n[index[3]]);
                    if (check_best(t, negative_flag, fraction_flag, plus_minus_count, best_negative_flag, best_fraction_flag, best_plus_minus_count))
                    {
                        std::stringstream ss;
                        ss << "(" << n[index[0]].str() << " " << format_op(*o0);
                        ss << " (" << n[index[1]].str() << " " << format_op(*o1) << " " << n[index[2]].str() << ")) ";
                        ss << format_op(*o2) << " " << n[index[3]].str();
                        best = ss.str();
                    }

                    // (n0 o0 n1) o1 (n2 o2 n3)
                    negative_flag = fraction_flag = false;
                    t = call_op(*o0, n[index[0]], n[index[1]]);
                    check_negative_fraction(t, negative_flag, fraction_flag);
                    auto t2 = call_op(*o2, n[index[2]], n[index[3]]);
                    check_negative_fraction(t2, negative_flag, fraction_flag);
                    t = call_op(*o1, t, t2);
                    if (check_best(t, negative_flag, fraction_flag, plus_minus_count, best_negative_flag, best_fraction_flag, best_plus_minus_count))
                    {
                        std::stringstream ss;
                        ss << "(" << n[index[0]].str() << " " << format_op(*o0) << " " << n[index[1]].str() << ") ";
                        ss << format_op(*o1);
                        ss << " (" << n[index[2]].str() << " " << format_op(*o2) << " " << n[index[3]].str() << ")";
                        best = ss.str();
                    }

                    // n0 o0 ((n1 o1 n2) o2 n3)
                    negative_flag = fraction_flag = false;
                    t = call_op(*o1, n[index[1]], n[index[2]]);
                    check_negative_fraction(t, negative_flag, fraction_flag);
                    t = call_op(*o2, t, n[index[3]]);
                    check_negative_fraction(t, negative_flag, fraction_flag);
                    t = call_op(*o0, n[index[0]], t);
                    if (check_best(t, negative_flag, fraction_flag, plus_minus_count, best_negative_flag, best_fraction_flag, best_plus_minus_count))
                    {
                        std::stringstream ss;
                        ss << n[index[0]].str() << " " << format_op(*o0);
                        ss << " ((" << n[index[1]].str() << " " << format_op(*o1) << " " << n[index[2]].str() << ") ";
                        ss << format_op(*o2) << " " << n[index[3]].str() << ")";
                        best = ss.str();
                    }

                    // n0 o0 (n1 o1 (n2 o2 n3))
                    negative_flag = fraction_flag = false;
                    t = call_op(*o2, n[index[2]], n[index[3]]);
                    check_negative_fraction(t, negative_flag, fraction_flag);
                    t = call_op(*o1, n[index[1]], t);
                    check_negative_fraction(t, negative_flag, fraction_flag);
                    t = call_op(*o0, n[index[0]], t);
                    if (check_best(t, negative_flag, fraction_flag, plus_minus_count, best_negative_flag, best_fraction_flag, best_plus_minus_count))
                    {
                        std::stringstream ss;
                        ss << n[index[0]].str() << " " << format_op(*o0);
                        ss << " (" << n[index[1]].str() << " " << format_op(*o1);
                        ss << " (" << n[index[2]].str() << " " << format_op(*o2) << " " << n[index[3]].str() << "))";
                        best = ss.str();
                    }
                }
            }
        }
    }

    return best;
}

int main()
{
    std::ofstream f_txt{"24.txt"}, f_md{"24.md"};
    f_md << "## 24 Game Cheat Sheet\n\n| Numbers | Best solution     |\n| ------- | ----------------- |" << std::endl;

    std::array<int, 4> n;
    for (n[0] = 1; n[0] <= 9; ++n[0])
    {
        for (n[1] = n[0]; n[1] <= 9; ++n[1])
        {
            for (n[2] = n[1]; n[2] <= 9; ++n[2])
            {
                for (n[3] = n[2]; n[3] <= 9; ++n[3])
                {
                    auto result = calc_24_best(n);

                    f_txt << n[0] << n[1] << n[2] << n[3] << ": ";
                    f_md << "| " << n[0] << n[1] << n[2] << n[3] << "    | ";
                    if (result.empty())
                    {
                        f_txt << "Impossible" << std::endl;
                        f_md << "Impossible        |" << std::endl;
                    }
                    else
                    {
                        f_txt << result << std::endl;
                        f_md << std::left << std::setw(17) << result << " |" << std::endl;
                    }
                }
            }
        }
    }
    return 0;
}