#include <string>
#include <variant>
#include <vector>

struct Ctx;  // tag memory, defined later

struct Operand { std::string text; };

struct Instruction {
    std::string mnemonic;
    std::vector<Operand> ops;
    bool eval(Ctx& ctx, bool power) const;  // dispatch on mnemonic
};

struct Sequence;  // forward declaration: Branch holds Sequences

struct Branch {
    std::vector<Sequence> paths;
    bool eval(Ctx& ctx, bool power) const;  // defined below, once Sequence is complete
};

struct Element {
    std::variant<Instruction, Branch> v;
    bool eval(Ctx& ctx, bool power) const {
        return std::visit([&](const auto& x) { return x.eval(ctx, power); }, v);
    }
};

struct Sequence {
    std::vector<Element> items;
    bool eval(Ctx& ctx, bool power) const {
        for (const Element& e : items)
            power = e.eval(ctx, power);  // every element runs, even when power is false
        return power;                    // an empty sequence is just a wire
    }
};

inline bool Branch::eval(Ctx& ctx, bool power) const {
    bool out = false;
    for (const Sequence& p : paths)
        out |= p.eval(ctx, power);  // every path runs with the same input; no early exit
    return out;
}

struct Rung { Sequence body; };