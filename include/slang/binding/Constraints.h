//------------------------------------------------------------------------------
//! @file Constraints.h
//! @brief Constraint creation and analysis
//
// File is under the MIT license; see LICENSE for details
//------------------------------------------------------------------------------
#pragma once

#include "slang/symbols/ASTSerializer.h"
#include "slang/util/Util.h"

namespace slang {

// clang-format off
#define CONSTRAINT(x) \
    x(Invalid) \
    x(List) \
    x(Expression) \
    x(Implication) \
    x(Conditional) \
    x(Uniqueness)
ENUM(ConstraintKind, CONSTRAINT);
#undef CONTROL
// clang-format on

class BindContext;
class Compilation;
struct ConstraintItemSyntax;

class Constraint {
public:
    ConstraintKind kind;

    const ConstraintItemSyntax* syntax = nullptr;

    bool bad() const { return kind == ConstraintKind::Invalid; }

    static const Constraint& bind(const ConstraintItemSyntax& syntax, const BindContext& context);

    template<typename T>
    T& as() {
        ASSERT(T::isKind(kind));
        return *static_cast<T*>(this);
    }

    template<typename T>
    const T& as() const {
        ASSERT(T::isKind(kind));
        return *static_cast<const T*>(this);
    }

    template<typename TVisitor, typename... Args>
    decltype(auto) visit(TVisitor& visitor, Args&&... args) const;

protected:
    explicit Constraint(ConstraintKind kind) : kind(kind) {}

    static Constraint& badConstraint(Compilation& compilation, const Constraint* ctrl);
};

class InvalidConstraint : public Constraint {
public:
    const Constraint* child;

    explicit InvalidConstraint(const Constraint* child) :
        Constraint(ConstraintKind::Invalid), child(child) {}

    static bool isKind(ConstraintKind kind) { return kind == ConstraintKind::Invalid; }

    void serializeTo(ASTSerializer& serializer) const;
};

struct ConstraintBlockSyntax;

/// Represents a list of constraints.
class ConstraintList : public Constraint {
public:
    span<const Constraint* const> list;

    explicit ConstraintList(span<const Constraint* const> list) :
        Constraint(ConstraintKind::List), list(list) {}

    static Constraint& fromSyntax(const ConstraintBlockSyntax& syntax, const BindContext& context);

    void serializeTo(ASTSerializer& serializer) const;

    static bool isKind(ConstraintKind kind) { return kind == ConstraintKind::List; }
};

struct ExpressionConstraintSyntax;

/// Represents a constraint defined by a logical expression.
class ExpressionConstraint : public Constraint {
public:
    const Expression& expr;
    bool isSoft;

    ExpressionConstraint(const Expression& expr, bool isSoft) :
        Constraint(ConstraintKind::Expression), expr(expr), isSoft(isSoft) {}

    static Constraint& fromSyntax(const ExpressionConstraintSyntax& syntax,
                                  const BindContext& context);

    void serializeTo(ASTSerializer& serializer) const;

    static bool isKind(ConstraintKind kind) { return kind == ConstraintKind::Expression; }
};

struct ImplicationConstraintSyntax;

/// Represents a constraint defined by an implication.
class ImplicationConstraint : public Constraint {
public:
    const Expression& predicate;
    const Constraint& body;

    ImplicationConstraint(const Expression& predicate, const Constraint& body) :
        Constraint(ConstraintKind::Implication), predicate(predicate), body(body) {}

    static Constraint& fromSyntax(const ImplicationConstraintSyntax& syntax,
                                  const BindContext& context);

    void serializeTo(ASTSerializer& serializer) const;

    static bool isKind(ConstraintKind kind) { return kind == ConstraintKind::Implication; }
};

struct ConditionalConstraintSyntax;

/// Represents a constraint defined by an if-else condition.
class ConditionalConstraint : public Constraint {
public:
    const Expression& predicate;
    const Constraint& ifBody;
    const Constraint* elseBody;

    ConditionalConstraint(const Expression& predicate, const Constraint& ifBody,
                          const Constraint* elseBody) :
        Constraint(ConstraintKind::Conditional),
        predicate(predicate), ifBody(ifBody), elseBody(elseBody) {}

    static Constraint& fromSyntax(const ConditionalConstraintSyntax& syntax,
                                  const BindContext& context);

    void serializeTo(ASTSerializer& serializer) const;

    static bool isKind(ConstraintKind kind) { return kind == ConstraintKind::Conditional; }
};

struct UniquenessConstraintSyntax;

/// Represents a constraint that enforces uniqueness of variables.
class UniquenessConstraint : public Constraint {
public:
    span<const Expression* const> items;

    explicit UniquenessConstraint(span<const Expression* const> items) :
        Constraint(ConstraintKind::Uniqueness), items(items) {}

    static Constraint& fromSyntax(const UniquenessConstraintSyntax& syntax,
                                  const BindContext& context);

    void serializeTo(ASTSerializer& serializer) const;

    static bool isKind(ConstraintKind kind) { return kind == ConstraintKind::Uniqueness; }
};

} // namespace slang
