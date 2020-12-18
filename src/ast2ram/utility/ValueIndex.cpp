/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file ValueIndex.h
 *
 * Defines the ValueIndex class, which indexes the location of variables
 * and record references within a loop nest during rule conversion.
 *
 ***********************************************************************/

#include "ast2ram/utility/ValueIndex.h"
#include "ast/Aggregator.h"
#include "ast/BranchInit.h"
#include "ast/Variable.h"
#include "ast2ram/utility/Location.h"
#include "ram/Relation.h"
#include "souffle/utility/FunctionalUtil.h"
#include "souffle/utility/StreamUtil.h"
#include <cassert>
#include <set>

namespace souffle::ast2ram {

ValueIndex::ValueIndex() = default;
ValueIndex::~ValueIndex() = default;

const std::set<Location>& ValueIndex::getVariableReferences(std::string var) const {
    assert(contains(varReferencePoints, var) && "variable not indexed");
    return varReferencePoints.at(var);
}

void ValueIndex::addVarReference(const ast::Variable& var, const Location& l) {
    std::set<Location>& locs = varReferencePoints[var.getName()];
    locs.insert(l);
}

void ValueIndex::addVarReference(const ast::Variable& var, int ident, int pos) {
    addVarReference(var, Location({ident, pos}));
}

bool ValueIndex::isDefined(const ast::Variable& var) const {
    return contains(varReferencePoints, var.getName());
}

const Location& ValueIndex::getDefinitionPoint(const ast::Variable& var) const {
    assert(isDefined(var) && "undefined variable reference");
    const auto& referencePoints = varReferencePoints.at(var.getName());
    assert(!referencePoints.empty() && "at least one reference point should exist");
    return *referencePoints.begin();
}

void ValueIndex::setGeneratorLoc(const ast::Argument& arg, const Location& loc) {
    generatorDefinitionPoints.insert({&arg, loc});
}

const Location& ValueIndex::getGeneratorLoc(const ast::Argument& arg) const {
    assert(contains(generatorDefinitionPoints, &arg) && "undefined generator");
    return generatorDefinitionPoints.at(&arg);
}

void ValueIndex::setRecordDefinition(const ast::RecordInit& init, int ident, int pos) {
    recordDefinitionPoints.insert({&init, Location({ident, pos})});
}

const Location& ValueIndex::getDefinitionPoint(const ast::RecordInit& init) const {
    assert(contains(recordDefinitionPoints, &init) && "undefined record");
    return recordDefinitionPoints.at(&init);
}

void ValueIndex::setAdtDefinition(const ast::BranchInit& adt, int ident, int pos) {
    adtDefinitionPoints.insert({&adt, Location({ident, pos})});
}

const Location& ValueIndex::getDefinitionPoint(const ast::BranchInit& adt) const {
    assert(contains(adtDefinitionPoints, &adt) && "undefined adt");
    return adtDefinitionPoints.at(&adt);
}

bool ValueIndex::isGenerator(const int level) const {
    // check for aggregator definitions
    return any_of(generatorDefinitionPoints,
            [&level](const auto& location) { return location.second.identifier == level; });
}

bool ValueIndex::isSomethingDefinedOn(int level) const {
    // check for variable definitions
    for (const auto& cur : varReferencePoints) {
        if (cur.second.begin()->identifier == level) {
            return true;
        }
    }
    // check for record definitions
    for (const auto& cur : recordDefinitionPoints) {
        if (cur.second.identifier == level) {
            return true;
        }
    }
    // nothing defined on this level
    return false;
}

void ValueIndex::print(std::ostream& out) const {
    out << "Variables:\n\t";
    out << join(varReferencePoints, "\n\t");
}

}  // namespace souffle::ast2ram
