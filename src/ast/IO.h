/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file IO.h
 *
 * Defines the I/O operation class
 *
 ***********************************************************************/

#pragma once

#include "ast/Node.h"
#include "ast/QualifiedName.h"
#include "parser/SrcLocation.h"
#include "souffle/utility/MiscUtil.h"
#include "souffle/utility/StreamUtil.h"
#include <map>
#include <ostream>
#include <string>
#include <utility>

namespace souffle {

enum class AstIoType { input, output, printsize, limitsize };

// FIXME: I'm going crazy defining these. There has to be a library that does this boilerplate for us.
inline std::ostream& operator<<(std::ostream& os, AstIoType e) {
    switch (e) {
        case AstIoType::input: return os << "input";
        case AstIoType::output: return os << "output";
        case AstIoType::printsize: return os << "printsize";
        case AstIoType::limitsize: return os << "limitsize";
    }

    UNREACHABLE_BAD_CASE_ANALYSIS
}

/**
 * @class AstIO
 * @brief I/O operation has a type (input/output/printsize), qualified relation name, and I/O directives.
 */
class AstIO : public AstNode {
public:
    AstIO(AstIoType type, AstQualifiedName name, SrcLocation loc = {})
            : AstNode(std::move(loc)), type(type), name(std::move(name)) {}

    /** Get I/O type */
    AstIoType getType() const {
        return type;
    }

    /** Set I/O type */
    void setType(AstIoType type) {
        this->type = type;
    }

    /** Get relation name */
    const AstQualifiedName& getQualifiedName() const {
        return name;
    }

    /** Set relation name */
    void setQualifiedName(AstQualifiedName name) {
        this->name = std::move(name);
    }

    /** Get value of I/O directive */
    const std::string& getDirective(const std::string& key) const {
        return directives.at(key);
    }

    /** Add new I/O directive */
    void addDirective(const std::string& key, std::string value) {
        directives[key] = std::move(value);
    }

    /** Check for I/O directive */
    bool hasDirective(const std::string& key) const {
        return directives.find(key) != directives.end();
    }

    /** Get I/O-directive map */
    const std::map<std::string, std::string>& getDirectives() const {
        return directives;
    }

    AstIO* clone() const override {
        auto res = new AstIO(type, name, getSrcLoc());
        res->directives = directives;
        return res;
    }

protected:
    void print(std::ostream& os) const override {
        os << "." << type << " " << name;
        if (!directives.empty()) {
            os << "(" << join(directives, ",", [](std::ostream& out, const auto& arg) {
                out << arg.first << "=\"" << arg.second << "\"";
            }) << ")";
        }
    }

    bool equal(const AstNode& node) const override {
        const auto& other = static_cast<const AstIO&>(node);
        return other.type == type && other.name == name && other.directives == directives;
    }

    /** Type of I/O operation */
    AstIoType type;

    /** Relation name of I/O operation */
    AstQualifiedName name;

    /** I/O directives */
    std::map<std::string, std::string> directives;
};

}  // end of namespace souffle
