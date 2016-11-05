#include "Catch/catch.hpp"
#include "slang.h"

using namespace slang;

namespace {

SourceManager sourceManager;

SyntaxTree parse(StringRef text) {
    return SyntaxTree::fromText(sourceManager, text);
}

TEST_CASE("Finding top level", "[binding:decls]") {
    auto file1 = parse("module A; A a(); endmodule\nmodule B; endmodule\nmodule C; endmodule");
    auto file2 = parse("module D; B b(); E e(); endmodule\nmodule E; module C; endmodule C c(); endmodule");

    BumpAllocator alloc;
    Diagnostics diagnostics;
    SemanticModel sem(alloc, diagnostics);

    Buffer<CompilationUnitSymbol*> compilationUnits;
    compilationUnits.append(sem.bindCompilationUnit(file1.root()));
    compilationUnits.append(sem.bindCompilationUnit(file2.root()));

    DeclarationTable declTable(ArrayRef<CompilationUnitSymbol*>(compilationUnits.begin(), compilationUnits.end()),
                               diagnostics);

    /*auto topLevelModules = declTable.getTopLevelModules(diagnostics);

    CHECK(diagnostics.empty());
    REQUIRE(topLevelModules.count() == 2);
    CHECK(topLevelModules[0]->header->name.valueText() == "D");
    CHECK(topLevelModules[1]->header->name.valueText() == "C");*/
}

TEST_CASE("Bind module implicit", "[binding:module_implicit]") {
    auto tree = parse(R"(
module Top #(parameter int foo = 4) ();
    Leaf l();
endmodule

module Leaf();

endmodule
)");

    /*DeclarationTable declTable;
    declTable.addSyntaxTree(&tree);

    Diagnostics diagnostics;
    auto topLevelModules = declTable.getTopLevelModules(diagnostics);

    REQUIRE(topLevelModules.count() == 1);

	BumpAllocator alloc;
    SemanticModel sem(alloc, diagnostics);
    sem.bindDesignElement(topLevelModules[0]);*/
}

}